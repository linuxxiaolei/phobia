#include <stddef.h>
#include <stdarg.h>

#include "freertos/FreeRTOS.h"
#include "hal/hal.h"

#include "main.h"
#ifdef HW_HAVE_NETWORK_EPCAN
#include "epcan.h"
#endif /* HW_HAVE_NETWORK_EPCAN */
#include "libc.h"
#include "shell.h"

app_main_t			ap;
pmc_t 				pm LD_CCRAM;
tlm_t				tlm;

uint8_t				ucHeap[configTOTAL_HEAP_SIZE] LD_CCRAM;

void xvprintf(io_ops_t *_io, const char *fmt, va_list ap);

void log_TRACE(const char *fmt, ...)
{
	va_list		ap;
	io_ops_t	ops = {

		.getc = NULL,
		.putc = &log_putc
	};

	xprintf(&ops, "[%i.%i] ", log.boot_COUNT, xTaskGetTickCount());

        va_start(ap, fmt);
	xvprintf(&ops, fmt, ap);
        va_end(ap);
}

void vAssertCalled(const char *file, int line)
{
	taskDISABLE_INTERRUPTS();
	log_TRACE("FreeRTOS: Assert %s:%i" EOL, file, line);

	hal_system_reset();
}

void vApplicationMallocFailedHook()
{
	taskDISABLE_INTERRUPTS();
	log_TRACE("FreeRTOS: Heap Allocation Failed" EOL);

	hal_system_reset();
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
	taskDISABLE_INTERRUPTS();
	log_TRACE("FreeRTOS: Stack Overflow in %8x task" EOL, (uint32_t) xTask);

	hal_system_reset();
}

void vApplicationIdleHook()
{
	hal_cpu_sleep();
}

#ifdef HW_HAVE_ANALOG_KNOB
static float
ADC_get_knob_ANG()
{
	float			knob;

	knob = ADC_get_sample(GPIO_ADC_KNOB_ANG);

	return knob * hal.const_ADC.GK;
}

#ifdef HW_HAVE_BRAKE_KNOB
static float
ADC_get_knob_BRK()
{
	float			knob;

	knob = ADC_get_sample(GPIO_ADC_KNOB_BRK);

	return knob * hal.const_ADC.GK;
}
#endif /* HW_HAVE_BRAKE_KNOB */
#endif /* HW_HAVE_ANALOG_KNOB */

#ifdef HW_HAVE_FAN_CONTROL
static void
GPIO_set_mode_FAN()
{
	GPIO_set_mode_OUTPUT(GPIO_FAN_EN);

#ifdef GPIO_FAN_OPEN_DRAIN
	GPIO_set_mode_OPEN_DRAIN(GPIO_FAN_EN);
#else /* HW_GPIO_FAN_OPEN_DRAIN */
	GPIO_set_mode_PUSH_PULL(GPIO_FAN_EN);
#endif
}

static void
GPIO_set_state_FAN(int knob)
{
#ifdef GPIO_FAN_OPEN_DRAIN
	if (knob == PM_ENABLED) {
#else /* HW_GPIO_FAN_OPEN_DRAIN */
	if (knob != PM_ENABLED) {
#endif
		GPIO_set_LOW(GPIO_FAN_EN);
	}
	else {
		GPIO_set_HIGH(GPIO_FAN_EN);
	}
}
#endif /* HW_HAVE_FAN_CONTROL */

static int
elapsed_IDLE()
{
	TickType_t		xIDLE, xNOW;
	int			elapsed = PM_DISABLED;

	xIDLE = (TickType_t) (ap.idle_timeout * (float) configTICK_RATE_HZ);

	if (xIDLE > 0) {

		xNOW = xTaskGetTickCount();

		if (xNOW - (TickType_t) ap.idle_INVOKE > (TickType_t) 50) {

			ap.idle_RESET = xNOW;
		}

		ap.idle_INVOKE = xNOW;

		if (pm.lu_total_revol != ap.idle_revol_cached) {

			ap.idle_RESET = xNOW;
			ap.idle_revol_cached = pm.lu_total_revol;
		}

		if (xNOW - (TickType_t) ap.idle_RESET > xIDLE) {

			elapsed = PM_ENABLED;
		}
	}

	return elapsed;
}

static int
elapsed_DISARM()
{
	TickType_t		xDISARM, xNOW;
	int			elapsed = PM_DISABLED;

	xDISARM = (TickType_t) (ap.disarm_timeout * (float) configTICK_RATE_HZ);

	if (xDISARM > 0) {

		xNOW = xTaskGetTickCount();

		if (xNOW - (TickType_t) ap.disarm_INVOKE > (TickType_t) 50) {

			ap.disarm_RESET = xNOW;
		}

		ap.disarm_INVOKE = xNOW;

		if (xNOW - (TickType_t) ap.disarm_RESET > xDISARM) {

			elapsed = PM_ENABLED;
		}
	}

	return elapsed;
}

void task_TEMP(void *pData)
{
	TickType_t		xWake;
	float			x_PCB, x_EXT, lock_PCB;

	if (ap.ntc_PCB.type != NTC_NONE) {

		GPIO_set_mode_ANALOG(ap.ntc_PCB.gpio);
	}

	if (ap.ntc_EXT.type != NTC_NONE) {

		GPIO_set_mode_ANALOG(ap.ntc_EXT.gpio);
	}

	xWake = xTaskGetTickCount();

	x_PCB = PM_MAX_F;
	x_EXT = PM_MAX_F;

	lock_PCB = 0.f;

	do {
		/* 10 Hz.
		 * */
		vTaskDelayUntil(&xWake, (TickType_t) 100);

		ap.temp_MCU = ADC_get_sample(GPIO_ADC_TEMPINT);

		if (ap.ntc_PCB.type != NTC_NONE) {

			ap.temp_PCB = ntc_read_temperature(&ap.ntc_PCB);
		}
		else {
			ap.temp_PCB = ap.temp_MCU;
		}

		if (ap.ntc_EXT.type != NTC_NONE) {

			ap.temp_EXT = ntc_read_temperature(&ap.ntc_EXT);
		}
		else {
			ap.temp_EXT = 0.f;
		}

		if (ap.temp_PCB > ap.tpro_PCB_temp_halt - lock_PCB) {

			x_PCB = 0.f;

			if (pm.lu_MODE != PM_LU_DISABLED) {

				pm.fsm_errno = PM_ERROR_HW_OVERTEMPERATURE;
				pm.fsm_req = PM_STATE_HALT;
			}

			lock_PCB = ap.tpro_temp_recovery;
		}
		else {
			if (ap.temp_PCB > ap.tpro_PCB_temp_derate) {

				/* Derate current in case of PCB thermal overload.
				 * */
				x_PCB = ap.tpro_derated_PCB;
			}
			else if (ap.temp_PCB < ap.tpro_PCB_temp_derate - ap.tpro_temp_recovery) {

				x_PCB = PM_MAX_F;
			}

			lock_PCB = 0.f;
		}

#ifdef HW_HAVE_FAN_CONTROL
		/* Enable FAN in case of PCB is warm enough.
		 * */
		if (ap.temp_PCB > ap.tpro_PCB_temp_FAN) {

			GPIO_set_state_FAN(PM_ENABLED);
		}
		else if (ap.temp_PCB < ap.tpro_PCB_temp_FAN - ap.tpro_temp_recovery) {

			GPIO_set_state_FAN(PM_DISABLED);
		}
#endif /* HW_HAVE_FAN_CONTROL */

		if (ap.tpro_EXT_temp_derate > M_EPS_F) {

			if (ap.temp_EXT > ap.tpro_EXT_temp_derate) {

				/* Derate current in case of external thermal
				 * overload (motor overheat).
				 * */
				x_EXT = ap.tpro_derated_EXT;
			}
			else if (ap.temp_EXT < ap.tpro_EXT_temp_derate - ap.tpro_temp_recovery) {

				x_EXT = PM_MAX_F;
			}
		}

		pm.i_derate_on_PCB = (x_PCB < x_EXT) ? x_PCB : x_EXT;

#ifdef HW_HAVE_DRV_ON_PCB
		if (		hal.DRV.auto_RESET == PM_ENABLED
				&& DRV_fault() != 0) {

			DRV_halt();
			DRV_startup();
		}
#endif /* HW_HAVE_DRV_ON_PCB */

#ifdef GPIO_LED_MODE
		if (pm.lu_MODE != PM_LU_DISABLED) {

			GPIO_set_HIGH(GPIO_LED_MODE);
		}
		else {
			GPIO_set_LOW(GPIO_LED_MODE);
		}
#endif /* GPIO_LED_MODE */
	}
	while (1);
}

void task_ALERT(void *pData)
{
	do {
		if (pm.fsm_errno != PM_OK) {

			GPIO_set_HIGH(GPIO_LED_ALERT);
			vTaskDelay((TickType_t) 100);

			GPIO_set_LOW(GPIO_LED_ALERT);
			vTaskDelay((TickType_t) 900);
		}
		else if (log_status() != 0) {

			GPIO_set_HIGH(GPIO_LED_ALERT);
			vTaskDelay((TickType_t) 100);

			GPIO_set_LOW(GPIO_LED_ALERT);
			vTaskDelay((TickType_t) 400);

			GPIO_set_HIGH(GPIO_LED_ALERT);
			vTaskDelay((TickType_t) 100);

			GPIO_set_LOW(GPIO_LED_ALERT);
			vTaskDelay((TickType_t) 900);
		}
		else {
			vTaskDelay((TickType_t) 100);
		}
	}
	while (1);
}

#ifdef HW_HAVE_ANALOG_KNOB
static void
inner_KNOB()
{
	float			control, range, scaled;

	if (		ap.knob_ACTIVE == PM_ENABLED
			&& pm.lu_MODE == PM_LU_DISABLED) {

		ap.knob_ACTIVE = PM_DISABLED;
		ap.knob_DISARM = PM_ENABLED;
	}

	if (		   ap.knob_in_ANG < ap.knob_range_LST[0]
			|| ap.knob_in_ANG > ap.knob_range_LST[1]) {

		/* Loss of KNOB signal.
		 * */
		scaled = - 1.f;

		if (ap.knob_ACTIVE == PM_ENABLED) {

			if (pm.lu_MODE != PM_LU_DISABLED) {

				pm.fsm_errno = PM_ERROR_KNOB_CONTROL_FAULT;
				pm.fsm_req = PM_STATE_LU_SHUTDOWN;
			}

			ap.knob_ACTIVE = PM_DISABLED;
		}

		ap.knob_DISARM = PM_ENABLED;
	}
	else {
		if (ap.knob_in_ANG < ap.knob_range_ANG[1]) {

			range = ap.knob_range_ANG[0] - ap.knob_range_ANG[1];
			scaled = (ap.knob_range_ANG[1] - ap.knob_in_ANG) / range;
		}
		else {
			range = ap.knob_range_ANG[2] - ap.knob_range_ANG[1];
			scaled = (ap.knob_in_ANG - ap.knob_range_ANG[1]) / range;
		}

		if (scaled < - 1.f) {

			scaled = - 1.f;

			if (ap.knob_ACTIVE == PM_ENABLED) {

				if (elapsed_IDLE() != 0) {

					if (pm.lu_MODE != PM_LU_DISABLED) {

						pm.fsm_req = PM_STATE_LU_SHUTDOWN;
					}

					ap.knob_ACTIVE = PM_DISABLED;
				}
			}
			else if (ap.knob_DISARM == PM_ENABLED) {

				if (elapsed_DISARM() == PM_ENABLED) {

					ap.knob_DISARM = PM_DISABLED;
				}
			}
		}
		else {
			if (scaled > 1.f) {

				scaled = 1.f;
			}

			if (		ap.knob_STARTUP == PM_ENABLED
					&& ap.knob_ACTIVE != PM_ENABLED) {

				if (ap.knob_DISARM == PM_ENABLED) {

					/* DISARMED */
				}
				else if (pm.lu_MODE == PM_LU_DISABLED) {

					pm.fsm_req = PM_STATE_LU_STARTUP;
					ap.knob_ACTIVE = PM_ENABLED;
				}
			}
		}
	}

	if (scaled < 0.f) {

		range = ap.knob_control_ANG[1] - ap.knob_control_ANG[0];
		control = ap.knob_control_ANG[1] + range * scaled;
	}
	else {
		range = ap.knob_control_ANG[2] - ap.knob_control_ANG[1];
		control = ap.knob_control_ANG[1] + range * scaled;
	}

#ifdef HW_HAVE_BRAKE_KNOB
	if (		   ap.knob_in_BRK < ap.knob_range_LST[0]
			|| ap.knob_in_BRK > ap.knob_range_LST[1]) {

		/* Loss of BRAKE signal.
		 * */
	}
	else if (ap.knob_BRAKE == PM_ENABLED) {

		range = ap.knob_range_BRK[1] - ap.knob_range_BRK[0];
		scaled = (ap.knob_in_BRK - ap.knob_range_BRK[0]) / range;

		scaled = (scaled < 0.f) ? 0.f : (scaled > 1.f) ? 1.f : scaled;

		control += (ap.knob_control_BRK - control) * scaled;
	}
#endif /* HW_HAVE_BRAKE_KNOB */

	if (ap.knob_reg_DATA != control) {

		ap.knob_reg_DATA = control;

		if (ap.knob_reg_ID != ID_NULL) {

			reg_SET_F(ap.knob_reg_ID, ap.knob_reg_DATA);
		}
	}
}

void task_KNOB(void *pData)
{
	TickType_t		xWake;

	GPIO_set_mode_ANALOG(GPIO_ADC_KNOB_ANG);

#ifdef HW_HAVE_BRAKE_KNOB
	GPIO_set_mode_ANALOG(GPIO_ADC_KNOB_BRK);
#endif /* HW_HAVE_BRAKE_KNOB */

	xWake = xTaskGetTickCount();

	do {
		if (ap.knob_ENABLED == PM_ENABLED) {

			/* 100 Hz.
			 * */
			vTaskDelayUntil(&xWake, (TickType_t) 10);

			ap.knob_in_ANG = ADC_get_knob_ANG();

#ifdef HW_HAVE_BRAKE_KNOB
			ap.knob_in_BRK = ADC_get_knob_BRK();
#endif /* HW_HAVE_BRAKE_KNOB */

			inner_KNOB();
		}
		else {
			/* 10 Hz.
			 * */
			vTaskDelayUntil(&xWake, (TickType_t) 100);

			ap.knob_in_ANG = ADC_get_knob_ANG();

#ifdef HW_HAVE_BRAKE_KNOB
			ap.knob_in_BRK = ADC_get_knob_BRK();
#endif /* HW_HAVE_BRAKE_KNOB */
		}
	}
	while (1);
}
#endif /* HW_HAVE_ANALOG_KNOB */

static void
default_flash_load()
{
	float			vm_D, halt_I, halt_U;

	hal.USART_baud_rate = 57600;

	hal.PWM_frequency = HW_PWM_FREQUENCY_HZ;
	hal.PWM_deadtime = HW_PWM_DEADTIME_NS;
	hal.ADC_reference_voltage = HW_ADC_REFERENCE_VOLTAGE;
	hal.ADC_shunt_resistance = HW_ADC_SHUNT_RESISTANCE;
	hal.ADC_amplifier_gain = HW_ADC_AMPLIFIER_GAIN;
	hal.ADC_voltage_ratio = HW_ADC_VOLTAGE_R2 / (HW_ADC_VOLTAGE_R1 + HW_ADC_VOLTAGE_R2);

	vm_D =    HW_ADC_VOLTAGE_R1 * HW_ADC_VOLTAGE_R2
		+ HW_ADC_VOLTAGE_R2 * HW_ADC_VOLTAGE_BIAS_R3
		+ HW_ADC_VOLTAGE_R1 * HW_ADC_VOLTAGE_BIAS_R3;

	hal.ADC_terminal_ratio = HW_ADC_VOLTAGE_R2 * HW_ADC_VOLTAGE_BIAS_R3 / vm_D;
	hal.ADC_terminal_bias = HW_ADC_VOLTAGE_R1 * HW_ADC_VOLTAGE_R2
				* hal.ADC_reference_voltage / vm_D;

#ifdef HW_HAVE_ANALOG_KNOB
	hal.ADC_knob_ratio = HW_ADC_KNOB_R2 / (HW_ADC_KNOB_R1 + HW_ADC_KNOB_R2);
#endif /* HW_HAVE_ANALOG_KNOB */

	hal.ADC_sample_time = ADC_SMP_28;
	hal.ADC_sample_advance = ADC_SAMPLE_ADVANCE;

	hal.DPS_mode = DPS_DISABLED;
	hal.PPM_mode = PPM_DISABLED;
	hal.PPM_frequency = 2000000U;

#ifdef HW_HAVE_DRV_ON_PCB
	hal.DRV.part = HW_DRV_PARTNO;
	hal.DRV.auto_RESET = PM_DISABLED;
	hal.DRV.gpio_GATE_EN = GPIO_DRV_GATE_EN;
	hal.DRV.gpio_FAULT = GPIO_DRV_FAULT;
	hal.DRV.gate_current = HW_DRV_GATE_CURRENT;
	hal.DRV.ocp_level = HW_DRV_OCP_LEVEL;
#endif /* HW_HAVE_DRV_ON_PCB */

#ifdef HW_HAVE_NETWORK_EPCAN
	net.node_ID = 0;
	net.log_MODE = EPCAN_LOG_DISABLED;
	net.timeout_EP = 100 * HW_PWM_FREQUENCY_HZ / 1000;
	net.ep[0].ID = 10;
	net.ep[0].rate = HW_PWM_FREQUENCY_HZ / 1000;
	net.ep[1].ID = 20;
	net.ep[1].rate = net.ep[0].rate;
	net.ep[2].ID = 30;
	net.ep[2].rate = net.ep[0].rate;
	net.ep[3].ID = 40;
	net.ep[3].rate = net.ep[0].rate;
	net.ep[4].ID = 50;
	net.ep[4].rate = net.ep[0].rate;
	net.ep[5].ID = 60;
	net.ep[5].rate = net.ep[0].rate;
	net.ep[6].ID = 70;
	net.ep[6].rate = net.ep[0].rate;
	net.ep[7].ID = 80;
	net.ep[7].rate = net.ep[0].rate;
#endif /* HW_HAVE_NETWORK_EPCAN */

	ap.ppm_reg_ID = ID_PM_S_SETPOINT_SPEED_PC;
	ap.ppm_STARTUP = PM_DISABLED;
	ap.ppm_DISARM = PM_ENABLED;
	ap.ppm_range_pulse[0] = 1.0f;	/* (ms) */
	ap.ppm_range_pulse[1] = 1.5f;	/* (ms) */
	ap.ppm_range_pulse[2] = 2.0f;	/* (ms) */
	ap.ppm_range_control[0] = 0.f;
	ap.ppm_range_control[1] = 50.f;
	ap.ppm_range_control[2] = 100.f;

	ap.step_reg_ID = ID_PM_X_SETPOINT_LOCATION_MM;
	ap.step_STARTUP = PM_DISABLED;
	ap.step_const_ld_EP = 0.f;

#ifdef HW_HAVE_ANALOG_KNOB
	ap.knob_reg_ID = ID_PM_I_SETPOINT_CURRENT_PC;
	ap.knob_ENABLED = PM_DISABLED;
	ap.knob_BRAKE = PM_DISABLED;
	ap.knob_STARTUP = PM_DISABLED;
	ap.knob_DISARM = PM_ENABLED;
	ap.knob_range_ANG[0] = 1.0f;	/* (V) */
	ap.knob_range_ANG[1] = 2.5f;	/* (V) */
	ap.knob_range_ANG[2] = 4.0f;	/* (V) */
	ap.knob_range_BRK[0] = 2.0f;	/* (V) */
	ap.knob_range_BRK[1] = 4.0f;	/* (V) */
	ap.knob_range_LST[0] = 0.2f;	/* (V) */
	ap.knob_range_LST[1] = 4.8f;	/* (V) */
	ap.knob_control_ANG[0] = 0.f;
	ap.knob_control_ANG[1] = 50.f;
	ap.knob_control_ANG[2] = 100.f;
	ap.knob_control_BRK = - 100.f;
#endif /* HW_HAVE_ANALOG_KNOB */

	ap.idle_timeout = 2.f;
	ap.disarm_timeout = 1.f;

#ifdef HW_HAVE_NTC_ON_PCB
	ap.ntc_PCB.type = HW_NTC_PCB_TYPE;
	ap.ntc_PCB.gpio = GPIO_ADC_NTC_PCB;
	ap.ntc_PCB.balance = HW_NTC_PCB_BALANCE;
	ap.ntc_PCB.ntc0 = HW_NTC_PCB_NTC0;
	ap.ntc_PCB.ta0 = HW_NTC_PCB_TA0;
	ap.ntc_PCB.betta = HW_NTC_PCB_BETTA;
#endif /* HW_HAVE_NTC_ON_PCB */

#ifdef HW_HAVE_NTC_MOTOR
	ap.ntc_EXT.type = NTC_GND;
	ap.ntc_EXT.gpio = GPIO_ADC_NTC_EXT;
	ap.ntc_EXT.balance = 10000.f;
	ap.ntc_EXT.ntc0 = 10000.f;
	ap.ntc_EXT.ta0 = 25.f;
	ap.ntc_EXT.betta = 3380.f;
#endif /* HW_HAVE_NTC_MOTOR */

	ap.tpro_PCB_temp_halt = 110.f;
	ap.tpro_PCB_temp_derate = 90.f;
	ap.tpro_PCB_temp_FAN = 50.f;
	ap.tpro_EXT_temp_derate = 0.f;
	ap.tpro_derated_PCB = 20.f;
	ap.tpro_derated_EXT = 20.f;
	ap.tpro_temp_recovery = 5.f;

	ap.adc_load_scale[0] = 0.f;
	ap.adc_load_scale[1] = 4.65E-6f;

	pm.m_freq = hal.PWM_frequency;
	pm.m_dT = 1.f / pm.m_freq;
	pm.dc_resolution = hal.PWM_resolution;
	pm.proc_set_DC = &PWM_set_DC;
	pm.proc_set_Z = &PWM_set_Z;

	/* Default PMC configuration.
	 * */
	pm_auto(&pm, PM_AUTO_BASIC_DEFAULT);
	pm_auto(&pm, PM_AUTO_CONFIG_DEFAULT);

	pm.dc_minimal = HW_PWM_MINIMAL_PULSE;
	pm.dc_clearance = HW_PWM_CLEARANCE_ZONE;
	pm.dc_skip = HW_PWM_SKIP_ZONE;
	pm.dc_bootstrap = HW_PWM_BOOTSTRAP_RETENTION;

#if ADC_HAVE_SEQUENCE__ABC(HW_ADC_SAMPLING_SEQUENCE)
#ifdef HW_HAVE_LOW_SIDE_SHUNT
	pm.config_IFB = PM_IFB_ABC_GND;
#else /* HW_HAVE_LOW_SIDE_SHUNT */
	pm.config_IFB = PM_IFB_ABC_INLINE;
#endif
#else /* ADC_HAVE_SEQUENCE__ABC(HW_ADC_SAMPLING_SEQUENCE) */
#ifdef HW_HAVE_LOW_SIDE_SHUNT
	pm.config_IFB = PM_IFB_AB_GND;
#else /* HW_HAVE_LOW_SIDE_SHUNT */
	pm.config_IFB = PM_IFB_AB_INLINE;
#endif
#endif

#if ADC_HAVE_SEQUENCE__TTT(HW_ADC_SAMPLING_SEQUENCE)
	pm.config_TVM = PM_ENABLED;
#else /* ADC_HAVE_SEQUENCE__TTT(HW_ADC_SAMPLING_SEQUENCE) */
	pm.config_TVM = PM_DISABLED;
#endif

	ADC_const_build();

	halt_I = m_fabsf(hal.const_ADC.GA * ADC_RESOLUTION / 2.f);
	halt_U = m_fabsf(hal.const_ADC.GU * ADC_RESOLUTION);

	pm.fault_current_halt = (float) (int) (.95f * halt_I);
	pm.fault_voltage_halt = (float) (int) (.95f * halt_U);

	pm_auto(&pm, PM_AUTO_MAXIMAL_CURRENT);

	pm.watt_wA_maximal = (float) (int) (0.66666667f * pm.i_maximal);
	pm.watt_wA_reverse = pm.watt_wA_maximal;

	pm.watt_uDC_maximal = (float) (int) (pm.fault_voltage_halt - 5.f);

	pm.watt_wP_maximal = (float) (int) (pm.watt_wA_maximal * pm.watt_uDC_maximal);
	pm.watt_wP_reverse = pm.watt_wP_maximal;

#ifdef HW_CONFIG_INLINE
	HW_CONFIG_INLINE;
#endif /* HW_CONFIG_INLINE */

	/* Default telemetry.
	 * */
	tlm_reg_default(&tlm);

	/* Try to load all above params from flash.
	 * */
	flash_block_regs_load();
}

void task_INIT(void *pData)
{
	uint32_t		seed[3];

	GPIO_set_mode_OUTPUT(GPIO_LED_ALERT);
	GPIO_set_HIGH(GPIO_LED_ALERT);

#ifdef GPIO_LED_MODE
	GPIO_set_mode_OUTPUT(GPIO_LED_MODE);
	GPIO_set_LOW(GPIO_LED_MODE);
#endif /* GPIO_LED_MODE */

	RNG_startup();
	TIM_startup();

	log_bootup();

	seed[0] = RNG_urand();

	/* We drop the first value and check if RNG
	 * gives the same number each time.
	 * */
	seed[1] = RNG_urand();
	seed[2] = RNG_urand();

	if (seed[1] == seed[2]) {

		log_TRACE("RNG failed" EOL);
	}

	/* Initial SEED.
	 * */
	rseed = seed[2] ^ RNG_make_UID();

	/* Load the configuration.
	 * */
	default_flash_load();

#ifdef HW_HAVE_DRV_ON_PCB
	DRV_startup();
#endif /* HW_HAVE_DRV_ON_PCB */

#ifdef GPIO_BOOST_EN
	GPIO_set_mode_OUTPUT(GPIO_BOOST_EN);
	GPIO_set_HIGH(GPIO_BOOST_EN);
#endif /* GPIO_BOOST_EN */

#ifdef HW_HAVE_FAN_CONTROL
	GPIO_set_mode_FAN();
	GPIO_set_state_FAN(PM_DISABLED);
#endif /* HW_HAVE_FAN_CONTROL */

	hal_lock_irq();

	/* Do CORE startup.
	 * */
	ADC_startup();
	PWM_startup();
	WD_startup();

	pm.m_freq = hal.PWM_frequency;
	pm.m_dT = 1.f / pm.m_freq;
	pm.dc_resolution = hal.PWM_resolution;

	pm.fsm_req = PM_STATE_ZERO_DRIFT;

	hal_unlock_irq(0);

	DPS_startup();
	PPM_startup();

	io_USART.getc = &USART_getc;
	io_USART.putc = &USART_putc;

#ifdef HW_HAVE_USB_CDC_ACM
	io_USB.getc = &USB_getc;
	io_USB.putc = &USB_putc;
#endif /* HW_HAVE_USB_CDC_ACM */

#ifdef HW_HAVE_NETWORK_EPCAN
	io_CAN.getc = &EPCAN_getc;
	io_CAN.putc = &EPCAN_putc;
#endif /* HW_HAVE_NETWORK_EPCAN */

	/* Default to USART.
	 * */
	iodef = &io_USART;

	USART_startup();

#ifdef HW_HAVE_USB_CDC_ACM
	USB_startup();
#endif /* HW_HAVE_USB_CDC_ACM */

#ifdef HW_HAVE_NETWORK_EPCAN
	EPCAN_startup();
#endif /* HW_HAVE_NETWORK_EPCAN */

	xTaskCreate(task_TEMP, "TEMP", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(task_ALERT, "ALERT", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

#ifdef HW_HAVE_ANALOG_KNOB
	xTaskCreate(task_KNOB, "KNOB", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
#endif /* HW_HAVE_ANALOG_KNOB */

	xTaskCreate(task_CMDSH, "CMDSH", 220, NULL, 1, NULL);

	GPIO_set_LOW(GPIO_LED_ALERT);

#undef APP_DEF
#define APP_DEF(name)		reg_TOUCH_I(ID_AP_TASK_ ## name);
#include "app/apdefs.h"

	vTaskDelete(NULL);
}

static void
inner_PULSE_WIDTH()
{
	float		control, range, scaled;

	if (		ap.ppm_ACTIVE == PM_ENABLED
			&& pm.lu_MODE == PM_LU_DISABLED) {

		ap.ppm_ACTIVE = PM_DISABLED;
		ap.ppm_DISARM = PM_ENABLED;
	}

	if (ap.ppm_in_pulse < ap.ppm_range_pulse[1]) {

		range = ap.ppm_range_pulse[0] - ap.ppm_range_pulse[1];
		scaled = (ap.ppm_range_pulse[1] - ap.ppm_in_pulse) / range;
	}
	else {
		range = ap.ppm_range_pulse[2] - ap.ppm_range_pulse[1];
		scaled = (ap.ppm_in_pulse - ap.ppm_range_pulse[1]) / range;
	}

	if (scaled < - 1.f) {

		scaled = - 1.f;

		if (ap.ppm_ACTIVE == PM_ENABLED) {

			if (elapsed_IDLE() != 0) {

				if (pm.lu_MODE != PM_LU_DISABLED) {

					pm.fsm_req = PM_STATE_LU_SHUTDOWN;
				}

				ap.ppm_ACTIVE = PM_DISABLED;
			}
		}
		else if (ap.ppm_DISARM == PM_ENABLED) {

			if (elapsed_DISARM() != 0) {

				ap.ppm_DISARM = PM_DISABLED;
			}
		}
	}
	else {
		if (scaled > 1.f) {

			scaled = 1.f;
		}

		if (		ap.ppm_STARTUP == PM_ENABLED
				&& ap.ppm_ACTIVE != PM_ENABLED) {

			if (ap.ppm_DISARM == PM_ENABLED) {

				/* DISARMED */
			}
			else if (pm.lu_MODE == PM_LU_DISABLED) {

				pm.fsm_req = PM_STATE_LU_STARTUP;
				ap.ppm_ACTIVE = PM_ENABLED;
			}
		}
	}

	if (scaled < 0.f) {

		range = ap.ppm_range_control[1] - ap.ppm_range_control[0];
		control = ap.ppm_range_control[1] + range * scaled;
	}
	else {
		range = ap.ppm_range_control[2] - ap.ppm_range_control[1];
		control = ap.ppm_range_control[1] + range * scaled;
	}

	if (ap.ppm_reg_DATA != control) {

		ap.ppm_reg_DATA = control;

		if (ap.ppm_reg_ID != ID_NULL) {

			reg_SET_F(ap.ppm_reg_ID, ap.ppm_reg_DATA);
		}
	}
}

static void
in_PULSE_WIDTH()
{
	ap.ppm_in_pulse = PPM_get_PULSE() * 1000.f;	/* (ms) */
	ap.ppm_in_freq = PPM_get_PERIOD();

	if (hal.PPM_caught != 0) {

		inner_PULSE_WIDTH();
	}
	else {
		if (ap.ppm_ACTIVE == PM_ENABLED) {

			if (pm.lu_MODE != PM_LU_DISABLED) {

				pm.fsm_errno = PM_ERROR_KNOB_CONTROL_FAULT;
				pm.fsm_req = PM_STATE_LU_SHUTDOWN;
			}

			ap.ppm_ACTIVE = PM_DISABLED;
		}

		ap.ppm_DISARM = PM_ENABLED;
	}
}

static void
in_STEP_DIR()
{
	int		relEP;

	ap.step_in_EP = PPM_get_STEP_DIR();

	relEP = (ap.step_in_EP - ap.step_prev_EP) & 0xFFFFU;
	ap.step_prev_EP = ap.step_in_EP;

	if (relEP != 0) {

		ap.step_accu_EP += relEP;
		ap.step_reg_DATA = ap.step_accu_EP * ap.step_const_ld_EP;

		if (ap.step_reg_ID != ID_NULL) {

			reg_SET_F(ap.step_reg_ID, ap.step_reg_DATA);
		}

		if (ap.step_STARTUP == PM_ENABLED) {

			if (pm.lu_MODE == PM_LU_DISABLED) {

				pm.fsm_req = PM_STATE_LU_STARTUP;
				ap.step_ACTIVE = PM_ENABLED;
			}
		}
	}
}

void ADC_IRQ()
{
	pmfb_t		fb;

	fb.current_A = hal.ADC_current_A;
	fb.current_B = hal.ADC_current_B;
	fb.current_C = hal.ADC_current_C;
	fb.voltage_U = hal.ADC_voltage_U;
	fb.voltage_A = hal.ADC_voltage_A;
	fb.voltage_B = hal.ADC_voltage_B;
	fb.voltage_C = hal.ADC_voltage_C;

	fb.pulse_HS = 0;
	fb.pulse_EP = 0;

	if (hal.DPS_mode == DPS_DRIVE_HALL) {

		fb.pulse_HS = DPS_get_HALL();
	}
	else if (hal.DPS_mode == DPS_DRIVE_EABI) {

		fb.pulse_EP = DPS_get_EP();
	}
	else if (hal.DPS_mode == DPS_DRIVE_SOFTWARE) {

		fb.pulse_EP = ap.pulse_EP;
	}

	if (hal.PPM_mode == PPM_PULSE_WIDTH) {

		in_PULSE_WIDTH();
	}
	else if (hal.PPM_mode == PPM_STEP_DIR) {

		in_STEP_DIR();
	}
	else if (hal.PPM_mode == PPM_BACKUP_EABI) {

		fb.pulse_EP = PPM_get_backup_EP();
	}

	if (hal.CNT_diag[1] >= pm.m_dT) {

		pm.fsm_errno = PM_ERROR_HW_UNMANAGED_IRQ;
		pm.fsm_req = PM_STATE_HALT;
	}

#ifdef HW_HAVE_DRV_ON_PCB
	if (		pm.lu_MODE != PM_LU_DISABLED
			&& DRV_fault() != 0) {

		pm.fsm_errno = PM_ERROR_HW_OVERCURRENT;
		pm.fsm_req = PM_STATE_HALT;
	}
#endif /* HW_HAVE_DRV_ON_PCB */

	pm_feedback(&pm, &fb);

#ifdef HW_HAVE_NETWORK_EPCAN
	EPCAN_pipe_REGULAR();
#endif /* HW_HAVE_NETWORK_EPCAN */

	tlm_reg_grab(&tlm);

	WD_kick();
}

void app_MAIN()
{
	xTaskCreate(task_INIT, "INIT", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
	vTaskStartScheduler();
}

void app_control(const reg_t *reg, void (* pvTask) (void *), const char *pcName)
{
	TaskHandle_t		xHandle;

	if (reg->link->i == PM_ENABLED) {

		xHandle = xTaskGetHandle(pcName);

		if (xHandle == NULL) {

			xTaskCreate(pvTask, pcName, configMINIMAL_STACK_SIZE,
					(void *) reg->link, 1, NULL);
		}
	}
}

void app_halt()
{
#ifdef GPIO_BOOST_EN
	GPIO_set_LOW(GPIO_BOOST_EN);

	vTaskDelay((TickType_t) 50);
#endif /* GPIO_BOOST_EN */

#ifdef HW_HAVE_DRV_ON_PCB
	DRV_halt();
#endif /* HW_HAVE_DRV_ON_PCB */
}

SH_DEF(rtos_version)
{
	uint32_t	flash_sizeof, flash_crc32;
	int		verified;

	printf("HW_revision \"%s\"" EOL, fw.hwrevision);
	printf("FW_build \"%s\"" EOL, fw.build);

	flash_sizeof = fw.ld_end - fw.ld_begin;
	flash_crc32 = * (uint32_t *) fw.ld_end;

	verified = (crc32b((const void *) fw.ld_begin, flash_sizeof) == flash_crc32) ? 1 : 0;

	printf("FW_sizeof %i" EOL, flash_sizeof);
	printf("FW_crc32 %8x (%s)" EOL, flash_crc32, (verified != 0) ? "verified" : "corrupted");
}

SH_DEF(rtos_uptime)
{
	struct {

		int	day;
		int	hour;
		int	min;
		int	sec;
	}
	tm;

	tm.sec = xTaskGetTickCount() / configTICK_RATE_HZ;

	tm.day = tm.sec / 86400;
	tm.sec -= tm.day * 86400;
	tm.hour = tm.sec / 3600;
	tm.sec -= tm.hour * 3600;
	tm.min = tm.sec / 60;
	tm.sec -= tm.min * 60;

	printf("[%i] %id %ih %im %is" EOL, log.boot_COUNT, tm.day, tm.hour, tm.min, tm.sec);
}

SH_DEF(rtos_task_info)
{
	TaskStatus_t		*list;
	int			len, symStat, n;

	len = uxTaskGetNumberOfTasks();
	list = pvPortMalloc(len * sizeof(TaskStatus_t));

	if (list != NULL) {

		len = uxTaskGetSystemState(list, len, NULL);

		printf("TCB      ID Name              Stat Prio Stack    Free" EOL);

		for (n = 0; n < len; ++n) {

			switch (list[n].eCurrentState) {

				case eRunning:
					symStat = 'R';
					break;

				case eReady:
					symStat = 'E';
					break;

				case eBlocked:
					symStat = 'B';
					break;

				case eSuspended:
					symStat = 'S';
					break;

				case eDeleted:
					symStat = 'D';
					break;

				case eInvalid:
				default:
					symStat = 'N';
					break;
			}

			printf("%8x %2i %17s %c    %2i   %8x %i" EOL,
					(uint32_t) list[n].xHandle,
					(int) list[n].xTaskNumber,
					list[n].pcTaskName, (int) symStat,
					(int) list[n].uxCurrentPriority,
					(uint32_t) list[n].pxStackBase,
					(int) list[n].usStackHighWaterMark);
		}

		vPortFree(list);
	}
}

SH_DEF(rtos_heap_info)
{
	HeapStats_t	heapinfo;

	vPortGetHeapStats(&heapinfo);

	printf("RAM      Total  Free   Minimum" EOL);

	printf("%8x %6i %6i %6i" EOL,
			configTOTAL_HEAP_SIZE,
			heapinfo.xAvailableHeapSpaceInBytes,
			heapinfo.xMinimumEverFreeBytesRemaining);

	printf("Diag     %6i %6i %6i %6i %6i" EOL,
			heapinfo.xSizeOfLargestFreeBlockInBytes,
			heapinfo.xSizeOfSmallestFreeBlockInBytes,
			heapinfo.xNumberOfFreeBlocks,
			heapinfo.xNumberOfSuccessfulAllocations,
			heapinfo.xNumberOfSuccessfulFrees);
}

SH_DEF(rtos_log_flush)
{
	if (log.textbuf[0] != 0) {

		puts(log.textbuf);
		puts(EOL);
	}
}

SH_DEF(rtos_log_clean)
{
	if (log.textbuf[0] != 0) {

		memset(log.textbuf, 0, sizeof(log.textbuf));

		log.len = 0;
	}
}

SH_DEF(rtos_hexdump)
{
	uint8_t			*m;
	int			n, i, ascii, line = 4;

	if (htoi((int *) &m, s) != NULL) {

		stoi(&line, sh_next_arg(s));

		for (n = 0; n < line; ++n) {

			printf("%8x  ", (uint32_t) m);

			for (i = 0; i < 8; ++i)
				printf("%2x ", m[i]);

			puts(" ");

			for (i = 8; i < 16; ++i)
				printf("%2x ", m[i]);

			puts(" |");

			for (i = 0; i < 16; ++i) {

				ascii = m[i];
				ascii = (ascii >= 0x20 && ascii <= 0x7E)
					? ascii : '.';

				putc(ascii);
			}

			puts("|" EOL);

			m += 16;
		}
	}
}

SH_DEF(rtos_reboot)
{
	if (pm.lu_MODE != PM_LU_DISABLED) {

		printf("Unable when PM is running" EOL);
		return ;
	}

	app_halt();
	hal_system_reset();
}

SH_DEF(rtos_bootload)
{
	if (pm.lu_MODE != PM_LU_DISABLED) {

		printf("Unable when PM is running" EOL);
		return ;
	}

	app_halt();
	hal_bootload_reset();
}

