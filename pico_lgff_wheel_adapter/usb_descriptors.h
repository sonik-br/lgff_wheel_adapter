#pragma once

//WingMan Formula GP
const char usb_fgp_string_product[] = "WingMan Formula GP";
const uint16_t usb_fgp_bcd_version = 0x0110;
const uint16_t usb_fgp_bcd_device_version = 0x00A0;

const uint8_t desc_fgp_hid_report[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x04,        // Usage (Joystick)
  0xA1, 0x01,        // Collection (Application)
  0xA1, 0x02,        //   Collection (Logical)
  0x15, 0x00,        //     Logical Minimum (0)
  0x26, 0xFF, 0x00,  //     Logical Maximum (255)
  0x35, 0x00,        //     Physical Minimum (0)
  0x46, 0xFF, 0x00,  //     Physical Maximum (255)
  0x75, 0x08,        //     Report Size (8)
  0x95, 0x01,        //     Report Count (1)
  0x09, 0x30,        //     Usage (X)
  0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0xA4,              //     Push
  0x25, 0x01,        //       Logical Maximum (1)
  0x45, 0x01,        //       Physical Maximum (1)
  0x75, 0x01,        //       Report Size (1)
  0x95, 0x02,        //       Report Count (2)
  0x81, 0x01,        //       Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x06,        //       Report Count (6)
  0x05, 0x09,        //       Usage Page (Button)
  0x19, 0x01,        //       Usage Minimum (0x01)
  0x29, 0x06,        //       Usage Maximum (0x06)
  0x81, 0x02,        //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x05, 0x01,        //       Usage Page (Generic Desktop Ctrls)
  0xB4,              //     Pop
  0x09, 0x31,        //     Usage (Y)
  0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x02,        //     Report Count (2)
  0x06, 0x00, 0xFF,  //     Usage Page (Vendor Defined 0xFF00)
  0x09, 0x01,        //     Usage (0x01)
  0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              //   End Collection
  0xA1, 0x02,        //   Collection (Logical)
  0x26, 0xFF, 0x00,  //     Logical Maximum (255)
  0x46, 0xFF, 0x00,  //     Physical Maximum (255)
  0x75, 0x08,        //     Report Size (8)
  0x95, 0x04,        //     Report Count (4)
  0x09, 0x02,        //     Usage (0x02)
  0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0xC0,              //   End Collection
  0xC0,              // End Collection
};

//WingMan Formula Force GP (GT Force)
const char usb_ffgp_string_product[] = "Logitech GT Force";
const uint16_t usb_ffgp_bcd_version = 0x0100;
const uint16_t usb_ffgp_bcd_device_version = 0x0000;

const uint8_t desc_ffgp_hid_report[] = {
  0x05, 0x01,         /*  Usage Page (Desktop),               */
  0x09, 0x04,         /*  Usage (Joystik),                    */
  0xA1, 0x01,         /*  Collection (Application),           */
  0xA1, 0x02,         /*      Collection (Logical),           */
  0x95, 0x01,         /*          Report Count (1),           */
  0x75, 0x0A,         /*          Report Size (10),           */
  0x15, 0x00,         /*          Logical Minimum (0),        */
  0x26, 0xFF, 0x03,   /*          Logical Maximum (1023),     */
  0x35, 0x00,         /*          Physical Minimum (0),       */
  0x46, 0xFF, 0x03,   /*          Physical Maximum (1023),    */
  0x09, 0x30,         /*          Usage (X),                  */
  0x81, 0x02,         /*          Input (Variable),           */
  0x95, 0x06,         /*          Report Count (6),           */
  0x75, 0x01,         /*          Report Size (1),            */
  0x25, 0x01,         /*          Logical Maximum (1),        */
  0x45, 0x01,         /*          Physical Maximum (1),       */
  0x05, 0x09,         /*          Usage Page (Button),        */
  0x19, 0x01,         /*          Usage Minimum (01h),        */
  0x29, 0x06,         /*          Usage Maximum (06h),        */
  0x81, 0x02,         /*          Input (Variable),           */
  0x95, 0x01,         /*          Report Count (1),           */
  0x75, 0x08,         /*          Report Size (8),            */
  0x26, 0xFF, 0x00,   /*          Logical Maximum (255),      */
  0x46, 0xFF, 0x00,   /*          Physical Maximum (255),     */
  0x06, 0x00, 0xFF,   /*          Usage Page (FF00h),         */
  0x09, 0x01,         /*          Usage (01h),                */
  0x81, 0x02,         /*          Input (Variable),           */
  0x05, 0x01,         /*          Usage Page (Desktop),       */
  0x09, 0x31,         /*          Usage (Y),                  */
  0x81, 0x02,         /*          Input (Variable),           */
  0x06, 0x00, 0xFF,   /*          Usage Page (FF00h),         */
  0x09, 0x01,         /*          Usage (01h),                */
  0x95, 0x03,         /*          Report Count (3),           */
  0x81, 0x02,         /*          Input (Variable),           */
  0xC0,               /*      End Collection,                 */
  0xA1, 0x02,         /*      Collection (Logical),           */
  0x09, 0x02,         /*          Usage (02h),                */
  0x95, 0x07,         /*          Report Count (7),           */
  0x91, 0x02,         /*          Output (Variable),          */
  0xC0,               /*      End Collection,                 */
  0xC0                /*  End Collection                      */
};

//Driving Force
// The original Driving Force
const char usb_df_string_product[] = "Logitech Driving Force";
const uint16_t usb_df_bcd_version = 0x0100;
const uint16_t usb_df_bcd_device_version = 0x0000;

// Formula Force EX - uses different autocenter command
//  const char usb_string_product[] = "Formula Force EX";
//  const uint16_t usb_bcd_version = 0x0100;
//  const uint16_t usb_bcd_device_version = 0x2100;

// Driving Force EX
//  const char usb_string_product[] = "Driving Force EX";
//  const uint16_t usb_bcd_version = 0x0100;
//  const uint16_t usb_bcd_device_version = 0x2000;

// GT Force RX
//  const char usb_string_product[] = "GT Force RX";
//  const uint16_t usb_bcd_version = 0x0100;
//  const uint16_t usb_bcd_device_version = 0x2200;
  
const uint8_t desc_df_hid_report[] = {
  0x05, 0x01,         /*  Usage Page (Desktop),                   */
  0x09, 0x04,         /*  Usage (Joystik),                        */
  0xA1, 0x01,         /*  Collection (Application),               */
  0xA1, 0x02,         /*      Collection (Logical),               */
  0x95, 0x01,         /*          Report Count (1),               */
  0x75, 0x0A,         /*          Report Size (10),               */
  0x15, 0x00,         /*          Logical Minimum (0),            */
  0x26, 0xFF, 0x03,   /*          Logical Maximum (1023),         */
  0x35, 0x00,         /*          Physical Minimum (0),           */
  0x46, 0xFF, 0x03,   /*          Physical Maximum (1023),        */
  0x09, 0x30,         /*          Usage (X),                      */
  0x81, 0x02,         /*          Input (Variable),               */
  0x95, 0x0C,         /*          Report Count (12),              */
  0x75, 0x01,         /*          Report Size (1),                */
  0x25, 0x01,         /*          Logical Maximum (1),            */
  0x45, 0x01,         /*          Physical Maximum (1),           */
  0x05, 0x09,         /*          Usage Page (Button),            */
  0x19, 0x01,         /*          Usage Minimum (01h),            */
  0x29, 0x0C,         /*          Usage Maximum (0Ch),            */
  0x81, 0x02,         /*          Input (Variable),               */
  0x95, 0x02,         /*          Report Count (2),               */
  0x06, 0x00, 0xFF,   /*          Usage Page (FF00h),             */
  0x09, 0x01,         /*          Usage (01h),                    */
  0x81, 0x02,         /*          Input (Variable),               */
  0x05, 0x01,         /*          Usage Page (Desktop),           */
  0x09, 0x31,         /*          Usage (Y),                      */
  0x26, 0xFF, 0x00,   /*          Logical Maximum (255),          */
  0x46, 0xFF, 0x00,   /*          Physical Maximum (255),         */
  0x95, 0x01,         /*          Report Count (1),               */
  0x75, 0x08,         /*          Report Size (8),                */
  0x81, 0x02,         /*          Input (Variable),               */
  0x25, 0x07,         /*          Logical Maximum (7),            */
  0x46, 0x3B, 0x01,   /*          Physical Maximum (315),         */
  0x75, 0x04,         /*          Report Size (4),                */
  0x65, 0x14,         /*          Unit (Degrees),                 */
  0x09, 0x39,         /*          Usage (Hat Switch),             */
  0x81, 0x42,         /*          Input (Variable, Null State),   */
  0x75, 0x01,         /*          Report Size (1),                */
  0x95, 0x04,         /*          Report Count (4),               */
  0x65, 0x00,         /*          Unit,                           */
  0x06, 0x00, 0xFF,   /*          Usage Page (FF00h),             */
  0x09, 0x01,         /*          Usage (01h),                    */
  0x25, 0x01,         /*          Logical Maximum (1),            */
  0x45, 0x01,         /*          Physical Maximum (1),           */
  0x81, 0x02,         /*          Input (Variable),               */
  0x95, 0x02,         /*          Report Count (2),               */
  0x75, 0x08,         /*          Report Size (8),                */
  0x26, 0xFF, 0x00,   /*          Logical Maximum (255),          */
  0x46, 0xFF, 0x00,   /*          Physical Maximum (255),         */
  0x09, 0x02,         /*          Usage (02h),                    */
  0x81, 0x02,         /*          Input (Variable),               */
  0xC0,               /*      End Collection,                     */
  0xA1, 0x02,         /*      Collection (Logical),               */
  0x26, 0xFF, 0x00,   /*          Logical Maximum (255),          */
  0x46, 0xFF, 0x00,   /*          Physical Maximum (255),         */
  0x95, 0x07,         /*          Report Count (7),               */
  0x75, 0x08,         /*          Report Size (8),                */
  0x09, 0x03,         /*          Usage (03h),                    */
  0x91, 0x02,         /*          Output (Variable),              */
  0xC0,               /*      End Collection,                     */
  0xC0                /*  End Collection                          */
};

//Driving Force Pro (GT Force Pro)
const uint16_t usb_dfp_bcd_version = 0x0100;
const char usb_dfp_string_product[] = "Logitech Driving Force Pro";
const uint16_t usb_dfp_bcd_device_version = 0x1106;
//  const uint16_t usb_dfp_bcd_device_version = 0x1102;
  
const uint8_t desc_dfp_hid_report[] = {
  0x05, 0x01,         /*  Usage Page (Desktop),                   */
  0x09, 0x04,         /*  Usage (Joystik),                        */
  0xA1, 0x01,         /*  Collection (Application),               */
  0xA1, 0x02,         /*      Collection (Logical),               */
  0x95, 0x01,         /*          Report Count (1),               */
  0x75, 0x0E,         /*          Report Size (14),               */
  0x15, 0x00,         /*          Logical Minimum (0),            */
  0x26, 0xFF, 0x3F,   /*          Logical Maximum (16383),        */
  0x35, 0x00,         /*          Physical Minimum (0),           */
  0x46, 0xFF, 0x3F,   /*          Physical Maximum (16383),       */
  0x09, 0x30,         /*          Usage (X),                      */
  0x81, 0x02,         /*          Input (Variable),               */
  0x95, 0x0E,         /*          Report Count (14),              */
  0x75, 0x01,         /*          Report Size (1),                */
  0x25, 0x01,         /*          Logical Maximum (1),            */
  0x45, 0x01,         /*          Physical Maximum (1),           */
  0x05, 0x09,         /*          Usage Page (Button),            */
  0x19, 0x01,         /*          Usage Minimum (01h),            */
  0x29, 0x0E,         /*          Usage Maximum (0Eh),            */
  0x81, 0x02,         /*          Input (Variable),               */
  0x05, 0x01,         /*          Usage Page (Desktop),           */
  0x95, 0x01,         /*          Report Count (1),               */
  0x75, 0x04,         /*          Report Size (4),                */
  0x25, 0x07,         /*          Logical Maximum (7),            */
  0x46, 0x3B, 0x01,   /*          Physical Maximum (315),         */
  0x65, 0x14,         /*          Unit (Degrees),                 */
  0x09, 0x39,         /*          Usage (Hat Switch),             */
  0x81, 0x42,         /*          Input (Variable, Null State),   */
  0x65, 0x00,         /*          Unit,                           */
  0x95, 0x01,         /*          Report Count (1),               */
  0x75, 0x08,         /*          Report Size (8),                */
  0x26, 0xFF, 0x00,   /*          Logical Maximum (255),          */
  0x46, 0xFF, 0x00,   /*          Physical Maximum (255),         */
  0x09, 0x31,         /*          Usage (Y),                      */
  0x81, 0x02,         /*          Input (Variable),               */
  0x06, 0x00, 0xFF,   /*          Usage Page (FF00h),             */
  0x09, 0x00,         /*          Usage (00h),                    */
  0x95, 0x03,         /*          Report Count (3),               */
  0x75, 0x08,         /*          Report Size (8),                */
  0x81, 0x02,         /*          Input (Variable),               */
  0xC0,               /*      End Collection,                     */
  0xA1, 0x02,         /*      Collection (Logical),               */
  0x09, 0x02,         /*          Usage (02h),                    */
  0x95, 0x07,         /*          Report Count (7),               */
  0x91, 0x02,         /*          Output (Variable),              */
  0xC0,               /*      End Collection,                     */
  0xC0                /*  End Collection                          */
};

//Driving Force GT
const uint16_t usb_dfgt_bcd_version = 0x0200;
const char usb_dfgt_string_product[] = "Driving Force GT";
const uint16_t usb_dfgt_bcd_device_version = 0x1322;
//notes: starts in compatibility mode (pid 0xc294, bcd 0x1322)

const uint8_t desc_dfgt_hid_report[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x04,        // Usage (Joystick)
  0xA1, 0x01,        // Collection (Application)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x07,        //   Logical Maximum (7)
  0x35, 0x00,        //   Physical Minimum (0)
  0x46, 0x3B, 0x01,  //   Physical Maximum (315)
  0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
  0x09, 0x39,        //   Usage (Hat switch)
  0x75, 0x04,        //   Report Size (4)
  0x95, 0x01,        //   Report Count (1)
  0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
  0x65, 0x00,        //   Unit (None)
  0x25, 0x01,        //   Logical Maximum (1)
  0x45, 0x01,        //   Physical Maximum (1)
  0x05, 0x09,        //   Usage Page (Button)
  0x19, 0x01,        //   Usage Minimum (0x01)
  0x29, 0x15,        //   Usage Maximum (0x15)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x15,        //   Report Count (21)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x01,        //   Usage (0x01)
  0x95, 0x07,        //   Report Count (7)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x26, 0xFF, 0x3F,  //   Logical Maximum (16383)
  0x46, 0xFF, 0x3F,  //   Physical Maximum (16383)
  0x75, 0x0E,        //   Report Size (14)
  0x95, 0x01,        //   Report Count (1)
  0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
  0x09, 0x30,        //   Usage (X)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x25, 0x01,        //   Logical Maximum (1)
  0x45, 0x01,        //   Physical Maximum (1)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x01,        //   Usage (0x01)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x02,        //   Report Count (2)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x46, 0xFF, 0x00,  //   Physical Maximum (255)
  0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
  0x09, 0x31,        //   Usage (Y)
  0x09, 0x32,        //   Usage (Z)
  0x75, 0x08,        //   Report Size (8)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x07,        //   Report Count (7)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x02,        //   Usage (0x02)
  0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x95, 0x83,        //   Report Count (-125)
  0x09, 0x03,        //   Usage (0x03)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0xC0,              // End Collection
};

//G25 Racing Wheel
const uint16_t usb_g25_bcd_version = 0x0200; // todo confirm
const char usb_g25_string_product[] = "G25 Racing Wheel";
//const uint16_t usb_g25_bcd_device_version = 0x0000; // todo confirm with a report from a real G25. when using 0x0000 the lg windows driver bugs out
const uint16_t usb_g25_bcd_device_version = 0x1239; // taken from a drivehub in g25 mode. looks like it emulates a G27 in G25 mode

const uint8_t desc_g25_hid_report[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x04,        // Usage (Joystick)
  0xA1, 0x01,        // Collection (Application)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x07,        //   Logical Maximum (7)
  0x35, 0x00,        //   Physical Minimum (0)
  0x46, 0x3B, 0x01,  //   Physical Maximum (315)
  0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
  0x09, 0x39,        //   Usage (Hat switch)
  0x75, 0x04,        //   Report Size (4)
  0x95, 0x01,        //   Report Count (1)
  0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
  0x65, 0x00,        //   Unit (None)
  0x25, 0x01,        //   Logical Maximum (1)
  0x45, 0x01,        //   Physical Maximum (1)
  0x05, 0x09,        //   Usage Page (Button)
  0x19, 0x01,        //   Usage Minimum (0x01)
  0x29, 0x13,        //   Usage Maximum (0x13)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x13,        //   Report Count (19)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x01,        //   Usage (0x01)
  0x95, 0x03,        //   Report Count (3)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x26, 0xFF, 0x3F,  //   Logical Maximum (16383)
  0x46, 0xFF, 0x3F,  //   Physical Maximum (16383)
  0x75, 0x0E,        //   Report Size (14)
  0x95, 0x01,        //   Report Count (1)
  0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
  0x09, 0x30,        //   Usage (X)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x46, 0xFF, 0x00,  //   Physical Maximum (255)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x03,        //   Report Count (3)
  0x09, 0x32,        //   Usage (Z)
  0x09, 0x35,        //   Usage (Rz)
  0x09, 0x31,        //   Usage (Y)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x04,        //   Usage (0x04)
  0x95, 0x03,        //   Report Count (3)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x07,        //   Report Count (7)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x02,        //   Usage (0x02)
  0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x95, 0x90,        //   Report Count (-112)
  0x09, 0x03,        //   Usage (0x03)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0xC0,              // End Collection
};

//G27 Racing Wheel
const uint16_t usb_g27_bcd_version = 0x0200;
const char usb_g27_string_product[] = "G27 Racing Wheel";
const uint16_t usb_g27_bcd_device_version = 0x1238;

const uint8_t desc_g27_hid_report[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x04,        // Usage (Joystick)
  0xA1, 0x01,        // Collection (Application)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x07,        //   Logical Maximum (7)
  0x35, 0x00,        //   Physical Minimum (0)
  0x46, 0x3B, 0x01,  //   Physical Maximum (315)
  0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
  0x09, 0x39,        //   Usage (Hat switch)
  0x75, 0x04,        //   Report Size (4)
  0x95, 0x01,        //   Report Count (1)
  0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
  0x65, 0x00,        //   Unit (None)
  0x25, 0x01,        //   Logical Maximum (1)
  0x45, 0x01,        //   Physical Maximum (1)
  0x05, 0x09,        //   Usage Page (Button)
  0x19, 0x01,        //   Usage Minimum (0x01)
  0x29, 0x16,        //   Usage Maximum (0x16)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x16,        //   Report Count (22)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x26, 0xFF, 0x3F,  //   Logical Maximum (16383)
  0x46, 0xFF, 0x3F,  //   Physical Maximum (16383)
  0x75, 0x0E,        //   Report Size (14)
  0x95, 0x01,        //   Report Count (1)
  0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
  0x09, 0x30,        //   Usage (X)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x46, 0xFF, 0x00,  //   Physical Maximum (255)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x03,        //   Report Count (3)
  0x09, 0x32,        //   Usage (Z)
  0x09, 0x35,        //   Usage (Rz)
  0x09, 0x31,        //   Usage (Y)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x01,        //   Usage (0x01)
  0x95, 0x02,        //   Report Count (2)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x01,        //   Report Count (1)
  0x75, 0x01,        //   Report Size (1)
  0x25, 0x01,        //   Logical Maximum (1)
  0x45, 0x01,        //   Physical Maximum (1)
  0x05, 0x09,        //   Usage Page (Button)
  0x09, 0x17,        //   Usage (0x17)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x01,        //   Usage (0x01)
  0x95, 0x07,        //   Report Count (7)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x46, 0xFF, 0x00,  //   Physical Maximum (255)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x02,        //   Usage (0x02)
  0x95, 0x07,        //   Report Count (7)
  0x75, 0x08,        //   Report Size (8)
  0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x95, 0x90,        //   Report Count (-112)
  0x09, 0x03,        //   Usage (0x03)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0xC0,              // End Collection
};

//Speed Force Wireless
const uint16_t usb_sfw_bcd_version = 0x0200;
const char usb_sfw_string_product[] = "Wii Cordless Wheel";
const uint16_t usb_sfw_bcd_device_version = 0x0000;

const uint8_t desc_sfw_hid_report[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x04,        // Usage (Joystick)
  0xA1, 0x01,        // Collection (Application)
  0xA1, 0x02,        //   Collection (Logical)
  0x95, 0x01,        //     Report Count (1)
  0x75, 0x0A,        //     Report Size (10)
  0x15, 0x00,        //     Logical Minimum (0)
  0x26, 0xFF, 0x03,  //     Logical Maximum (1023)
  0x35, 0x00,        //     Physical Minimum (0)
  0x46, 0xFF, 0x03,  //     Physical Maximum (1023)
  0x09, 0x30,        //     Usage (X)
  0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x06, 0x00, 0xFF,  //     Usage Page (Vendor Defined 0xFF00)
  0x95, 0x02,        //     Report Count (2)
  0x75, 0x01,        //     Report Size (1)
  0x25, 0x01,        //     Logical Maximum (1)
  0x45, 0x01,        //     Physical Maximum (1)
  0x09, 0x01,        //     Usage (0x01)
  0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x0B,        //     Report Count (11)
  0x19, 0x01,        //     Usage Minimum (0x01)
  0x29, 0x0B,        //     Usage Maximum (0x0B)
  0x05, 0x09,        //     Usage Page (Button)
  0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x06, 0x00, 0xFF,  //     Usage Page (Vendor Defined 0xFF00)
  0x95, 0x01,        //     Report Count (1)
  0x75, 0x01,        //     Report Size (1)
  0x09, 0x02,        //     Usage (0x02)
  0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
  0x75, 0x08,        //     Report Size (8)
  0x26, 0xFF, 0x00,  //     Logical Maximum (255)
  0x46, 0xFF, 0x00,  //     Physical Maximum (255)
  0x09, 0x31,        //     Usage (Y)
  0x09, 0x32,        //     Usage (Z)
  0x95, 0x02,        //     Report Count (2)
  0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              //   End Collection
  0xA1, 0x02,        //   Collection (Logical)
  0x06, 0x00, 0xFF,  //     Usage Page (Vendor Defined 0xFF00)
  0x95, 0x07,        //     Report Count (7)
  0x09, 0x03,        //     Usage (0x03)
  0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0xC0,              //   End Collection
  0x0A, 0xFF, 0xFF,  //   Usage (0xFFFF)
  0x95, 0x08,        //   Report Count (8)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0xC0,              // End Collection
};
