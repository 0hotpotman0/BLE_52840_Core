 /*
  Copyright (c) 2014-2015 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.
  Copyright (c) 2018, Adafruit Industries (adafruit.com)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _SEEED_SMART_NODE_GENERIC_EDITION_H_
#define _SEEED_SMART_NODE_GENERIC_EDITION_H_

/** Master clock frequency */
#define VARIANT_MCK       (64000000ul)

#define USE_LFXO      // Board uses 32khz crystal for LF
//#define USE_LFRC    // Board uses RC for LF

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
// #define USE_TINYUSB 1
// #define CFG_DEBUG 1
// #define TUSB_OPT_DEVICE_ENABLED 1
// Number of pins defined in PinDescription array
#define PINS_COUNT           (35)
#define NUM_DIGITAL_PINS     (35)
#define NUM_ANALOG_INPUTS    (4)
#define NUM_ANALOG_OUTPUTS   (0)

// LEDs
//#define PIN_LED              (13)
//#define LED_PWR              (25)
//#define PIN_NEOPIXEL         (23)
//#define NEOPIXEL_NUM         1

//#define LED_BUILTIN          PIN_LED
//#define LED_CONN             PIN_LED2

#define LED_RED              (PINS_COUNT)//22
#define LED_GREEN            (PINS_COUNT)//23
#define LED_BLUE             (PINS_COUNT)//24
#define LED_BUILTIN          (PINS_COUNT)

#define LED_STATE_ON         1         // State when LED is litted

/*
 * Buttons
 */
//#define PIN_BUTTON1          (7)
#define PIN_KEY1             (4)
#define PIN_KEY2             (5)

/*
 * HALL
 */
#define PIN_HALL_POWER       (2)
#define PIN_HALL_READ        (3)

/*
 * Analog pins
 */
#define PIN_A0               (0)
#define PIN_A1               (1)
//#define PIN_A2               (16)
//#define PIN_A3               (17)
//#define PIN_A4               (18)
//#define PIN_A5               (19)
#define PIN_A6               (6)
#define PIN_A7               (7)

static const uint8_t A0  = PIN_A0 ;
static const uint8_t A1  = PIN_A1 ;
//static const uint8_t A2  = PIN_A2 ;
//static const uint8_t A3  = PIN_A3 ;
//static const uint8_t A4  = PIN_A4 ;
//static const uint8_t A5  = PIN_A5 ;
static const uint8_t A6  = PIN_A6 ;
static const uint8_t A7  = PIN_A7 ;
#define ADC_RESOLUTION    12

// Other pins
//#define PIN_AREF           PIN_A7
//#define PIN_VBAT           PIN_A6
#define PIN_NFC1           (18)
#define PIN_NFC2           (19)

//static const uint8_t AREF = PIN_AREF;

/*
 * Serial interfaces
 */
#define PIN_SERIAL1_RX       (28)//(27)
#define PIN_SERIAL1_TX       (27)//(28)

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 1

#define PIN_SPI_MISO         (11)
#define PIN_SPI_MOSI         (12)
#define PIN_SPI_SCK          (10)

static const uint8_t SS   = (9);
static const uint8_t MOSI = PIN_SPI_MOSI ;
static const uint8_t MISO = PIN_SPI_MISO ;
static const uint8_t SCK  = PIN_SPI_SCK ;

#define PIN_SD_POWER         (14)
#define PIN_SD_CS            (9)
/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 1

#define PIN_WIRE_SDA         (17)
#define PIN_WIRE_SCL         (16)

//#define PIN_WIRE1_SDA        (30)
//#define PIN_WIRE1_SCL        (31)

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#endif
