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
/*******************************************************************************/

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


/*
Im not sure if its needed but I edited some TUSB options.
Could not find a way to do this from code.

tusb_option.h
CFG_TUD_ENDPOINT0_SIZE 8

tusb_config_rp2040.h
CFG_TUD_HID_EP_BUFSIZE 8

hid_host.h
CFG_TUH_HID_EPIN_BUFSIZE 8
CFG_TUH_HID_EPOUT_BUFSIZE 8
*/

// USB Host object
Adafruit_USBH_Host USBHost;

// USB Device object
Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_NONE, 10, true);

// connected device
uint8_t wheel_addr = 0;
uint8_t wheel_idx = 0;

// initialization step
uint8_t mode_step = 0;
lg_wheel_type change_mode_to = NONE;
init_stage_status init_stage = DISCONNECTED;

// received command buffer (ffb, leds)
uint8_t cmd_buffer[7] = { 0x00 };
volatile uint8_t leds_buffer = { 0x00 };

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

void set_led(bool value) {
  #ifdef LED_BUILTIN
    gpio_put(LED_BUILTIN, value);
  #endif
}

void receive_device_descriptor(tuh_xfer_t *xfer) {
  if (XFER_RESULT_SUCCESS != xfer->result) {
    // failed to get device descriptor. query it again
    tuh_descriptor_get_device(wheel_addr, &desc, 18, receive_device_descriptor, 0);
    return;
  }

  /*
  Notes for G929:
  The Logitech G929 and G923 Playstation wheels start out by default in Playstation Force Feedback Mode. To switch to
  Classic Mode, issue the following byte sequence as an output report to the USB endpoint:
  Logitech G29 Playstation 3: Send this report first 0x21, 0x09, 0x00, 0x02, 0x00, 0x00, 0x10, 0x00
                              Send this report second 0xf8, 0x09, 0x05, 0x01
  Logitech G29 Playstation 4 0x30, 0xf8, 0x09, 0x05, 0x01
  Logitech G923 Playstation 0x30, 0xf8, 0x09, 0x05, 0x01
  */

  uint16_t vid;
  uint16_t pid;
  tuh_vid_pid_get(wheel_addr, &vid, &pid);

  // check if wheel is in compatibility or native mode
  if ( ((pid_df == pid) || (pid_dfp == pid) || (pid_g25 == pid)) && (0x1350 == (desc.bcdDevice & 0xfff0)) ) // G29 in compatibility mode
    change_mode_to = G29;
  else if ( ((pid_df == pid) || (pid_dfp == pid)) && (0x1300 == (desc.bcdDevice & 0xff00)) ) // DFGT in compatibility mode
    change_mode_to = DFGT;
  else if ( ((pid_df == pid) || (pid_dfp == pid) || (pid_g25 == pid)) && (0x1230 == (desc.bcdDevice & 0xfff0)) ) // G27 in compatibility mode
    change_mode_to = G27;
  else if ( ((pid_df == pid) || (pid_dfp == pid)) && (0x1200 == (desc.bcdDevice & 0xff00)) ) // G25 in compatibility mode
    change_mode_to = G25;
  else if ( (pid_df == pid) && (0x1000 == (desc.bcdDevice & 0xf000)) ) // DFP in compatibility mode
    change_mode_to = DFP;
  else // native mode
    change_mode_to = NONE;

  // force a specific mode
  if (force_input_mode != NONE) {
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
    else
      change_mode_to = NONE;
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

  
  if(buffer[0] == 0xf8) { // Extended Command
    /*
    0x01 Change Mode to Driving Force Pro
    0x02 Change Wheel Range to 200 Degrees
    0x03 Change Wheel Range to 900 Degrees
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
      leds_buffer = buffer[2];
    } else if (ext_cmd == 0x02 || ext_cmd == 0x03 || ext_cmd == 0x81) { // Wheel Range Change
      // skipt for now. need to test
      return;
    } else { //mode change commands
      // skip as we curently can't change our output mode at runtime
      return;
    }
  }


  // todo handle spring effect differences for Logitech Formula Force GP nad Driving Force series?
//  if ((buffer[0] == 0x00 || buffer[0] == 0x01) && buffer[1] == 0x01) { // Download Force, Download and Play Force, Spring
//  }

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

    // set next stage
    init_stage = READING_DESCRIPTOR;

    // Get Device Descriptor
    tuh_descriptor_get_device(dev_addr, &desc, 18, receive_device_descriptor, 0);
    
    //set_led(HIGH);
    //tuh_hid_receive_report(dev_addr, idx);
  }
}

void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t idx) {
  if (dev_addr == wheel_addr && idx == wheel_idx) {
    wheel_addr = 0;
    wheel_idx = 0;
    mode_step = 0;
    
    // set next stage
    init_stage = DISCONNECTED;
    change_mode_to = NONE;
    
    tud_disconnect(); // disconnect to host
    memset(&generic_report, 0, sizeof(generic_report));
    set_led(LOW);
    #ifdef BOARD_RGB_PIN
      ledStrip.fill( WS2812::RGB(0, 0, 0) );
      ledStrip.show();
    #endif
  }
}

void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t idx, uint8_t const* report, uint16_t len) {

  // safe check
  if (len > 0 && dev_addr == wheel_addr && idx == wheel_idx) {
    // map the received report to generic_output
    map_input(report);

    // now map the generic_output to the output_mode
    map_output();
  }

  // receive next report
  tuh_hid_receive_report(dev_addr, idx);
}


void setup() {
  Serial1.end();
  Serial.end();

  rp2040.enableDoubleResetBootloader();

  //Configure led pin
  #ifdef LED_BUILTIN
    gpio_init(LED_BUILTIN);
    gpio_set_dir(LED_BUILTIN, 1);
    gpio_put(LED_BUILTIN, LOW);
  #endif

  #if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
    // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
    TinyUSB_Device_Init(0);
  #endif

  usb_hid.setReportCallback(NULL, hid_set_report_callback);

  // set usb device properties
  TinyUSBDevice.setID(0x046d, usb_product_id);
  TinyUSBDevice.setManufacturerDescriptor("Logitech");
  TinyUSBDevice.setProductDescriptor(usb_string_product);
  TinyUSBDevice.setVersion(usb_bcd_version); // Set bcdUSB version e.g 1.0, 2.0, 2.1
  TinyUSBDevice.setDeviceVersion(usb_bcd_device_version); // Set bcdDevice version

  tud_disconnect();
  usb_hid.begin();
//
//  // wait until device mounted
//  while ( !TinyUSBDevice.mounted() ) delay(1);
  
}

void loop() {
  static uint8_t last_cmd_buffer[7] { 0x00 };
  static output_report_t last_report { 0x00 };

  //initialization

  static uint32_t last_millis = 0;

  
  if (init_stage == SENDING_CMDS) {

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
    else
      cmd_mode = &cmd_mode_none;

    if (last_millis == 0) { // force an initial delay
      last_millis = millis();
    } else if (millis() - last_millis > 20) { // delay commands
      if (mode_step < cmd_mode->cmd_count) {
        if (tuh_hid_send_report(wheel_addr, wheel_idx, 0, &cmd_mode->cmd[7*(mode_step)], 7)) {
          ++mode_step;
        }
      }
      // after all initialization commands are sent, disconnect/reconnect the device to force host re-enumeration
      if(mode_step >= cmd_mode->cmd_count) {
        last_millis == 0;
        init_stage = RESTARTING; // set next stage
        
        // no mode change was sent. wheel must be in native mode now. starts receiving inputs!
        if (change_mode_to == NONE) {
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

  // send hid report to host
  if (usb_hid.ready()) {
    if (memcmp(&last_report, &out_report, sizeof(out_report))) {
      usb_hid.sendReport(0, &out_report, sizeof(out_report));
      memcpy(&last_report, &out_report, sizeof(out_report));
    }
  }
  
  // send command to device
  if (memcmp(last_cmd_buffer, cmd_buffer, sizeof(cmd_buffer))) {
    if (init_stage == READY && wheel_addr) {
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

void setup1() {
  uint32_t cpu_hz = clock_get_hz(clk_sys);
  if ( cpu_hz != 120000000UL && cpu_hz != 240000000UL ) {
    while ( !Serial ) delay(10);   // wait for native usb
    Serial.printf("Error: CPU Clock = %u, PIO USB require CPU clock must be multiple of 120 Mhz\r\n", cpu_hz);
    Serial.printf("Change your CPU Clock to either 120 or 240 Mhz in Menu->CPU Speed \r\n", cpu_hz);
    while (1) delay(1);
  }

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
  // Note: For rp2040 pico-pio-usb, calling USBHost.begin() on core1 will have most of the
  // host bit-banging processing works done in core1 to free up core0 for other works
  USBHost.begin(1);
}
void loop1() {
  USBHost.task();
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

  } else if (pid == pid_g29) { // G29

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

  } else if (pid == pid_ffgp) { // Formula Force GP

    // map the received report to output report
    ffgp_report_t* input_report = (ffgp_report_t*)report;

    generic_report.wheel_precision = wheel_10bits;
    generic_report.pedals_precision_16bits = false;
    
    generic_report.wheel_10 = input_report->wheel;
    generic_report.gasPedal_8 = input_report->gasPedal;
    generic_report.brakePedal_8 = input_report->brakePedal;
    
    generic_report.cross = input_report->cross;
    generic_report.square = input_report->square;
    generic_report.circle = input_report->circle;
    generic_report.triangle = input_report->triangle;
    generic_report.R1 = input_report->R1;
    generic_report.L1 = input_report->L1;
  }
  // todo add input support for the momo
}

void map_output() {
  // shift axis values
  const bool pedals_output_precision_16bits = false;

  #if WHEEL_TYPE == WHEEL_T_FFGP
    const uint8_t wheel_output_precision = wheel_10bits;
  #elif WHEEL_TYPE == WHEEL_T_DF
    const uint8_t wheel_output_precision = wheel_10bits;
  #elif WHEEL_TYPE == WHEEL_T_DFP
    const uint8_t wheel_output_precision = wheel_14bits;
  #elif WHEEL_TYPE == WHEEL_T_DFGT
    const uint8_t wheel_output_precision = wheel_14bits; //wheel_16bits;
  #elif WHEEL_TYPE == WHEEL_T_G25
    const uint8_t wheel_output_precision = wheel_14bits;
  #elif WHEEL_TYPE == WHEEL_T_G27
    const uint8_t wheel_output_precision = wheel_14bits;
  #endif

  uint16_t wheel;
  uint16_t gas;
  uint16_t brake;
  uint16_t clutch;

  if (wheel_output_precision == generic_report.wheel_precision) { // no conversion
    if (generic_report.wheel_precision == wheel_10bits) {
      wheel = generic_report.wheel_10;
    } else if (generic_report.wheel_precision == wheel_14bits) {
      wheel = generic_report.wheel_14;
    } else if (generic_report.wheel_precision == wheel_16bits) {
      wheel = generic_report.wheel_16;
    }
  } else if (generic_report.wheel_precision == wheel_10bits) {
    if (wheel_output_precision == wheel_14bits) {
      wheel = generic_report.wheel_10 << 4;
    } else if (wheel_output_precision == wheel_16bits) {
      wheel = generic_report.wheel_10 << 6;
    }
  } else if (generic_report.wheel_precision == wheel_14bits) {
    if (wheel_output_precision == wheel_10bits) {
      wheel = generic_report.wheel_14 >> 4;
    } else if (wheel_output_precision == wheel_16bits) {
      wheel = generic_report.wheel_14 << 2;
    }
  } else if (generic_report.wheel_precision == wheel_16bits) {
    if (wheel_output_precision == wheel_10bits) {
      wheel = generic_report.wheel_16 >> 6;
    } else if (wheel_output_precision == wheel_14bits) {
      wheel = generic_report.wheel_16 >> 2;
    }
  }

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

  // all wheels
  out_report.wheel = wheel;
  out_report.gasPedal = gas;
  out_report.brakePedal = brake;
  out_report.cross = generic_report.cross;
  out_report.square = generic_report.square;
  out_report.circle = generic_report.circle;
  out_report.triangle = generic_report.triangle;
  out_report.R1 = generic_report.R1;
  out_report.L1 = generic_report.L1;

  // all wheels except te FFGP
  #if WHEEL_TYPE != WHEEL_T_FFGP
    out_report.hat = generic_report.hat;
    out_report.R2 = generic_report.R2;
    out_report.L2 = generic_report.L2;
    out_report.select = generic_report.select;
    out_report.start = generic_report.start;
    out_report.R3 = generic_report.R3;
    out_report.L3 = generic_report.L3;
  #endif

  #if WHEEL_TYPE == WHEEL_T_FFGP || WHEEL_TYPE == WHEEL_T_DF || WHEEL_TYPE == WHEEL_T_DFP
    //combined pedals. mid: 0x7F. gas pulls to 0x0, brake pulls to 0xFF;
    out_report.pedals = (~(out_report.brakePedal>>1) - ~(out_report.gasPedal>>1)) + 0x7f;
  #endif

  #if WHEEL_TYPE == WHEEL_T_DFP || WHEEL_TYPE == WHEEL_T_DFGT
    out_report.gear_minus = generic_report.gear_minus;
    out_report.gear_plus = generic_report.gear_plus;
  #endif

  #if WHEEL_TYPE == WHEEL_T_DFGT
    out_report.enter = generic_report.enter;
    out_report.plus = generic_report.plus;
    out_report.dial_cw = generic_report.dial_cw;
    out_report.dial_ccw = generic_report.dial_ccw;
    out_report.minus = generic_report.minus;
    out_report.horn = generic_report.horn;
    out_report.PS = generic_report.PS;
  #endif

  #if WHEEL_TYPE == WHEEL_T_G27
    out_report.R4 = generic_report.R4;
    out_report.L5 = generic_report.L5;
  #endif
  
  #if WHEEL_TYPE == WHEEL_T_G25 || WHEEL_TYPE == WHEEL_T_G27
    out_report.clutchPedal = clutch;
    out_report.shifter_x = generic_report.shifter_x;
    out_report.shifter_y = generic_report.shifter_y;
    out_report.shifter_1 = generic_report.shifter_1;
    out_report.shifter_2 = generic_report.shifter_2;
    out_report.shifter_3 = generic_report.shifter_3;
    out_report.shifter_4 = generic_report.shifter_4;
    out_report.shifter_5 = generic_report.shifter_5;
    out_report.shifter_6 = generic_report.shifter_6;
    out_report.shifter_r = generic_report.shifter_r;
    out_report.shifter_stick_down = generic_report.shifter_stick_down;
  #endif
}
