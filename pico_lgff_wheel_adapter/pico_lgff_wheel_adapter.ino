/*******************************************************************************
 * Logitech FFB Wheel Adapter 
 * By Matheus Fraguas (sonik-br)
 * https://github.com/sonik-br/lgff_wheel_adapter
 * 
 * Settings are on the config.h file
 *  
 * Credits:
 * Heavily based on matlo's work on GIMX
 * I'm very grateful for it being released as a open source software
 * https://github.com/matlo
 * 
 * Other useful sources of information
 * https://opensource.logitech.com/wiki/Technical_Information/
 * https://github.com/mathijsvandenberg/g29emu
 * https://github.com/berarma/new-lg4ff
 * https://www.lfs.net/forum/thread/74115-LTWheelConf----Setup-Logitech-DFP-G25-G27-on-linux
 * 
 *******************************************************************************/

// pio-usb is required for rp2040 host
#include "pio_usb.h"
#include "Adafruit_TinyUSB.h"
#include "pico/stdlib.h"

#include "enums.h"
#include "wheel_ids.h"
#include "wheel_commands.h"
#include "config.h"
#include "reports.h"
#include "usb_descriptors.h"

// validation
#if defined(EXTERNAL_PEDAL_TYPE) && (!defined(PEDAL_GAS) || !defined(PEDAL_BRAKE))
  #error Need to set PEDAL_GAS and PEDAL_BRAKE analog pins
#endif
#if !defined(USE_TINYUSB) || defined(USE_TINYUSB_HOST)
  #error USB Stack must be configured as "Tools -> USB Stack -> Adafruit TinyUSB"
#endif
#if F_CPU != 120000000 && F_CPU != 240000000
  #error PIO USB require CPU Speed must be 120 or 240 MHz
#endif

// USB Host object
Adafruit_USBH_Host USBHost;

// USB Device object
Adafruit_USBD_HID usb_hid(NULL, 0, HID_ITF_PROTOCOL_NONE, 10, true);

// connected device
uint8_t wheel_addr = 0;
uint8_t wheel_idx = 0;
bool wheel_supports_cmd = 0;

// initialization step
uint8_t mode_step = 0;
uint8_t dongle_step = 0;
lg_wheel_type change_mode_to = NATIVE;
init_stage_status init_stage = DISCONNECTED;

// received command buffer (ffb, leds)
uint8_t cmd_buffer[7] = { 0x00 };
volatile uint8_t leds_buffer = { 0x00 };

#ifdef EXTERNAL_PEDAL_TYPE
  uint32_t external_pedals_values = ~0x0;
#endif

// report to hold input from any wheel
generic_report_t generic_report;


#ifdef BOARD_RGB_PIN
  #include "src/Pico_WS2812/WS2812.hpp" //https://github.com/ForsakenNGS/Pico_WS2812
  WS2812 ledStrip(
      BOARD_RGB_PIN,      // Data line is connected to pin 0. (GP0)
      1,                  // Strip is 1 LEDs long.
      pio0,               // Use PIO 0 for creating the state machine.
      3,                  // Index of the state machine that will be created for controlling the LED strip
      WS2812::FORMAT_GRB  // Pixel format used by the LED strip
  );
#endif

// holds the device description
tusb_desc_device_t desc;


// persisted variables. got the idea from:
// https://github.com/sanjay900/Santroller/blob/master/src/pico/main.cpp
#include "hardware/watchdog.h"
#define PERSISTED_OUTPUT_MODE_VALID 0x3A2F
static uint32_t __uninitialized_ram(persisted_output_mode);
static uint32_t __uninitialized_ram(persisted_output_mode_valid);
void reset_usb(void) {
  persisted_output_mode = output_mode;
  persisted_output_mode_valid = PERSISTED_OUTPUT_MODE_VALID;
  reboot();
}
void reboot(void) {
  watchdog_enable(1, false);
  for (;;) {
  }
}


// default report values
// WingMan Formula GP
fgp_report_t out_fgp_report {
  .wheel = 0x7f,
  .pedals = 0x7f,
  .gasPedal = 0xff,
  .brakePedal = 0xff,
};

// WingMan Formula Force GP (GT Force)
ffgp_report_t out_ffgp_report {
  .wheel = 0x7f,
  .pedals = 0x7f,
  .gasPedal = 0xff,
  .brakePedal = 0xff,
};

//Driving Force
df_report_t out_df_report {
  .wheel = 0x7f,
  //.pedal_connected = 1, // drivehub: clear. need to confirm from real a device
  //.power_connected = 1, // drivehub: set. need to confirm from real a device
  .pedals = 0x7f,
  .hat = 0x08,
  .calibated = 1, // drivehub: set
  //.unknown = 1, // drivehub: set
  .gasPedal = 0xff,
  .brakePedal = 0xff
};

//Driving Force Pro (GT Force Pro)
dfp_report_t out_dfp_report {
  .wheel = 0x7f,
  .hat = 0x08,
  .pedals = 0x7f,
  .gasPedal = 0xff,
  .brakePedal = 0xff,
  .pedal_connected = 1,
  .power_connected = 1,
  .calibated = 1,
  .unknown = 1,
};

//Driving Force GT
dfgt_report_t out_dfgt_report {
  .hat = 0x08,
  .pedal_connected = 1,
  .power_connected = 1,
  .calibated = 1,
  .unknown = 0x5,
  .wheel = 0x7f,
  .gasPedal = 0xff,
  .brakePedal = 0xff,
};

//G25 Racing Wheel
g25_report_t out_g25_report {
  .hat = 0x08,
  .pedal_disconnected = 0,
  .power_connected = 1,
  .wheel = 0x7f,
  .gasPedal = 0xff,
  .brakePedal = 0xff,
  .clutchPedal = 0xff,
  .shifter_x = 0x80,
  .shifter_y = 0x80,
  .shifter = 1,
  .unknown = 1
};

//G27 Racing Wheel
g27_report_t out_g27_report {
  .hat = 0x08,
  .wheel = 0x7f,
  .gasPedal = 0xff,
  .brakePedal = 0xff,
  .clutchPedal = 0xff,
  .shifter_x = 0x80,
  .shifter_y = 0x80,
  .pedal_disconnected = 0,
  .calibrated = 1,
  .shifter_connected = 1,
  .unknown = 1,
};

//Speed Force Wireless
sfw_report_t out_sfw_report {
  .wheel = 0x7f,
  .connected = 1, // 0 when not connecded? or not paired?
  .gasPedal = 0xff,
  .brakePedal = 0xff
};

//Momo Force
momofo_report_t out_momofo_report {
  .wheel = 0x7f,
  .pedals = 0x7f,
  .gasPedal = 0xff,
  .brakePedal = 0x00
};

//Momo Racing
momora_report_t out_momora_report {
  .wheel = 0x7f,
  .pedals = 0x7f,
  .gasPedal = 0xff,
  .brakePedal = 0xff
};

void set_led(bool value) {
  #ifdef LED_BUILTIN
    gpio_put(LED_BUILTIN, value);
  #endif
}

void reset_generic_report() {
  memset(&generic_report, 0, sizeof(generic_report));
  generic_report.hat = 0x08;
  generic_report.wheel_8 = 0x7F;
  generic_report.wheel_10 = 0x7F;
  generic_report.wheel_14 = 0x7F;
  generic_report.wheel_16 = 0x7F;
  generic_report.gasPedal_8 = 0xFF;
  generic_report.brakePedal_8 = 0xFF;
  generic_report.clutchPedal_8 = 0xFF;
  generic_report.gasPedal_16 = 0xFFFF;
  generic_report.brakePedal_16 = 0xFFFF;
  generic_report.clutchPedal_16 = 0xFFFF;
  generic_report.shifter_x = 0x80;
  generic_report.shifter_y = 0x80;
}

void handle_bcd_device(uint16_t vid, uint16_t pid, uint16_t bcdDevice) {
  /*
  Notes for G929:
  The Logitech G929 and G923 Playstation wheels start out by default in Playstation Force Feedback Mode. To switch to
  Classic Mode, issue the following byte sequence as an output report to the USB endpoint:
  Logitech G29 Playstation 3: Send this report first 0x21, 0x09, 0x00, 0x02, 0x00, 0x00, 0x10, 0x00
                              Send this report second 0xf8, 0x09, 0x05, 0x01
  Logitech G29 Playstation 4 0x30, 0xf8, 0x09, 0x05, 0x01
  Logitech G923 Playstation 0x30, 0xf8, 0x09, 0x05, 0x01
  */

//  uint16_t vid;
//  uint16_t pid;
//  tuh_vid_pid_get(wheel_addr, &vid, &pid);

  // check if wheel is in compatibility or native mode
  if (pid_g923ps == pid) // G923 in PS mode
    change_mode_to = G923;
  else if ( ((pid_df == pid) || (pid_dfp == pid) || (pid_g25 == pid)) && (0x1350 == (bcdDevice & 0xfff0)) ) // G29 in compatibility mode
    change_mode_to = G29;
  else if ( ((pid_df == pid) || (pid_dfp == pid)) && (0x1300 == (bcdDevice & 0xff00)) ) // DFGT in compatibility mode
    change_mode_to = DFGT;
  else if ( ((pid_df == pid) || (pid_dfp == pid) || (pid_g25 == pid)) && (0x1230 == (bcdDevice & 0xfff0)) ) // G27 in compatibility mode
    change_mode_to = G27;
  else if ( ((pid_df == pid) || (pid_dfp == pid)) && (0x1200 == (bcdDevice & 0xff00)) ) // G25 in compatibility mode
    change_mode_to = G25;
  else if ( (pid_df == pid) && (0x1000 == (bcdDevice & 0xf000)) ) // DFP in compatibility mode
    change_mode_to = DFP;
  else // native mode
    change_mode_to = NATIVE;

  // force a specific mode
  if (force_input_mode != NATIVE) {

    // try to best match input/output mode
    if (auto_mode && force_input_mode == AUTO) {
      // use the previous detected native mode (change_mode_to)
      auto detected_input = change_mode_to;
      switch (detected_input) {
        case DFP:
        {
          if (output_mode == WHEEL_T_DF)
            change_mode_to = DF;
          break;
        }
        case DFGT:
        case G25:
        {
          if (output_mode == WHEEL_T_DF)
            change_mode_to = DF;
          else if (output_mode == WHEEL_T_DFP)
            change_mode_to = DFP;
          break;
        }
        case G27:
        {
          if (output_mode == WHEEL_T_DF)
            change_mode_to = DF;
          else if (output_mode == WHEEL_T_DFP)
            change_mode_to = DFP;
          else if (output_mode == WHEEL_T_G25)
            change_mode_to = G25;
          break;
        }
        case G29:
        {
//          if (output_mode == WHEEL_T_DF)
//            change_mode_to = DF;
          if (output_mode == WHEEL_T_DFP)
            change_mode_to = DFP;
          else if (output_mode == WHEEL_T_DFGT)
            change_mode_to = DFGT;
          else if (output_mode == WHEEL_T_G25)
            change_mode_to = G25;
          else if (output_mode == WHEEL_T_G27)
            change_mode_to = G27;
          break;
        }
      }//end switch

      // force it into DF mode when emulating FGP, FFGP and SFW
      if (output_mode == WHEEL_T_FGP ||output_mode == WHEEL_T_FFGP ||output_mode == WHEEL_T_SFW) {
        // todo implement: only if it supports emulating another mode? 
        change_mode_to = DF;
      }

      //check if it's already in the correct mode, then skip mode change (by setting NATIVE)
      if (   (pid == pid_df   && (output_mode == WHEEL_T_DF || output_mode == WHEEL_T_FGP || output_mode == WHEEL_T_FFGP || output_mode == WHEEL_T_SFW))
          || (pid == pid_dfp  && output_mode == WHEEL_T_DFP)
          || (pid == pid_dfgt && output_mode == WHEEL_T_DFGT)
          || (pid == pid_g25  && output_mode == WHEEL_T_G25)
          || (pid == pid_g27  && output_mode == WHEEL_T_G27) ) {
        change_mode_to = NATIVE;
      }
    } else {
      if (force_input_mode == DF && pid != pid_df)
        change_mode_to = force_input_mode;
      else if (force_input_mode == DFP && pid != pid_dfp)
        change_mode_to = force_input_mode;
      else if (force_input_mode == DFGT && pid != pid_dfgt)
        change_mode_to = force_input_mode;
      else if (force_input_mode == G25 && pid != pid_g25)
        change_mode_to = force_input_mode;
      else if (force_input_mode == G27 && pid != pid_g27)
        change_mode_to = force_input_mode;
      else if (force_input_mode == G29 && pid != pid_g29)
        change_mode_to = force_input_mode;
      else if (force_input_mode == G923 && pid != pid_g923)
        change_mode_to = force_input_mode;
      else if (pid == pid_g923ps) // G923PS can only be changed to G923Classic
        change_mode_to = G923;
      else
        change_mode_to = NATIVE;
    }
  }

  #ifdef BOARD_RGB_PIN
    // set rgb led color. using this for debug
    if (pid == pid_df)
      ledStrip.fill( WS2812::RGB(20, 0, 0) ); //red
    else if (pid == pid_dfp)
      ledStrip.fill( WS2812::RGB(0, 20, 0) ); //green
    else if (pid == pid_dfgt)
      ledStrip.fill( WS2812::RGB(0, 0, 20) ); //blue
    else if (pid == pid_g25)
      ledStrip.fill( WS2812::RGB(20, 20, 0) ); //yellow
    else if (pid == pid_g27)
      ledStrip.fill( WS2812::RGB(20, 0, 20) ); //pink
    else if (pid == pid_g29)
      ledStrip.fill( WS2812::RGB(20, 20, 20) ); //white
    ledStrip.show();
  #endif

  // set next stage
  if (pid_sfw == pid)
    init_stage = CONFIGURING_DONGLE;
  else
    init_stage = SENDING_CMDS;
}

// receive commands from host and keep them to pass to device later
void hid_set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
  if (bufsize < sizeof(cmd_buffer))
    return;

  // autocentering command compatible with Formula Force EX 
//  if(buffer[0] == 0xfe && buffer[1] == 0x03)
//  {
//    //set_led(HIGH);
//    //return;
//  }


// On Momo Racing, logitech profiler puts the device in raw mode. also sends longer commands
// 0x07, 0x03, 0x89, 0x00, 0x00, 0x00, 0x00, 0x00, // reserved, do not use
// 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // turn on Raw Mode
// 0x09, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // led
// On Momo Force it also sends some longer commands
// 81, 0b, 82, 83, bb, 00, cc, 00,
  
  if(buffer[0] == 0xf8) { // Extended Command
    /*
    0x01 Change Mode to Driving Force Pro
    0x02 Change Wheel Range to 200 Degrees
    0x03 Change Wheel Range to 900 Degrees
    0x06 Unknown. Some PS2 game sends it
    0x09 Change Device Mode
    0x0a Revert Identity
    0x10 Switch to G25 Identity with USB Detach
    0x11 Switch to G25 Identity without USB Detach
    0x12 Set RPM LEDs
    0x81 Wheel Range Change
    */
    uint8_t ext_cmd = buffer[1];

    if (ext_cmd == 0x12) { // Set RPM LEDs
      // todo use memcpy?
      // todo check if connected device does not support leds, then don't send?
      // todo momo force/racing does have two leds.
      leds_buffer = buffer[2];
    } else if (ext_cmd == 0x02 || ext_cmd == 0x03 || ext_cmd == 0x81) { // Wheel Range Change
      // skip for now. need to test
//      return;
    } else if (ext_cmd == 0x01 || ext_cmd == 0x10 || ext_cmd == 0x11 || ext_cmd == 0x09 || ext_cmd == 0x06) { //mode change commands
      // skip as we curently can't change our output mode at runtime
      
      if (!auto_mode)
        return;

      // testing
      bool detach = false;
      lg_wheel_output_type new_mode = output_mode;

      if (ext_cmd == 0x01) { // Change Mode to Driving Force Pro
        new_mode = WHEEL_T_DFP;
        detach = true;
      } else if (ext_cmd == 0x06) { // Unknow command. Looks to be "Change Mode to Driving Force Pro without USB Detach"
        return;
        //new_mode = WHEEL_T_DFP;
        //detach = false;
      } else if (ext_cmd == 0x10) { // Switch to G25 Identity with USB Detach
        new_mode = WHEEL_T_G25;
        detach = true;
      } else if (ext_cmd == 0x11) { // Switch to G25 Identity without USB Detach
        new_mode = WHEEL_T_G25;
        detach = false;
      } else if (ext_cmd == 0x09) { // Change Device Mode
        detach = buffer[3];
        if (buffer[2] == 0x00) { //  Logitech Driving Force EX
          new_mode = WHEEL_T_DF;
        } else if (buffer[2] == 0x01) { // Logitech Driving Force Pro
          new_mode = WHEEL_T_DFP;
        } else if (buffer[2] == 0x02) { // Logitech G25 Racing Wheel
          new_mode = WHEEL_T_G25;
        } else if (buffer[2] == 0x03) { // Logitech Driving Force GT
          new_mode = WHEEL_T_DFGT;
        } else if (buffer[2] == 0x04) { // Logitech G27 Racing Wheel
          new_mode = WHEEL_T_G27;
        }
      }

      // forcing detach
      detach = true;

      if (new_mode != output_mode) {
        if (detach)
          tud_disconnect();
        delay(2000); // testing. probably not needed.
        output_mode = new_mode;
        if (detach) {
          reset_usb();
        }
      }
      
      return;
    }
  } else { // non extended command

    uint8_t cmd = buffer[0] & 0xf;
    
    // todo handle spring effect differences for Logitech Formula Force GP and Driving Force series?
    // friction only supported on DFP, G25, DFGT, G27 (also g29, g920?)
    if (cmd == 0x00 || cmd == 0x01) { // Download Force, Download and Play Force
      // code below does nothing... yet.
      if (buffer[1] == 0x01) { // Spring
        // todo check if connected devices needs translation
        uint8_t k1 = (buffer[4] & 0x07);
        uint8_t k2 = (buffer[4] & 0x70) >> 4;
        if (k1 == 0x05)
          k1 = 0x06;
        else if (k1 == 0x06)
          k1 = 0x05;
        if (k2 == 0x05)
          k2 = 0x06;
        else if (k2 == 0x06)
          k2 = 0x05;
        //buffer[4] = (k2<<4) | (k1);
      } else if (buffer[1] == 0x03) { // Auto-Centering Spring
        uint8_t k1 = (buffer[3] & 0x07);
        uint8_t k2 = (buffer[4] & 0x07);
        if (k1 == 0x05)
          k1 = 0x06;
        else if (k1 == 0x06)
          k1 = 0x05;
        if (k2 == 0x05)
          k2 = 0x06;
        else if (k2 == 0x06)
          k2 = 0x05;
//        buffer[3] = k1;
//        buffer[4] = k2;
      } else if (buffer[1] == 0x0e) { // Friction
      } else if (buffer[1] == 0x0b) { // High-Resolution Spring (all devices supports?)
      } else if (buffer[1] == 0x0c) { // High-Resolution Damper (all devices supports?)
      } else if (buffer[1] == 0x0d) { // High-Resolution Auto-Centering Spring (all devices supports?)
      }
    } // END Download Force, Download and Play Force
    else if (cmd == 0x07) { // Unknown (seen on logitech profiler with momo racing)
      return; // skip
    } else if (cmd == 0x08) { // Normal mode
      return; // skip
    } else if (cmd == 0x0b) { // Raw mode
      return; // skip
    }

  }

  memcpy(cmd_buffer, buffer, sizeof(cmd_buffer));
}
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t idx, uint8_t const* report_desc, uint16_t desc_len) {
  uint16_t vid;
  uint16_t pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);

  if ((vid == 0x046d) && ((pid & 0xff00) == 0xc200)) { // device is a logitech wheel
    wheel_addr = dev_addr;
    wheel_idx = idx;
    mode_step = 0;
    dongle_step = 0;

    wheel_supports_cmd = (pid != pid_fgp);

    init_stage = DISCONNECTED;

    // Get Device Descriptor
//    tuh_descriptor_get_device(dev_addr, &desc, 18, receive_device_descriptor, 0);
    tusb_desc_device_t desc_device;
    for (uint8_t i = 0; i < 5; ++i) {
      if (tuh_descriptor_get_device(dev_addr, &desc_device, 18, NULL, 0)) {
        handle_bcd_device(vid, pid, desc_device.bcdDevice);
        break;
      }
      delay(10);
    }
      
    
    //set_led(HIGH);
    //tuh_hid_receive_report(dev_addr, idx);
  }
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t idx) {
  if (dev_addr == wheel_addr && idx == wheel_idx) {
    wheel_addr = 0;
    wheel_idx = 0;
    mode_step = 0;
    dongle_step = 0;
    
    // set next stage
    init_stage = DISCONNECTED;
    change_mode_to = NATIVE;
    
//    tud_disconnect(); // disconnect to host
    reset_generic_report();
    
    set_led(LOW);
    #ifdef BOARD_RGB_PIN
      ledStrip.fill( WS2812::RGB(0, 0, 0) );
      ledStrip.show();
    #endif
    
//    persisted_output_mode_valid = 0x0;
//    reboot();
    
  }
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t idx, uint8_t const* report, uint16_t len) {
  // safe check
  if (len > 0 && dev_addr == wheel_addr && idx == wheel_idx) {

    reset_generic_report();
    
    // map the received report to generic_output
    map_input(report);

//    // now map the generic_output to the output_mode
//    map_output();

  }

  // receive next report
  tuh_hid_receive_report(dev_addr, idx);
}


void setup() {
  Serial.end();

  rp2040.enableDoubleResetBootloader();

  //Configure led pin
  #ifdef LED_BUILTIN
    gpio_init(LED_BUILTIN);
    gpio_set_dir(LED_BUILTIN, 1);
    gpio_put(LED_BUILTIN, LOW);
  #endif

  //using arduino code to keep it simple
  #ifdef EXTERNAL_PEDAL_TYPE
    pinMode(PEDAL_GAS, INPUT);
    pinMode(PEDAL_BRAKE, INPUT);
  #endif

  #if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
    // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
    TinyUSB_Device_Init(0);
  #endif

  // change output mode if it was persisted during a reboot
  if (persisted_output_mode_valid == PERSISTED_OUTPUT_MODE_VALID)
    output_mode = static_cast<lg_wheel_output_type>(persisted_output_mode);

  // set usb device properties
  TinyUSBDevice.setManufacturerDescriptor("Logitech");
  //usb_hid.setPollInterval(10);
  //usb_hid.enableOutEndpoint(true);

  switch (output_mode) {
    case WHEEL_T_FGP:
      TinyUSBDevice.setID(0x046d, pid_fgp);
      TinyUSBDevice.setProductDescriptor(usb_fgp_string_product);
      TinyUSBDevice.setVersion(usb_fgp_bcd_version); // Set bcdUSB version e.g 1.0, 2.0, 2.1
      TinyUSBDevice.setDeviceVersion(usb_fgp_bcd_device_version); // Set bcdDevice version
      usb_hid.setReportDescriptor(desc_fgp_hid_report, sizeof(desc_fgp_hid_report));
      break;
    case WHEEL_T_FFGP:
      TinyUSBDevice.setID(0x046d, pid_ffgp);
      TinyUSBDevice.setProductDescriptor(usb_ffgp_string_product);
      TinyUSBDevice.setVersion(usb_ffgp_bcd_version); // Set bcdUSB version e.g 1.0, 2.0, 2.1
      TinyUSBDevice.setDeviceVersion(usb_ffgp_bcd_device_version); // Set bcdDevice version
      usb_hid.setReportDescriptor(desc_ffgp_hid_report, sizeof(desc_ffgp_hid_report));
      break;
    case WHEEL_T_DF:
      TinyUSBDevice.setID(0x046d, pid_df);
      TinyUSBDevice.setProductDescriptor(usb_df_string_product);
      TinyUSBDevice.setVersion(usb_df_bcd_version); // Set bcdUSB version e.g 1.0, 2.0, 2.1
      TinyUSBDevice.setDeviceVersion(usb_df_bcd_device_version); // Set bcdDevice version
      usb_hid.setReportDescriptor(desc_df_hid_report, sizeof(desc_df_hid_report));
      break;
    case WHEEL_T_DFP:
      TinyUSBDevice.setID(0x046d, pid_dfp);
      TinyUSBDevice.setProductDescriptor(usb_dfp_string_product);
      TinyUSBDevice.setVersion(usb_dfp_bcd_version); // Set bcdUSB version e.g 1.0, 2.0, 2.1
      TinyUSBDevice.setDeviceVersion(usb_dfp_bcd_device_version); // Set bcdDevice version
      usb_hid.setReportDescriptor(desc_dfp_hid_report, sizeof(desc_dfp_hid_report));
      break;
    case WHEEL_T_DFGT:
      TinyUSBDevice.setID(0x046d, pid_dfgt);
      TinyUSBDevice.setProductDescriptor(usb_dfgt_string_product);
      TinyUSBDevice.setVersion(usb_dfgt_bcd_version); // Set bcdUSB version e.g 1.0, 2.0, 2.1
      TinyUSBDevice.setDeviceVersion(usb_dfgt_bcd_device_version); // Set bcdDevice version
      usb_hid.setReportDescriptor(desc_dfgt_hid_report, sizeof(desc_dfgt_hid_report));
      break;
    case WHEEL_T_G25:
      TinyUSBDevice.setID(0x046d, pid_g25);
      TinyUSBDevice.setProductDescriptor(usb_g25_string_product);
      TinyUSBDevice.setVersion(usb_g25_bcd_version); // Set bcdUSB version e.g 1.0, 2.0, 2.1
      TinyUSBDevice.setDeviceVersion(usb_g25_bcd_device_version); // Set bcdDevice version
      usb_hid.setReportDescriptor(desc_g25_hid_report, sizeof(desc_g25_hid_report));
      break;
    case WHEEL_T_G27:
      TinyUSBDevice.setID(0x046d, pid_g27);
      TinyUSBDevice.setProductDescriptor(usb_g27_string_product);
      TinyUSBDevice.setVersion(usb_g27_bcd_version); // Set bcdUSB version e.g 1.0, 2.0, 2.1
      TinyUSBDevice.setDeviceVersion(usb_g27_bcd_device_version); // Set bcdDevice version
      usb_hid.setReportDescriptor(desc_g27_hid_report, sizeof(desc_g27_hid_report));
      break;
    case WHEEL_T_MOMOFO:
      TinyUSBDevice.setID(0x046d, pid_momofo);
      TinyUSBDevice.setProductDescriptor(usb_momoforce_string_product);
      TinyUSBDevice.setVersion(usb_momoforce_bcd_version); // Set bcdUSB version e.g 1.0, 2.0, 2.1
      TinyUSBDevice.setDeviceVersion(usb_momoforce_bcd_device_version); // Set bcdDevice version
      usb_hid.setReportDescriptor(desc_momoforce_hid_report, sizeof(desc_momoforce_hid_report));
      break;
    case WHEEL_T_MOMORA:
      TinyUSBDevice.setID(0x046d, pid_momora);
      TinyUSBDevice.setProductDescriptor(usb_momoracing_string_product);
      TinyUSBDevice.setVersion(usb_momoracing_bcd_version); // Set bcdUSB version e.g 1.0, 2.0, 2.1
      TinyUSBDevice.setDeviceVersion(usb_momoracing_bcd_device_version); // Set bcdDevice version
      usb_hid.setReportDescriptor(desc_momoracing_hid_report, sizeof(desc_momoracing_hid_report));
      break;
    case WHEEL_T_SFW:
      TinyUSBDevice.setID(0x046d, pid_sfw);
      TinyUSBDevice.setProductDescriptor(usb_sfw_string_product);
      TinyUSBDevice.setVersion(usb_sfw_bcd_version); // Set bcdUSB version e.g 1.0, 2.0, 2.1
      TinyUSBDevice.setDeviceVersion(usb_sfw_bcd_device_version); // Set bcdDevice version
      usb_hid.setReportDescriptor(desc_sfw_hid_report, sizeof(desc_sfw_hid_report));
      // bMaxPower 0x1C (56 mA), bInterval 0x02
      break;
  }

  // override version info
  if (auto_mode && persisted_output_mode_valid != PERSISTED_OUTPUT_MODE_VALID) {
    // automode allways start as a Driving Force. But keep the desired Version info
    // only possible with some devices.
    switch (output_mode) {
      case WHEEL_T_DFP:
      case WHEEL_T_DFGT:
      case WHEEL_T_G25:
      case WHEEL_T_G27: {
        output_mode = WHEEL_T_DF;
        TinyUSBDevice.setID(0x046d, pid_df);
        TinyUSBDevice.setProductDescriptor(usb_df_string_product);
        usb_hid.setReportDescriptor(desc_df_hid_report, sizeof(desc_df_hid_report));
        break;
      }
    }
  }

  usb_hid.setReportCallback(NULL, hid_set_report_callback);

  // Leave Device disconnected until a wheel is connected in Host
  tud_disconnect();
  usb_hid.begin();
//
//  // wait until device mounted
//  while ( !TinyUSBDevice.mounted() ) delay(1);


  // USB Host (PIO)
  pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
  pio_cfg.pin_dp = PIN_USB_HOST_DP;

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
  /* https://github.com/sekigon-gonnoc/Pico-PIO-USB/issues/46 */
  pio_cfg.sm_tx      = 3;
  pio_cfg.sm_rx      = 2;
  pio_cfg.sm_eop     = 3;
  pio_cfg.pio_rx_num = 0;
  pio_cfg.pio_tx_num = 1;
  pio_cfg.tx_ch      = 9;
#endif /* ARDUINO_RASPBERRY_PI_PICO_W */

  USBHost.configure_pio_usb(1, &pio_cfg);

  // run host stack on controller (rhport) 1
  USBHost.begin(1);
}

void loop() {
  static uint8_t last_cmd_buffer[7] { 0x00 };
  static generic_report_t last_report { 0x00 };
  static uint32_t last_millis = 0;

  // TinyUSBDevice.task(); // no need to call here. Arduino Core handles this
  
  USBHost.task();

  //initialization

  if (init_stage == CONFIGURING_DONGLE) { // initialize wii wireless dongle. todo check if command was success

    const uint8_t dongle_cmd_init_comm[]   = { 0xAF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    const uint8_t dongle_cmd_change_addr[] = { 0xB2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    static uint8_t dongle_buffer[8] { 0x0 };

    if (last_millis == 0) { // force an initial delay
      last_millis = millis();
    } else if (millis() - last_millis > 200) { // delay commands. the dongle needs a longer delay than usual
      if (dongle_step == 0) {
        memcpy(dongle_buffer, dongle_cmd_init_comm, sizeof(dongle_buffer));
        if(tuh_hid_set_report(wheel_addr, wheel_idx, 0x00, HID_REPORT_TYPE_FEATURE, dongle_buffer, sizeof(dongle_buffer))) {
          ++dongle_step;
        }
      } else if(dongle_step == 1) {
        memcpy(dongle_buffer, dongle_cmd_change_addr, sizeof(dongle_buffer));
        dongle_buffer[1] = random(0, 255); // random address
        dongle_buffer[2] = random(0, 255);
        if(tuh_hid_set_report(wheel_addr, wheel_idx, 0x00, HID_REPORT_TYPE_FEATURE, dongle_buffer, sizeof(dongle_buffer))) {
          ++dongle_step;
        }
      } else { 
        init_stage = SENDING_CMDS;
      }
      last_millis = millis();
    }

  } else if (init_stage == SENDING_CMDS) {

    const struct init_mode_commands *cmd_mode;
    
    if (change_mode_to == DF)
      cmd_mode = &cmd_mode_df;
    else if (change_mode_to == DFP)
      cmd_mode = &cmd_mode_dfp;
    else if (change_mode_to == DFGT)
      cmd_mode = &cmd_mode_dfgt;
    else if (change_mode_to == G25)
      cmd_mode = &cmd_mode_g25;
    else if (change_mode_to == G27)
      cmd_mode = &cmd_mode_g27;
    else if (change_mode_to == G29)
      cmd_mode = &cmd_mode_g29;
    else if (change_mode_to == G923)
      cmd_mode = &cmd_mode_g923;
    else {
      uint16_t vid, pid;
      tuh_vid_pid_get(wheel_addr, &vid, &pid);
      if (pid_g923 == pid) // Set range
        cmd_mode = &cmd_mode_g923_range;
      else
        cmd_mode = &cmd_mode_none;
    }

    if (last_millis == 0) { // force an initial delay
      last_millis = millis();
    } else if (millis() - last_millis > 20) { // delay commands
      if (!wheel_supports_cmd) // skip commands
        mode_step = 255;
      if (mode_step < cmd_mode->cmd_count) {

        // should skip command? skip if all bytes are zero
        bool skip_command = true;
        for (uint8_t i = 0; i < 7; ++i) {
          if (cmd_mode->cmd[(7*(mode_step)) + i]) {
            skip_command = false;
            break;
          }
        }

        uint8_t report_id = (change_mode_to == G923) ? 0x30 : 0x0;

        if (skip_command) {
          ++mode_step;
        }
        else if (tuh_hid_send_report(wheel_addr, wheel_idx, report_id, &cmd_mode->cmd[7*(mode_step)], 7)) {
          ++mode_step;
        }
      }
      // after all initialization commands are sent, disconnect/reconnect the device to force host re-enumeration
      if(mode_step >= cmd_mode->cmd_count) {
        last_millis = 0;
        init_stage = RESTARTING; // set next stage
        
        // no mode change was sent. wheel must be in native mode now. starts receiving inputs!
        if (change_mode_to == NATIVE) {
          init_stage = READY; // set next stage
          set_led(LOW);
          delay(1000);
          set_led(HIGH);
          tud_connect(); //connect to host
          // wait until device mounted
          while ( !TinyUSBDevice.mounted() ) delay(1);
        
          tuh_hid_receive_report(wheel_addr, wheel_idx);
        }
        return;
      }
      last_millis = millis();
    }
  }
  
  //if using external pedals, override values
  bool external_pedals_updated = false;
  #ifdef EXTERNAL_PEDAL_TYPE
    // GAS
    static uint8_t last_external_gas = 255;
    static uint16_t gas_min = 4096;
    static uint16_t gas_max = 0;
    uint16_t gas_now = analogRead(PEDAL_GAS);
    gas_min = min(gas_min, gas_now);
    gas_max = max(gas_max, gas_now);
    uint8_t external_gas_value = map(gas_now, gas_min, gas_max, 0, 255);
    if (last_external_gas != external_gas_value)
      external_pedals_updated = true;
    last_external_gas = external_gas_value;

    // BRAKE
    static uint8_t last_external_brake = 255;
    static uint16_t brake_min = 4096;
    static uint16_t brake_max = 0;
    uint16_t brake_now = analogRead(PEDAL_BRAKE);
    brake_min = min(brake_min, brake_now);
    brake_max = max(brake_max, brake_now);
    uint8_t external_brake_value = map(brake_now, brake_min, brake_max, 0, 255);
    if (last_external_brake != external_brake_value)
      external_pedals_updated = true;
    last_external_brake = external_brake_value;
    
    external_pedals_values = (external_brake_value << 8) | external_gas_value;

//    if (external_pedals_updated) {
//      switch (output_mode) {
//        case WHEEL_T_FGP:
//          out_fgp_report.gasPedal = external_gas_value;
//          out_fgp_report.brakePedal = external_brake_value;
//          out_fgp_report.pedals = (~(out_fgp_report.brakePedal>>1) - ~(out_fgp_report.gasPedal>>1)) + 0x7f;
//          break;
//        case WHEEL_T_FFGP:
//          out_ffgp_report.gasPedal = external_gas_value;
//          out_ffgp_report.brakePedal = external_brake_value;
//          out_ffgp_report.pedals = (~(out_ffgp_report.brakePedal>>1) - ~(out_ffgp_report.gasPedal>>1)) + 0x7f;
//          break;
//        case WHEEL_T_DF:
//          out_df_report.gasPedal = external_gas_value;
//          out_df_report.brakePedal = external_brake_value;
//          out_df_report.pedals = (~(out_df_report.brakePedal>>1) - ~(out_df_report.gasPedal>>1)) + 0x7f;
//          break;
//        case WHEEL_T_DFP:
//          out_dfp_report.gasPedal = external_gas_value;
//          out_dfp_report.brakePedal = external_brake_value;
//          out_dfp_report.pedals = (~(out_dfp_report.brakePedal>>1) - ~(out_dfp_report.gasPedal>>1)) + 0x7f;
//          break;
//        case WHEEL_T_DFGT:
//          out_dfgt_report.gasPedal = external_gas_value;
//          out_dfgt_report.brakePedal = external_brake_value;
//          break;
//        case WHEEL_T_G25:
//          out_g25_report.gasPedal = external_gas_value;
//          out_g25_report.brakePedal = external_brake_value;
//          break;
//        case WHEEL_T_G27:
//          out_g27_report.gasPedal = external_gas_value;
//          out_g27_report.brakePedal = external_brake_value;
//          break;
//        case WHEEL_T_SFW:
//          out_sfw_report.gasPedal = external_gas_value;
//          out_sfw_report.brakePedal = external_brake_value;
//          break;
//      }
//      
//      external_pedals_values = (external_brake_value << 8) | external_gas_value;
//    }
  #endif

  // input report was updated?
  bool report_was_updated = false;
  if (memcmp(&last_report, &generic_report, sizeof(generic_report))) {
    report_was_updated = true;
    memcpy(&last_report, &generic_report, sizeof(generic_report));
  } else if (external_pedals_updated) {
    report_was_updated = true;
  }

  // map generic_output to the output_mode
  if (report_was_updated)
    map_output();

  // send hid report to host
  if (report_was_updated && usb_hid.ready()) {
    switch (output_mode) {
      case WHEEL_T_FGP:
        usb_hid.sendReport(0, &out_fgp_report, sizeof(out_fgp_report));
        break;
      case WHEEL_T_FFGP:
        usb_hid.sendReport(0, &out_ffgp_report, sizeof(out_ffgp_report));
        break;
      case WHEEL_T_DF:
        usb_hid.sendReport(0, &out_df_report, sizeof(out_df_report));
        break;
      case WHEEL_T_DFP:
        usb_hid.sendReport(0, &out_dfp_report, sizeof(out_dfp_report));
        break;
      case WHEEL_T_DFGT:
        usb_hid.sendReport(0, &out_dfgt_report, sizeof(out_dfgt_report));
        break;
      case WHEEL_T_G25:
        usb_hid.sendReport(0, &out_g25_report, sizeof(out_g25_report));
        break;
      case WHEEL_T_G27:
        usb_hid.sendReport(0, &out_g27_report, sizeof(out_g27_report));
        break;
      case WHEEL_T_MOMOFO:
        usb_hid.sendReport(0, &out_momofo_report, sizeof(out_momofo_report));
        break;
      case WHEEL_T_MOMORA:
          usb_hid.sendReport(0, &out_momora_report, sizeof(out_momora_report));
        break;
      case WHEEL_T_SFW:
        usb_hid.sendReport(0, &out_sfw_report, sizeof(out_sfw_report));
        break;
    }
  }
  
  // send command to device
  if (memcmp(last_cmd_buffer, cmd_buffer, sizeof(cmd_buffer))) {
    if (init_stage == READY && wheel_addr && wheel_supports_cmd) {
      tuh_hid_send_report(wheel_addr, wheel_idx, 0, cmd_buffer, sizeof(cmd_buffer));
    }
    memcpy(last_cmd_buffer, cmd_buffer, sizeof(cmd_buffer));
  }

  //leds
  #ifdef BOARD_RGB_PIN
    static uint8_t last_leds = 0;
    if(last_leds != leds_buffer) {
      if (leds_buffer & (1<<4))
        ledStrip.fill( WS2812::RGB(60, 00, 0) );
      else if (leds_buffer & (1<<3))
        ledStrip.fill( WS2812::RGB(60, 50, 0) );
      else if (leds_buffer & (1<<2))
        ledStrip.fill( WS2812::RGB(40, 30, 0) );
      else if (leds_buffer & (1<<1))
        ledStrip.fill( WS2812::RGB(10, 40, 0) );
      else if (leds_buffer & 1)
        ledStrip.fill( WS2812::RGB(0, 20, 0) );
      else
        ledStrip.fill( WS2812::RGB(0, 0, 0) );
      last_leds = leds_buffer;
      ledStrip.show();
    }
  #endif

}

void map_input(uint8_t const* report) {
  uint16_t vid, pid;
  tuh_vid_pid_get(wheel_addr, &vid, &pid);

  if (pid == pid_df) { // Driving Force. most logitech wheels will start in this mode

    // map the received report to the generic report
    df_report_t* input_report = (df_report_t*)report;

    generic_report.wheel_precision = wheel_10bits;
    generic_report.pedals_precision_16bits = false;

    generic_report.wheel_10 = input_report->wheel;
    generic_report.gasPedal_8 = input_report->gasPedal;
    generic_report.brakePedal_8 = input_report->brakePedal;

    generic_report.hat = input_report->hat;
    generic_report.cross = input_report->cross;
    generic_report.square = input_report->square;
    generic_report.circle = input_report->circle;
    generic_report.triangle = input_report->triangle;
    generic_report.R1 = input_report->R1;
    generic_report.L1 = input_report->L1;      
    generic_report.R2 = input_report->R2;
    generic_report.L2 = input_report->L2;
    generic_report.R3 = input_report->R3;
    generic_report.L3 = input_report->L3;
    generic_report.select = input_report->select;
    generic_report.start = input_report->start;

  } else if (pid == pid_dfp) { // Driving Force Pro UNTESTED
    
    // map the received report to the generic report
    dfp_report_t* input_report = (dfp_report_t*)report;
    
    generic_report.wheel_precision = wheel_14bits;
    generic_report.pedals_precision_16bits = false;
    
    generic_report.wheel_14 = input_report->wheel;
    generic_report.gasPedal_8 = input_report->gasPedal;
    generic_report.brakePedal_8 = input_report->brakePedal;

    generic_report.hat = input_report->hat;
    generic_report.cross = input_report->cross;
    generic_report.square = input_report->square;
    generic_report.circle = input_report->circle;
    generic_report.triangle = input_report->triangle;
    generic_report.R1 = input_report->R1;
    generic_report.L1 = input_report->L1;
    generic_report.R2 = input_report->R2;
    generic_report.L2 = input_report->L2;
    generic_report.R3 = input_report->R3;
    generic_report.L3 = input_report->L3;
    generic_report.select = input_report->select;
    generic_report.start = input_report->start;
    generic_report.gear_minus = input_report->gear_minus;
    generic_report.gear_plus = input_report->gear_plus;

  } else if (pid == pid_dfgt) { // Driving Force GT

    // map the received report to the generic report
    dfgt_report_t* input_report = (dfgt_report_t*)report;

    generic_report.wheel_precision = wheel_14bits;
    generic_report.pedals_precision_16bits = false;
    
    generic_report.wheel_14 = input_report->wheel;
    generic_report.gasPedal_8 = input_report->gasPedal;
    generic_report.brakePedal_8 = input_report->brakePedal;

    generic_report.hat = input_report->hat;
    generic_report.cross = input_report->cross;
    generic_report.square = input_report->square;
    generic_report.circle = input_report->circle;
    generic_report.triangle = input_report->triangle;
    generic_report.R1 = input_report->R1;
    generic_report.L1 = input_report->L1;
    generic_report.R2 = input_report->R2;
    generic_report.L2 = input_report->L2;
    generic_report.R3 = input_report->R3;
    generic_report.L3 = input_report->L3;
    generic_report.select = input_report->select;
    generic_report.start = input_report->start;
    generic_report.gear_minus = input_report->gear_minus;
    generic_report.gear_plus = input_report->gear_plus;
    generic_report.dial_cw = input_report->dial_cw;
    generic_report.dial_ccw = input_report->dial_ccw;
    generic_report.enter = input_report->enter;
    generic_report.plus = input_report->plus;
    generic_report.minus = input_report->minus;
    generic_report.horn = input_report->horn;
    generic_report.PS = input_report->PS;

  } else if (pid == pid_g25) { // G25

    // map the received report to output report
    g25_report_t* input_report = (g25_report_t*)report;

    generic_report.wheel_precision = wheel_14bits;
    generic_report.pedals_precision_16bits = false;
    
    generic_report.wheel_14 = input_report->wheel;
    generic_report.gasPedal_8 = input_report->gasPedal;
    generic_report.brakePedal_8 = input_report->brakePedal;
    generic_report.clutchPedal_8 = input_report->clutchPedal;

    generic_report.hat = input_report->hat;
    generic_report.cross = input_report->cross;
    generic_report.square = input_report->square;
    generic_report.circle = input_report->circle;
    generic_report.triangle = input_report->triangle;
    generic_report.R1 = input_report->R1;
    generic_report.L1 = input_report->L1;
    generic_report.R2 = input_report->R2;
    generic_report.L2 = input_report->L2;
    generic_report.R3 = input_report->R3;
    generic_report.L3 = input_report->L3;
    generic_report.select = input_report->select;
    generic_report.start = input_report->start;
    generic_report.shifter_x = input_report->shifter_x;
    generic_report.shifter_y = input_report->shifter_y;
    generic_report.shifter_1 = input_report->shifter_1;
    generic_report.shifter_2 = input_report->shifter_2;
    generic_report.shifter_3 = input_report->shifter_3;
    generic_report.shifter_4 = input_report->shifter_4;
    generic_report.shifter_5 = input_report->shifter_5;
    generic_report.shifter_6 = input_report->shifter_6;
    generic_report.shifter_r = input_report->shifter_r;
    generic_report.shifter_stick_down = input_report->shifter_stick_down;
    
  } else if (pid == pid_g27) { // G27

    // map the received report to output report
    g27_report_t* input_report = (g27_report_t*)report;

    generic_report.wheel_precision = wheel_14bits;
    generic_report.pedals_precision_16bits = false;
    
    generic_report.wheel_14 = input_report->wheel;
    generic_report.gasPedal_8 = input_report->gasPedal;
    generic_report.brakePedal_8 = input_report->brakePedal;
    generic_report.clutchPedal_8 = input_report->clutchPedal;

    generic_report.hat = input_report->hat;
    generic_report.cross = input_report->cross;
    generic_report.square = input_report->square;
    generic_report.circle = input_report->circle;
    generic_report.triangle = input_report->triangle;
    generic_report.R1 = input_report->R1;
    generic_report.L1 = input_report->L1;
    generic_report.R2 = input_report->R2;
    generic_report.L2 = input_report->L2;
    generic_report.R3 = input_report->R3;
    generic_report.L3 = input_report->L3;
    generic_report.R4 = input_report->R4;
    generic_report.L4 = input_report->L4;
    generic_report.R5 = input_report->R5;
    generic_report.L5 = input_report->L5;
    generic_report.select = input_report->select;
    generic_report.start = input_report->start;
    generic_report.shifter_x = input_report->shifter_x;
    generic_report.shifter_y = input_report->shifter_y;
    generic_report.shifter_1 = input_report->shifter_1;
    generic_report.shifter_2 = input_report->shifter_2;
    generic_report.shifter_3 = input_report->shifter_3;
    generic_report.shifter_4 = input_report->shifter_4;
    generic_report.shifter_5 = input_report->shifter_5;
    generic_report.shifter_6 = input_report->shifter_6;
    generic_report.shifter_r = input_report->shifter_r;
    generic_report.shifter_stick_down = input_report->shifter_stick_down;

  } else if (pid == pid_g29 || pid == pid_g923) { // G29 or G923

    // map the received report to output report
    g29_report_t* input_report = (g29_report_t*)report;

    generic_report.wheel_precision = wheel_16bits;
    generic_report.pedals_precision_16bits = false;

    generic_report.wheel_16 = input_report->wheel;
    generic_report.gasPedal_8 = input_report->gasPedal;
    generic_report.brakePedal_8 = input_report->brakePedal;
    generic_report.clutchPedal_8 = input_report->clutchPedal;

    generic_report.hat = input_report->hat;
    generic_report.cross = input_report->cross;
    generic_report.square = input_report->square;
    generic_report.circle = input_report->circle;
    generic_report.triangle = input_report->triangle;
    generic_report.R1 = input_report->R1;
    generic_report.L1 = input_report->L1;
    generic_report.R2 = input_report->R2;
    generic_report.L2 = input_report->L2;
    generic_report.select = input_report->share;
    generic_report.start = input_report->options;
    generic_report.R3 = input_report->R3;
    generic_report.L3 = input_report->L3;
    generic_report.shifter_1 = input_report->shifter_1;
    generic_report.shifter_2 = input_report->shifter_2;
    generic_report.shifter_3 = input_report->shifter_3;
    generic_report.shifter_4 = input_report->shifter_4;
    generic_report.shifter_5 = input_report->shifter_5;
    generic_report.shifter_6 = input_report->shifter_6;
    generic_report.shifter_r = input_report->shifter_r;
    generic_report.plus = input_report->plus;
    generic_report.minus = input_report->minus;
    generic_report.dial_cw = input_report->dial_cw;
    generic_report.dial_ccw = input_report->dial_ccw;
    generic_report.enter = input_report->enter;
    generic_report.PS = input_report->PS;
    generic_report.shifter_x = input_report->shifter_x;
    generic_report.shifter_y = input_report->shifter_y;
    generic_report.shifter_stick_down = input_report->shifter_stick_down;

  } else if (pid == pid_fgp) { // Formula GP

    // map the received report to output report
    fgp_report_t* input_report = (fgp_report_t*)report;

    generic_report.wheel_precision = wheel_8bits;
    generic_report.pedals_precision_16bits = false;
    
    generic_report.wheel_8 = input_report->wheel;
    generic_report.gasPedal_8 = input_report->gasPedal;
    generic_report.brakePedal_8 = input_report->brakePedal;

    generic_report.hat = 0x8;
    generic_report.cross = input_report->cross;
    generic_report.square = input_report->square;
    generic_report.circle = input_report->circle;
    generic_report.triangle = input_report->triangle;
    generic_report.R1 = input_report->R1;
    generic_report.L1 = input_report->L1;

  } else if (pid == pid_ffgp) { // Formula Force GP

    // map the received report to output report
    ffgp_report_t* input_report = (ffgp_report_t*)report;

    generic_report.wheel_precision = wheel_10bits;
    generic_report.pedals_precision_16bits = false;
    
    generic_report.wheel_10 = input_report->wheel;
    generic_report.gasPedal_8 = input_report->gasPedal;
    generic_report.brakePedal_8 = input_report->brakePedal;

    generic_report.hat = 0x8;
    generic_report.cross = input_report->cross;
    generic_report.square = input_report->square;
    generic_report.circle = input_report->circle;
    generic_report.triangle = input_report->triangle;
    generic_report.R1 = input_report->R1;
    generic_report.L1 = input_report->L1;
    
  } else if (pid == pid_momofo) { // Momo Force

    // map the received report to output report
    momofo_report_t* input_report = (momofo_report_t*)report;

    generic_report.wheel_precision = wheel_8bits;
    generic_report.pedals_precision_16bits = false;
    
    generic_report.wheel_8 = input_report->wheel;
    generic_report.gasPedal_8 = input_report->gasPedal;
    generic_report.brakePedal_8 = ~input_report->brakePedal;

    generic_report.hat = 0x8;
    generic_report.cross = input_report->cross;
    generic_report.square = input_report->square;
    generic_report.circle = input_report->circle;
    generic_report.triangle = input_report->triangle;
    generic_report.R1 = input_report->R1;
    generic_report.L1 = input_report->L1;
    generic_report.select = input_report->select;
    generic_report.start = input_report->start;
    
  } else if (pid == pid_momora) { // Momo Racing

    // map the received report to output report
    momora_report_t* input_report = (momora_report_t*)report;

    generic_report.wheel_precision = wheel_8bits;
    generic_report.pedals_precision_16bits = false;
    
    generic_report.wheel_8 = input_report->wheel;
    generic_report.gasPedal_8 = input_report->gasPedal;
    generic_report.brakePedal_8 = input_report->brakePedal;

    generic_report.hat = 0x8;
    generic_report.cross = input_report->cross;
    generic_report.square = input_report->square;
    generic_report.circle = input_report->circle;
    generic_report.triangle = input_report->triangle;
    generic_report.R1 = input_report->R1;
    generic_report.L1 = input_report->L1;
    generic_report.select = input_report->select;
    generic_report.start = input_report->start;
    generic_report.gear_minus = input_report->gear_minus;
    generic_report.gear_plus = input_report->gear_plus;
    
  } else if (pid == pid_sfw) { // Speed Force Wireless

    // map the received report to output report
    sfw_report_t* input_report = (sfw_report_t*)report;

    generic_report.wheel_precision = wheel_10bits;
    generic_report.pedals_precision_16bits = false;

    if (input_report->hat_u) {
      if (input_report->hat_l)
        generic_report.hat = 0x7;
      else if (input_report->hat_r)
        generic_report.hat = 0x1;
      else
        generic_report.hat = 0x0;
    } else if (input_report->hat_d) {
      if (input_report->hat_l)
        generic_report.hat = 0x5;
      else if (input_report->hat_r)
        generic_report.hat = 0x3;
      else
        generic_report.hat = 0x4;
    } else if (input_report->hat_l) {
      generic_report.hat = 0x6;
    } else if (input_report->hat_r) {
      generic_report.hat = 0x2;
    } else {
      generic_report.hat = 0x8;
    }
    
    generic_report.wheel_10 = input_report->wheel;
    generic_report.gasPedal_8 = input_report->gasPedal;
    generic_report.brakePedal_8 = input_report->brakePedal;
    
    generic_report.cross = input_report->b;
    generic_report.square = input_report->one;
    generic_report.circle = input_report->a;
    generic_report.triangle = input_report->two;
    generic_report.select = input_report->minus;
    generic_report.start = input_report->plus;
    generic_report.PS = input_report->home;

    // if using external pedals, reuse the analog paddles as L1/R1
    #ifdef EXTERNAL_PEDAL_TYPE
      generic_report.R1 = input_report->gasPedal < 127;
      generic_report.L1 = input_report->brakePedal < 127;
    #endif
    
  }

}

void map_output() {
  // shift axis values
  const bool pedals_output_precision_16bits = false;
  uint8_t wheel_output_precision;

  switch (output_mode) {
    case WHEEL_T_FGP:
    case WHEEL_T_MOMOFO:
      wheel_output_precision = wheel_8bits;
      break;
    case WHEEL_T_FFGP:
    case WHEEL_T_DF:
    case WHEEL_T_SFW:
    case WHEEL_T_MOMORA:
      wheel_output_precision = wheel_10bits;
      break;
    case WHEEL_T_DFP:
    case WHEEL_T_DFGT:
    case WHEEL_T_G25:
    case WHEEL_T_G27:
      wheel_output_precision = wheel_14bits;
      break;
    default:
      wheel_output_precision = wheel_14bits;
      break;
  }

  uint16_t wheel;
  uint16_t gas;
  uint16_t brake;
  uint16_t clutch;

  if (wheel_output_precision == generic_report.wheel_precision) { // no conversion
    if (generic_report.wheel_precision == wheel_8bits) {
      wheel = generic_report.wheel_8;
    } else if (generic_report.wheel_precision == wheel_10bits) {
      wheel = generic_report.wheel_10;
    } else if (generic_report.wheel_precision == wheel_14bits) {
      wheel = generic_report.wheel_14;
    } else if (generic_report.wheel_precision == wheel_16bits) {
      wheel = generic_report.wheel_16;
    }
  } else if (generic_report.wheel_precision == wheel_8bits) {
    if (wheel_output_precision == wheel_10bits) {
      wheel = generic_report.wheel_8 << 2;
    } else if (wheel_output_precision == wheel_14bits) {
      wheel = generic_report.wheel_8 << 4;
    } else if (wheel_output_precision == wheel_16bits) {
      wheel = generic_report.wheel_8 << 6;
    }
  } else if (generic_report.wheel_precision == wheel_10bits) {
    if (wheel_output_precision == wheel_8bits) {
      wheel = generic_report.wheel_10 >> 2;
    } else if (wheel_output_precision == wheel_14bits) {
      wheel = generic_report.wheel_10 << 4;
    } else if (wheel_output_precision == wheel_16bits) {
      wheel = generic_report.wheel_10 << 6;
    }
  } else if (generic_report.wheel_precision == wheel_14bits) {
    if (wheel_output_precision == wheel_8bits) {
      wheel = generic_report.wheel_14 >> 6;
    }else if (wheel_output_precision == wheel_10bits) {
      wheel = generic_report.wheel_14 >> 4;
    } else if (wheel_output_precision == wheel_16bits) {
      wheel = generic_report.wheel_14 << 2;
    }
  } else if (generic_report.wheel_precision == wheel_16bits) {
    if (wheel_output_precision == wheel_8bits) {
      wheel = generic_report.wheel_16 >> 8;
    } else if (wheel_output_precision == wheel_10bits) {
      wheel = generic_report.wheel_16 >> 6;
    } else if (wheel_output_precision == wheel_14bits) {
      wheel = generic_report.wheel_16 >> 2;
    }
  }

  //if using external pedals, override input.
  #ifdef EXTERNAL_PEDAL_TYPE
    generic_report.pedals_precision_16bits = false;
    generic_report.gasPedal_8 = external_pedals_values & 0xFF;
    generic_report.brakePedal_8 = (external_pedals_values >> 8) & 0xFF;
  #endif

  if (pedals_output_precision_16bits == generic_report.pedals_precision_16bits) { // no conversion
    if (generic_report.pedals_precision_16bits) {
      gas = generic_report.gasPedal_16;
      brake = generic_report.brakePedal_16;
      clutch = generic_report.clutchPedal_16;
    } else {
      gas = generic_report.gasPedal_8;
      brake = generic_report.brakePedal_8;
      clutch = generic_report.clutchPedal_8;
    }
  } else if (pedals_output_precision_16bits && !generic_report.pedals_precision_16bits) {
      gas = generic_report.gasPedal_8 << 8;
      brake = generic_report.brakePedal_8 << 8;
      clutch = generic_report.clutchPedal_8 << 8;
  } else if (!pedals_output_precision_16bits && generic_report.pedals_precision_16bits) {
      gas = generic_report.gasPedal_16 >> 8;
      brake = generic_report.brakePedal_16 >> 8;
      clutch = generic_report.clutchPedal_16 >> 8;
  }

  switch (output_mode) {
    case WHEEL_T_FGP:
      map_fgp_out(wheel, gas, brake, clutch);
      break;
    case WHEEL_T_FFGP:
      map_ffgp_out(wheel, gas, brake, clutch);
      break;
    case WHEEL_T_DF:
      map_df_out(wheel, gas, brake, clutch);
      break;
    case WHEEL_T_DFP:
      map_dfp_out(wheel, gas, brake, clutch);
      break;
    case WHEEL_T_DFGT:
      map_dfgt_out(wheel, gas, brake, clutch);
      break;
    case WHEEL_T_G25:
      map_g25_out(wheel, gas, brake, clutch);
      break;
    case WHEEL_T_G27:
      map_g27_out(wheel, gas, brake, clutch);
      break;
    case WHEEL_T_MOMOFO:
      map_momofo_out(wheel, gas, brake, clutch);
      break;
    case WHEEL_T_MOMORA:
      map_momora_out(wheel, gas, brake, clutch);
      break;
    case WHEEL_T_SFW:
      map_sfw_out(wheel, gas, brake, clutch);
  }
}

void map_fgp_out(uint16_t wheel, uint16_t gas, uint16_t brake, uint16_t clutch) {
  out_fgp_report.wheel = wheel;
  out_fgp_report.gasPedal = gas;
  out_fgp_report.brakePedal = brake;
  out_fgp_report.cross = generic_report.cross;
  out_fgp_report.square = generic_report.square;
  out_fgp_report.circle = generic_report.circle;
  out_fgp_report.triangle = generic_report.triangle;
  out_fgp_report.R1 = generic_report.R1;
  out_fgp_report.L1 = generic_report.L1;

  //combined pedals. mid: 0x7F. gas pulls to 0x0, brake pulls to 0xFF;
  out_fgp_report.pedals = (~(out_fgp_report.brakePedal>>1) - ~(out_fgp_report.gasPedal>>1)) + 0x7f;
}

void map_ffgp_out(uint16_t wheel, uint16_t gas, uint16_t brake, uint16_t clutch) {
  out_ffgp_report.wheel = wheel;
  out_ffgp_report.gasPedal = gas;
  out_ffgp_report.brakePedal = brake;
  out_ffgp_report.cross = generic_report.cross;
  out_ffgp_report.square = generic_report.square;
  out_ffgp_report.circle = generic_report.circle;
  out_ffgp_report.triangle = generic_report.triangle;
  out_ffgp_report.R1 = generic_report.R1;
  out_ffgp_report.L1 = generic_report.L1;

  //combined pedals. mid: 0x7F. gas pulls to 0x0, brake pulls to 0xFF;
  out_ffgp_report.pedals = (~(out_ffgp_report.brakePedal>>1) - ~(out_ffgp_report.gasPedal>>1)) + 0x7f;
}

void map_df_out(uint16_t wheel, uint16_t gas, uint16_t brake, uint16_t clutch) {
  out_df_report.wheel = wheel;
  out_df_report.gasPedal = gas;
  out_df_report.brakePedal = brake;
  out_df_report.cross = generic_report.cross;
  out_df_report.square = generic_report.square;
  out_df_report.circle = generic_report.circle;
  out_df_report.triangle = generic_report.triangle;
  out_df_report.R1 = generic_report.R1;
  out_df_report.L1 = generic_report.L1;

  out_df_report.hat = generic_report.hat;
  out_df_report.R2 = generic_report.R2;
  out_df_report.L2 = generic_report.L2;
  out_df_report.select = generic_report.select;
  out_df_report.start = generic_report.start;
  out_df_report.R3 = generic_report.R3;
  out_df_report.L3 = generic_report.L3;

  //combined pedals. mid: 0x7F. gas pulls to 0x0, brake pulls to 0xFF;
  out_df_report.pedals = (~(out_df_report.brakePedal>>1) - ~(out_df_report.gasPedal>>1)) + 0x7f;
}

void map_dfp_out(uint16_t wheel, uint16_t gas, uint16_t brake, uint16_t clutch) {
  out_dfp_report.wheel = wheel;
  out_dfp_report.gasPedal = gas;
  out_dfp_report.brakePedal = brake;
  out_dfp_report.cross = generic_report.cross;
  out_dfp_report.square = generic_report.square;
  out_dfp_report.circle = generic_report.circle;
  out_dfp_report.triangle = generic_report.triangle;
  out_dfp_report.R1 = generic_report.R1;
  out_dfp_report.L1 = generic_report.L1;

  out_dfp_report.hat = generic_report.hat;
  out_dfp_report.R2 = generic_report.R2;
  out_dfp_report.L2 = generic_report.L2;
  out_dfp_report.select = generic_report.select;
  out_dfp_report.start = generic_report.start;
  out_dfp_report.R3 = generic_report.R3;
  out_dfp_report.L3 = generic_report.L3;

  //combined pedals. mid: 0x7F. gas pulls to 0x0, brake pulls to 0xFF;
  out_dfp_report.pedals = (~(out_dfp_report.brakePedal>>1) - ~(out_dfp_report.gasPedal>>1)) + 0x7f;

  out_dfp_report.gear_minus = generic_report.gear_minus;
  out_dfp_report.gear_plus = generic_report.gear_plus;
}

void map_dfgt_out(uint16_t wheel, uint16_t gas, uint16_t brake, uint16_t clutch) {
  out_dfgt_report.wheel = wheel;
  out_dfgt_report.gasPedal = gas;
  out_dfgt_report.brakePedal = brake;
  out_dfgt_report.cross = generic_report.cross;
  out_dfgt_report.square = generic_report.square;
  out_dfgt_report.circle = generic_report.circle;
  out_dfgt_report.triangle = generic_report.triangle;
  out_dfgt_report.R1 = generic_report.R1;
  out_dfgt_report.L1 = generic_report.L1;

  out_dfgt_report.hat = generic_report.hat;
  out_dfgt_report.R2 = generic_report.R2;
  out_dfgt_report.L2 = generic_report.L2;
  out_dfgt_report.select = generic_report.select;
  out_dfgt_report.start = generic_report.start;
  out_dfgt_report.R3 = generic_report.R3;
  out_dfgt_report.L3 = generic_report.L3;

  out_dfgt_report.gear_minus = generic_report.gear_minus;
  out_dfgt_report.gear_plus = generic_report.gear_plus;

  out_dfgt_report.enter = generic_report.enter;
  out_dfgt_report.plus = generic_report.plus;
  out_dfgt_report.dial_cw = generic_report.dial_cw;
  out_dfgt_report.dial_ccw = generic_report.dial_ccw;
  out_dfgt_report.minus = generic_report.minus;
  out_dfgt_report.horn = generic_report.horn;
  out_dfgt_report.PS = generic_report.PS;
}

void map_g25_out(uint16_t wheel, uint16_t gas, uint16_t brake, uint16_t clutch) {
  out_g25_report.wheel = wheel;
  out_g25_report.gasPedal = gas;
  out_g25_report.brakePedal = brake;
  out_g25_report.cross = generic_report.cross;
  out_g25_report.square = generic_report.square;
  out_g25_report.circle = generic_report.circle;
  out_g25_report.triangle = generic_report.triangle;
  out_g25_report.R1 = generic_report.R1;
  out_g25_report.L1 = generic_report.L1;

  out_g25_report.hat = generic_report.hat;
  out_g25_report.R2 = generic_report.R2;
  out_g25_report.L2 = generic_report.L2;
  out_g25_report.select = generic_report.select;
  out_g25_report.start = generic_report.start;
  out_g25_report.R3 = generic_report.R3;
  out_g25_report.L3 = generic_report.L3;

  out_g25_report.clutchPedal = clutch;
  out_g25_report.shifter_x = generic_report.shifter_x;
  out_g25_report.shifter_y = generic_report.shifter_y;
  out_g25_report.shifter_1 = generic_report.shifter_1;
  out_g25_report.shifter_2 = generic_report.shifter_2;
  out_g25_report.shifter_3 = generic_report.shifter_3;
  out_g25_report.shifter_4 = generic_report.shifter_4;
  out_g25_report.shifter_5 = generic_report.shifter_5;
  out_g25_report.shifter_6 = generic_report.shifter_6;
  out_g25_report.shifter_r = generic_report.shifter_r;
  out_g25_report.shifter_stick_down = generic_report.shifter_stick_down;
}

void map_g27_out(uint16_t wheel, uint16_t gas, uint16_t brake, uint16_t clutch) {
  out_g27_report.wheel = wheel;
  out_g27_report.gasPedal = gas;
  out_g27_report.brakePedal = brake;
  out_g27_report.cross = generic_report.cross;
  out_g27_report.square = generic_report.square;
  out_g27_report.circle = generic_report.circle;
  out_g27_report.triangle = generic_report.triangle;
  out_g27_report.R1 = generic_report.R1;
  out_g27_report.L1 = generic_report.L1;

  out_g27_report.hat = generic_report.hat;
  out_g27_report.R2 = generic_report.R2;
  out_g27_report.L2 = generic_report.L2;
  out_g27_report.select = generic_report.select;
  out_g27_report.start = generic_report.start;
  out_g27_report.R3 = generic_report.R3;
  out_g27_report.L3 = generic_report.L3;

  out_g27_report.R4 = generic_report.R4;
  out_g27_report.L4 = generic_report.L4;
  out_g27_report.R5 = generic_report.R5;
  out_g27_report.L5 = generic_report.L5;
  
  out_g27_report.clutchPedal = clutch;
  out_g27_report.shifter_x = generic_report.shifter_x;
  out_g27_report.shifter_y = generic_report.shifter_y;
  out_g27_report.shifter_1 = generic_report.shifter_1;
  out_g27_report.shifter_2 = generic_report.shifter_2;
  out_g27_report.shifter_3 = generic_report.shifter_3;
  out_g27_report.shifter_4 = generic_report.shifter_4;
  out_g27_report.shifter_5 = generic_report.shifter_5;
  out_g27_report.shifter_6 = generic_report.shifter_6;
  out_g27_report.shifter_r = generic_report.shifter_r;
  out_g27_report.shifter_stick_down = generic_report.shifter_stick_down;
}

void map_momofo_out(uint16_t wheel, uint16_t gas, uint16_t brake, uint16_t clutch) {
  out_momofo_report.wheel = wheel;
  out_momofo_report.gasPedal = gas;
  out_momofo_report.brakePedal = ~brake; // this is correct?
  out_momofo_report.cross = generic_report.cross;
  out_momofo_report.square = generic_report.square;
  out_momofo_report.circle = generic_report.circle;
  out_momofo_report.triangle = generic_report.triangle;
  out_momofo_report.R1 = generic_report.R1;
  out_momofo_report.L1 = generic_report.L1;
  out_momofo_report.select = generic_report.select;
  out_momofo_report.start = generic_report.start;

  //combined pedals. mid: 0x7F. gas pulls to 0x0, brake pulls to 0xFF;
  //out_momofo_report.pedals = (~(out_momofo_report.brakePedal>>1) - ~(out_momofo_report.gasPedal>>1)) + 0x7f;
}

void map_momora_out(uint16_t wheel, uint16_t gas, uint16_t brake, uint16_t clutch) {
  out_momora_report.wheel = wheel;
  out_momora_report.gasPedal = gas;
  out_momora_report.brakePedal = brake; // this is correct?
  out_momora_report.cross = generic_report.cross;
  out_momora_report.square = generic_report.square;
  out_momora_report.circle = generic_report.circle;
  out_momora_report.triangle = generic_report.triangle;
  out_momora_report.R1 = generic_report.R1;
  out_momora_report.L1 = generic_report.L1;
  out_momora_report.select = generic_report.select;
  out_momora_report.start = generic_report.start;

  out_dfgt_report.gear_minus = generic_report.gear_minus;
  out_dfgt_report.gear_plus = generic_report.gear_plus;

  //combined pedals. mid: 0x7F. gas pulls to 0x0, brake pulls to 0xFF;
  //out_momora_report.pedals = (~(out_momora_report.brakePedal>>1) - ~(out_momora_report.gasPedal>>1)) + 0x7f;
}

void map_sfw_out(uint16_t wheel, uint16_t gas, uint16_t brake, uint16_t clutch) {
  out_sfw_report.wheel = wheel;
  out_sfw_report.gasPedal = gas;
  out_sfw_report.brakePedal = brake;
  out_sfw_report.b = generic_report.cross;
  out_sfw_report.one = generic_report.square;
  out_sfw_report.a = generic_report.circle;
  out_sfw_report.two = generic_report.triangle;

  out_sfw_report.hat_u = 0;
  out_sfw_report.hat_d = 0;
  out_sfw_report.hat_l = 0;
  out_sfw_report.hat_r = 0;
  switch (generic_report.hat) {
    case 0x0:
      out_sfw_report.hat_u = 1;
      break;
    case 0x1:
      out_sfw_report.hat_u = 1;
      out_sfw_report.hat_r = 1;
      break;
    case 0x2:
      out_sfw_report.hat_r = 1;
      break;
    case 0x3:
      out_sfw_report.hat_d = 1;
      out_sfw_report.hat_r = 1;
      break;
    case 0x4:
      out_sfw_report.hat_d = 1;
      break;
    case 0x5:
      out_sfw_report.hat_d = 1;
      out_sfw_report.hat_l = 1;
      break;
    case 0x6:
      out_sfw_report.hat_l = 1;
      break;
    case 0x7:
      out_sfw_report.hat_u = 1;
      out_sfw_report.hat_l = 1;
      break;
    default:
      break;
  }

  out_sfw_report.minus = generic_report.select;
  out_sfw_report.plus = generic_report.start;
  out_sfw_report.home = generic_report.PS;
}
