# lgff_wheel_adapter
Adapter for Logitech force feedback wheels

A DIY adapter that can receive input from a real logitech wheel and output as another emulated logitech wheel.

IE: Use a G29 for input and output as a Driving Force, to use it on a PS2!<br/>
Or use any supported wheel and output as a GT Force for some unique PS2 games that only supports this wheel.

### PRs are welcome!
Would be amazing to support the xbox version of the wheel for input and even better for emulated output.
Let me know if you can help.

## Input/Output modes
| Wheel                               | Input (from real hardware) | Output (emulated) |
|-------------------------------------|----------------------------|-------------------|
| WingMan Formula GP (no FFB)         | Yes (untested)             | Yes               |
| WingMan Formula Force GP (GT Force) | Yes (untested)             | Yes               |
| Driving Force                       | Yes                        | Yes               |
| Driving Force Pro (GT Force Pro)    | Yes                        | Yes               |
| Driving Force GT                    | Yes                        | Yes               |
| G25 Racing Wheel                    | Yes                        | Yes               |
| G27 Racing Wheel                    | Yes                        | Yes               |
| G29 Racing Wheel                    | Yes                        | No                |


## Building
Requires a Raspberry Pi Pico (RP2040) board and a USB Type-A port for input.

Check the wiring guidance [here](https://github.com/sekigon-gonnoc/Pico-PIO-USB/discussions/7).

USB Pins can be changed. Just need to set them in code.

Define the `D+` pin on sketch. `D-` will be `D+` + 1.

Required configuration are on `config.h` file

Firmware builds under Arduino IDE.

Required libs. Install using Arduino IDE.

[arduino-pico (3.1.1)](https://github.com/earlephilhower/arduino-pico#installing-via-arduino-boards-manager)<br/>
[Pico-PIO-USB (0.5.3)](https://github.com/sekigon-gonnoc/Pico-PIO-USB)<br/>
[Adafruit_TinyUSB_Arduino (2.2.6)](https://github.com/adafruit/Adafruit_TinyUSB_Arduino)

Configure IDE as:
* Board: Raspberry Pi Pico
* CPU Speed: 120MHz
* USB Stack: Adafruit TinyUSB

## Ready to use binaries
Don't want to build from source? Check the releases page.

## Logitech Command differences
Spring Effect is different for Formula Force GP and Driving Force.
I'm not sure if games handles this correctly.

I think it's better to have the input wheel forced into compatibility mode as a Driving Force when emulating a FFGP or DF wheel.

Be aware that it's not possible to force a G29 into Driving Force mode.

## Credits
[GIMX](https://github.com/matlo) from by matlo

This would not be possible without matlo's work on GIMX.

[g29emu](https://github.com/mathijsvandenberg/g29emu) from mathijsvandenberg.

[lg4ff](https://github.com/berarma/new-lg4ff) from berarma.

Open Source [documents](https://opensource.logitech.com/wiki/Technical_Information/) from Logitech.

Special thanks for [NolanNicholson](https://github.com/NolanNicholson) for doing the G29 data input and beta testing.

## Disclaimer

Code and wiring directions are provided to you 'as is' and without any warranties. Use at your own risk.