/****************************************************************************************************************************
  ESP_AT_Debug.h - Dead simple ESP8266/ESP32-AT-command wrapper
  For ESP8266/ESP32-AT-command running shields

  ESP_AT_Lib is a wrapper library for the ESP8266/ESP32 AT-command shields

  Based on and modified from ESP8266 https://github.com/esp8266/Arduino/releases
  Built by Khoi Hoang https://github.com/khoih-prog/ESP_AT_Lib
  Licensed under MIT license
  Version: 1.4.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      12/02/2020 Initial coding for ESP8266/ESP32-AT shields to support Mega, nRF52, SAMD, DUE, STM32, etc.
  1.1.0   K Hoang      10/05/2021 Add support to BOARD_SIPEED_MAIX_DUINO and RASPBERRY_PI_PICO
  1.2.0   K Hoang      17/05/2021 Add support to RP2040-based boards using Arduino-mbed RP2040 core. Fix compiler warnings
  1.3.0   K Hoang      29/05/2021 Add support to RP2040-based Nano_RP2040_Connect using Arduino-mbed RP2040 core
  1.4.0   K Hoang      13/08/2021 Add support to Adafruit nRF52 core v0.22.0+
 *****************************************************************************************************************************/

#ifndef __ESP_AT_DEBUG_H__
#define __ESP_AT_DEBUG_H__

#include <stdio.h>

#if (defined(NRF52840_FEATHER) || defined(NRF52832_FEATHER) || defined(NRF52_SERIES) || defined(ARDUINO_NRF52_ADAFRUIT) || \
     defined(NRF52840_FEATHER_SENSE) || defined(NRF52840_ITSYBITSY) || defined(NRF52840_CIRCUITPLAY) || \
     defined(NRF52840_CLUE) || defined(NRF52840_METRO) || defined(NRF52840_PCA10056) || defined(PARTICLE_XENON) || \
     defined(MDBT50Q_RX) || defined(NINA_B302_ublox) || defined(NINA_B112_ublox) )
  //#include <Adafruit_TinyUSB.h>
#endif

#ifdef ESP_AT_LIB_DEBUG_OUTPUT
  #define AT_LIB_DEBUG_OUTPUT      ESP_AT_LIB_DEBUG_OUTPUT
  #warning Use Serial in ESP_AT_Lib_Debug.h
#else
  #define AT_LIB_DEBUG_OUTPUT      Serial
#endif

// Change _ESP_AT_LIB_LOGLEVEL_ to set tracing and logging verbosity
// 0: DISABLED: no logging
// 1: ERROR: errors
// 2: WARN: errors and warnings
// 3: INFO: errors, warnings and informational (default)
// 4: DEBUG: errors, warnings, informational and debug

#ifndef _ESP_AT_LIB_LOGLEVEL_
  #define _ESP_AT_LIB_LOGLEVEL_       0
#endif

#define AT_LIB_PRINT        AT_LIB_DEBUG_OUTPUT.print
#define AT_LIB_PRINTLN      AT_LIB_DEBUG_OUTPUT.println

const char AT_LIB_MARK[] = "[ESP_AT_LIB] ";
const char AT_LIB_SP[]   = " ";

#define AT_LIB_PRINT_MARK   AT_LIB_PRINT(AT_LIB_MARK)
#define AT_LIB_PRINT_SP     AT_LIB_PRINT(AT_LIB_SP)

//////////////////////////////////////////////

#define AT_LIB_LOGERROR(x)    if(_ESP_AT_LIB_LOGLEVEL_>0) { AT_LIB_PRINT_MARK; AT_LIB_PRINTLN(x); }
#define AT_LIB_LOGERROR1(x,y) if(_ESP_AT_LIB_LOGLEVEL_>0) { AT_LIB_PRINT_MARK; AT_LIB_PRINT(x); AT_LIB_PRINT_SP; AT_LIB_PRINTLN(y); }

//////////////////////////////////////////////

#define AT_LIB_LOGWARN(x)     if(_ESP_AT_LIB_LOGLEVEL_>1) { AT_LIB_PRINT_MARK; AT_LIB_PRINTLN(x); }
#define AT_LIB_LOGWARN1(x,y)  if(_ESP_AT_LIB_LOGLEVEL_>1) { AT_LIB_PRINT_MARK; AT_LIB_PRINT(x); AT_LIB_PRINT_SP; AT_LIB_PRINTLN(y); }
#define AT_LIB_LOGINFO(x)     if(_ESP_AT_LIB_LOGLEVEL_>2) { AT_LIB_PRINT_MARK; AT_LIB_PRINTLN(x); }
#define AT_LIB_LOGINFO1(x,y)  if(_ESP_AT_LIB_LOGLEVEL_>2) { AT_LIB_PRINT_MARK; AT_LIB_PRINT(x); AT_LIB_PRINT_SP; AT_LIB_PRINTLN(y); }

//////////////////////////////////////////////

#define AT_LIB_LOGDEBUG(x)      if(_ESP_AT_LIB_LOGLEVEL_>3) { AT_LIB_PRINTLN(x); }
#define AT_LIB_LOGDEBUG0(x)     if(_ESP_AT_LIB_LOGLEVEL_>3) { AT_LIB_PRINT(x); }
#define AT_LIB_LOGDEBUG1(x,y)   if(_ESP_AT_LIB_LOGLEVEL_>3) { AT_LIB_PRINT(x); AT_LIB_PRINT_SP; AT_LIB_PRINTLN(y); }
#define AT_LIB_LOGDEBUG2(x,y,z) if(_ESP_AT_LIB_LOGLEVEL_>3) { AT_LIB_PRINT(x); AT_LIB_PRINT_SP; AT_LIB_PRINT(y); AT_LIB_PRINT_SP; AT_LIB_PRINTLN(z); }

//////////////////////////////////////////////

#endif    // __ESP_AT_DEBUG_H__
