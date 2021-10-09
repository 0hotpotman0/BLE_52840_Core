/****************************************************************************************************************************
  TCPClientSingle.ino
  For ESP8266/ESP32-AT-command running shields
  
  ESP_AT_Lib is a wrapper library for the ESP8266/ESP32 AT-command shields
  
  Based on and modified from ESP8266 https://github.com/esp8266/Arduino/releases
  Built by Khoi Hoang https://github.com/khoih-prog/ESP_AT_Lib
  Licensed under MIT license
  
  @example TCPClientSingle.ino
  @brief The TCPClientSingle demo of library WeeESP8266.
  @author Wu Pengfei<pengfei.wu@itead.cc>
  @date 2015.02
  
  @par Copyright:
  Copyright (c) 2015 ITEAD Intelligent Systems Co., Ltd. \n\n
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License, or (at your option) any later version. \n\n
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
  
  Version: 1.4.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      12/02/2020 Initial coding for ESP8266/ESP32-AT shields to support Mega, nRF52, SAMD, DUE, STM32, etc.
  1.1.0   K Hoang      10/05/2021 Add support to BOARD_SIPEED_MAIX_DUINO and RASPBERRY_PI_PICO
  1.2.0   K Hoang      17/05/2021 Add support to RP2040-based boards using Arduino-mbed RP2040 core. Fix compiler warnings
  1.3.0   K Hoang      29/05/2021 Add support to RP2040-based Nano_RP2040_Connect using Arduino-mbed RP2040 core
  1.4.0   K Hoang      13/08/2021 Add support to Adafruit nRF52 core v0.22.0+
 *****************************************************************************************************************************/

#include "ESP_AT_Lib.h"

#define SSID        "SSID"
#define PASSWORD    "password"

#define HOST_NAME   "192.168.4.108"//"www.yahoo.com"
#define HOST_PORT   (8080)//(80)

ESP8266 wifi(&EspSerial);

// Your board <-> ESP_AT baud rate:
#define ESP_AT_BAUD       115200

void setup(void)
{
  Serial.begin(115200);
  while (!Serial);

#if defined(BOARD_NAME)
  Serial.println("\nStart TCPClientSingle on " + String(BOARD_NAME));
#else
  Serial.println("\nStart TCPClientSingle");
#endif

  Serial.println(ESP_AT_LIB_VERSION);

  // initialize serial for ESP module
  EspSerial.begin(ESP_AT_BAUD);

  Serial.print("FW Version:");
  Serial.println(wifi.getVersion().c_str());

  Serial.print("Set STA Mode ");
  
  if (wifi.setOprToStation())
  {
    Serial.println("OK");
  }
  else
  {
    Serial.println("failed");
  }

  if (wifi.joinAP(SSID, PASSWORD))
  {
    Serial.println("Connect to WiFi OK");
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  }
  else
  {
    Serial.println("Connect to WiFi failed");
  }

  Serial.print("disableMUX ");
  
  if (wifi.disableMUX()) 
  {
    Serial.println("OK");
  } 
  else 
  {
    Serial.println("failed");
  }

  Serial.println("Done");
}

void loop(void)
{
  uint8_t buffer[128] = {0};

  Serial.print("Create TCP ");
  
  if (wifi.createTCP(HOST_NAME, HOST_PORT)) 
  {
    Serial.println("OK");
  } 
  else 
  {
    Serial.println("failed");
  }

  char hello[] = "Hello, this is client!";
  
  if (wifi.send((const uint8_t*)hello, strlen(hello))) 
  {
    Serial.println("Send OK");
  } 
  else 
  {
    Serial.println("Send failed");
  }

  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
  
  if (len > 0) 
  {
    Serial.print("Received:[");
    
    for (uint32_t i = 0; i < len; i++) 
    {
      Serial.print((char)buffer[i]);
    }
    
    Serial.println("]");
  }

  Serial.print("Release TCP ");
  
  if (wifi.releaseTCP()) 
  {
    Serial.println("OK");
  } 
  else 
  {
    Serial.println("failed");
  }

  delay(5000);
}
