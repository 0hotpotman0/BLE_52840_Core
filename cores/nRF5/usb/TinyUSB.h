/*
 * The MIT License (MIT)
 *
 * 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _TINYUSB_H_
#define _TINYUSB_H_

// Error message for Core that must select TinyUSB via menu
#if !defined(TINYUSB_ENABLED) && ( defined(ARDUINO_ARCH_SAMD) || \
                               (defined(ARDUINO_ARCH_RP2040) && !defined(ARDUINO_ARCH_MBED)) || \
                               defined(ARDUINO_ARCH_ESP32) )
#error TinyUSB is not selected, please select it in "Tools->Menu->USB Stack"
#endif

#include "tusb_option.h"

//#if TUSB_OPT_DEVICE_ENABLED

#include "arduino/USBD_Device.h"
#include "arduino/USBD_CDC.h"

#include "arduino/hid/USBD_HID.h"
#include "arduino/midi/USBD_MIDI.h"
#include "arduino/msc/USBD_MSC.h"
#include "arduino/webusb/USBD_WebUSB.h"

// Initialize device hardware, stack, also Serial as CDC
// Wrapper for TinyUSBDevice.begin(rhport)
void TinyUSB_Device_Init(uint8_t rhport);

//#endif

#endif 
