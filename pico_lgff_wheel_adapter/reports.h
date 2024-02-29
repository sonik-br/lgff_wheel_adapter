#pragma once

// generic report
typedef struct TU_ATTR_PACKED {
  uint8_t hat : 4;
  uint8_t cross : 1;
  uint8_t square : 1;
  uint8_t circle : 1;
  uint8_t triangle : 1;
  
  uint8_t R1 : 1;
  uint8_t L1 : 1;
  uint8_t R2 : 1;
  uint8_t L2 : 1;
  uint8_t R3 : 1;
  uint8_t L3 : 1;
  uint8_t R4 : 1;
  uint8_t L4 : 1;
  
  uint8_t R5 : 1;
  uint8_t L5 : 1;
  uint8_t select : 1;
  uint8_t start : 1;
  uint8_t horn : 1;
  uint8_t gear_plus : 1;
  uint8_t gear_minus : 1;
  uint8_t enter : 1;

  uint8_t shifter_1 : 1;
  uint8_t shifter_2 : 1;
  uint8_t shifter_3 : 1;
  uint8_t shifter_4 : 1;
  uint8_t shifter_5 : 1;
  uint8_t shifter_6 : 1;
  uint8_t shifter_r : 1;
  uint8_t shifter_stick_down : 1;

  uint8_t shifter_x;
  uint8_t shifter_y;  

  uint8_t wheel_8;
  uint8_t gasPedal_8;
  uint8_t brakePedal_8;
  uint8_t clutchPedal_8;
  
  uint16_t gasPedal_16;
  uint16_t brakePedal_16;
  uint16_t clutchPedal_16;

  uint16_t wheel_16 : 16;

  uint16_t wheel_14 : 14;
  uint8_t dial_cw : 1;
  uint8_t dial_ccw : 1;

  uint16_t wheel_10 : 10;
  uint8_t PS : 1;
  uint8_t plus : 1;
  uint8_t minus : 1;

  uint8_t pedals_precision_16bits : 1; // pedals uses 16bits?
  //wheel_precision wheel_precision : 2; // wheel precision enum
  uint8_t wheel_precision : 2; // wheel precision

} generic_report_t;


// WingMan Formula GP
typedef struct TU_ATTR_PACKED {
  uint8_t wheel;
  
  uint8_t : 2; // padding? alwats zero
  uint8_t L1 : 1;
  uint8_t R1 : 1;
  //no idea if the button mapping is correct.
  uint8_t square : 1;   //top left.    label X. GT3: rear view
  uint8_t triangle : 1; //top right.   label Y. GT3: ebrakePedal
  uint8_t cross : 1;    //lower left.  label A. GT3: pause
  uint8_t circle : 1;   //lower right. label B. GT3: reverse

  uint8_t pedals; // combined pedals?
  uint8_t gasPedal;
  uint8_t brakePedal;
} fgp_report_t;

// WingMan Formula Force GP (GT Force)
typedef struct TU_ATTR_PACKED {
  uint16_t wheel : 10;
  uint8_t L1 : 1;
  uint8_t R1 : 1;
  //no idea if the button mapping is correct. labels are aligned with what shows on logitech tester
  uint8_t square : 1;   //label X. GT3: rear view
  uint8_t triangle : 1; //label Y. GT3: ebrakePedal
  uint8_t cross : 1;    //label A. GT3: pause
  uint8_t circle : 1;   //label B. GT3: reverse

  uint8_t : 8; // unknown
  uint8_t pedals; // combined pedals?
  uint8_t gasPedal;
  uint8_t brakePedal;
  uint8_t : 8; // unknown
} ffgp_report_t;

// Driving Force
typedef struct TU_ATTR_PACKED {
    uint16_t wheel : 10;
    uint8_t cross : 1;
    uint8_t square : 1;
    uint8_t circle : 1;
    uint8_t triangle : 1;
    uint8_t R1 : 1;
    uint8_t L1 : 1;

    uint8_t R2 : 1;
    uint8_t L2 : 1;
    uint8_t select : 1;
    uint8_t start : 1;
    uint8_t R3 : 1;
    uint8_t L3 : 1;
    uint8_t pedal_connected : 1; // not sure
    uint8_t power_connected : 1; //not sure

    uint8_t pedals; // combined pedals
    uint8_t hat : 4;
    uint8_t : 1; // allways 0
    uint8_t calibated : 1; //not sure (or ready to use?)
    uint8_t : 1; // allways 0
    uint8_t unknown : 1; // allways 1
    uint8_t gasPedal;
    uint8_t brakePedal;
} df_report_t;

// Driving Force Pro (GT Force Pro)
typedef struct TU_ATTR_PACKED {
    uint16_t wheel : 14;
    uint8_t cross : 1;
    uint8_t square : 1;
  
    uint8_t circle : 1;
    uint8_t triangle : 1;
    uint8_t R1 : 1;
    uint8_t L1 : 1;
    uint8_t R2 : 1;
    uint8_t L2 : 1;
    uint8_t select : 1;
    uint8_t start : 1;
    
    uint8_t R3 : 1;
    uint8_t L3 : 1;
    uint8_t gear_plus : 1;
    uint8_t gear_minus : 1;
    uint8_t hat : 4;

  uint8_t pedals; //combined pedals
  uint8_t gasPedal;
  uint8_t brakePedal;

  uint8_t pedal_connected : 1;
  uint8_t power_connected : 1;
  uint8_t : 1; // always 0
  uint8_t calibated : 1; //not sure (or ready to use?)
  uint8_t unknown : 4; //always 1
} dfp_report_t;

// Driving Force GT
typedef struct TU_ATTR_PACKED {
    uint8_t hat : 4;
    uint8_t cross : 1;
    uint8_t square : 1;
    uint8_t circle : 1;
    uint8_t triangle : 1;
    
    uint8_t R1 : 1;
    uint8_t L1 : 1;
    uint8_t R2 : 1;
    uint8_t L2 : 1;
    uint8_t select : 1;
    uint8_t start : 1;
    uint8_t R3 : 1;
    uint8_t L3 : 1;
    
    uint8_t gear_plus : 1;
    uint8_t gear_minus : 1;
    uint8_t enter : 1;
    uint8_t plus : 1;
    uint8_t dial_cw : 1;
    uint8_t dial_ccw : 1;
    uint8_t minus : 1;
    uint8_t horn : 1;
    
    uint8_t PS : 1;
    uint8_t pedal_connected : 1;
    uint8_t power_connected : 1;
    uint8_t calibated : 1; //not sure (or ready to use?)
    uint8_t unknown: 4; //always 0x5?
    
    uint16_t wheel : 16; //looks to be 14 bits. what could be the two other bits?
    uint8_t gasPedal;
    uint8_t brakePedal;
} dfgt_report_t;

//G25 Racing Wheel
typedef struct TU_ATTR_PACKED {
  uint8_t hat : 4;
  uint8_t cross : 1;
  uint8_t square : 1;
  uint8_t circle : 1;
  uint8_t triangle : 1;
  
  uint8_t R1 : 1;
  uint8_t L1 : 1;
  uint8_t R2 : 1;
  uint8_t L2 : 1;

  uint8_t select : 1;
  uint8_t start : 1;
  uint8_t R3 : 1;
  uint8_t L3 : 1;
  
  uint8_t shifter_1 : 1;
  uint8_t shifter_2 : 1;
  uint8_t shifter_3 : 1;
  uint8_t shifter_4 : 1;
  uint8_t shifter_5 : 1;
  uint8_t shifter_6 : 1;
  uint8_t shifter_r : 1;
  uint8_t : 1; //always 0
  
  uint8_t pedal_disconnected : 1;
  uint8_t power_connected : 1; //(power or calibrated? , not sure)
  uint16_t wheel : 14; // test!
  
  uint8_t gasPedal;
  uint8_t brakePedal;
  uint8_t clutchPedal;

  uint8_t shifter_x;
  uint8_t shifter_y;

  uint8_t shifter : 1;
  uint8_t unknown : 1; //always 1
  uint8_t : 1; //always 0
  uint8_t shifter_stick_down : 1;
  uint8_t : 1; //always 0
  uint8_t : 1; //always 0
  uint8_t : 1; //always 0
  uint8_t : 1; //always 0

} g25_report_t;

//G27 Racing Wheel
typedef struct TU_ATTR_PACKED {
  uint8_t hat : 4;
  uint8_t cross : 1;
  uint8_t square : 1;
  uint8_t circle : 1;
  uint8_t triangle : 1;
  
  uint8_t R1 : 1;
  uint8_t L1 : 1;
  uint8_t R2 : 1;
  uint8_t L2 : 1;
  uint8_t select : 1;
  uint8_t start : 1;
  uint8_t R3 : 1;
  uint8_t L3 : 1;

  uint8_t R4 : 1;
  uint8_t R5 : 1;
  uint8_t shifter_1 : 1;
  uint8_t shifter_2 : 1;
  uint8_t shifter_3 : 1;
  uint8_t shifter_4 : 1;
  uint8_t shifter_5 : 1;
  uint8_t shifter_6 : 1;
  
  uint8_t L4 : 1;
  uint8_t L5 : 1;
  uint16_t wheel : 14;
  
  uint8_t gasPedal;
  uint8_t brakePedal;
  uint8_t clutchPedal;
  
  uint8_t shifter_x;
  uint8_t shifter_y;
  
  uint8_t shifter_r : 1;
  uint8_t pedal_disconnected : 1;
  uint8_t calibrated : 1; // not sure
  uint8_t shifter_connected : 1;
  uint8_t unknown: 1; //always 1
  uint8_t : 1; //always 0
  uint8_t shifter_stick_down : 1;
  uint8_t : 1; // always 0
      
} g27_report_t;

//G29 Racing Wheel (PS3/PS4 switch set to "PS3")
typedef struct TU_ATTR_PACKED {
  // byte 1: hat/buttons
  uint8_t hat : 4;
  uint8_t cross : 1;
  uint8_t square : 1;
  uint8_t circle : 1;
  uint8_t triangle : 1;

  uint8_t R1 : 1;
  uint8_t L1 : 1;
  uint8_t R2 : 1;
  uint8_t L2 : 1;
  uint8_t share : 1;
  uint8_t options : 1;
  uint8_t R3 : 1;
  uint8_t L3 : 1;

  uint8_t shifter_1 : 1;
  uint8_t shifter_2 : 1;
  uint8_t shifter_3 : 1;
  uint8_t shifter_4 : 1;
  uint8_t shifter_5 : 1;
  uint8_t shifter_6 : 1;
  uint8_t shifter_r : 1;
  uint8_t plus : 1;

  uint8_t minus : 1;
  uint8_t dial_cw : 1;
  uint8_t dial_ccw : 1;
  uint8_t enter : 1;
  uint8_t PS : 1;
  uint8_t : 3; // unknown

  uint16_t wheel : 16;

  uint8_t gasPedal;
  uint8_t brakePedal;
  uint8_t clutchPedal;

  uint8_t shifter_x;
  uint8_t shifter_y;

  uint8_t : 6; // unknown
  uint8_t shifter_stick_down : 1;
  uint8_t : 1; // unknown

} g29_report_t;
