#pragma once

// output mode
enum lg_wheel_output_type {
  WHEEL_T_FGP,
  WHEEL_T_FFGP,
  WHEEL_T_DF,
  WHEEL_T_DFP,
  WHEEL_T_DFGT,
  WHEEL_T_G25,
  WHEEL_T_G27, //(need to test) endpoint size 16, interval 2
  WHEEL_T_SFW
};

enum init_stage_status {
  DISCONNECTED,
  READING_DESCRIPTOR,
  CONFIGURING_DONGLE,
  SENDING_CMDS,
  RESTARTING,
  READY
};

enum lg_wheel_type {
  NATIVE,
  AUTO,
  DF,
  DFP,
  DFGT,
  G25,
  G27,
  G29
};

//enum wheel_precision {
//  wheel_10bits = 0,
//  wheel_14bits,
//  wheel_16bits,
//};
const uint8_t wheel_8bits  = 0;
const uint8_t wheel_10bits = 1;
const uint8_t wheel_14bits = 2;
const uint8_t wheel_16bits = 3;
