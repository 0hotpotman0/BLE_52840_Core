# Arduino Core for Adafruit Bluefruit nRF52 Boards

[![Build Status](https://github.com/adafruit/Adafruit_nRF52_Arduino/workflows/Build/badge.svg)](https://github.com/adafruit/Adafruit_nRF52_Arduino/actions)

This repository contains the Arduino BSP for Adafruit Bluefruit nRF52 series:

- [Seeed XIAO BLE](https://www.adafruit.com/product/4500)
- [Seeed Wio node](https://www.adafruit.com/product/4333)


## BSP Installation

There are two methods that you can use to install this BSP. We highly recommend the first option unless you wish to participate in active development of this codebase via Github.

### Recommended: Adafruit nRF52 BSP via the Arduino Board Manager

 1. [Download and install the Arduino IDE](https://www.arduino.cc/en/Main/Software) (At least v1.6.12)
 2. Start the Arduino IDE
 3. Go into Preferences
 4. Add https://files.seeedstudio.com/arduino/package_seeeduino_boards_index.json as an 'Additional Board Manager URL'
 5. Restart the Arduino IDE
 6. Open the Boards Manager from the Tools -> Board menu and install 'Seeed nRF52'
 7. Once the BSP is installed, select 'Seeed nRF52840' from the Tools -> Board menu, which will update your system config to use the right compiler and settings for the nRF52.




## Credits

This core is based on [Arduino-nRF5](https://github.com/sandeepmistry/arduino-nRF5) by Sandeep Mistry,
which in turn is based on the [Arduino SAMD Core](https://github.com/arduino/ArduinoCore-samd).

The following libraries are used:

- [FreeRTOS](https://www.freertos.org/) as operating system
- [LittleFS](https://github.com/ARMmbed/littlefs) for internal file system
- [nrfx](https://github.com/NordicSemiconductor/nrfx) for peripherals driver
- [TinyUSB](https://github.com/hathach/tinyusb) as usb stack
