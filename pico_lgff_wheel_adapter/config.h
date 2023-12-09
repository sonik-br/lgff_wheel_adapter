#pragma once

/*******************************************************************************
 * Required config
/*******************************************************************************/

// USB D+ pin
#define PIN_USB_HOST_DP 0

// emulated output wheel type
// possible modes: WHEEL_T_FFGP, WHEEL_T_DF, WHEEL_T_DFP, WHEEL_T_DFGT, WHEEL_T_G25, WHEEL_T_G27
#define WHEEL_TYPE WHEEL_T_FFGP

/*******************************************************************************
 * Optional config.
/*******************************************************************************/

// force input wheel into specific compatibility mode (NOT USED ANYMORE)
// possible modes: NONE, DF, DFP, DFGT, G25, G27, G29.
// NONE will force the wheel into it's native mode
// don't force a mode not supported by your wheel!
const lg_wheel_type force_input_mode = DF;

// WS2812 rgb led pin (comment out if not using)
//#define BOARD_RGB_PIN 23
