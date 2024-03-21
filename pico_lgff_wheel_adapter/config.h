#pragma once

/*******************************************************************************
 * Required config
/*******************************************************************************/

// USB D+ pin
#define PIN_USB_HOST_DP 0

// emulated output wheel type
// possible modes: WHEEL_T_FFGP, WHEEL_T_DF, WHEEL_T_DFP, WHEEL_T_DFGT, WHEEL_T_G25, WHEEL_T_G27, WHEEL_T_SFW
lg_wheel_output_type output_mode = WHEEL_T_G27;

// force input wheel into specific compatibility mode
// possible modes: NATIVE, DF, DFP, DFGT, G25, G27, G29.
// NATIVE will force the wheel into it's native mode.
// don't force a mode not supported by your wheel!
const lg_wheel_type force_input_mode = NATIVE;

/*******************************************************************************
 * Optional config.
/*******************************************************************************/

// WS2812 rgb led pin (comment out if not using)
//#define BOARD_RGB_PIN 23
