#pragma once

// output mode
#define WHEEL_T_FFGP 1
#define WHEEL_T_DF   2
#define WHEEL_T_DFP  3
#define WHEEL_T_DFGT 4
#define WHEEL_T_G25  5
#define WHEEL_T_G27  6 //(need to test) endpoint size 16, interval 2

enum init_stage_status {
  DISCONNECTED,
  READING_DESCRIPTOR,
  SENDING_CMDS,
  RESTARTING,
  READY
};

enum lg_wheel_type {
  NONE,
//  OTHER,
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
const uint8_t wheel_10bits = 0;
const uint8_t wheel_14bits = 1;
const uint8_t wheel_16bits = 2;
