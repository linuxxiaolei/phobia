#include "libm.h"
#include "pm.h"

void pm_quick_build(pmc_t *pm)
{
	if (PM_CONFIG_NOP(pm) == PM_NOP_THREE_PHASE) {

		pm->k_UMAX = 0.66666667f;	/* 2 / NOP */
		pm->k_EMAX = 0.57735027f;	/* 1 / sqrt(NOP) */
		pm->k_KWAT = 1.5f;		/* NOP / 2 */
	}
	else {
		pm->k_UMAX = 1.f;		/* 2 / NOP */
		pm->k_EMAX = 0.70710678f;	/* 1 / sqrt(NOP) */
		pm->k_KWAT = 1.f;		/* NOP / 2 */
	}

	pm->ts_minimal = (int) (pm->dc_minimal * (1.f / 1000000.f)
			* pm->m_freq * (float) pm->dc_resolution);
	pm->ts_clearance = (int) (pm->dc_clearance * (1.f / 1000000.f)
			* pm->m_freq * (float) pm->dc_resolution);
	pm->ts_skip = (int) (pm->dc_skip * (1.f / 1000000.f)
			* pm->m_freq * (float) pm->dc_resolution);
	pm->ts_bootstrap = PM_TSMS(pm, pm->dc_bootstrap);
	pm->ts_inverted = 1.f / (float) pm->dc_resolution;

	if (pm->const_lambda > M_EPSILON) {

		pm->quick_iWb = 1.f / pm->const_lambda;
		pm->quick_iWb2 = pm->quick_iWb * pm->quick_iWb;
	}

	if (pm->const_im_L1 > M_EPSILON) {

		pm->quick_iL1 = 1.f / pm->const_im_L1;
		pm->quick_TiL1 = pm->m_dT * pm->quick_iL1;
	}

	if (pm->const_im_L2 > M_EPSILON) {

		pm->quick_iL2 = 1.f / pm->const_im_L2;
		pm->quick_TiL2 = pm->m_dT * pm->quick_iL2;
	}

	pm->quick_HFwS = M_2_PI_F * pm->hfi_freq;

	if (		pm->eabi_const_Zq != 0
			&& pm->eabi_const_EP != 0) {

		pm->quick_ZiEP = M_2_PI_F * (float) (pm->const_Zp * pm->eabi_const_Zs)
			/ (float) (pm->eabi_const_Zq * pm->eabi_const_EP);
	}

	if (pm->sincos_const_Zq != 0) {

		pm->quick_ZiSQ = (float) (pm->const_Zp * pm->sincos_const_Zs)
			/ (float) pm->sincos_const_Zq;
	}

	pm->quick_WiL4 = pm->const_lambda / (4.f * (pm->const_im_L1 - pm->const_im_L2));
}

static void
pm_auto_basic_default(pmc_t *pm)
{
	pm->dc_minimal = 0.2f;			/* (us) */
	pm->dc_clearance = 5.0f;		/* (us) */
	pm->dc_skip = 2.0f;			/* (us) */
	pm->dc_bootstrap = 100.f;		/* (ms) */

	pm->config_NOP = PM_NOP_THREE_PHASE;
	pm->config_IFB = PM_IFB_ABC_INLINE;
	pm->config_TVM = PM_ENABLED;
	pm->config_DBG = PM_DISABLED;

	pm->fault_voltage_tol = 4.f;		/* (V) */
	pm->fault_current_tol = 4.f;		/* (A) */
	pm->fault_accuracy_tol = 0.10f;		/*     */
	pm->fault_terminal_tol = 0.090f;	/* (V) */
	pm->fault_current_halt = 156.f;		/* (A) */
	pm->fault_voltage_halt = 57.f;		/* (V) */

	pm->vsi_AF = 1;				/* NOTE: Disable all flags until  */
	pm->vsi_BF = 1;				/* clearance zone is calculated.  */
	pm->vsi_CF = 1;
	pm->vsi_SF = 1;
	pm->vsi_UF = 1;

	pm->watt_wP_maximal = 4000.f;		/* (Watt) */
	pm->watt_wA_maximal = 80.f;		/* (A) */
	pm->watt_wP_reverse = 4000.f;		/* (Watt) */
	pm->watt_wA_reverse = 80.f;		/* (A) */
	pm->watt_uDC_maximal = 52.f;		/* (V) */
	pm->watt_uDC_minimal = 7.f;		/* (V) */

	pm->i_maximal = 120.f;			/* (A) */
	pm->i_reverse = pm->i_maximal;

	m_lf_randseed(&pm->hfi_seed, 75);	/* NOTE: lfg initial random seed. */
}

static void
pm_auto_config_default(pmc_t *pm)
{
	pm->config_VSI_ZERO = PM_VSI_GND;
	pm->config_VSI_CLAMP = PM_DISABLED;
	pm->config_LU_FORCED = PM_ENABLED;
	pm->config_LU_ESTIMATE = PM_FLUX_ORTEGA;
	pm->config_LU_SENSOR = PM_SENSOR_NONE;
	pm->config_LU_LOCATION = PM_LOCATION_NONE;
	pm->config_LU_DRIVE = PM_DRIVE_SPEED;
	pm->config_HFI_WAVETYPE = PM_HFI_NONE;
	pm->config_HFI_PERMANENT = PM_DISABLED;
	pm->config_EXCITATION = PM_EXCITATION_CONST;
	pm->config_SALIENCY = PM_SALIENCY_NEGATIVE;
	pm->config_RELUCTANCE = PM_DISABLED;
	pm->config_WEAKENING = PM_DISABLED;
	pm->config_REVERSE_BRAKE = PM_DISABLED;
	pm->config_SPEED_MAXIMAL = PM_ENABLED;
	pm->config_EABI_FRONTEND = PM_EABI_INCREMENTAL;
	pm->config_SINCOS_FRONTEND = PM_SINCOS_ANALOG;
	pm->config_BOOST_CHARGE = PM_DISABLED;

	pm->tm_transient_slow = 40.f;		/* (ms) */
	pm->tm_transient_fast = 2.f;		/* (ms) */
	pm->tm_voltage_hold = 100.f;		/* (ms) */
	pm->tm_current_hold = 300.f;		/* (ms) */
	pm->tm_current_ramp = 400.f;		/* (ms) */
	pm->tm_instant_probe = 2.f;		/* (ms) */
	pm->tm_average_probe = 200.f;		/* (ms) */
	pm->tm_average_drift = 100.f;		/* (ms) */
	pm->tm_average_inertia = 700.f;		/* (ms) */
	pm->tm_pause_startup = 100.f;		/* (ms) */
	pm->tm_pause_forced = 1000.f;		/* (ms) */
	pm->tm_pause_halt = 2000.f;		/* (ms) */

	pm->scale_iA[0] = 0.f;
	pm->scale_iA[1] = 1.f;
	pm->scale_iB[0] = 0.f;
	pm->scale_iB[1] = 1.f;
	pm->scale_iC[0] = 0.f;
	pm->scale_iC[1] = 1.f;
	pm->scale_uS[0] = 0.f;
	pm->scale_uS[1] = 1.f;
	pm->scale_uA[0] = 0.f;
	pm->scale_uA[1] = 1.f;
	pm->scale_uB[0] = 0.f;
	pm->scale_uB[1] = 1.f;
	pm->scale_uC[0] = 0.f;
	pm->scale_uC[1] = 1.f;

	pm->probe_current_hold = 20.f;
	pm->probe_current_weak = 5.f;
	pm->probe_hold_angle = 0.f;
	pm->probe_current_sine = 5.f;
	pm->probe_current_bias = 0.f;
	pm->probe_freq_sine = 1100.f;
	pm->probe_speed_hold = 900.f;
	pm->probe_speed_tol = 50.f;
	pm->probe_location_tol = 0.10f;
	pm->probe_loss_maximal = 400.f;
	pm->probe_gain_P = 1E-2f;
	pm->probe_gain_I = 1E-3f;

	pm->vsi_gain_LP = 5E-3f;
	pm->vsi_mask_XF = PM_MASK_NONE;

	pm->tvm_ACTIVE = PM_DISABLED;
	pm->tvm_clean_zone = 0.10f;
	pm->tvm_FIR_A[0] = 0.f;
	pm->tvm_FIR_A[1] = 0.f;
	pm->tvm_FIR_A[2] = 0.f;
	pm->tvm_FIR_B[0] = 0.f;
	pm->tvm_FIR_B[1] = 0.f;
	pm->tvm_FIR_B[2] = 0.f;
	pm->tvm_FIR_C[0] = 0.f;
	pm->tvm_FIR_C[1] = 0.f;
	pm->tvm_FIR_C[2] = 0.f;

	pm->lu_rate = 900.f;
	pm->lu_gain_mq_LP = 4E-3f;

	pm->forced_hold_D = 20.f;
	pm->forced_maximal = 900.f;
	pm->forced_reverse = pm->forced_maximal;
	pm->forced_accel = 400.f;
	pm->forced_slew_rate = 100.f;
	pm->forced_stop_DC = 0.7f;

	pm->detach_threshold = 1.f;
	pm->detach_trip_AP = 2E-1f;
	pm->detach_gain_SF = 5E-2f;

	pm->flux_trip_AP = 2E-1f;
	pm->flux_gain_IN = 5E-3f;
	pm->flux_gain_LO = 2E-6f;
	pm->flux_gain_HI = 5E-5f;
	pm->flux_gain_SF = 5E-2f;
	pm->flux_gain_IF = 0.5f;

	pm->kalman_gain_Q[0] = 5E-2f;
	pm->kalman_gain_Q[1] = 5E-2f;
	pm->kalman_gain_Q[2] = 5E-4f;
	pm->kalman_gain_Q[3] = 7E+1f;
	pm->kalman_gain_Q[4] = 5E-5f;
	pm->kalman_gain_R = 5E-1f;

	pm->zone_noise = 50.f;
	pm->zone_threshold = 90.f;
	pm->zone_gain_TH = 0.7f;
	pm->zone_gain_LP = 5E-3f;

	pm->hfi_freq = 2380.f;
	pm->hfi_sine = 5.f;

	pm->hall_trip_AP = 5E-3f;
	pm->hall_gain_LO = 5E-4f;
	pm->hall_gain_SF = 7E-3f;
	pm->hall_gain_IF = 0.9f;

	pm->eabi_const_EP = 2400;
	pm->eabi_const_Zs = 1;
	pm->eabi_const_Zq = 1;
	pm->eabi_trip_AP = 5E-2f;
	pm->eabi_gain_LO = 5E-3f;
	pm->eabi_gain_SF = 5E-2f;
	pm->eabi_gain_IF = 0.1f;

	pm->sincos_const_Zs = 1;
	pm->sincos_const_Zq = 1;

	pm->const_lambda = 0.f;
	pm->const_Rs = 0.f;
	pm->const_Zp = 1;
	pm->const_Ja = 0.f;
	pm->const_im_L1 = 0.f;
	pm->const_im_L2 = 0.f;
	pm->const_im_B = 0.f;
	pm->const_im_R = 0.f;

	pm->watt_gain_LP = 5E-2f;

	pm->i_derate_on_HFI = 10.f;
	pm->i_slew_rate = 7000.f;
	pm->i_damping = 1.f;
	pm->i_gain_P = 2E-1f;
	pm->i_gain_I = 5E-3f;
	pm->i_gain_Q = 1.f;

	pm->mtpa_gain_LP = 5E-2f;

	pm->weak_maximal = 30.f;
	pm->weak_gain_EU = 5E-3f;

	pm->v_maximal = 90.f;
	pm->v_reverse = pm->v_maximal;

	pm->v_uDC_tol = 4.f;
	pm->v_gain_P = 5E+1f;
	pm->v_gain_I = 5E-1f;

	pm->s_maximal = 15000.f;
	pm->s_reverse = pm->s_maximal;
	pm->s_accel = 7000.f;
	pm->s_damping = 1.f;
	pm->s_gain_P = 4E-2f;
	pm->s_gain_I = 0.f;
	pm->s_gain_D = 5E-5f;
	pm->s_gain_Q = 1.f;

	pm->l_track_tol = 50.f;
	pm->l_gain_LP = 5E-3f;

	pm->x_maximal = 100.f;
	pm->x_minimal = - pm->x_maximal;
	pm->x_damping = 0.10f;
	pm->x_residual_tol = 0.f;
	pm->x_gain_P = 35.f;
	pm->x_gain_D = 10.f;
}

static void
pm_auto_machine_default(pmc_t *pm)
{
	pm->probe_speed_hold = 900.f;

	pm->lu_gain_mq_LP = 4E-3f;

	pm->forced_maximal = 900.f;
	pm->forced_reverse = pm->forced_maximal;
	pm->forced_accel = 400.f;

	pm->zone_noise = 50.f;
	pm->zone_threshold = 90.f;

	pm->const_lambda = 0.f;
	pm->const_Rs = 0.f;
	pm->const_Ja = 0.f;
	pm->const_im_L1 = 0.f;
	pm->const_im_L2 = 0.f;
	pm->const_im_B = 0.f;
	pm->const_im_R = 0.f;

	pm->i_slew_rate = 7000.f;
	pm->i_gain_P = 2E-1f;
	pm->i_gain_I = 5E-3f;
	pm->i_gain_Q = 1.f;

	pm->s_gain_P = 4E-2f;
	pm->s_gain_I = 0.f;
	pm->s_gain_D = 5E-5f;
	pm->s_gain_Q = 1.f;
}

static void
pm_auto_scale_default(pmc_t *pm)
{
	pm->scale_iA[0] = 0.f;
	pm->scale_iA[1] = 1.f;
	pm->scale_iB[0] = 0.f;
	pm->scale_iB[1] = 1.f;
	pm->scale_iC[0] = 0.f;
	pm->scale_iC[1] = 1.f;
	pm->scale_uS[0] = 0.f;
	pm->scale_uS[1] = 1.f;
	pm->scale_uA[0] = 0.f;
	pm->scale_uA[1] = 1.f;
	pm->scale_uB[0] = 0.f;
	pm->scale_uB[1] = 1.f;
	pm->scale_uC[0] = 0.f;
	pm->scale_uC[1] = 1.f;

	pm->tvm_ACTIVE = PM_DISABLED;
	pm->tvm_FIR_A[0] = 0.f;
	pm->tvm_FIR_A[1] = 0.f;
	pm->tvm_FIR_A[2] = 0.f;
	pm->tvm_FIR_B[0] = 0.f;
	pm->tvm_FIR_B[1] = 0.f;
	pm->tvm_FIR_B[2] = 0.f;
	pm->tvm_FIR_C[0] = 0.f;
	pm->tvm_FIR_C[1] = 0.f;
	pm->tvm_FIR_C[2] = 0.f;
}

static void
pm_auto_maximal_current(pmc_t *pm)
{
	float			maximal_A, few_A;

	/* Get the maximal inline current.
	 * */
	maximal_A = pm->fault_current_halt * 0.8f;

	if (pm->const_Rs > M_EPSILON) {

		/* Based on DC link voltage.
		 * */
		few_A = pm->k_UMAX * pm->const_fb_U / pm->const_Rs;
		maximal_A = (few_A < maximal_A) ? few_A : maximal_A;

		/* Based on resistive LOSSES.
		 * */
		few_A = m_sqrtf(pm->k_UMAX * pm->probe_loss_maximal / pm->const_Rs);
		maximal_A = (few_A < maximal_A) ? few_A : maximal_A;

		if (maximal_A < pm->i_maximal) {

			pm->i_maximal = (float) (int) maximal_A;
			pm->i_reverse = pm->i_maximal;
		}
	}
	else {
		pm->i_maximal = (float) (int) maximal_A;
		pm->i_reverse = pm->i_maximal;
	}
}

static void
pm_auto_probe_speed_hold(pmc_t *pm)
{
	float			probe_MAX, probe_MIN;

	probe_MIN = 1.5f * (pm->zone_threshold + pm->zone_noise);

	if (pm->probe_speed_hold < probe_MIN) {

		pm->probe_speed_hold = probe_MIN;
	}

	if (pm->const_lambda > M_EPSILON) {

		probe_MAX = 0.7f * pm->k_EMAX * pm->const_fb_U / pm->const_lambda;

		if (pm->probe_speed_hold > probe_MAX) {

			pm->probe_speed_hold = probe_MAX;
		}
	}
}

static void
pm_auto_zone_threshold(pmc_t *pm)
{
	float			thld_MAX, thld_MIN, thld_IRU;

	if (pm->const_Rs > M_EPSILON) {

		/* Allowable range of the noise threshold.
		 * */
		thld_MAX = 0.4f * pm->forced_maximal;
		thld_MIN = 10.f;

		if (pm->zone_noise > thld_MAX) {

			pm->zone_noise = thld_MAX;
		}

		if (pm->const_lambda > M_EPSILON) {

			thld_MAX = 10.f / pm->const_lambda;

			if (pm->zone_noise > thld_MAX) {

				pm->zone_noise = thld_MAX;
			}
		}

		if (pm->zone_noise < thld_MIN) {

			pm->zone_noise = thld_MIN;
		}

		/* Based on uncertainty due to resistance thermal drift.
		 * */
		thld_IRU = 0.2f * pm->i_maximal * pm->const_Rs;

		if (		PM_CONFIG_TVM(pm) == PM_ENABLED
				&& pm->tvm_ACTIVE == PM_ENABLED) {

			/* Based on TVM terminal accuracy.
			 * */
			thld_IRU += pm->fault_terminal_tol;
		}
		else {
			/* Based on voltage uncertainty.
			 * */
			thld_IRU += pm->dc_minimal * (2.f / 1000000.f)
				* pm->m_freq * pm->const_fb_U;
		}

		if (pm->const_lambda > M_EPSILON) {

			/* Total zone threshold.
			 * */
			pm->zone_threshold = thld_IRU / pm->const_lambda;
		}

		thld_MAX = 0.8f * pm->forced_maximal - pm->zone_noise;

		if (pm->zone_threshold > thld_MAX) {

			pm->zone_threshold = thld_MAX;
		}

		if (pm->zone_threshold < thld_MIN) {

			pm->zone_threshold = thld_MIN;
		}
	}
}

static void
pm_auto_forced_maximal(pmc_t *pm)
{
	float		forced_MAX, forced_MIN;

	forced_MIN = pm->probe_speed_hold;

	if (pm->forced_maximal < forced_MIN) {

		pm->forced_maximal = forced_MIN;
		pm->forced_reverse = pm->forced_maximal;
	}

	if (pm->const_lambda > M_EPSILON) {

		forced_MAX = 0.7f * pm->k_EMAX * pm->const_fb_U / pm->const_lambda;

		if (pm->forced_maximal > forced_MAX) {

			pm->forced_maximal = forced_MAX;
			pm->forced_reverse = pm->forced_maximal;
		}
	}
}

static void
pm_auto_forced_accel(pmc_t *pm)
{
	float		mQ;

	if (pm->const_Ja > 0.f) {

		mQ = pm_torque_feasible(pm, pm->forced_hold_D);

		/* Tune forced control based on the motor constants.
		 * */
		pm->forced_accel = 0.1f * mQ / pm->const_Ja;
	}
}

static void
pm_auto_loop_current(pmc_t *pm)
{
	float		Lmin, Df, Kp, Ki;

	if (		   pm->const_im_L1 > M_EPSILON
			&& pm->const_im_L1 > M_EPSILON) {

		Lmin = (pm->const_im_L1 < pm->const_im_L2)
			? pm->const_im_L1 : pm->const_im_L2;

		Df = pm->i_damping;

		/* We tune the current loop based on state-space model.
		 *
		 *          [1-R*T/L-Kp*T/L  -Ki*T/L]
		 * x(k+1) = [1                1     ] * x(k)
		 *
		 * */
		Kp = 0.5f * Lmin * Df * pm->m_freq - pm->const_Rs;
		Ki = 0.02f * Lmin * Df * pm->m_freq;

		pm->i_gain_P = (Kp > 0.f) ? Kp : 0.f;
		pm->i_gain_I = Ki;
		pm->i_gain_Q = 1.f;

		pm->i_slew_rate = 0.05f * Df * pm->const_fb_U / Lmin;
	}
}

static void
pm_auto_mq_load_torque(pmc_t *pm)
{
	float		Df, TiJ, releq;

	if (		pm->zone_noise > M_EPSILON
			&& pm->const_Ja > 0.f) {

		Df = pm->s_damping;
		TiJ = pm->m_dT / (pm->const_Ja * pm->zone_noise);

		if (pm->const_lambda > M_EPSILON) {

			pm->lu_gain_mq_LP = 2.f * Df * pm->const_lambda * TiJ;
		}
		else {
			releq = (pm->const_im_L1 - pm->const_im_L2) * pm->i_maximal;

			pm->lu_gain_mq_LP = 1.f * Df * releq * TiJ;
		}
	}
}

static void
pm_auto_loop_speed(pmc_t *pm)
{
	float		Df;

	if (pm->zone_noise > M_EPSILON) {

		Df = pm->s_damping;

		pm->s_gain_P = 2.f * Df / pm->zone_noise;
		pm->s_gain_D = 5.E-4f * Df / pm->zone_noise;

		pm->s_gain_I = 0.f;
		pm->s_gain_Q = 1.f;
	}
}

void pm_auto(pmc_t *pm, int req)
{
	switch (req) {

		case PM_AUTO_BASIC_DEFAULT:
			pm_auto_basic_default(pm);
			break;

		case PM_AUTO_CONFIG_DEFAULT:
			pm_auto_config_default(pm);
			break;

		case PM_AUTO_MACHINE_DEFAULT:
			pm_auto_machine_default(pm);
			break;

		case PM_AUTO_SCALE_DEFAULT:
			pm_auto_scale_default(pm);
			break;

		case PM_AUTO_MAXIMAL_CURRENT:
			pm_auto_maximal_current(pm);
			break;

		case PM_AUTO_PROBE_SPEED_HOLD:
			pm_auto_probe_speed_hold(pm);
			break;

		case PM_AUTO_ZONE_THRESHOLD:
			pm_auto_zone_threshold(pm);
			break;

		case PM_AUTO_FORCED_MAXIMAL:
			pm_auto_forced_maximal(pm);
			break;

		case PM_AUTO_FORCED_ACCEL:
			pm_auto_forced_accel(pm);
			break;

		case PM_AUTO_LOOP_CURRENT:
			pm_auto_loop_current(pm);
			break;

		case PM_AUTO_MQ_LOAD_TORQUE:
			pm_auto_mq_load_torque(pm);
			break;

		case PM_AUTO_LOOP_SPEED:
			pm_auto_loop_speed(pm);
			break;

		default:
			break;
	}
}

float pm_torque_equation(pmc_t *pm, float iD, float iQ)
{
	float		mQ, releq;

	if (pm->config_RELUCTANCE == PM_ENABLED) {

		releq = (pm->const_im_L1 - pm->const_im_L2) * iD;
	}
	else {
		releq = 0.f;
	}

	mQ = pm->k_KWAT * (pm->const_lambda + releq) * iQ;

	return mQ;
}

static float
pm_torque_approx_MTPA(pmc_t *pm, float iD, float iQ)
{
	float		Wq, Bq;

	iD *= iD;
	iQ *= iQ;

	Wq = pm->quick_WiL4 * pm->quick_WiL4;
	Bq = (iD < iQ) ? iD + iQ : iQ + iQ;

	if (pm->quick_WiL4 < 0.f) {

		iD = - m_sqrtf(Wq + Bq * 0.5f) - pm->quick_WiL4;
	}
	else {
		iD = m_sqrtf(Wq + Bq * 0.5f) - pm->quick_WiL4;
	}

	return iD;
}

float pm_torque_feasible(pmc_t *pm, float iQ)
{
	float		MTPA_iD, mQ;

	if (pm->config_RELUCTANCE == PM_ENABLED) {

		MTPA_iD = pm_torque_approx_MTPA(pm, 0.f, iQ);
		MTPA_iD = pm_torque_approx_MTPA(pm, MTPA_iD, iQ);

		mQ = pm_torque_equation(pm, MTPA_iD, iQ);
	}
	else {
		mQ = pm_torque_equation(pm, 0.f, iQ);
	}

	return mQ;
}

static float
pm_torque_get_current(pmc_t *pm, float mQ)
{
	float           iQ, releq;

	if (pm->config_RELUCTANCE == PM_ENABLED) {

		if (		pm->const_lambda < M_EPSILON
				&& pm->mtpa_approx_Q < 1.f) {

			pm->mtpa_approx_Q = 1.f;
		}

		releq = (pm->const_im_L1 - pm->const_im_L2) * pm->mtpa_approx_Q;
		iQ = mQ / (pm->k_KWAT * (pm->const_lambda + releq));

		pm->mtpa_approx_Q = pm_torque_approx_MTPA(pm, pm->mtpa_approx_Q, iQ);
	}
	else {
		if (pm->const_lambda > M_EPSILON) {

			iQ = mQ / (pm->k_KWAT * pm->const_lambda);
		}
		else {
			iQ = 0.f;
		}
	}

	return iQ;
}

static float
pm_torque_get_accel(pmc_t *pm)
{
	float			tA = 0.f;

	if (pm->const_Ja > 0.f) {

		tA = (pm->lu_mq_produce - pm->lu_mq_load) / pm->const_Ja;
	}

	return tA;
}

static void
pm_forced(pmc_t *pm)
{
	float		wSP, dSA, xRF;

	/* Get the SETPOINT of forced speed.
	 * */
	if (pm->config_LU_DRIVE == PM_DRIVE_CURRENT) {

		wSP = (pm->i_setpoint_current < - M_EPSILON) ? - PM_MAX_F
			: (pm->i_setpoint_current > M_EPSILON) ? PM_MAX_F : 0.f;
	}
	else {
		wSP = pm->s_setpoint_speed;
	}

	/* Maximal forced speed constraint.
	 * */
	wSP = (wSP > pm->forced_maximal) ? pm->forced_maximal :
		(wSP < - pm->forced_reverse) ? - pm->forced_reverse : wSP;

	/* Reduce the acceleration in case of current lack.
	 * */
	xRF = m_fabsf(pm->forced_track_D / pm->forced_hold_D);
	dSA = pm->forced_accel * xRF * pm->m_dT;

	if (		pm->vsi_lpf_DC < pm->forced_stop_DC
			|| m_fabsf(wSP) < m_fabsf(pm->forced_wS)) {

		/* Update actual speed with allowed acceleration.
		 * */
		pm->forced_wS = (pm->forced_wS < wSP - dSA) ? pm->forced_wS + dSA :
			(pm->forced_wS > wSP + dSA) ? pm->forced_wS - dSA : wSP;
	}

	/* Update DQ-axes.
	 * */
	m_rotatef(pm->forced_F, pm->forced_wS * pm->m_dT);
}

static void
pm_flux_detached(pmc_t *pm)
{
	float		uA, uB, uC, uX, uY, U, A, B, blend;

	/* Get back EMF voltage.
	 * */
	uA = pm->fb_uA;
	uB = pm->fb_uB;
	uC = pm->fb_uC;

	if (PM_CONFIG_NOP(pm) == PM_NOP_THREE_PHASE) {

		U = 0.33333333f * (uA + uB + uC);

		uA = uA - U;
		uB = uB - U;

		uX = uA;
		uY = 0.57735027f * uA + 1.1547005f * uB;
	}
	else {
		uX = uA - uC;
		uY = uB - uC;
	}

	pm->vsi_X = uX;
	pm->vsi_Y = uY;

	/* Absolute back EMF voltage.
	 * */
	U = m_sqrtf(uX * uX + uY * uY);

	if (U > pm->detach_threshold) {

		A = 1.f / U;

		uX *= A;
		uY *= A;

		if (pm->detach_TIM != 0) {

			/* Speed estimation (PLL).
			 * */
			m_rotatef(pm->flux_X, pm->flux_wS * pm->m_dT);

			A = uX * pm->flux_X[0] + uY * pm->flux_X[1];
			B = uY * pm->flux_X[0] - uX * pm->flux_X[1];

			if (A > M_EPSILON) {

				blend = U * pm->detach_trip_AP;
				blend = (blend > 1.f) ? 1.f : blend;

				A = pm->detach_gain_SF * blend;

				pm->flux_wS += B * pm->m_freq * A;
			}

			pm->flux_lambda = U / m_fabsf(pm->flux_wS);

			A = (pm->flux_wS < 0.f) ? - 1.f : 1.f;

			pm->flux_F[0] = uY * A;
			pm->flux_F[1] = - uX * A;
		}

		pm->detach_TIM++;

		pm->flux_X[0] = uX;
		pm->flux_X[1] = uY;
	}
	else {
		pm->detach_TIM = 0;

		pm->flux_wS = 0.f;
	}
}

static void
pm_flux_ortega(pmc_t *pm)
{
	float		uX, uY, lX, lY, EX, EY, E, A, B, blend;

	/* Get the actual voltage.
	 * */
	uX = pm->vsi_X - pm->const_Rs * pm->lu_iX;
	uY = pm->vsi_Y - pm->const_Rs * pm->lu_iY;

	if (PM_CONFIG_TVM(pm) == PM_ENABLED) {

		uX += pm->tvm_X0 - pm->vsi_X0;
		uY += pm->tvm_Y0 - pm->vsi_Y0;
	}

	/* Stator FLUX.
	 * */
	lX = pm->const_im_L2 * pm->lu_iX;
	lY = pm->const_im_L2 * pm->lu_iY;

	if (pm->flux_LINKAGE == PM_ENABLED) {

		/* FLUX equations.
		 * */
		pm->flux_X[0] += uX * pm->m_dT;
		pm->flux_X[1] += uY * pm->m_dT;

		EX = pm->flux_X[0] - lX;
		EY = pm->flux_X[1] - lY;

		blend = m_fabsf(pm->flux_wS * pm->const_lambda) * pm->flux_trip_AP;
		blend = (blend > 1.f) ? 1.f : blend;

		/* Get the flux RESIDUE.
		 * */
		E = 1.f - (EX * EX + EY * EY) * pm->quick_iWb2;

		/* Adaptive GAIN.
		 * */
		E *=	  pm->flux_gain_HI * blend
			+ pm->flux_gain_LO * (1.f - blend);

		pm->flux_X[0] += EX * E * pm->quick_iWb;
		pm->flux_X[1] += EY * E * pm->quick_iWb;
	}
	else {
		/* Startup estimation.
		 * */
		pm->flux_X[0] += uX * pm->m_dT;
		pm->flux_X[1] += uY * pm->m_dT;

		EX = pm->flux_X[0] - lX;
		EY = pm->flux_X[1] - lY;

		E = - pm->flux_gain_IN;

		pm->flux_X[0] += EX * E;
		pm->flux_X[1] += EY * E;
	}

	/* Extract the rotor flux linkage.
	 * */
	EX = pm->flux_X[0] - lX;
	EY = pm->flux_X[1] - lY;

	E = m_sqrtf(EX * EX + EY * EY);

	pm->flux_lambda = E;

	if (E > M_EPSILON) {

		A = 1.f / E;

		EX *= A;
		EY *= A;

		if (pm->flux_LINKAGE == PM_ENABLED) {

			/* Speed estimation (PLL).
			 * */
			m_rotatef(pm->flux_F, pm->flux_wS * pm->m_dT);

			A = EX * pm->flux_F[0] + EY * pm->flux_F[1];
			B = EY * pm->flux_F[0] - EX * pm->flux_F[1];

			if (A > M_EPSILON) {

				pm->flux_wS += B * pm->m_freq * pm->flux_gain_SF;
			}

			if (pm->flux_gain_IF > M_EPSILON) {

				A = pm_torque_get_accel(pm);
				pm->flux_wS += A * pm->m_dT * pm->flux_gain_IF;
			}
		}
		else {
			/* Startup estimate borrowing.
			 * */
			pm->flux_wS = pm->lu_wS;
		}

		pm->flux_F[0] = EX;
		pm->flux_F[1] = EY;
	}
}

static void
pm_kalman_equation(pmc_t *pm, float Y[2], const float X[2], const float F[2])
{
	float		uD, uQ, R1, E1, flux_D, flux_Q;

	uD = F[0] * pm->vsi_X + F[1] * pm->vsi_Y;
	uQ = F[0] * pm->vsi_Y - F[1] * pm->vsi_X;

	R1 = pm->const_Rs;
	E1 = pm->const_lambda;

	uQ += pm->kalman_bias_Q;

	flux_D = pm->const_im_L1 * X[0] + E1;
	flux_Q = pm->const_im_L2 * X[1];

	Y[0] = (uD - R1 * X[0] + flux_Q * pm->flux_wS) * pm->quick_iL1;
	Y[1] = (uQ - R1 * X[1] - flux_D * pm->flux_wS) * pm->quick_iL2;
}

static void
pm_kalman_solve(pmc_t *pm, float X[2], float F[2], float wS)
{
	float		Y1[2], Y2[2];

	/* Second-order ODE solver.
	 * */

	pm_kalman_equation(pm, Y1, X, F);

	X[0] += Y1[0] * pm->m_dT;
	X[1] += Y1[1] * pm->m_dT;

	m_rotatef(F, wS * pm->m_dT);

	pm_kalman_equation(pm, Y2, X, F);

	X[0] += (Y2[0] - Y1[0]) * pm->m_dT * 0.5f;
	X[1] += (Y2[1] - Y1[1]) * pm->m_dT * 0.5f;
}

static void
pm_kalman_solve_tvm(pmc_t *pm, float X[2], float F[2])
{
	float		uX, uY, uD, uQ;

	/* First-order ODE solver.
	 * */

	uX = pm->tvm_X0 - pm->vsi_X0;
	uY = pm->tvm_Y0 - pm->vsi_Y0;

	uD = F[0] * uX + F[1] * uY;
	uQ = F[0] * uY - F[1] * uX;

	X[0] += uD * pm->m_dT * pm->quick_iL1;
	X[1] += uQ * pm->m_dT * pm->quick_iL2;
}

static void
pm_kalman_jacobian(pmc_t *pm, const float X[2], const float F[2], float wS)
{
	float		*A = pm->kalman_A;

	/*
	 *     [ A(0) A(1) A(2) A(3) 0    ]
	 *     [ A(4) A(5) A(6) A(7) A(8) ]
	 * A = [ 0    0    1    A(9) 0    ]
	 *     [ 0    0    0    1    0    ]
	 *     [ 0    0    0    0    1    ]
	 * */

	A[0] = 1.f - pm->const_Rs * pm->quick_TiL1;
	A[1] = wS * pm->const_im_L2 * pm->quick_TiL1;
	A[2] = (F[0] * pm->vsi_Y - F[1] * pm->vsi_X) * pm->quick_TiL1;
	A[3] = X[1] * pm->const_im_L2 * pm->quick_TiL1;

	A[4] = - wS * pm->const_im_L1 * pm->quick_TiL2;
	A[5] = 1.f - pm->const_Rs * pm->quick_TiL2;
	A[6] = (- F[0] * pm->vsi_X - F[1] * pm->vsi_Y) * pm->quick_TiL2;
	A[7] = (- pm->const_lambda - X[0] * pm->const_im_L1) * pm->quick_TiL2;
	A[8] = pm->quick_TiL2;
	A[9] = pm->m_dT;
}

static void
pm_kalman_forecast(pmc_t *pm)
{
	float		*P = pm->kalman_P;
	const float	*A = pm->kalman_A;
	const float	*Q = pm->kalman_gain_Q;

	float		AP[16];

	/*
	 * Calculate predicted (a priori) covariance to the next cycle.
	 *
	 * P = A * P * A' + Q.
	 *
	 *     [ P(0)  P(1)  P(3)  P(6)  P(10) ]
	 *     [ P(1)  P(2)  P(4)  P(7)  P(11) ]
	 * P = [ P(3)  P(4)  P(5)  P(8)  P(12) ]
	 *     [ P(6)  P(7)  P(8)  P(9)  P(13) ]
	 *     [ P(10) P(11) P(12) P(13) P(14) ]
	 *
	 * */

	AP[0] = A[0] * P[0] + A[1] * P[1] + A[2] * P[3] + A[3] * P[6];
	AP[1] = A[0] * P[1] + A[1] * P[2] + A[2] * P[4] + A[3] * P[7];
	AP[2] = A[0] * P[3] + A[1] * P[4] + A[2] * P[5] + A[3] * P[8];
	AP[3] = A[0] * P[6] + A[1] * P[7] + A[2] * P[8] + A[3] * P[9];

	AP[4] = A[4] * P[0] + A[5] * P[1] + A[6] * P[3] + A[7] * P[6] + A[8] * P[10];
	AP[5] = A[4] * P[1] + A[5] * P[2] + A[6] * P[4] + A[7] * P[7] + A[8] * P[11];
	AP[6] = A[4] * P[3] + A[5] * P[4] + A[6] * P[5] + A[7] * P[8] + A[8] * P[12];
	AP[7] = A[4] * P[6] + A[5] * P[7] + A[6] * P[8] + A[7] * P[9] + A[8] * P[13];
	AP[8] = A[4] * P[10] + A[5] * P[11] + A[6] * P[12] + A[7] * P[13] + A[8] * P[14];

	AP[9] = P[3] + A[9] * P[6];
	AP[10] = P[4] + A[9] * P[7];
	AP[11] = P[5] + A[9] * P[8];
	AP[12] = P[8] + A[9] * P[9];
	AP[13] = P[12] + A[9] * P[13];

	AP[14] = P[6];
	AP[15] = P[10];

	P[0] = AP[0] * A[0] + AP[1] * A[1] + AP[2] * A[2] + AP[3] * A[3];
	P[1] = AP[4] * A[0] + AP[5] * A[1] + AP[6] * A[2] + AP[7] * A[3];
	P[2] = AP[4] * A[4] + AP[5] * A[5] + AP[6] * A[6] + AP[7] * A[7] + AP[8] * A[8];
	P[3] = AP[9] * A[0] + AP[10] * A[1] + AP[11] * A[2] + AP[12] * A[3];
	P[4] = AP[9] * A[4] + AP[10] * A[5] + AP[11] * A[6] + AP[12] * A[7] + AP[13] * A[8];
	P[5] = AP[11] + AP[12] * A[9];
	P[6] = AP[14] * A[0] + P[7] * A[1] + P[8] * A[2] + P[9] * A[3];
	P[7] = AP[14] * A[4] + P[7] * A[5] + P[8] * A[6] + P[9] * A[7] + P[13] * A[8];
	P[8] = P[8] + P[9] * A[9];
	P[10] = AP[15] * A[0] + P[11] * A[1] + P[12] * A[2] + P[13] * A[3];
	P[11] = AP[15] * A[4] + P[11] * A[5] + P[12] * A[6] + P[13] * A[7] + P[14] * A[8];
	P[12] = P[12] + P[13] * A[9];

	P[0] += Q[0];
	P[2] += Q[1];
	P[5] += Q[2];
	P[9] += Q[3];
	P[14] += Q[4];
}

static void
pm_kalman_update(pmc_t *pm, const float X[2])
{
	float		*P = pm->kalman_P;
	float		*K = pm->kalman_K;

	float		CP[5], SI;

	/*
	 * Calculate updated (a posteriori) covariance and Kalman gain.
	 *
	 * S = C * P * C' + R.
	 * K = P * C' / S.
	 * P = P - K * C * P.
	 *
	 * C(1) = [ 1  0  -X[1]  0  0 ]
	 * C(2) = [ 0  1   X[0]  0  0 ]
	 *
	 * */

	CP[0] = P[0] - X[1] * P[3];
	CP[1] = P[1] - X[1] * P[4];
	CP[2] = P[3] - X[1] * P[5];
	CP[3] = P[6] - X[1] * P[8];
	CP[4] = P[10] - X[1] * P[12];

	SI = 1.f / (CP[0] - CP[2] * X[1] + pm->kalman_gain_R);

	K[0] = CP[0] * SI;
	K[2] = CP[1] * SI;
	K[4] = CP[2] * SI;
	K[6] = CP[3] * SI;
	K[8] = CP[4] * SI;

	P[0] += - K[0] * CP[0];
	P[1] += - K[2] * CP[0];
	P[2] += - K[2] * CP[1];
	P[3] += - K[4] * CP[0];
	P[4] += - K[4] * CP[1];
	P[5] += - K[4] * CP[2];
	P[6] += - K[6] * CP[0];
	P[7] += - K[6] * CP[1];
	P[8] += - K[6] * CP[2];
	P[9] += - K[6] * CP[3];
	P[10] += - K[8] * CP[0];
	P[11] += - K[8] * CP[1];
	P[12] += - K[8] * CP[2];
	P[13] += - K[8] * CP[3];
	P[14] += - K[8] * CP[4];

	CP[0] = P[1] + X[0] * P[3];
	CP[1] = P[2] + X[0] * P[4];
	CP[2] = P[4] + X[0] * P[5];
	CP[3] = P[7] + X[0] * P[8];
	CP[4] = P[11] + X[0] * P[12];

	SI = 1.f / (CP[1] + CP[2] * X[0] + pm->kalman_gain_R);

	K[1] = CP[0] * SI;
	K[3] = CP[1] * SI;
	K[5] = CP[2] * SI;
	K[7] = CP[3] * SI;
	K[9] = CP[4] * SI;

	P[0] += - K[1] * CP[0];
	P[1] += - K[3] * CP[0];
	P[2] += - K[3] * CP[1];
	P[3] += - K[5] * CP[0];
	P[4] += - K[5] * CP[1];
	P[5] += - K[5] * CP[2];
	P[6] += - K[7] * CP[0];
	P[7] += - K[7] * CP[1];
	P[8] += - K[7] * CP[2];
	P[9] += - K[7] * CP[3];
	P[10] += - K[9] * CP[0];
	P[11] += - K[9] * CP[1];
	P[12] += - K[9] * CP[2];
	P[13] += - K[9] * CP[3];
	P[14] += - K[9] * CP[4];
}

static void
pm_kalman_lockout_guard(pmc_t *pm, float A)
{
	float		thld_wS, bCOS;
	int		flux_RESET = PM_DISABLED;

	/* Bare speed estiamte filtered.
	 * */
	pm->kalman_lpf_wS += (A * pm->m_freq - pm->kalman_lpf_wS) * pm->zone_gain_LP;

	if (		   pm->flux_ZONE == PM_ZONE_NONE
			|| pm->flux_ZONE == PM_ZONE_UNCERTAIN) {

		thld_wS = pm->zone_threshold * pm->zone_gain_TH;

		if (		pm->kalman_lpf_wS < - thld_wS
				&& pm->flux_wS > thld_wS) {

			flux_RESET = PM_ENABLED;
		}
		else if (	pm->kalman_lpf_wS > thld_wS
				&& pm->flux_wS < - thld_wS) {

			flux_RESET = PM_ENABLED;
		}

		bCOS = pm->lu_F[0] * pm->flux_F[0] + pm->lu_F[1] * pm->flux_F[1];

		if (bCOS < 0.f) {

			flux_RESET = PM_ENABLED;
		}

		if (flux_RESET == PM_ENABLED) {

			/* Restart Kalman and flip DQ-axes.
			 * */
			pm->flux_TYPE = PM_FLUX_NONE;

			pm->flux_F[0] = - pm->flux_F[0];
			pm->flux_F[1] = - pm->flux_F[1];
			pm->flux_wS = pm->kalman_lpf_wS;

			pm->kalman_POSTPONED = PM_DISABLED;
		}
	}
}

static void
pm_flux_kalman(pmc_t *pm)
{
	const float		*K = pm->kalman_K;
	float			eD, eQ, A, bF[2];

	if (PM_CONFIG_TVM(pm) == PM_ENABLED) {

		pm_kalman_solve_tvm(pm, pm->flux_X, pm->flux_F);
	}

	/* DQ-axes frame.
	 * */
	bF[0] = pm->flux_F[0];
	bF[1] = pm->flux_F[1];

	/* Get the current RESIDUE in DQ-axes.
	 * */
	eD = bF[0] * pm->lu_iX + bF[1] * pm->lu_iY - pm->flux_X[0];
	eQ = bF[0] * pm->lu_iY - bF[1] * pm->lu_iX - pm->flux_X[1];

	if (pm->vsi_IF == 0) {

		pm->flux_X[0] += K[0] * eD + K[1] * eQ;
		pm->flux_X[1] += K[2] * eD + K[3] * eQ;

		A = K[4] * eD + K[5] * eQ;
		A = (A < - 1.f) ? - 1.f : (A > 1.f) ? 1.f : A;

		m_rotatef(pm->flux_F, A);

		if (pm->flux_LINKAGE == PM_ENABLED) {

			pm->flux_wS += K[6] * eD + K[7] * eQ;

			if (pm->flux_gain_IF > M_EPSILON) {

				A = pm_torque_get_accel(pm);
				pm->flux_wS += A * pm->m_dT * pm->flux_gain_IF;
			}
		}
		else {
			/* Startup estimate borrowing.
			 * */
			pm->flux_wS = pm->lu_wS;
		}

		pm->kalman_bias_Q += K[8] * eD + K[9] * eQ;

		if (		   pm->flux_ZONE == PM_ZONE_NONE
				|| pm->flux_ZONE == PM_ZONE_UNCERTAIN) {

			pm->kalman_bias_Q = (pm->kalman_bias_Q < 0.f)
				? pm->kalman_bias_Q : 0.f;
		}
	}

	pm->kalman_POSTPONED = PM_ENABLED;

	/* Build sparse Jacobian and postpone the rest of work.
	 * */
	pm_kalman_jacobian(pm, pm->flux_X, pm->flux_F, pm->flux_wS);

	/* Time update to the next cycle.
	 * */
	pm_kalman_solve(pm, pm->flux_X, pm->flux_F, pm->flux_wS);

	/* Guard against lockout of DQ-axes in reverse position.
	 * */
	pm_kalman_lockout_guard(pm, bF[0] * pm->flux_F[1] - bF[1] * pm->flux_F[0]);
}

static void
pm_flux_zone(pmc_t *pm)
{
	float			thld_wS;

	/* Get FLUX speed filtered to switch ZONE.
	 * */
	pm->zone_lpf_wS += (pm->flux_wS - pm->zone_lpf_wS) * pm->zone_gain_LP;

	if (		   pm->flux_ZONE == PM_ZONE_NONE
			|| pm->flux_ZONE == PM_ZONE_UNCERTAIN) {

		thld_wS = pm->zone_threshold + pm->zone_noise;

		if (pm->lu_MODE == PM_LU_DETACHED) {

			int	lev_TIM = PM_TSMS(pm, pm->tm_transient_slow);

			if (		m_fabsf(pm->zone_lpf_wS) > thld_wS
					&& pm->detach_TIM > lev_TIM) {

				pm->flux_ZONE = PM_ZONE_HIGH;
			}
		}
		else {
			if (		pm->zone_lpf_wS > thld_wS
					&& pm->lu_wS > thld_wS) {

				pm->flux_ZONE = PM_ZONE_HIGH;
			}
			else if (	pm->zone_lpf_wS < - thld_wS
					&& pm->lu_wS < - thld_wS) {

				pm->flux_ZONE = PM_ZONE_HIGH;
			}
		}
	}
	else if (pm->flux_ZONE == PM_ZONE_HIGH) {

		thld_wS = pm->zone_threshold * pm->zone_gain_TH;

		if (pm->lu_MODE == PM_LU_DETACHED) {

			if (		m_fabsf(pm->zone_lpf_wS) < thld_wS
					|| pm->detach_TIM < 10) {

				pm->flux_ZONE = PM_ZONE_UNCERTAIN;
			}
		}
		else {
			if (m_fabsf(pm->zone_lpf_wS) < thld_wS) {

				pm->flux_ZONE = PM_ZONE_UNCERTAIN;
			}
		}
	}
}

static void
pm_estimate(pmc_t *pm)
{
	if (pm->config_LU_ESTIMATE == PM_FLUX_ORTEGA) {

		if (pm->flux_TYPE != PM_FLUX_ORTEGA) {

			float			E1, L2;

			E1 = pm->const_lambda;
			L2 = pm->const_im_L2;

			pm->flux_X[0] = E1 * pm->flux_F[0] + L2 * pm->lu_iX;
			pm->flux_X[1] = E1 * pm->flux_F[1] + L2 * pm->lu_iY;

			pm->flux_TYPE = PM_FLUX_ORTEGA;
		}

		pm_flux_ortega(pm);
		pm_flux_zone(pm);
	}
	else if (pm->config_LU_ESTIMATE == PM_FLUX_KALMAN) {

		if (pm->flux_TYPE != PM_FLUX_KALMAN) {

			pm->flux_X[0] = pm->lu_iD;
			pm->flux_X[1] = pm->lu_iQ;

			pm->kalman_P[0] = 0.f;
			pm->kalman_P[1] = 0.f;
			pm->kalman_P[2] = 0.f;
			pm->kalman_P[3] = 0.f;
			pm->kalman_P[4] = 0.f;
			pm->kalman_P[5] = 0.f;
			pm->kalman_P[6] = 0.f;
			pm->kalman_P[7] = 0.f;
			pm->kalman_P[8] = 0.f;
			pm->kalman_P[9] = 0.f;
			pm->kalman_P[10] = 0.f;
			pm->kalman_P[11] = 0.f;
			pm->kalman_P[12] = 0.f;
			pm->kalman_P[13] = 0.f;
			pm->kalman_P[14] = 0.f;

			pm->kalman_K[0] = 0.f;
			pm->kalman_K[1] = 0.f;
			pm->kalman_K[2] = 0.f;
			pm->kalman_K[3] = 0.f;
			pm->kalman_K[4] = 0.f;
			pm->kalman_K[5] = 0.f;
			pm->kalman_K[6] = 0.f;
			pm->kalman_K[7] = 0.f;
			pm->kalman_K[8] = 0.f;
			pm->kalman_K[9] = 0.f;

			pm->kalman_bias_Q = 0.f;

			pm->flux_TYPE = PM_FLUX_KALMAN;
		}

		pm_flux_kalman(pm);
		pm_flux_zone(pm);
	}
	else {
		/* NOTE: No sensorless observer selected. It is ok when you
		 * only need a SENSORED drive */

		if (pm->flux_TYPE != PM_FLUX_NONE) {

			pm->flux_TYPE = PM_FLUX_NONE;
		}
	}
}

static void
pm_hfi_wave(pmc_t *pm)
{
	if (pm->config_HFI_WAVETYPE == PM_HFI_SINE) {

		/* HF sine wave synthesis.
		 * */
		m_rotatef(pm->hfi_wave, pm->quick_HFwS * pm->m_dT);
	}
	else if (pm->config_HFI_WAVETYPE == PM_HFI_RANDOM) {

		/* HF random sequence.
		 * */
		if (pm->hfi_wave[1] > M_PI_F) {

			pm->hfi_wave[0] = m_lf_gaussf(&pm->hfi_seed) * .3f;
			pm->hfi_wave[1] += - M_PI_F;
		}

		pm->hfi_wave[1] += pm->quick_HFwS * pm->m_dT;
	}
	else {
		/* No HF wave injection.
		 * */
		pm->hfi_wave[0] = 0.f;
		pm->hfi_wave[1] = 1.f;
	}
}

static void
pm_sensor_hall(pmc_t *pm)
{
	float		F[2], A, B, blend, rel;
	int		HS;

	const float	tol = .6f;		/* ~34 degrees */

	HS = pm->fb_HS;

	if (likely(HS >= 1 && HS <= 6)) {

		pm->hall_ERN = 0;

		F[0] = pm->hall_ST[HS].X;
		F[1] = pm->hall_ST[HS].Y;

		A = F[0] * pm->hall_F[0] + F[1] * pm->hall_F[1];
		B = F[1] * pm->hall_F[0] - F[0] * pm->hall_F[1];

		rel = m_atan2f(B, A);

		if (m_fabsf(rel) > tol) {

			rel += (rel < 0.f) ? tol : - tol;

			m_rotatef(pm->hall_F, rel);

			blend = m_fabsf(pm->hall_wS) * pm->hall_trip_AP;
			blend = (blend > 1.f) ? 1.f : blend;

			A =	  pm->hall_gain_SF * blend
				+ pm->hall_gain_LO * (1.f - blend);

			pm->hall_wS += rel * pm->m_freq * A;
		}

		if (pm->hall_gain_IF > M_EPSILON) {

			A = pm_torque_get_accel(pm);
			pm->hall_wS += A * pm->m_dT * pm->hall_gain_IF;
		}

		m_rotatef(pm->hall_F, pm->hall_wS * pm->m_dT);
	}
	else {
		pm->hall_ERN++;

		if (unlikely(pm->hall_ERN >= 10)) {

			pm->fsm_errno = PM_ERROR_SENSOR_HALL_FAULT;
			pm->fsm_req = PM_STATE_HALT;
		}
	}
}

static void
pm_sensor_eabi(pmc_t *pm)
{
	float		F[2], A, blend, ANG, rel;
	int		relEP, WRAP;

	const float	tol = m_fabsf(pm->quick_ZiEP) * .6f;

	if (pm->eabi_RECENT != PM_ENABLED) {

		pm->eabi_bEP = pm->fb_EP;
		pm->eabi_unwrap = 0;
		pm->eabi_interp = 0.f;

		if (pm->config_EABI_FRONTEND == PM_EABI_INCREMENTAL) {

			pm->eabi_lEP = 0;
		}
		else if (pm->config_EABI_FRONTEND == PM_EABI_ABSOLUTE) {

			pm->eabi_lEP = pm->fb_EP;
		}

		if (pm->config_LU_SENSOR == PM_SENSOR_EABI) {

			pm->eabi_F[0] = pm->lu_F[0];
			pm->eabi_F[1] = pm->lu_F[1];
			pm->eabi_wS = pm->lu_wS;
		}

		pm->eabi_RECENT = PM_ENABLED;
	}

	if (pm->config_EABI_FRONTEND == PM_EABI_INCREMENTAL) {

		WRAP = 0x10000;

		relEP = pm->fb_EP - pm->eabi_bEP;
		relEP +=  unlikely(relEP > WRAP / 2 - 1) ? - WRAP
			: unlikely(relEP < - WRAP / 2) ? WRAP : 0;

		pm->eabi_bEP = pm->fb_EP;
	}
	else if (pm->config_EABI_FRONTEND == PM_EABI_ABSOLUTE) {

		WRAP = pm->eabi_const_EP;

		pm->eabi_bEP = pm->eabi_lEP - (pm->eabi_lEP / WRAP) * WRAP;
		pm->eabi_bEP += (pm->eabi_bEP < 0) ? WRAP : 0;

		relEP = pm->fb_EP - pm->eabi_bEP;
		relEP +=  unlikely(relEP > WRAP / 2 - 1) ? - WRAP
			: unlikely(relEP < - WRAP / 2) ? WRAP : 0;
	}
	else {
		relEP = 0;
	}

	if (relEP != 0) {

		pm->eabi_lEP += relEP;
		pm->eabi_interp += - (float) relEP * pm->quick_ZiEP;

		WRAP = pm->eabi_const_EP * pm->eabi_const_Zq;

		if (pm->eabi_lEP < - WRAP) {

			pm->eabi_unwrap += - pm->eabi_const_Zq;
			pm->eabi_lEP += WRAP;
		}
		else if (pm->eabi_lEP > WRAP) {

			pm->eabi_unwrap += pm->eabi_const_Zq;
			pm->eabi_lEP += - WRAP;
		}
	}

	/* We get residual when interp goes out of tolerance.
	 * */
	rel = (pm->eabi_interp > tol) ? tol - pm->eabi_interp
		: (pm->eabi_interp < - tol) ? - tol - pm->eabi_interp : 0.f;

	pm->eabi_interp += rel;

	blend = m_fabsf(pm->eabi_wS) * pm->eabi_trip_AP;
	blend = (blend > 1.f) ? 1.f : blend;

	A =	  pm->eabi_gain_SF * blend
		+ pm->eabi_gain_LO * (1.f - blend);

	pm->eabi_wS += rel * pm->m_freq * A;

	if (pm->eabi_gain_IF > M_EPSILON) {

		A = pm_torque_get_accel(pm);
		pm->eabi_wS += A * pm->m_dT * pm->eabi_gain_IF;
	}

	pm->eabi_interp += pm->eabi_wS * pm->m_dT;

	if (pm->config_LU_SENSOR == PM_SENSOR_EABI) {

		/* Take the electrical position DQ-axes.
		 * */
		ANG = (float) pm->eabi_lEP * pm->quick_ZiEP + pm->eabi_interp;

		ANG = m_wrapf(ANG);

		F[0] = m_cosf(ANG);
		F[1] = m_sinf(ANG);

		if (pm->eabi_ADJUST != PM_ENABLED) {

			pm->eabi_F0[0] = F[0] * pm->lu_F[0] + F[1] * pm->lu_F[1];
			pm->eabi_F0[1] = F[0] * pm->lu_F[1] - F[1] * pm->lu_F[0];

			pm->eabi_ADJUST = PM_ENABLED;
		}

		pm->eabi_F[0] = F[0] * pm->eabi_F0[0] - F[1] * pm->eabi_F0[1];
		pm->eabi_F[1] = F[1] * pm->eabi_F0[0] + F[0] * pm->eabi_F0[1];
	}

	if (pm->config_LU_LOCATION == PM_LOCATION_EABI) {

		/* Take the electrical absolute LOCATION.
		 * */
		ANG = (float) pm->eabi_lEP + (float) pm->eabi_const_EP * pm->eabi_unwrap;

		pm->eabi_location = ANG * pm->quick_ZiEP + pm->eabi_interp;
	}
}

static void
pm_sensor_sincos(pmc_t *pm)
{
	float			*FIR = pm->sincos_FIR;
	float			scAN, locAN;
	int			WRAP;

	if (pm->sincos_RECENT != PM_ENABLED) {

		pm->sincos_RECENT = PM_ENABLED;
	}

	if (pm->config_SINCOS_FRONTEND == PM_SINCOS_ANALOG) {

		float		Q[9];

		Q[0] = pm->fb_COS;
		Q[1] = pm->fb_SIN;
		Q[2] = Q[0] * Q[1];
		Q[3] = Q[0] * Q[0];
		Q[4] = Q[1] * Q[1];
		Q[5] = Q[3] * Q[1];
		Q[6] = Q[4] * Q[0];
		Q[7] = Q[3] * Q[0];
		Q[8] = Q[4] * Q[1];

		pm->sincos_SC[0] = FIR[0] + FIR[2] * Q[0] + FIR[4]  * Q[1]
			+ FIR[6]  * Q[2] + FIR[8]  * Q[3] + FIR[10] * Q[4]
			+ FIR[12] * Q[5] + FIR[14] * Q[6] + FIR[16] * Q[7]
			+ FIR[18] * Q[8];

		pm->sincos_SC[0] = FIR[1] + FIR[3] * Q[0] + FIR[5]  * Q[1]
			+ FIR[7]  * Q[2] + FIR[9]  * Q[3] + FIR[11] * Q[4]
			+ FIR[13] * Q[5] + FIR[15] * Q[6] + FIR[17] * Q[7]
			+ FIR[19] * Q[8];
	}
	else if (pm->config_SINCOS_FRONTEND == PM_SINCOS_RESOLVER) {

		/* TODO */
	}

	/* Track the SIN/COS position.
	 * */
	if (pm->sincos_SC[0] < 0.f) {

		if (		   pm->sincos_SC[1] < 0.f
				&& pm->sincos_SC[2] >= 0.f) {

			pm->sincos_revol += 1;
		}
		else if (	   pm->sincos_SC[1] >= 0.f
				&& pm->sincos_SC[2] < 0.f) {

			pm->sincos_revol += - 1;
		}
	}

	pm->sincos_SC[2] = pm->sincos_SC[1];

	WRAP = pm->sincos_const_Zq;

	if (pm->sincos_revol < - WRAP) {

		pm->sincos_unwrap += - WRAP;
		pm->sincos_revol += WRAP;
	}
	else if (pm->sincos_revol > WRAP) {

		pm->sincos_unwrap += WRAP;
		pm->sincos_revol += - WRAP;
	}

	scAN = m_atan2f(pm->sincos_SC[1], pm->sincos_SC[0])
		+ (float) pm->sincos_revol * M_2_PI_F;

	if (pm->config_LU_LOCATION == PM_LOCATION_SINCOS) {

		float		scLOC;

		scLOC = scAN + (float) pm->sincos_unwrap * M_2_PI_F;

		/* Take the electrical absolute LOCATION.
		 * */
		pm->sincos_location = scLOC * pm->quick_ZiSQ;
	}

	/* Take the electrical position.
	 * */
	locAN = m_wrapf(scAN * pm->quick_ZiSQ);

	pm->sincos_F[0] = m_cosf(locAN);
	pm->sincos_F[1] = m_sinf(locAN);

	/* TODO */
}

static void
pm_lu_FSM(pmc_t *pm)
{
	float			lu_F[2], hS, A, B;

	int			lu_EABI = PM_DISABLED;

	/* Get the current on DQ-axes.
	 * */
	pm->lu_iD = pm->lu_F[0] * pm->lu_iX + pm->lu_F[1] * pm->lu_iY;
	pm->lu_iQ = pm->lu_F[0] * pm->lu_iY - pm->lu_F[1] * pm->lu_iX;

	/* Get TVM voltage on DQ-axes.
	 * */
	pm->lu_uD = pm->lu_F[0] * pm->tvm_X0 + pm->lu_F[1] * pm->tvm_Y0;
	pm->lu_uQ = pm->lu_F[0] * pm->tvm_Y0 - pm->lu_F[1] * pm->tvm_X0;

	A = pm->lu_wS * pm->m_dT * 0.5f;

	/* Approximate to the middle of past cycle.
	 * */
	pm->lu_uD += - pm->lu_uQ * A;
	pm->lu_uQ += pm->lu_uD * A;

	/* Transfer to the next apriori position.
	 * */
	m_rotatef(pm->lu_F, pm->lu_wS * pm->m_dT);

	if (pm->vsi_IF != 0) {

		/* We transform DQ-axes current back to XY-axes throught
		 * apriori DQ-axes if there are no clean measurements available.
		 * */
		pm->lu_iX = pm->lu_F[0] * pm->lu_iD - pm->lu_F[1] * pm->lu_iQ;
		pm->lu_iY = pm->lu_F[1] * pm->lu_iD + pm->lu_F[0] * pm->lu_iQ;
	}

	if (pm->lu_MODE == PM_LU_DETACHED) {

		if (pm->base_TIM >= 0) {

			pm_flux_detached(pm);
		}

		if (pm->config_LU_ESTIMATE != PM_FLUX_NONE) {

			pm_flux_zone(pm);
		}

		lu_F[0] = pm->flux_F[0];
		lu_F[1] = pm->flux_F[1];

		pm->lu_wS = pm->flux_wS;

		if (pm->flux_ZONE == PM_ZONE_LOCKED_IN_DETACH) {

			/* Lock in detached mode permanently.
			 * */
		}
		else if (pm->base_TIM < PM_TSMS(pm, pm->tm_pause_startup)) {

			/* Not enough time passed to go into control loop.
			 * */
			pm->base_TIM++;
		}
		else if (	pm->config_LU_ESTIMATE != PM_FLUX_NONE
				&& pm->flux_ZONE == PM_ZONE_HIGH) {

			pm->lu_MODE = PM_LU_ESTIMATE;

			pm->proc_set_Z(PM_Z_NONE);
		}
		else if (pm->config_LU_SENSOR == PM_SENSOR_HALL) {

			pm->lu_MODE = PM_LU_SENSOR_HALL;

			pm->hall_ERN = 0;
			pm->hall_F[0] = pm->lu_F[0];
			pm->hall_F[1] = pm->lu_F[1];
			pm->hall_wS = pm->lu_wS;

			pm->proc_set_Z(PM_Z_NONE);
		}
		else if (	pm->config_LU_SENSOR == PM_SENSOR_EABI
				&& (	pm->eabi_ADJUST  == PM_ENABLED
					|| pm->flux_ZONE == PM_ZONE_HIGH)) {

			pm->lu_MODE = PM_LU_SENSOR_EABI;

			pm->proc_set_Z(PM_Z_NONE);
		}
		else if (pm->config_LU_FORCED == PM_ENABLED) {

			pm->lu_MODE = PM_LU_FORCED;

			pm->hold_TIM = 0;

			pm->forced_F[0] = pm->lu_F[0];
			pm->forced_F[1] = pm->lu_F[1];
			pm->forced_wS = pm->lu_wS;

			pm->proc_set_Z(PM_Z_NONE);
		}
		else if (       pm->config_LU_ESTIMATE == PM_FLUX_KALMAN
				&& pm->config_HFI_WAVETYPE != PM_HFI_NONE) {

			pm->lu_MODE = PM_LU_ON_HFI;

			pm->hold_TIM = 0;

			pm->proc_set_Z(PM_Z_NONE);
		}
	}
	else if (pm->lu_MODE == PM_LU_FORCED) {

		pm_estimate(pm);
		pm_forced(pm);

		lu_F[0] = pm->forced_F[0];
		lu_F[1] = pm->forced_F[1];

		pm->lu_wS = pm->forced_wS;

		if (		pm->flux_TYPE != PM_FLUX_NONE
				&& pm->flux_LINKAGE != PM_ENABLED) {

			/* Hold on until flux linkage is estimated.
			 * */
		}
		else if (pm->flux_ZONE == PM_ZONE_HIGH) {

			pm->lu_MODE = PM_LU_ESTIMATE;
		}
		else {
			if (pm->hold_TIM < PM_TSMS(pm, pm->tm_pause_forced)) {

				pm->hold_TIM++;
			}
			else if (pm->config_LU_SENSOR == PM_SENSOR_EABI) {

				pm->lu_MODE = PM_LU_SENSOR_EABI;
			}
			else if (	pm->config_LU_ESTIMATE == PM_FLUX_KALMAN
					&& pm->config_HFI_WAVETYPE != PM_HFI_NONE) {

				pm->lu_MODE = PM_LU_ON_HFI;
			}
		}
	}
	else if (pm->lu_MODE == PM_LU_ESTIMATE) {

		pm_estimate(pm);

		lu_F[0] = pm->flux_F[0];
		lu_F[1] = pm->flux_F[1];

		pm->lu_wS = pm->flux_wS;

		if (pm->base_TIM < PM_TSMS(pm, pm->tm_pause_startup)) {

			/* Not enough time passed to go into low speed mode.
			 * */
			pm->base_TIM++;
		}
		else if (	   pm->flux_ZONE == PM_ZONE_NONE
				|| pm->flux_ZONE == PM_ZONE_UNCERTAIN) {

			if (pm->config_LU_SENSOR == PM_SENSOR_HALL) {

				pm->lu_MODE = PM_LU_SENSOR_HALL;

				pm->hall_ERN = 0;
				pm->hall_F[0] = pm->lu_F[0];
				pm->hall_F[1] = pm->lu_F[1];
				pm->hall_wS = pm->lu_wS;
			}
			else if (	pm->config_LU_SENSOR == PM_SENSOR_EABI
					&& (	pm->eabi_ADJUST  == PM_ENABLED
						|| pm->flux_ZONE == PM_ZONE_UNCERTAIN)) {

				pm->lu_MODE = PM_LU_SENSOR_EABI;
			}
			else if (	pm->config_LU_ESTIMATE == PM_FLUX_KALMAN
					&& pm->config_HFI_WAVETYPE != PM_HFI_NONE) {

				pm->lu_MODE = PM_LU_ON_HFI;

				pm->hold_TIM = 0;
			}
			else if (pm->config_LU_FORCED == PM_ENABLED) {

				pm->lu_MODE = PM_LU_FORCED;

				pm->hold_TIM = 0;

				pm->forced_F[0] = pm->lu_F[0];
				pm->forced_F[1] = pm->lu_F[1];
				pm->forced_wS = pm->lu_wS;
			}
			else if (PM_CONFIG_TVM(pm) == PM_ENABLED) {

				pm->lu_MODE = PM_LU_DETACHED;

				pm->base_TIM = - PM_TSMS(pm, pm->tm_transient_fast);
				pm->detach_TIM = 0;

				pm->flux_TYPE = PM_FLUX_NONE;

				pm->lu_uD = 0.f;
				pm->lu_uQ = 0.f;

				pm->watt_lpf_D = 0.f;
				pm->watt_lpf_Q = 0.f;
				pm->watt_drain_wP = 0.f;
				pm->watt_drain_wA = 0.f;

				pm->i_track_D = 0.f;
				pm->i_track_Q = 0.f;
				pm->i_integral_D = 0.f;
				pm->i_integral_Q = 0.f;

				pm->v_integral = 0.f;

				pm->proc_set_Z(PM_Z_ABC);
			}
		}
	}
	else if (pm->lu_MODE == PM_LU_ON_HFI) {

		pm_estimate(pm);

		lu_F[0] = pm->flux_F[0];
		lu_F[1] = pm->flux_F[1];

		pm->lu_wS = pm->flux_wS;

		if (		pm->flux_ZONE == PM_ZONE_HIGH
				|| pm->config_HFI_WAVETYPE == PM_HFI_NONE) {

			pm->lu_MODE = PM_LU_ESTIMATE;
		}
		else {
			if (pm->hold_TIM < PM_TSMS(pm, pm->tm_pause_startup)) {

				pm->hold_TIM++;
			}
			else if (pm->config_LU_SENSOR == PM_SENSOR_EABI) {

				pm->lu_MODE = PM_LU_SENSOR_EABI;
			}
		}
	}
	else if (pm->lu_MODE == PM_LU_SENSOR_HALL) {

		pm_estimate(pm);
		pm_sensor_hall(pm);

		lu_F[0] = pm->hall_F[0];
		lu_F[1] = pm->hall_F[1];

		pm->lu_wS = pm->hall_wS;

		if (pm->flux_ZONE == PM_ZONE_HIGH) {

			pm->lu_MODE = PM_LU_ESTIMATE;
		}
	}
	else if (pm->lu_MODE == PM_LU_SENSOR_EABI) {

		pm_estimate(pm);
		pm_sensor_eabi(pm);

		lu_F[0] = pm->eabi_F[0];
		lu_F[1] = pm->eabi_F[1];

		lu_EABI = PM_ENABLED;

		pm->lu_wS = pm->eabi_wS;

		if (pm->flux_ZONE == PM_ZONE_HIGH) {

			pm->lu_MODE = PM_LU_ESTIMATE;
		}
	}
	else if (pm->lu_MODE == PM_LU_SENSOR_SINCOS) {

		pm_estimate(pm);
		pm_sensor_sincos(pm);

		/* TODO */
	}
	else {
		lu_F[0] = pm->lu_F[0];
		lu_F[1] = pm->lu_F[1];
	}

	/* Take the LU position estimate with TRANSIENT rate limited.
	 * */
	hS = pm->lu_rate * pm->m_dT;

	A = lu_F[0] * pm->lu_F[0] + lu_F[1] * pm->lu_F[1];
	B = lu_F[1] * pm->lu_F[0] - lu_F[0] * pm->lu_F[1];

	if (A > M_EPSILON && B < - hS) {

		m_rotatef(pm->lu_F, - hS);
	}
	else if (A > M_EPSILON && B > hS) {

		m_rotatef(pm->lu_F, hS);
	}
	else {
		pm->lu_F[0] = lu_F[0];
		pm->lu_F[1] = lu_F[1];

		if (A < M_EPSILON) {

			/* NOTE: We reset current loop integrals in
			 * case of position flip.
			 * */
			pm->i_integral_D = 0.f;
			pm->i_integral_Q = 0.f;
		}
	}

	/* Track the position to get full number of revolutions.
	 * */
	if (pm->lu_F[0] < 0.f) {

		if (		   pm->lu_F[1] < 0.f
				&& pm->lu_F[2] >= 0.f) {

			pm->lu_revol += 1;
		}
		else if (	   pm->lu_F[1] >= 0.f
				&& pm->lu_F[2] < 0.f) {

			pm->lu_revol += - 1;
		}
	}

	pm->lu_F[2] = pm->lu_F[1];

	if (pm->lu_revol - pm->lu_revob < - 4) {

		pm->lu_total_revol += pm->lu_revob - pm->lu_revol;
		pm->lu_revob = pm->lu_revol;
	}
	else if (pm->lu_revol - pm->lu_revob > 4) {

		pm->lu_total_revol += pm->lu_revol - pm->lu_revob;
		pm->lu_revob = pm->lu_revol;
	}

	/* Take the LOCATION according to the configuration.
	 * */
	if (pm->config_LU_LOCATION == PM_LOCATION_INHERITED) {

		pm->lu_location = m_atan2f(pm->lu_F[1], pm->lu_F[0])
			+ (float) pm->lu_revol * M_2_PI_F;
	}
	else if (pm->config_LU_LOCATION == PM_LOCATION_EABI) {

		if (lu_EABI != PM_ENABLED) {

			pm_sensor_eabi(pm);

			lu_EABI = PM_ENABLED;
		}

		pm->lu_wS = pm->eabi_wS;
		pm->lu_location = pm->eabi_location;
	}
	else if (pm->config_LU_LOCATION == PM_LOCATION_SINCOS) {

		/* TODO */
	}

	if (		pm->eabi_RECENT == PM_ENABLED
			&& lu_EABI != PM_ENABLED) {

		pm->eabi_RECENT = PM_DISABLED;

		if (pm->config_EABI_FRONTEND == PM_EABI_INCREMENTAL) {

			pm->eabi_ADJUST = PM_DISABLED;
		}
	}

	if (		pm->flux_TYPE == PM_FLUX_KALMAN
			&& pm->lu_MODE == PM_LU_ESTIMATE) {

		/* Replace the current on DQ-axes with predicted one.
		 * */
		pm->lu_iD = pm->flux_X[0];
		pm->lu_iQ = pm->flux_X[1];
	}

	if (pm->lu_MODE == PM_LU_FORCED) {

		pm->lu_mq_produce = pm_torque_feasible(pm, pm->lu_iD);
		pm->lu_mq_load = 0.f;
	}
	else {
		float		mQ;

		pm->lu_mq_produce = pm_torque_equation(pm, pm->lu_iD, pm->lu_iQ);

		/* Get an external mechanical LOAD torque estimate.
		 * */
		mQ = pm->lu_mq_produce - (pm->lu_wS - pm->lu_wS0) * pm->m_freq * pm->const_Ja;
		pm->lu_mq_load += (mQ - pm->lu_mq_load) * pm->lu_gain_mq_LP;
	}

	pm->lu_wS0 = pm->lu_wS;

	if (		pm->config_LU_DRIVE == PM_DRIVE_CURRENT
			&& pm->config_SPEED_MAXIMAL == PM_ENABLED) {

		pm->l_track_TIM++;
	}
}

void pm_clearance(pmc_t *pm, int xA, int xB, int xC)
{
	int		xZONE, xSKIP, xMIN, xTOP;

	xZONE = pm->dc_resolution - pm->ts_clearance;
	xSKIP = pm->dc_resolution - pm->ts_skip;

	xTOP = pm->dc_resolution;

	/* Check if there are PWM edges within clearance zone. The CURRENT
	 * measurements will be used or rejected based on this flags.
	 *
	 * NOTE: In case of inline current sensors placement we can sometimes
	 * clamp voltage to the TOP level to get more valid samples.
	 *
	 * NOTE: To get the best result you should have a current sensor with a
	 * fast transient that allows you to specify narrow clearance zone.
	 *
	 * */
	if (PM_CONFIG_IFB(pm) == PM_IFB_AB_INLINE) {

		pm->vsi_AF = (pm->vsi_AQ < xZONE || pm->vsi_AQ == xTOP) ? 0 : 1;
		pm->vsi_BF = (pm->vsi_BQ < xZONE || pm->vsi_BQ == xTOP) ? 0 : 1;
		pm->vsi_CF = 1;
	}
	else if (PM_CONFIG_IFB(pm) == PM_IFB_AB_GND) {

		pm->vsi_AF = (pm->vsi_AQ < xZONE) ? 0 : 1;
		pm->vsi_BF = (pm->vsi_BQ < xZONE) ? 0 : 1;
		pm->vsi_CF = 1;
	}
	else if (PM_CONFIG_IFB(pm) == PM_IFB_ABC_INLINE) {

		pm->vsi_AF = (pm->vsi_AQ < xZONE || pm->vsi_AQ == xTOP) ? 0 : 1;
		pm->vsi_BF = (pm->vsi_BQ < xZONE || pm->vsi_BQ == xTOP) ? 0 : 1;
		pm->vsi_CF = (pm->vsi_CQ < xZONE || pm->vsi_CQ == xTOP) ? 0 : 1;
	}
	else if (PM_CONFIG_IFB(pm) == PM_IFB_ABC_GND) {

		pm->vsi_AF = (pm->vsi_AQ < xZONE) ? 0 : 1;
		pm->vsi_BF = (pm->vsi_BQ < xZONE) ? 0 : 1;
		pm->vsi_CF = (pm->vsi_CQ < xZONE) ? 0 : 1;
	}

	/* NOTE: You can mask a specific current measurement channel on your
	 * own considerations.
	 * */
	pm->vsi_AF = (pm->vsi_mask_XF == PM_MASK_A) ? 1 : pm->vsi_AF;
	pm->vsi_BF = (pm->vsi_mask_XF == PM_MASK_B) ? 1 : pm->vsi_BF;
	pm->vsi_CF = (pm->vsi_mask_XF == PM_MASK_C) ? 1 : pm->vsi_CF;

	/* Chech if at least TWO samples are clean so they can be used in
	 * control loops.
	 * */
	pm->vsi_IF = likely(pm->vsi_AF + pm->vsi_BF + pm->vsi_CF < 2) ? 0 : 1;

	/* Check if there are PWM edges within clearance zone. The DC link
	 * voltage measurement will be used or rejected based on this flag.
	 * */
	pm->vsi_SF = (	   ((pm->vsi_AQ < xSKIP && xA < xSKIP)
				|| (pm->vsi_AQ == xTOP && xA == xTOP))
			&& ((pm->vsi_BQ < xSKIP && xB < xSKIP)
				|| (pm->vsi_BQ == xTOP && xB == xTOP))
			&& ((pm->vsi_CQ < xSKIP && xC < xSKIP)
				|| (pm->vsi_CQ == xTOP && xC == xTOP))) ? 0 : 1;

	if (		PM_CONFIG_TVM(pm) == PM_ENABLED
			&& pm->tvm_ACTIVE == PM_ENABLED) {

		xMIN = (int) (pm->dc_resolution * pm->tvm_clean_zone);

		/* Check if terminal voltages were sampled within acceptable
		 * zone. The VOLTAGE measurement will be used or rejected based
		 * on these flags.
		 * */
		pm->vsi_UF = (	   pm->vsi_AQ < xMIN
				&& pm->vsi_BQ < xMIN
				&& pm->vsi_CQ < xMIN
				&& xA < xMIN
				&& xB < xMIN
				&& xC < xMIN) ? 0 : 1;

		/* Check if terminal voltages are exactly ZERO to get more
		 * accuracy.
		 * */
		pm->vsi_AZ = (pm->vsi_AQ == 0) ? 0 : 1;
		pm->vsi_BZ = (pm->vsi_BQ == 0) ? 0 : 1;
		pm->vsi_CZ = (pm->vsi_CQ == 0) ? 0 : 1;
	}
	else {
		pm->vsi_UF = 1;
	}

	pm->vsi_AQ = xA;
	pm->vsi_BQ = xB;
	pm->vsi_CQ = xC;
}

void pm_voltage(pmc_t *pm, float uX, float uY)
{
	float		uA, uB, uC, uMIN, uMAX, uDC;
	int		xA, xB, xC, xMIN, xMAX, nZONE;

	uX *= pm->quick_iUDC;
	uY *= pm->quick_iUDC;

	uDC = m_sqrtf(uX * uX + uY * uY);

	pm->vsi_DC = uDC / pm->k_EMAX;
	pm->vsi_lpf_DC += (pm->vsi_DC - pm->vsi_lpf_DC) * pm->vsi_gain_LP;

	if (		pm->config_VSI_CLAMP == PM_ENABLED
			&& uDC > pm->k_EMAX) {

		/* CLAMP voltage along an inscribed circle.
		 * */
		uDC = pm->k_EMAX / uDC;

		uX *= uDC;
		uY *= uDC;
	}

	if (PM_CONFIG_NOP(pm) == PM_NOP_THREE_PHASE) {

		uA = uX;
		uB = - 0.5f * uX + 0.8660254f * uY;
		uC = - 0.5f * uX - 0.8660254f * uY;
	}
	else {
		uA = uX;
		uB = uY;
		uC = 0.f;
	}

	if (uA < uB) {

		uMIN = (uC < uA) ? uC : uA;
		uMAX = (uB > uC) ? uB : uC;
	}
	else {
		uMIN = (uC < uB) ? uC : uB;
		uMAX = (uA > uC) ? uA : uC;
	}

	uDC = uMAX - uMIN;

	if (uDC > 1.f) {

		/* CLAMP voltage along an hexagon sides.
		 * */
		uDC = 1.f / uDC;

		uA *= uDC;
		uB *= uDC;
		uC *= uDC;

		uMIN *= uDC;
		uMAX *= uDC;
	}

	if (pm->config_VSI_ZERO == PM_VSI_GND) {

		uDC = 0.f - uMIN;
	}
	else if (pm->config_VSI_ZERO == PM_VSI_CENTER) {

		uDC = 0.5f - (uMAX + uMIN) * 0.5f;
	}
	else if (pm->config_VSI_ZERO == PM_VSI_EXTREME) {

		float	bA, bB, bC, bMIN, bMAX;

		if (PM_CONFIG_NOP(pm) == PM_NOP_THREE_PHASE) {

			bA = m_fabsf(pm->lu_iX);
			bB = m_fabsf(0.5f * pm->lu_iX - 0.8660254f * pm->lu_iY);
			bC = m_fabsf(0.5f * pm->lu_iX + 0.8660254f * pm->lu_iY);
		}
		else {
			bA = m_fabsf(pm->lu_iX);
			bB = m_fabsf(pm->lu_iY);
			bC = 0.f;
		}

		if (uA < uB) {

			bMIN = (uC < uA) ? bC : bA;
			bMAX = (uB > uC) ? bB : bC;
		}
		else {
			bMIN = (uC < uB) ? bC : bB;
			bMAX = (uA > uC) ? bA : bC;
		}

		bA = pm->fault_current_tol;

		bA = (		   pm->vsi_AQ < pm->dc_resolution
				&& pm->vsi_BQ < pm->dc_resolution
				&& pm->vsi_CQ < pm->dc_resolution) ? bA : 0.f;

		uDC = (bMIN + bA < bMAX) ? 1.f - uMAX : 0.f - uMIN;
	}
	else {
		uDC = 0.f;
	}

	uA += uDC;
	uB += uDC;
	uC += uDC;

	xA = (int) (pm->dc_resolution * uA);
	xB = (int) (pm->dc_resolution * uB);
	xC = (int) (pm->dc_resolution * uC);

	if (pm->lu_MODE != PM_LU_DISABLED) {

		if (PM_CONFIG_IFB(pm) == PM_IFB_AB_INLINE) {

			xMAX = pm->dc_resolution - pm->ts_minimal;
			xMIN = pm->dc_resolution - pm->ts_clearance;

			nZONE  =  ((xA < xMIN || xA > xMAX) ? 1 : 0)
				+ ((xB < xMIN || xB > xMAX) ? 1 : 0);

			if (nZONE < 2) {

				xMIN = (xA < xB) ? (xC < xA) ? xC : xA
					: (xC < xB) ? xC : xB;

				if (xMIN > 0) {

					/* Forced clamp to GND.
					 * */
					xA -= xMIN;
					xB -= xMIN;
					xC -= xMIN;
				}
			}

			xMAX = pm->dc_resolution - pm->ts_minimal;
			xMIN = pm->dc_resolution - pm->ts_clearance;

			nZONE  =  ((xA < xMIN || xA > xMAX) ? 1 : 0)
				+ ((xB < xMIN || xB > xMAX) ? 1 : 0);

			if (nZONE < 2) {

				xMAX = (xA > xB) ? (xC > xA) ? xC : xA
					: (xC > xB) ? xC : xB;

				if (xMAX < pm->dc_resolution) {

					/* Forced clamp to TOP.
					 * */
					xA += pm->dc_resolution - xMAX;
					xB += pm->dc_resolution - xMAX;
					xC += pm->dc_resolution - xMAX;
				}
			}

			xMAX = pm->dc_resolution - pm->ts_minimal;
			xMIN = pm->ts_minimal;

			xA = (xA < xMIN) ? 0 : (xA > xMAX) ? pm->dc_resolution : xA;
			xB = (xB < xMIN) ? 0 : (xB > xMAX) ? pm->dc_resolution : xB;
			xC = (xC < xMIN) ? 0 : (xC > xMAX) ? pm->dc_resolution : xC;
		}
		else if (PM_CONFIG_IFB(pm) == PM_IFB_AB_GND) {

			xMAX = pm->dc_resolution - pm->ts_clearance;

			if (xA >= xMAX || xB >= xMAX) {

				xMIN = (xA < xB) ? (xC < xA) ? xC : xA
					: (xC < xB) ? xC : xB;

				if (xMIN > 0) {

					/* Forced clamp to GND.
					 * */
					xA -= xMIN;
					xB -= xMIN;
					xC -= xMIN;
				}
			}

			xMAX = pm->dc_resolution - (pm->ts_clearance + 1);
			xMIN = pm->ts_minimal;

			xA = (xA < xMIN) ? 0 : (xA > xMAX) ? xMAX : xA;
			xB = (xB < xMIN) ? 0 : (xB > xMAX) ? xMAX : xB;
			xC = (xC < xMIN) ? 0 : (xC > xMAX) ? xMAX : xC;
		}
		else if (PM_CONFIG_IFB(pm) == PM_IFB_ABC_INLINE) {

			xMAX = pm->dc_resolution - pm->ts_minimal;
			xMIN = pm->dc_resolution - pm->ts_clearance;

			nZONE  =  ((xA < xMIN || xA > xMAX) ? 1 : 0)
				+ ((xB < xMIN || xB > xMAX) ? 1 : 0)
				+ ((xC < xMIN || xC > xMAX) ? 1 : 0);

			if (nZONE < 2) {

				xMIN = (xA < xB) ? (xC < xA) ? xC : xA
					: (xC < xB) ? xC : xB;

				if (xMIN > 0) {

					/* Forced clamp to GND.
					 * */
					xA -= xMIN;
					xB -= xMIN;
					xC -= xMIN;
				}
			}

			xMAX = pm->dc_resolution - pm->ts_minimal;
			xMIN = pm->dc_resolution - pm->ts_clearance;

			nZONE  =  ((xA < xMIN || xA > xMAX) ? 1 : 0)
				+ ((xB < xMIN || xB > xMAX) ? 1 : 0)
				+ ((xC < xMIN || xC > xMAX) ? 1 : 0);

			if (nZONE < 2) {

				xMAX = (xA > xB) ? (xC > xA) ? xC : xA
					: (xC > xB) ? xC : xB;

				if (xMAX < pm->dc_resolution) {

					/* Forced clamp to TOP.
					 * */
					xA += pm->dc_resolution - xMAX;
					xB += pm->dc_resolution - xMAX;
					xC += pm->dc_resolution - xMAX;
				}
			}

			xMAX = pm->dc_resolution - pm->ts_minimal;
			xMIN = pm->ts_minimal;

			xA = (xA < xMIN) ? 0 : (xA > xMAX) ? pm->dc_resolution : xA;
			xB = (xB < xMIN) ? 0 : (xB > xMAX) ? pm->dc_resolution : xB;
			xC = (xC < xMIN) ? 0 : (xC > xMAX) ? pm->dc_resolution : xC;
		}
		else if (PM_CONFIG_IFB(pm) == PM_IFB_ABC_GND) {

			xMAX = pm->dc_resolution - pm->ts_clearance;

			nZONE  =  ((xA < xMAX) ? 1 : 0)
				+ ((xB < xMAX) ? 1 : 0)
				+ ((xC < xMAX) ? 1 : 0);

			if (nZONE < 2) {

				xMIN = (xA < xB) ? (xC < xA) ? xC : xA
					: (xC < xB) ? xC : xB;

				if (xMIN > 0) {

					/* Forced clamp to GND.
					 * */
					xA -= xMIN;
					xB -= xMIN;
					xC -= xMIN;
				}
			}

			xMAX = pm->dc_resolution - pm->ts_minimal;
			xMIN = pm->ts_minimal;

			xA = (xA < xMIN) ? 0 : (xA > xMAX) ? pm->dc_resolution : xA;
			xB = (xB < xMIN) ? 0 : (xB > xMAX) ? pm->dc_resolution : xB;
			xC = (xC < xMIN) ? 0 : (xC > xMAX) ? pm->dc_resolution : xC;
		}
	}
	else {
		xMIN = (xA < xB) ? (xC < xA) ? xC : xA : (xC < xB) ? xC : xB;

		xA += pm->ts_minimal - xMIN;
		xB += pm->ts_minimal - xMIN;
		xC += pm->ts_minimal - xMIN;

		xMAX = pm->dc_resolution - (pm->ts_clearance + 1);

		xA = (xA > xMAX) ? xMAX : xA;
		xB = (xB > xMAX) ? xMAX : xB;
		xC = (xC > xMAX) ? xMAX : xC;
	}

	if (pm->ts_bootstrap != 0) {

		pm->vsi_AT = (xA == pm->dc_resolution) ? pm->vsi_AT + 1 : 0;
		pm->vsi_BT = (xB == pm->dc_resolution) ? pm->vsi_BT + 1 : 0;
		pm->vsi_CT = (xC == pm->dc_resolution) ? pm->vsi_CT + 1 : 0;

		if (unlikely(		   pm->vsi_AT > pm->ts_bootstrap
					|| pm->vsi_BT > pm->ts_bootstrap
					|| pm->vsi_CT > pm->ts_bootstrap)) {

			pm->fsm_errno = PM_ERROR_BOOTSTRAP_FAULT;
			pm->fsm_state = PM_STATE_HALT;
		}
	}

	/* Output DC values to PWM.
	 * */
	pm->proc_set_DC(xA, xB, xC);

	pm->vsi_X0 = pm->vsi_X;
	pm->vsi_Y0 = pm->vsi_Y;

	if (PM_CONFIG_NOP(pm) == PM_NOP_THREE_PHASE) {

		uDC = 0.33333333f * (xA + xB + xC);

		uA = (xA - uDC) * pm->const_fb_U * pm->ts_inverted;
		uB = (xB - uDC) * pm->const_fb_U * pm->ts_inverted;

		pm->vsi_X = uA;
		pm->vsi_Y = 0.57735027f * uA + 1.1547005f * uB;
	}
	else {
		uA = (xA - xC) * pm->const_fb_U * pm->ts_inverted;
		uB = (xB - xC) * pm->const_fb_U * pm->ts_inverted;

		pm->vsi_X = uA;
		pm->vsi_Y = uB;
	}

	/* Update the measurement clearance flags according to the DC values.
	 * */
	pm_clearance(pm, xA, xB, xC);
}

static float
pm_form_SP(pmc_t *pm, float eSP)
{
	float		iSP;

	/* Basic proportional-integral regulator.
	 * */
	iSP = pm->s_gain_P * eSP + pm->s_integral;

	/* Add an load torque estimate as feed forward term.
	 * */
	iSP += pm->s_gain_Q * pm_torque_get_current(pm, pm->lu_mq_load);

	/* Add derivative term based on the estimated acceleration.
	 * */
	iSP += - pm->s_gain_D * pm_torque_get_accel(pm);

	if (		(iSP < pm->i_maximal || eSP < 0.f)
			&& (iSP > - pm->i_reverse || eSP > 0.f)) {

		pm->s_integral += pm->s_gain_I * eSP;
	}

	return iSP;
}

static void
pm_wattage(pmc_t *pm)
{
	float		wP, TiH, Wh, Ah;

	/* Actual operating POWER is a scalar product of voltage and current.
	 * */
	wP = pm->k_KWAT * (pm->lu_iD * pm->lu_uD + pm->lu_iQ * pm->lu_uQ);

	pm->watt_drain_wP += (wP - pm->watt_drain_wP) * pm->watt_gain_LP;
	pm->watt_drain_wA = pm->watt_drain_wP * pm->quick_iUDC;

	/* Traveled distance.
	 * */
	pm->watt_traveled = (float) pm->lu_total_revol
		* pm->const_ld_S / (float) pm->const_Zp;

	TiH = pm->m_dT * 0.00027777778f;

	/* Get WATT per HOUR.
	 * */
	Wh = pm->watt_drain_wP * TiH;
	Ah = pm->watt_drain_wA * TiH;

	if (likely(Wh > 0.f)) {

		m_rsumf(&pm->watt_consumed_Wh, &pm->watt_rem[0], Wh);
		m_rsumf(&pm->watt_consumed_Ah, &pm->watt_rem[1], Ah);
	}
	else {
		m_rsumf(&pm->watt_reverted_Wh, &pm->watt_rem[2], - Wh);
		m_rsumf(&pm->watt_reverted_Ah, &pm->watt_rem[3], - Ah);
	}

	/* Fuel gauge.
	 * */
	if (pm->watt_capacity_Ah > M_EPSILON) {

		Ah = pm->watt_capacity_Ah - pm->watt_consumed_Ah + pm->watt_reverted_Ah;

		pm->watt_fuel_gauge = 100.f * Ah / pm->watt_capacity_Ah;
	}
}

static void
pm_loop_current(pmc_t *pm)
{
	float		track_D, track_Q, eD, eQ, uD, uQ, uX, uY, wP;
	float		iMAX, iREV, uMAX, uREV, wMAX, wREV, wKEP, dSA;

	if (pm->lu_MODE == PM_LU_FORCED) {

		track_D = pm->forced_hold_D;
	}
	else {
		track_D = 0.f;
	}

	if (		pm->forced_track_D > M_EPSILON
			|| track_D > M_EPSILON) {

		/* Forced current slew rate limited tracking.
		 * */
		dSA = pm->forced_slew_rate * pm->m_dT;
		pm->forced_track_D = (pm->forced_track_D < track_D - dSA)
			? pm->forced_track_D + dSA : (pm->forced_track_D > track_D + dSA)
			? pm->forced_track_D - dSA : track_D;
	}

	track_D = pm->forced_track_D;
	track_Q = 0.f;

	if (		pm->lu_MODE == PM_LU_FORCED
			|| (	pm->lu_MODE == PM_LU_ESTIMATE
				&& pm->flux_ZONE != PM_ZONE_HIGH)) {

		/* No current drive in this case. */
	}
	else {
		track_Q = pm->i_setpoint_current;

		if (pm->config_LU_DRIVE == PM_DRIVE_CURRENT) {

			if (pm->config_REVERSE_BRAKE == PM_ENABLED) {

				if (track_Q < - M_EPSILON) {

					track_Q = pm_form_SP(pm, 0.f - pm->lu_wS);

					iMAX = m_fabsf(track_Q);

					track_Q = (track_Q > iMAX) ? iMAX
						: (track_Q < - iMAX) ? - iMAX : track_Q;
				}
			}

			if (pm->config_SPEED_MAXIMAL == PM_ENABLED) {

				float		wSP, eSP, blend;

				if (pm->l_track_TIM >= 10) {

					/* Update not relevant speed trackpoint.
					 * */
					pm->l_track = pm->lu_wS;
				}

				wSP = pm->lu_wS;
				wSP = (wSP > pm->s_maximal) ? pm->s_maximal
					: (wSP < - pm->s_reverse) ? - pm->s_reverse : wSP;

				dSA = pm->s_accel * pm->m_dT;
				pm->l_track = (pm->l_track < wSP - dSA) ? pm->l_track + dSA
					: (pm->l_track > wSP + dSA) ? pm->l_track - dSA : wSP;

				eSP = pm->l_track - pm->lu_wS;

				blend = m_fabsf(eSP) / pm->l_track_tol;
				blend = (blend > 1.f) ? 1.f : blend;

				pm->l_blend += (blend - pm->l_blend) * pm->l_gain_LP;
				track_Q += (pm_form_SP(pm, eSP) - track_Q) * pm->l_blend;

				pm->l_track_TIM = 0;
			}
		}

		if (pm->config_RELUCTANCE == PM_ENABLED) {

			float		iD;

			iD = pm_torque_approx_MTPA(pm, pm->mtpa_D, pm->lu_iQ);
			pm->mtpa_D += (iD - pm->mtpa_D) * pm->mtpa_gain_LP;

			/* Maximum torque per ampere (MTPA) control.
			 * */
			track_D += pm->mtpa_D;
		}

		if (pm->config_WEAKENING == PM_ENABLED) {

			float		eDC, wLS;

			eDC = (1.f - pm->vsi_DC) * pm->const_fb_U;

			pm->weak_D += eDC * pm->weak_gain_EU;
			pm->weak_D = (pm->weak_D < - pm->weak_maximal) ? - pm->weak_maximal
				: (pm->weak_D > 0.f) ? 0.f : pm->weak_D;

			if (pm->weak_D < - M_EPSILON) {

				eDC = pm->k_EMAX * pm->const_fb_U;
				wLS = pm->lu_wS * pm->const_im_L2;

				iMAX = eDC / m_fabsf(wLS);

				track_Q = (track_Q > iMAX) ? iMAX
					: (track_Q < - iMAX) ? - iMAX : track_Q;

				/* Flux weakening control.
				 * */
				track_D += pm->weak_D;
			}
		}
	}

	/* Update filtered voltage to use in POWER constraints.
	 * */
	pm->watt_lpf_D += (pm->lu_uD - pm->watt_lpf_D) * pm->watt_gain_LP;
	pm->watt_lpf_Q += (pm->lu_uQ - pm->watt_lpf_Q) * pm->watt_gain_LP;

	/* Maximal CURRENT constraints.
	 * */
	iMAX = pm->i_maximal;
	iREV = - pm->i_reverse;

	if (pm->lu_MODE == PM_LU_ON_HFI) {

		/* Add CURRENT constraint from HFI.
		 * */
		iMAX = (iMAX < pm->i_derate_on_HFI) ? iMAX : pm->i_derate_on_HFI;
		iREV = (iREV > - pm->i_derate_on_HFI) ? iREV : - pm->i_derate_on_HFI;
	}

	track_D = (track_D > iMAX) ? iMAX : (track_D < - iMAX) ? - iMAX : track_D;
	track_Q = (track_Q > iMAX) ? iMAX : (track_Q < iREV) ? iREV : track_Q;

	/* Add CURRENT constraint from PCB.
	 * */
	iMAX = pm->i_derate_on_PCB;

	track_Q = (track_Q > iMAX) ? iMAX : (track_Q < - iMAX) ? - iMAX : track_Q;

	if (pm->weak_D > - M_EPSILON) {

		/* In case of no flux weakening also constraint D-axis current.
		 * */
		track_D = (track_D > iMAX) ? iMAX : (track_D < - iMAX) ? - iMAX : track_D;
	}

	/* Maximal POWER constraint.
	 * */
	wMAX = pm->watt_wP_maximal;
	wREV = - pm->watt_wP_reverse;

	/* Maximal DC link CURRENT constraint.
	 * */
	wP = pm->watt_wA_maximal * pm->const_fb_U;
	wMAX = (wP < wMAX) ? wP : wMAX;
	wP = - pm->watt_wA_reverse * pm->const_fb_U;
	wREV = (wP > wREV) ? wP : wREV;

	/* Keep D-axis CURRENT in case of flux weakening.
	 * */
	wKEP = (pm->weak_D < - M_EPSILON) ? pm->k_KWAT * track_D * pm->watt_lpf_Q : 0.f;

	uMAX = (pm->v_DC_MAX != PM_ENABLED)
		? pm->watt_uDC_maximal
		: pm->watt_uDC_maximal - pm->v_uDC_tol;

	/* Prevent DC link OVERVOLTAGE.
	 * */
	if (unlikely(pm->const_fb_U > uMAX)) {

		float		eDC, bSP;

		eDC = pm->const_fb_U - pm->watt_uDC_maximal;

		if (pm->v_DC_MAX != PM_ENABLED) {

			pm->v_DC_MAX = PM_ENABLED;
			pm->v_integral = pm->watt_drain_wP;
		}

		bSP = pm->v_gain_P * eDC + pm->v_integral;
		bSP = (bSP < 0.f) ? bSP : 0.f;

		if (		(bSP < 0.f || eDC < 0.f)
				&& (bSP > wREV || eDC > 0.f)) {

			pm->v_integral += pm->v_gain_I * eDC;
		}

		wREV = bSP;
	}
	else if (pm->v_DC_MAX == PM_ENABLED) {

		pm->v_DC_MAX = PM_DISABLED;
	}

	uREV = (pm->v_DC_MIN != PM_ENABLED)
		? pm->watt_uDC_minimal
		: pm->watt_uDC_minimal + pm->v_uDC_tol;

	/* Prevent DC link UNDERVOLTAGE.
	 * */
	if (unlikely(pm->const_fb_U < uREV)) {

		float		eDC, bSP;

		eDC = pm->const_fb_U - pm->watt_uDC_minimal;

		if (pm->v_DC_MIN != PM_ENABLED) {

			pm->v_DC_MIN = PM_ENABLED;
			pm->v_integral = pm->watt_drain_wP;
		}

		bSP = pm->v_gain_P * eDC + pm->v_integral;
		bSP = (bSP > wKEP) ? bSP : wKEP;

		wMAX = (wMAX < wKEP) ? wKEP : wMAX;

		if (		(bSP < wKEP || eDC < 0.f)
				&& (bSP > wMAX || eDC > 0.f)) {

			pm->v_integral += pm->v_gain_I * eDC;
		}

		wMAX = bSP;
	}
	else if (pm->v_DC_MIN == PM_ENABLED) {

		pm->v_DC_MIN = PM_DISABLED;
	}

	wP = pm->k_KWAT * (track_D * pm->watt_lpf_D + track_Q * pm->watt_lpf_Q);

	/* Apply POWER constraints of regeneration.
	 * */
	if (unlikely(wP < wREV)) {

		wP = pm->k_KWAT * track_D * pm->watt_lpf_D;

		if (wP < wREV) {

			track_D *= wREV / wP;
			track_Q = 0.f;
		}
		else {
			wREV -= wP;
			wP = pm->k_KWAT * track_Q * pm->watt_lpf_Q;

			if (wP < - M_EPSILON) {

				track_Q *= wREV / wP;
			}
		}
	}
	else if (pm->v_DC_MAX == PM_ENABLED) {

		pm->v_DC_MAX = PM_DISABLED;
	}

	wMAX = (wMAX < wKEP) ? wKEP : wMAX;

	/* Apply POWER constraints of consumption (separately along DQ-axes).
	 * */
	if (unlikely(wP > wMAX)) {

		wP = pm->k_KWAT * track_D * pm->watt_lpf_D;

		if (wP > wMAX) {

			track_D *= wMAX / wP;
			track_Q = 0.f;
		}
		else {
			wMAX -= wP;
			wP = pm->k_KWAT * track_Q * pm->watt_lpf_Q;

			if (wP > M_EPSILON) {

				track_Q *= wMAX / wP;
			}
		}
	}
	else if (pm->v_DC_MIN == PM_ENABLED) {

		pm->v_DC_MIN = PM_DISABLED;
	}

	dSA = pm->i_slew_rate * pm->m_dT;

	/* Slew rate limited current tracking.
	 * */
	pm->i_track_D = (pm->i_track_D < track_D - dSA) ? pm->i_track_D + dSA
		: (pm->i_track_D > track_D + dSA) ? pm->i_track_D - dSA : track_D;
	pm->i_track_Q = (pm->i_track_Q < track_Q - dSA) ? pm->i_track_Q + dSA
		: (pm->i_track_Q > track_Q + dSA) ? pm->i_track_Q - dSA : track_Q;

	/* Obtain discrepancy in DQ-axes.
	 * */
	eD = pm->i_track_D - pm->lu_iD;
	eQ = pm->i_track_Q - pm->lu_iQ;

	/* Basic proportional-integral regulator.
	 * */
	uD = pm->i_gain_P * eD + pm->i_integral_D;
	uQ = pm->i_gain_P * eQ + pm->i_integral_Q;

	/* Feed forward compensation (R).
	 * */
	uD += pm->i_gain_Q * pm->const_Rs * pm->i_track_D;
	uQ += pm->i_gain_Q * pm->const_Rs * pm->i_track_Q;

	/* Feed forward compensation (L).
	 * */
	uD += - pm->i_gain_Q * pm->lu_wS * pm->const_im_L2 * pm->i_track_Q;
	uQ += pm->i_gain_Q * pm->lu_wS * (pm->const_im_L1 * pm->i_track_D + pm->const_lambda);

	uMAX = pm->k_UMAX * pm->const_fb_U;

	if (		(uD < uMAX || eD < 0.f)
			&& (uD > - uMAX || eD > 0.f)) {

		pm->i_integral_D += pm->i_gain_I * eD;
	}

	if (		(uQ < uMAX || eQ < 0.f)
			&& (uQ > - uMAX || eQ > 0.f)) {

		pm->i_integral_Q += pm->i_gain_I * eQ;
	}

	/* Output voltage CLAMP.
	 * */
	uD = (uD > uMAX) ? uMAX : (uD < - uMAX) ? - uMAX : uD;
	uQ = (uQ > uMAX) ? uMAX : (uQ < - uMAX) ? - uMAX : uQ;

	uMAX = pm->k_EMAX * pm->v_maximal;
	uREV = - pm->k_EMAX * pm->v_reverse;

	/* Output voltage (Q) specified constraint.
	 * */
	uQ = (uQ > uMAX) ? uMAX : (uQ < uREV) ? uREV : uQ;

	if (		pm->config_HFI_PERMANENT == PM_ENABLED
			|| pm->lu_MODE == PM_LU_ON_HFI) {

		float		uHF;

		/* HF waveform synthesis.
		 * */
		pm_hfi_wave(pm);

		uHF = pm->hfi_sine * pm->quick_HFwS * pm->const_im_L1;

		/* HF voltage injection.
		 * */
		uD += pm->hfi_wave[0] * uHF;
	}

	/* Go to XY-axes.
	 * */
	uX = pm->lu_F[0] * uD - pm->lu_F[1] * uQ;
	uY = pm->lu_F[1] * uD + pm->lu_F[0] * uQ;

	pm_voltage(pm, uX, uY);
}

static void
pm_loop_speed(pmc_t *pm)
{
	float		wSP, eSP, dSA;

	wSP = pm->s_setpoint_speed;

	/* Maximal SPEED constraint.
	 * */
	wSP = (wSP > pm->s_maximal) ? pm->s_maximal :
		(wSP < - pm->s_reverse) ? - pm->s_reverse : wSP;

	if (pm->lu_MODE == PM_LU_FORCED) {

		pm->s_track = pm->forced_wS;
	}
	else {
		if (pm->config_LU_DRIVE == PM_DRIVE_SPEED) {

			/* Maximal ACCELERATION constraint.
			 * */
			dSA = pm->s_accel * pm->m_dT;
			pm->s_track = (pm->s_track < wSP - dSA) ? pm->s_track + dSA
				: (pm->s_track > wSP + dSA) ? pm->s_track - dSA : wSP;
		}
		else {
			/* No constraint required.
			 * */
			pm->s_track = wSP;
		}

		/* Get speed discrepancy.
		 * */
		eSP = pm->s_track - pm->lu_wS;

		/* Update current loop SETPOINT.
		 * */
		pm->i_setpoint_current = pm_form_SP(pm, eSP);
	}
}

static void
pm_loop_location(pmc_t *pm)
{
	float		xSP, wSP, eLOC, eABS, blend, gain;

	xSP = pm->x_setpoint_location;
	wSP = pm->x_setpoint_speed;

	/* Move location setpoint in accordance with speed setpoint.
	 * */
	xSP += wSP * pm->m_dT;

	/* Allowed location range constraints.
	 * */
	if (xSP > pm->x_maximal) {

		xSP = pm->x_maximal;
		wSP = 0.f;
	}
	else if (xSP < pm->x_minimal) {

		xSP = pm->x_minimal;
		wSP = 0.f;
	}

	pm->x_setpoint_location = xSP;

	/* Get location discrepancy.
	 * */
	eLOC = pm->x_setpoint_location - pm->lu_location;

	/* Servo is based on constant acceleration formula.
	 * */
	eABS= m_fabsf(eLOC);
	eLOC = (eLOC < 0.f) ? - m_sqrtf(eABS) : m_sqrtf(eABS);

	/* There is a residual tolerance.
	 * */
	eLOC = (eABS > pm->x_residual_tol) ? eLOC : 0.f;

	/* Damping inside NEAR zone.
	 * */
	blend = (eABS < pm->x_damping) ? eABS / pm->x_damping : 1.f;
	gain = pm->x_gain_P * blend + pm->x_gain_D * (1.f - blend);

	wSP += gain * eLOC;

	/* Update speed loop SETPOINT.
	 * */
	pm->s_setpoint_speed = wSP;
}

void pm_feedback(pmc_t *pm, pmfb_t *fb)
{
	float		vA, vB, vC, Q;

	if (likely(pm->vsi_AF == 0)) {

		/* Get inline current A.
		 * */
		pm->fb_iA = pm->scale_iA[1] * fb->current_A + pm->scale_iA[0];

		if (unlikely(m_fabsf(pm->fb_iA) > pm->fault_current_halt)) {

			pm->fsm_errno = PM_ERROR_INSTANT_OVERCURRENT;
			pm->fsm_req = PM_STATE_HALT;
		}
	}

	if (likely(pm->vsi_BF == 0)) {

		/* Get inline current B.
		 * */
		pm->fb_iB = pm->scale_iB[1] * fb->current_B + pm->scale_iB[0];

		if (unlikely(m_fabsf(pm->fb_iB) > pm->fault_current_halt)) {

			pm->fsm_errno = PM_ERROR_INSTANT_OVERCURRENT;
			pm->fsm_req = PM_STATE_HALT;
		}
	}

	if (likely(pm->vsi_CF == 0)) {

		/* Get inline current C.
		 * */
		pm->fb_iC = pm->scale_iC[1] * fb->current_C + pm->scale_iC[0];

		if (unlikely(m_fabsf(pm->fb_iC) > pm->fault_current_halt)) {

			pm->fsm_errno = PM_ERROR_INSTANT_OVERCURRENT;
			pm->fsm_req = PM_STATE_HALT;
		}
	}

	if (PM_CONFIG_NOP(pm) == PM_NOP_THREE_PHASE) {

		if (		   pm->vsi_AF == 0
				&& pm->vsi_BF == 0
				&& pm->vsi_CF == 0) {

			Q = 0.33333333f * (pm->fb_iA + pm->fb_iB + pm->fb_iC);

			vA = pm->fb_iA - Q;
			vB = pm->fb_iB - Q;

			pm->lu_iX = vA;
			pm->lu_iY = 0.57735027f * vA + 1.1547005f * vB;
		}
		else if (	   pm->vsi_AF == 0
				&& pm->vsi_BF == 0) {

			pm->lu_iX = pm->fb_iA;
			pm->lu_iY = 0.57735027f * pm->fb_iA + 1.1547005f * pm->fb_iB;
		}
		else if (	   pm->vsi_BF == 0
				&& pm->vsi_CF == 0) {

			pm->lu_iX = - pm->fb_iB - pm->fb_iC;
			pm->lu_iY = 0.57735027f * pm->fb_iB - 0.57735027f * pm->fb_iC;
		}
		else if (	   pm->vsi_AF == 0
				&& pm->vsi_CF == 0) {

			pm->lu_iX = pm->fb_iA;
			pm->lu_iY = - 0.57735027f * pm->fb_iA - 1.1547005f * pm->fb_iC;
		}
	}
	else {
		if (		   pm->vsi_AF == 0
				&& pm->vsi_BF == 0
				&& pm->vsi_CF == 0) {

			Q = 0.33333333f * (pm->fb_iA + pm->fb_iB + pm->fb_iC);

			vA = pm->fb_iA - Q;
			vB = pm->fb_iB - Q;

			pm->lu_iX = vA;
			pm->lu_iY = vB;
		}
		else if (	   pm->vsi_AF == 0
				&& pm->vsi_BF == 0) {

			pm->lu_iX = pm->fb_iA;
			pm->lu_iY = pm->fb_iB;
		}
		else if (	   pm->vsi_BF == 0
				&& pm->vsi_CF == 0) {

			pm->lu_iX = - pm->fb_iB - pm->fb_iC;
			pm->lu_iY = pm->fb_iB;
		}
		else if (	   pm->vsi_AF == 0
				&& pm->vsi_CF == 0) {

			pm->lu_iX = pm->fb_iA;
			pm->lu_iY = - pm->fb_iA - pm->fb_iC;
		}
	}

	if (likely(pm->vsi_SF == 0)) {

		/* Get DC link voltage.
		 * */
		pm->const_fb_U = pm->scale_uS[1] * fb->voltage_U + pm->scale_uS[0];
		pm->quick_iUDC = 1.f / pm->const_fb_U;

		if (unlikely(		pm->const_fb_U > pm->fault_voltage_halt
					&& pm->weak_D > - M_EPSILON)) {

			pm->fsm_errno = PM_ERROR_DC_LINK_OVERVOLTAGE;
			pm->fsm_req = PM_STATE_HALT;
		}
	}

	pm->tvm_X0 = pm->vsi_X0;
	pm->tvm_Y0 = pm->vsi_Y0;

	if (PM_CONFIG_TVM(pm) == PM_ENABLED) {

		vA = pm->fb_uA;
		vB = pm->fb_uB;
		vC = pm->fb_uC;

		/* Get instant terminal voltages.
		 * */
		pm->fb_uA = pm->scale_uA[1] * fb->voltage_A + pm->scale_uA[0];
		pm->fb_uB = pm->scale_uB[1] * fb->voltage_B + pm->scale_uB[0];
		pm->fb_uC = pm->scale_uC[1] * fb->voltage_C + pm->scale_uC[0];

		if (		pm->lu_MODE != PM_LU_DETACHED
				&& pm->vsi_UF == 0) {

			/* Extract the average terminal voltages using FIR.
			 * */

			vA = (pm->vsi_AZ != 0) ? pm->tvm_FIR_A[0] * pm->fb_uA
				+ pm->tvm_FIR_A[1] * vA + pm->tvm_FIR_A[2] : 0.f;

			vB = (pm->vsi_BZ != 0) ? pm->tvm_FIR_B[0] * pm->fb_uB
				+ pm->tvm_FIR_B[1] * vB + pm->tvm_FIR_B[2] : 0.f;

			vC = (pm->vsi_CZ != 0) ? pm->tvm_FIR_C[0] * pm->fb_uC
				+ pm->tvm_FIR_C[1] * vC + pm->tvm_FIR_C[2] : 0.f;

			pm->tvm_A = vA;
			pm->tvm_B = vB;
			pm->tvm_C = vC;

			if (PM_CONFIG_NOP(pm) == PM_NOP_THREE_PHASE) {

				Q = .33333333f * (vA + vB + vC);

				vA = vA - Q;
				vB = vB - Q;

				pm->tvm_X0 = vA;
				pm->tvm_Y0 = 0.57735027f * vA + 1.1547005f * vB;
			}
			else {
				vA = vA - vC;
				vB = vB - vC;

				pm->tvm_X0 = vA;
				pm->tvm_Y0 = vB;
			}
		}
	}

	pm->fb_SIN = fb->analog_SIN;
	pm->fb_COS = fb->analog_COS;
	pm->fb_HS = fb->pulse_HS;
	pm->fb_EP = fb->pulse_EP;

	if (pm->lu_MODE != PM_LU_DISABLED) {

		/* The observer FSM.
		 * */
		pm_lu_FSM(pm);

		if (pm->lu_MODE == PM_LU_DETACHED) {

			pm_voltage(pm, pm->vsi_X, pm->vsi_Y);
		}
		else {
			if (pm->config_LU_DRIVE == PM_DRIVE_SPEED) {

				pm_loop_speed(pm);
			}
			else if (pm->config_LU_DRIVE == PM_DRIVE_LOCATION) {

				pm_loop_location(pm);
				pm_loop_speed(pm);
			}

			/* Current loop is always enabled.
			 * */
			pm_loop_current(pm);

			if (likely(pm->kalman_POSTPONED == PM_ENABLED)) {

				/* We have to do most expensive work after DC
				 * values are output to the PWM. This allows
				 * efficient use of CPU.
				 * */
				pm_kalman_forecast(pm);

				if (likely(pm->vsi_IF == 0)) {

					pm_kalman_update(pm, pm->flux_X);
				}

				pm->kalman_POSTPONED = PM_DISABLED;
			}

			/* Wattage information.
			 * */
			pm_wattage(pm);
		}

		if (PM_CONFIG_DBG(pm) == PM_ENABLED) {

			float		A, B;

			A = pm->lu_F[0] * pm->flux_F[0] + pm->lu_F[1] * pm->flux_F[1];
			B = pm->lu_F[1] * pm->flux_F[0] - pm->lu_F[0] * pm->flux_F[1];

			pm->d_flux_rsu = m_atan2f(B, A) * (180.f / M_PI_F);
		}

		if (unlikely(m_isfinitef(pm->lu_F[0]) == 0)) {

			pm->fsm_errno = PM_ERROR_INVALID_OPERATION;
			pm->fsm_state = PM_STATE_HALT;
		}
	}

	/* The FSM is used to execute assistive routines.
	 * */
	pm_FSM(pm);
}

