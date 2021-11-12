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

#define PINS_COUNT           (35)
#define NUM_DIGITAL_PINS     (35)
#define NUM_ANALOG_INPUTS    (4)
#define NUM_ANALOG_OUTPUTS   (0)

#define LED_RED              (PINS_COUNT)
#define LED_GREEN            (PINS_COUNT)
#define LED_BLUE             (PINS_COUNT)
#define LED_BUILTIN          (PINS_COUNT)

#define LED_STATE_ON         1         // State when LED is litted

/*
 * Buttons
 */
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
#define PIN_A6               (6)
#define PIN_A7               (7)

static const uint8_t A0  = PIN_A0 ;
static const uint8_t A1  = PIN_A1 ;
static const uint8_t A6  = PIN_A6 ;
static const uint8_t A7  = PIN_A7 ;
#define ADC_RESOLUTION    12

// Other pins
#define PIN_NFC1           (18)
#define PIN_NFC2           (19)

/*
 * Serial interfaces
 */
#define PIN_SERIAL1_RX       (28)
#define PIN_SERIAL1_TX       (27)

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

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#endif
