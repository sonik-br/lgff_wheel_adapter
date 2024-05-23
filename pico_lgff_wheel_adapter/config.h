#pragma once

/*******************************************************************************
 * Required config
 *******************************************************************************/

// USB D+ pin
#define PIN_USB_HOST_DP 0

// disable initial wheel auto-center
// most (all) logitech wheels starts with auto-centering enabled
// this will send a command to disable it during device start up process
// #define DEACTIVATE_AUTO_CENTER

// enable automatic mode switch.
// device will start in Driving Force compatibility mode (as most logitech wheels does)
// only works with output_mode: WHEEL_T_DFP, WHEEL_T_DFGT, WHEEL_T_G25, WHEEL_T_G27
const bool auto_mode = true;

// emulated output wheel type
// possible modes: WHEEL_T_FFGP, WHEEL_T_DF, WHEEL_T_DFP, WHEEL_T_DFGT, WHEEL_T_G25, WHEEL_T_G27, WHEEL_T_SFW
lg_wheel_output_type output_mode = WHEEL_T_G27;

// force input wheel into specific compatibility mode
// possible modes: NATIVE, AUTO, DF, DFP, DFGT, G25, G27, G29, G923.
// NATIVE will force the wheel into it's native mode.
// don't force a mode not supported by your wheel!
// AUTO will try to best match with the output_mode, with fall back to NATIVE
const lg_wheel_type force_input_mode = AUTO;

/*******************************************************************************
 * Optional config.
 *******************************************************************************/

// external pedal type
// possible types: PEDAL_G25
// if enabled, also need to set the pins for GAS and BRAKE. Must use analog pin
//#define EXTERNAL_PEDAL_TYPE PEDAL_G25
//#define PEDAL_GAS 26
//#define PEDAL_BRAKE 27

// WS2812 rgb led pin (comment out if not using)
//#define BOARD_RGB_PIN 23
