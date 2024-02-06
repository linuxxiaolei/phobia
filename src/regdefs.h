ID_NULL,
ID_HAL_USART_BAUDRATE,
ID_HAL_USART_PARITY,
ID_HAL_PWM_FREQUENCY,
ID_HAL_PWM_DEADTIME,
ID_HAL_ADC_REFERENCE_VOLTAGE,
ID_HAL_ADC_SHUNT_RESISTANCE,
ID_HAL_ADC_AMPLIFIER_GAIN,
ID_HAL_ADC_VOLTAGE_RATIO,
ID_HAL_ADC_TERMINAL_RATIO,
ID_HAL_ADC_TERMINAL_BIAS,
#ifdef HW_HAVE_ANALOG_KNOB
ID_HAL_ADC_KNOB_RATIO,
#endif /* HW_HAVE_ANALOG_KNOB */
ID_HAL_ADC_SAMPLE_TIME,
ID_HAL_ADC_SAMPLE_ADVANCE,
#ifdef HW_HAVE_NETWORK_EPCAN
ID_HAL_CAN_BITFREQ,
ID_HAL_CAN_ERRATE,
#endif /* HW_HAVE_NETWORK_EPCAN */
ID_HAL_DPS_MODE,
ID_HAL_PPM_MODE,
ID_HAL_PPM_FREQUENCY,
#ifdef HW_HAVE_STEP_DIR_KNOB
ID_HAL_STEP_MODE,
ID_HAL_STEP_FREQUENCY,
#endif /* HW_HAVE_STEP_DIR_KNOB */
#ifdef HW_HAVE_DRV_ON_PCB
ID_HAL_DRV_PARTNO,
ID_HAL_DRV_AUTO_RESTART,
ID_HAL_DRV_STATUS_RAW,
ID_HAL_DRV_GATE_CURRENT,
ID_HAL_DRV_OCP_LEVEL,
#endif /* HW_HAVE_DRV_ON_PCB */
ID_HAL_CNT_DIAG0,
ID_HAL_CNT_DIAG0_PC,
ID_HAL_CNT_DIAG1,
ID_HAL_CNT_DIAG1_PC,
ID_HAL_CNT_DIAG2,
ID_HAL_CNT_DIAG2_PC,
#ifdef HW_HAVE_NETWORK_EPCAN
ID_NET_NODE_ID,
ID_NET_LOG_MODE,
ID_NET_TIMEOUT_EP,
ID_NET_EP0_MODE,
ID_NET_EP0_ID,
ID_NET_EP0_CLOCK_ID,
ID_NET_EP0_REG_DATA,
ID_NET_EP0_REG_ID,
ID_NET_EP0_PAYLOAD,
ID_NET_EP0_STARTUP,
ID_NET_EP0_RATE,
ID_NET_EP0_RANGE0,
ID_NET_EP0_RANGE1,
ID_NET_EP1_MODE,
ID_NET_EP1_ID,
ID_NET_EP1_CLOCK_ID,
ID_NET_EP1_REG_DATA,
ID_NET_EP1_REG_ID,
ID_NET_EP1_PAYLOAD,
ID_NET_EP1_STARTUP,
ID_NET_EP1_RATE,
ID_NET_EP1_RANGE0,
ID_NET_EP1_RANGE1,
ID_NET_EP2_MODE,
ID_NET_EP2_ID,
ID_NET_EP2_CLOCK_ID,
ID_NET_EP2_REG_DATA,
ID_NET_EP2_REG_ID,
ID_NET_EP2_PAYLOAD,
ID_NET_EP2_STARTUP,
ID_NET_EP2_RATE,
ID_NET_EP2_RANGE0,
ID_NET_EP2_RANGE1,
ID_NET_EP3_MODE,
ID_NET_EP3_ID,
ID_NET_EP3_CLOCK_ID,
ID_NET_EP3_REG_DATA,
ID_NET_EP3_REG_ID,
ID_NET_EP3_PAYLOAD,
ID_NET_EP3_STARTUP,
ID_NET_EP3_RATE,
ID_NET_EP3_RANGE0,
ID_NET_EP3_RANGE1,
#endif /* HW_HAVE_NETWORK_EPCAN */
ID_AP_PPM_PULSE,
ID_AP_PPM_FREQ,
ID_AP_PPM_REG_DATA,
ID_AP_PPM_REG_ID,
ID_AP_PPM_STARTUP,
ID_AP_PPM_RANGE0,
ID_AP_PPM_RANGE1,
ID_AP_PPM_RANGE2,
ID_AP_PPM_CONTROL0,
ID_AP_PPM_CONTROL1,
ID_AP_PPM_CONTROL2,
#ifdef HW_HAVE_STEP_DIR_KNOB
ID_AP_STEP_POS,
ID_AP_STEP_REG_DATA,
ID_AP_STEP_REG_ID,
ID_AP_STEP_STARTUP,
ID_AP_STEP_CONST_S,
ID_AP_STEP_CONST_S_DEG,
ID_AP_STEP_CONST_S_MM,
#endif /* HW_HAVE_STEP_DIR_KNOB */
#ifdef HW_HAVE_ANALOG_KNOB
ID_AP_KNOB_IN_ANG,
#ifdef HW_HAVE_BRAKE_KNOB
ID_AP_KNOB_IN_BRK,
#endif /* HW_HAVE_BRAKE_KNOB */
ID_AP_KNOB_REG_DATA,
ID_AP_KNOB_REG_ID,
ID_AP_KNOB_ENABLED,
#ifdef HW_HAVE_BRAKE_KNOB
ID_AP_KNOB_BRAKE,
#endif /* HW_HAVE_BRAKE_KNOB */
ID_AP_KNOB_STARTUP,
ID_AP_KNOB_RANGE_ANG0,
ID_AP_KNOB_RANGE_ANG1,
ID_AP_KNOB_RANGE_ANG2,
#ifdef HW_HAVE_BRAKE_KNOB
ID_AP_KNOB_RANGE_BRK0,
ID_AP_KNOB_RANGE_BRK1,
#endif /* HW_HAVE_BRAKE_KNOB */
ID_AP_KNOB_RANGE_LOS0,
ID_AP_KNOB_RANGE_LOS1,
ID_AP_KNOB_CONTROL_ANG0,
ID_AP_KNOB_CONTROL_ANG1,
ID_AP_KNOB_CONTROL_ANG2,
#ifdef HW_HAVE_BRAKE_KNOB
ID_AP_KNOB_CONTROL_BRK,
#endif /* HW_HAVE_BRAKE_KNOB */
#endif /* HW_HAVE_ANALOG_KNOB */
ID_AP_IDLE_TIMEOUT,
ID_AP_DISARM_TIMEOUT,
#ifdef HW_HAVE_NTC_ON_PCB
ID_AP_NTC_PCB_TYPE,
ID_AP_NTC_PCB_BALANCE,
ID_AP_NTC_PCB_NTC0,
ID_AP_NTC_PCB_TA0,
ID_AP_NTC_PCB_BETTA,
#endif /* HW_HAVE_NTC_ON_PCB */
#ifdef HW_HAVE_NTC_MACHINE
ID_AP_NTC_EXT_TYPE,
ID_AP_NTC_EXT_BALANCE,
ID_AP_NTC_EXT_NTC0,
ID_AP_NTC_EXT_TA0,
ID_AP_NTC_EXT_BETTA,
#endif /* HW_HAVE_NTC_MACHINE */
ID_AP_TEMP_PCB,
#ifdef HW_HAVE_NTC_MACHINE
ID_AP_TEMP_EXT,
#endif /* HW_HAVE_NTC_MACHINE */
ID_AP_TEMP_MCU,
ID_AP_HEAT_PCB_TEMP_HALT,
ID_AP_HEAT_PCB_TEMP_DERATE,
ID_AP_HEAT_PCB_TEMP_FAN,
ID_AP_HEAT_EXT_TEMP_DERATE,
ID_AP_HEAT_DERATED_PCB,
ID_AP_HEAT_DERATED_EXT,
ID_AP_HEAT_TEMP_RECOVERY,
ID_AP_TASK_AUTOSTART,
ID_AP_TASK_BUTTON,
ID_AP_TASK_AS5047,
ID_AP_TASK_HX711,
ID_AP_TASK_MPU6050,
ID_AP_AUTO_REG_DATA,
ID_AP_AUTO_REG_ID,
ID_AP_LOAD_HX711,
ID_PM_DC_RESOLUTION,
ID_PM_DC_MINIMAL,
ID_PM_DC_CLEARANCE,
ID_PM_DC_SKIP,
ID_PM_DC_BOOTSTRAP,
ID_PM_SELF_BST,
ID_PM_SELF_IST,
ID_PM_SELF_STDI,
ID_PM_SELF_RMSI,
ID_PM_SELF_RMSU,
ID_PM_CONFIG_NOP,
ID_PM_CONFIG_IFB,
ID_PM_CONFIG_TVM,
ID_PM_CONFIG_DBG,
ID_PM_CONFIG_VSI_ZERO,
ID_PM_CONFIG_VSI_CLAMP,
ID_PM_CONFIG_LU_FORCED,
ID_PM_CONFIG_LU_ESTIMATE,
ID_PM_CONFIG_LU_SENSOR,
ID_PM_CONFIG_LU_LOCATION,
ID_PM_CONFIG_LU_DRIVE,
ID_PM_CONFIG_HFI_WAVETYPE,
ID_PM_CONFIG_HFI_PERMANENT,
ID_PM_CONFIG_EXCITATION,
ID_PM_CONFIG_SALIENCY,
ID_PM_CONFIG_RELUCTANCE,
ID_PM_CONFIG_WEAKENING,
ID_PM_CONFIG_REVERSE_BRAKE,
ID_PM_CONFIG_SPEED_MAXIMAL,
ID_PM_CONFIG_EABI_FRONTEND,
ID_PM_CONFIG_SINCOS_FRONTEND,
ID_PM_CONFIG_BOOST_CHARGE,
ID_PM_FSM_REQ,
ID_PM_FSM_STATE,
ID_PM_FSM_ERRNO,
ID_PM_TM_TRANSIENT_SLOW,
ID_PM_TM_TRANSIENT_FAST,
ID_PM_TM_VOLTAGE_HOLD,
ID_PM_TM_CURRENT_HOLD,
ID_PM_TM_CURRENT_RAMP,
ID_PM_TM_INSTANT_PROBE,
ID_PM_TM_AVERAGE_PROBE,
ID_PM_TM_AVERAGE_DRIFT,
ID_PM_TM_AVERAGE_INERTIA,
ID_PM_TM_PAUSE_STARTUP,
ID_PM_TM_PAUSE_FORCED,
ID_PM_TM_PAUSE_HALT,
ID_PM_SCALE_IA0,
ID_PM_SCALE_IA1,
ID_PM_SCALE_IB0,
ID_PM_SCALE_IB1,
ID_PM_SCALE_IC0,
ID_PM_SCALE_IC1,
ID_PM_SCALE_US0,
ID_PM_SCALE_US1,
ID_PM_SCALE_UA0,
ID_PM_SCALE_UA1,
ID_PM_SCALE_UB0,
ID_PM_SCALE_UB1,
ID_PM_SCALE_UC0,
ID_PM_SCALE_UC1,
ID_PM_FB_IA,
ID_PM_FB_IB,
ID_PM_FB_IC,
ID_PM_FB_UA,
ID_PM_FB_UB,
ID_PM_FB_UC,
ID_PM_FB_HS,
ID_PM_FB_EP,
ID_PM_FB_SIN,
ID_PM_FB_COS,
ID_PM_PROBE_CURRENT_HOLD,
ID_PM_PROBE_CURRENT_WEAK,
ID_PM_PROBE_HOLD_ANGLE,
ID_PM_PROBE_CURRENT_SINE,
ID_PM_PROBE_CURRENT_BIAS,
ID_PM_PROBE_FREQ_SINE,
ID_PM_PROBE_SPEED_HOLD,
ID_PM_PROBE_SPEED_HOLD_RPM,
ID_PM_PROBE_SPEED_TOL,
ID_PM_PROBE_SPEED_TOL_RPM,
ID_PM_PROBE_LOCATION_TOL,
ID_PM_PROBE_LOCATION_TOL_MM,
ID_PM_PROBE_LOSS_MAXIMAL,
ID_PM_PROBE_GAIN_P,
ID_PM_PROBE_GAIN_I,
ID_PM_FAULT_VOLTAGE_TOL,
ID_PM_FAULT_CURRENT_TOL,
ID_PM_FAULT_ACCURACY_TOL,
ID_PM_FAULT_TERMINAL_TOL,
ID_PM_FAULT_CURRENT_HALT,
ID_PM_FAULT_VOLTAGE_HALT,
ID_PM_VSI_DC,
ID_PM_VSI_LPF_DC,
ID_PM_VSI_X,
ID_PM_VSI_Y,
ID_PM_VSI_GAIN_LP,
ID_PM_VSI_MASK_XF,
ID_PM_VSI_AF,
ID_PM_VSI_BF,
ID_PM_VSI_CF,
ID_PM_VSI_IF,
ID_PM_VSI_SF,
ID_PM_VSI_UF,
ID_PM_VSI_AQ,
ID_PM_VSI_BQ,
ID_PM_VSI_CQ,
ID_PM_TVM_ACTIVE,
ID_PM_TVM_CLEAN_ZONE,
ID_PM_TVM_A,
ID_PM_TVM_B,
ID_PM_TVM_C,
ID_PM_TVM_FIR_A0,
ID_PM_TVM_FIR_A1,
ID_PM_TVM_FIR_A2,
ID_PM_TVM_FIR_A_TAU,
ID_PM_TVM_FIR_B0,
ID_PM_TVM_FIR_B1,
ID_PM_TVM_FIR_B2,
ID_PM_TVM_FIR_B_TAU,
ID_PM_TVM_FIR_C0,
ID_PM_TVM_FIR_C1,
ID_PM_TVM_FIR_C2,
ID_PM_TVM_FIR_C_TAU,
ID_PM_TVM_X0,
ID_PM_TVM_Y0,
ID_PM_LU_MODE,
ID_PM_LU_IX,
ID_PM_LU_IY,
ID_PM_LU_ID,
ID_PM_LU_IQ,
ID_PM_LU_UD,
ID_PM_LU_UQ,
ID_PM_LU_F0,
ID_PM_LU_F1,
ID_PM_LU_WS,
ID_PM_LU_WS_RPM,
ID_PM_LU_WS_MMPS,
ID_PM_LU_WS_KMH,
ID_PM_LU_LOCATION,
ID_PM_LU_LOCATION_DEG,
ID_PM_LU_LOCATION_MM,
ID_PM_LU_TOTAL_REVOL,
ID_PM_LU_RATE,
ID_PM_LU_MQ_PRODUCE,
ID_PM_LU_MQ_LOAD,
ID_PM_LU_GAIN_MQ_LP,
ID_PM_FORCED_HOLD_D,
ID_PM_FORCED_MAXIMAL,
ID_PM_FORCED_MAXIMAL_RPM,
ID_PM_FORCED_REVERSE,
ID_PM_FORCED_REVERSE_RPM,
ID_PM_FORCED_ACCEL,
ID_PM_FORCED_ACCEL_RPM,
ID_PM_FORCED_ACCEL_MMPS,
ID_PM_FORCED_SLEW_RATE,
ID_PM_FORCED_STOP_DC,
ID_PM_DETACH_THRESHOLD,
ID_PM_DETACH_TRIP_AP,
ID_PM_DETACH_GAIN_SF,
ID_PM_FLUX_ZONE,
ID_PM_FLUX_LAMBDA,
ID_PM_FLUX_WS,
ID_PM_FLUX_WS_RPM,
ID_PM_FLUX_WS_MMPS,
ID_PM_FLUX_WS_KMH,
ID_PM_FLUX_TRIP_AP,
ID_PM_FLUX_GAIN_IN,
ID_PM_FLUX_GAIN_LO,
ID_PM_FLUX_GAIN_HI,
ID_PM_FLUX_GAIN_SF,
ID_PM_FLUX_GAIN_IF,
ID_PM_KALMAN_BIAS_Q,
ID_PM_KALMAN_GAIN_Q0,
ID_PM_KALMAN_GAIN_Q1,
ID_PM_KALMAN_GAIN_Q2,
ID_PM_KALMAN_GAIN_Q3,
ID_PM_KALMAN_GAIN_Q4,
ID_PM_KALMAN_GAIN_R,
ID_PM_ZONE_NOISE,
ID_PM_ZONE_NOISE_U,
ID_PM_ZONE_THRESHOLD,
ID_PM_ZONE_THRESHOLD_U,
ID_PM_ZONE_LPF_WS,
ID_PM_ZONE_GAIN_TH,
ID_PM_ZONE_GAIN_LP,
ID_PM_HFI_FREQ,
ID_PM_HFI_SINE,
ID_PM_HALL_ST1_X,
ID_PM_HALL_ST1_Y,
ID_PM_HALL_ST1,
ID_PM_HALL_ST2_X,
ID_PM_HALL_ST2_Y,
ID_PM_HALL_ST2,
ID_PM_HALL_ST3_X,
ID_PM_HALL_ST3_Y,
ID_PM_HALL_ST3,
ID_PM_HALL_ST4_X,
ID_PM_HALL_ST4_Y,
ID_PM_HALL_ST4,
ID_PM_HALL_ST5_X,
ID_PM_HALL_ST5_Y,
ID_PM_HALL_ST5,
ID_PM_HALL_ST6_X,
ID_PM_HALL_ST6_Y,
ID_PM_HALL_ST6,
ID_PM_HALL_WS,
ID_PM_HALL_WS_RPM,
ID_PM_HALL_WS_MMPS,
ID_PM_HALL_WS_KMH,
ID_PM_HALL_TRIP_AP,
ID_PM_HALL_GAIN_LO,
ID_PM_HALL_GAIN_SF,
ID_PM_HALL_GAIN_IF,
ID_PM_EABI_ADJUST,
ID_PM_EABI_F0_X,
ID_PM_EABI_F0_Y,
ID_PM_EABI_F0,
ID_PM_EABI_CONST_EP,
ID_PM_EABI_CONST_ZS,
ID_PM_EABI_CONST_ZQ,
ID_PM_EABI_WS,
ID_PM_EABI_WS_RPM,
ID_PM_EABI_WS_MMPS,
ID_PM_EABI_TRIP_AP,
ID_PM_EABI_GAIN_LO,
ID_PM_EABI_GAIN_SF,
ID_PM_EABI_GAIN_IF,
ID_PM_CONST_FB_U,
ID_PM_CONST_LAMBDA,
ID_PM_CONST_LAMBDA_KV,
ID_PM_CONST_LAMBDA_NM,
ID_PM_CONST_RS,
ID_PM_CONST_ZP,
ID_PM_CONST_JA,
ID_PM_CONST_JA_KGM2,
ID_PM_CONST_JA_KG,
ID_PM_CONST_IM_L1,
ID_PM_CONST_IM_L2,
ID_PM_CONST_IM_B,
ID_PM_CONST_IM_R,
ID_PM_CONST_LD_S,
ID_PM_WATT_WP_MAXIMAL,
ID_PM_WATT_WA_MAXIMAL,
ID_PM_WATT_WP_REVERSE,
ID_PM_WATT_WA_REVERSE,
ID_PM_WATT_UDC_MAXIMAL,
ID_PM_WATT_UDC_MINIMAL,
ID_PM_WATT_LPF_D,
ID_PM_WATT_LPF_Q,
ID_PM_WATT_DRAIN_WP,
ID_PM_WATT_DRAIN_WA,
ID_PM_WATT_TRAVELED,
ID_PM_WATT_TRAVELED_KM,
ID_PM_WATT_CONSUMED_WH,
ID_PM_WATT_CONSUMED_AH,
ID_PM_WATT_REVERTED_WH,
ID_PM_WATT_REVERTED_AH,
ID_PM_WATT_CAPACITY_AH,
ID_PM_WATT_FUEL_GAUGE,
ID_PM_WATT_GAIN_LP,
ID_PM_I_DERATE_ON_HFI,
ID_PM_I_SETPOINT_CURRENT,
ID_PM_I_SETPOINT_CURRENT_PC,
ID_PM_I_MAXIMAL,
ID_PM_I_REVERSE,
ID_PM_I_TRACK_D,
ID_PM_I_TRACK_Q,
ID_PM_I_SLEW_RATE,
ID_PM_I_DAMPING,
ID_PM_I_GAIN_P,
ID_PM_I_GAIN_I,
ID_PM_I_GAIN_Q,
ID_PM_MTPA_D,
ID_PM_MTPA_GAIN_LP,
ID_PM_WEAK_MAXIMAL,
ID_PM_WEAK_D,
ID_PM_WEAK_GAIN_EU,
ID_PM_V_MAXIMAL,
ID_PM_V_REVERSE,
ID_PM_V_DC_MAX,
ID_PM_V_DC_MIN,
ID_PM_V_UDC_TOL,
ID_PM_V_GAIN_P,
ID_PM_V_GAIN_I,
ID_PM_S_SETPOINT_SPEED,
ID_PM_S_SETPOINT_SPEED_RPM,
ID_PM_S_SETPOINT_SPEED_MMPS,
ID_PM_S_SETPOINT_SPEED_KMH,
ID_PM_S_SETPOINT_SPEED_PC,
ID_PM_S_SETPOINT_SPEED_KNOB,
ID_PM_S_MAXIMAL,
ID_PM_S_MAXIMAL_RPM,
ID_PM_S_MAXIMAL_MMPS,
ID_PM_S_MAXIMAL_KMH,
ID_PM_S_REVERSE,
ID_PM_S_REVERSE_RPM,
ID_PM_S_REVERSE_MMPS,
ID_PM_S_REVERSE_KMH,
ID_PM_S_TRACK,
ID_PM_S_ACCEL,
ID_PM_S_ACCEL_RPM,
ID_PM_S_ACCEL_KMH,
ID_PM_S_DAMPING,
ID_PM_S_GAIN_P,
ID_PM_S_GAIN_I,
ID_PM_S_GAIN_D,
ID_PM_S_GAIN_Q,
ID_PM_L_TRACK,
ID_PM_L_TRACK_TOL,
ID_PM_L_GAIN_LP,
ID_PM_X_SETPOINT_LOCATION,
ID_PM_X_SETPOINT_LOCATION_DEG,
ID_PM_X_SETPOINT_LOCATION_MM,
ID_PM_X_SETPOINT_SPEED,
ID_PM_X_SETPOINT_SPEED_RPM,
ID_PM_X_SETPOINT_SPEED_MMPS,
ID_PM_X_MAXIMAL,
ID_PM_X_MAXIMAL_DEG,
ID_PM_X_MAXIMAL_MM,
ID_PM_X_MINIMAL,
ID_PM_X_MINIMAL_DEG,
ID_PM_X_MINIMAL_MM,
ID_PM_X_DAMPING,
ID_PM_X_DAMPING_MM,
ID_PM_X_RESIDUAL_TOL,
ID_PM_X_RESIDUAL_TOL_MM,
ID_PM_X_GAIN_P,
ID_PM_X_GAIN_P_RADPS,
ID_PM_X_GAIN_P_MMPS,
ID_PM_X_GAIN_D,
ID_PM_D_FLUX_RSU,
ID_TLM_RATE_GRAB,
ID_TLM_RATE_LIVE,
ID_TLM_MODE,
ID_TLM_REG_ID0,
ID_TLM_REG_ID1,
ID_TLM_REG_ID2,
ID_TLM_REG_ID3,
ID_TLM_REG_ID4,
ID_TLM_REG_ID5,
ID_TLM_REG_ID6,
ID_TLM_REG_ID7,
ID_TLM_REG_ID8,
ID_TLM_REG_ID9,
