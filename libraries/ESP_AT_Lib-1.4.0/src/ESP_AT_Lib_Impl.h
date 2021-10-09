/****************************************************************************************************************************
  ESP_AT_Lib_Impl.h - Dead simple ESP8266/ESP32-AT-command wrapper
  For ESP8266/ESP32-AT-command running shields

  ESP_AT_Lib is a wrapper library for the ESP8266/ESP32 AT-command shields

  Based on and modified from ESP8266 https://github.com/esp8266/Arduino/releases
  Built by Khoi Hoang https://github.com/khoih-prog/ESP_AT_Lib
  Licensed under MIT license

  @file ESP_AT_Lib.cpp
  @brief The implementation of class ESP_AT for ESP8266/ESP32-AT-command.
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

#ifndef __ESP_AT_LIB_IMPL_H__
#define __ESP_AT_LIB_IMPL_H__

#include <avr/pgmspace.h>

ESP8266::ESP8266(Stream *uart)
  : m_puart(uart)
{
  m_onData = NULL;
  m_onDataPtr = NULL;
}

bool ESP8266::kick(void)
{
  return eAT();
}

bool ESP8266::restart(void)
{
  unsigned long start;
  if (eATRST())
  {
    delay(2000);
    start = millis();

    while (millis() - start < 3000)
    {
      if (eAT())
      {
        delay(1500); /* Waiting for stable */
        return true;
      }

      delay(100);
    }
  }
  return false;
}

String ESP8266::getVersion(void)
{
  String version;

  eATGMR(version);
  return version;
}

bool ESP8266::setEcho(uint8_t mode)
{
  return eATE(mode);
}

bool ESP8266::restore(void)
{
  return eATRESTORE();
}

bool ESP8266::setUart(uint32_t baudrate, uint8_t pattern)
{
  return eATSETUART(baudrate, pattern);
}

bool ESP8266::deepSleep(uint32_t time)
{
  return eATGSLP(time);
}

bool ESP8266::setOprToStation(uint8_t pattern1, uint8_t pattern2)
{
  uint8_t mode;
  if (!qATCWMODE(&mode, pattern1))
  {
    return false;
  }

  if (mode == 1)
  {
    return true;
  }
  else
  {
    if (sATCWMODE(1, pattern2))
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}

String ESP8266::getWifiModeList(void)
{
  String list;
  eATCWMODE(list);
  return list;
}

bool ESP8266::setOprToSoftAP(uint8_t pattern1, uint8_t pattern2)
{
  uint8_t mode;

  if (!qATCWMODE(&mode, pattern1))
  {
    return false;
  }

  if (mode == 2)
  {
    return true;
  }
  else
  {
    if (sATCWMODE(2, pattern2) )
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}

bool ESP8266::setOprToStationSoftAP(uint8_t pattern1, uint8_t pattern2)
{
  uint8_t mode;

  if (!qATCWMODE(&mode, pattern1))
  {
    return false;
  }

  if (mode == 3)
  {
    return true;
  }
  else
  {
    if (sATCWMODE(3, pattern2) )
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}

uint8_t ESP8266::getOprMode(uint8_t pattern1)
{
  uint8_t mode;

  if (!qATCWMODE(&mode, pattern1))
  {
    return 0;
  }
  else
  {
    return mode;
  }
}

String ESP8266::getNowConecAp(uint8_t pattern)
{
  String ssid;

  qATCWJAP(ssid, pattern);
  return ssid;
}


String ESP8266::getAPList(void)
{
  String list;

  eATCWLAP(list);
  return list;
}

bool ESP8266::joinAP(String ssid, String pwd, uint8_t pattern)
{
  return sATCWJAP(ssid, pwd, pattern);
}

bool ESP8266::leaveAP(void)
{
  return eATCWQAP();
}

String ESP8266::getSoftAPParam(uint8_t pattern)
{
  String list;

  qATCWSAP(list, pattern);
  return list;
}

bool ESP8266::setSoftAPParam(String ssid, String pwd, uint8_t chl, uint8_t ecn, uint8_t pattern)
{
  return sATCWSAP(ssid, pwd, chl, ecn, pattern);
}

String ESP8266::getJoinedDeviceIP(void)
{
  String list;

  eATCWLIF(list);
  return list;
}

String ESP8266::getDHCP(uint8_t pattern)
{
  String dhcp;

  qATCWDHCP(dhcp, pattern);
  return dhcp;
}

#if USE_ESP32_AT
// For ESP32-AT
/**
  Set the  state of DHCP.
  @param pattern -1, -2, -3 send "AT+CWDHCP=".
  @param op - 0 disable DHCP  - 1 enable DHCP.
  @param mode - Bit 0 : STA DHCP / Bit 1: SoftAP DHCP

  @retval true - success.
  @retval false - failure.
*/
bool ESP8266::setDHCP(uint8_t op, uint8_t mode, uint8_t pattern)
{
  return sATCWDHCP(op, mode, pattern);
}

#else
// For ESP8266-AT
/**
  Set the  state of DHCP.
  @param pattern -1 send "AT+CWDHCP_DEF=" -2 send "AT+CWDHCP_CUR=" -3 send "AT+CWDHCP=".
  @param mode - ‣ 0: Sets ESP8266 SoftAP, ‣ 1: Sets ESP8266 Station, ‣ 2: Sets both SoftAP and Station
  @param en - 0 disable DHCP  - 1 enable DHCP.
  @retval true - success.
  @retval false - failure.
*/
bool ESP8266::setDHCP(uint8_t mode, uint8_t en, uint8_t pattern)
{
  return sATCWDHCP(mode, en, pattern);
}
#endif

bool ESP8266::setAutoConnect(uint8_t en)
{
  return eATCWAUTOCONN(en);
}

String ESP8266::getStationMac(uint8_t pattern)
{
  String mac;

  qATCIPSTAMAC(mac, pattern);
  return mac;
}

bool ESP8266::setStationMac(String mac, uint8_t pattern)
{
  return eATCIPSTAMAC(mac, pattern);
}

String ESP8266::getStationIp(uint8_t pattern)
{
  String ip;

  qATCIPSTAIP(ip, pattern);
  return ip;
}

bool ESP8266::setStationIp(String ip, String gateway, String netmask, uint8_t pattern)
{
  return eATCIPSTAIP(ip, gateway, netmask, pattern);
}

String ESP8266::getAPIp(uint8_t pattern)
{
  String ip;

  qATCIPAP(ip, pattern);
  return ip;
}

bool ESP8266::setAPIp(String ip, uint8_t pattern)
{
  return eATCIPAP(ip, pattern);
}

bool ESP8266::startSmartConfig(uint8_t type)
{
  return eCWSTARTSMART(type);
}

bool ESP8266::stopSmartConfig(void)
{
  return eCWSTOPSMART();
}

String ESP8266::getIPStatus(void)
{
  String list;

  eATCIPSTATUS(list);
  return list;
}

String ESP8266::getLocalIP(void)
{
  String list;

  eATCIFSR(list);
  return list;
}

bool ESP8266::enableMUX(void)
{
  return sATCIPMUX(1);
}

bool ESP8266::disableMUX(void)
{
  return sATCIPMUX(0);
}

bool ESP8266::createTCP(String addr, uint32_t port)
{
  return sATCIPSTARTSingle("TCP", addr, port);
}

bool ESP8266::releaseTCP(void)
{
  return eATCIPCLOSESingle();
}

bool ESP8266::registerUDP(String addr, uint32_t port)
{
  return sATCIPSTARTSingle("UDP", addr, port);
}

bool ESP8266::unregisterUDP(void)
{
  return eATCIPCLOSESingle();
}

bool ESP8266::createTCP(uint8_t mux_id, String addr, uint32_t port)
{
  return sATCIPSTARTMultiple(mux_id, "TCP", addr, port);
}

bool ESP8266::releaseTCP(uint8_t mux_id)
{
  return sATCIPCLOSEMultiple(mux_id);
}

bool ESP8266::registerUDP(uint8_t mux_id, String addr, uint32_t port)
{
  return sATCIPSTARTMultiple(mux_id, "UDP", addr, port);
}

bool ESP8266::unregisterUDP(uint8_t mux_id)
{
  return sATCIPCLOSEMultiple(mux_id);
}

bool ESP8266::setTCPServerTimeout(uint32_t timeout)
{
  return sATCIPSTO(timeout);
}

bool ESP8266::startTCPServer(uint32_t port)
{
  if (sATCIPSERVER(1, port))
  {
    return true;
  }

  return false;
}

bool ESP8266::stopTCPServer(void)
{
  sATCIPSERVER(0);
  restart();
  return false;
}

bool ESP8266::setCIPMODE(uint8_t mode)
{
  return sATCIPMODE(mode);
}

bool ESP8266::saveTransLink (uint8_t mode, String ip, uint32_t port)
{
  return eATSAVETRANSLINK(mode, ip, port);
}

bool ESP8266::setPing(String ip)
{
  return eATPING(ip);
}

bool ESP8266::startServer(uint32_t port)
{
  return startTCPServer(port);
}

bool ESP8266::stopServer(void)
{
  return stopTCPServer();
}

bool ESP8266::send(const uint8_t *buffer, uint32_t len)
{
  return sATCIPSENDSingle(buffer, len);
}

bool ESP8266::sendFromFlash(uint8_t mux_id, const uint8_t *buffer, uint32_t len)
{
  return sATCIPSENDMultipleFromFlash(mux_id, buffer, len);
}

bool ESP8266::sendFromFlash(const uint8_t *buffer, uint32_t len)
{
  return sATCIPSENDSingleFromFlash(buffer, len);
}

bool ESP8266::send(uint8_t mux_id, const uint8_t *buffer, uint32_t len)
{
  return sATCIPSENDMultiple(mux_id, buffer, len);
}

void ESP8266::run()
{
  rx_empty();
}

/*----------------------------------------------------------------------------*/
/* +IPD,<id>,<len>:<data> */
/* +IPD,<len>:<data> */

uint32_t ESP8266::checkIPD(String& data)
{
  //Serial.print("### check: ");
  //Serial.println(data);

  int32_t index_PIPDcomma = -1;
  int32_t index_colon     = -1; /* : */
  int32_t index_comma     = -1; /* , */

  int32_t len = -1;
  int8_t id   = -1;

  { // Just for easier diffing
    index_PIPDcomma = data.indexOf("+IPD,");
    
    if (index_PIPDcomma != -1)
    {
      index_colon = data.indexOf(':', index_PIPDcomma + 5);

      if (index_colon != -1)
      {
        index_comma = data.indexOf(',', index_PIPDcomma + 5);

        /* +IPD,id,len:data */
        if (index_comma != -1 && index_comma < index_colon)
        {
          id = data.substring(index_PIPDcomma + 5, index_comma).toInt();

          if (id < 0 || id > 4)
          {
            return 0;
          }

          len = data.substring(index_comma + 1, index_colon).toInt();

          if (len <= 0)
          {
            return 0;
          }
        }
        else
        {
          /* +IPD,len:data */
          len = data.substring(index_PIPDcomma + 5, index_colon).toInt();

          if (len <= 0)
          {
            return 0;
          }
        }

        if (m_onData)
        {
          m_onData(id, len, m_onDataPtr);
        }

        return len;
      }
    }
  }

  return 0;
}

void ESP8266::rx_empty(void)
{
  String data;
  char a;
  unsigned long start = millis();

  while (millis() - start < 10)
  {
    if (m_puart->available())
    {
      a = m_puart->read();

      if (a == '\0')
        continue;

      data += a;

      if (checkIPD(data))
      {
        data = "";
      }

      start = millis();
    }
  }
}

String ESP8266::recvString(String target, uint32_t timeout)
{
  int length = 0;
  int maxlength;
  maxlength = target.length();
  if(maxlength<=5){  // "+IPD," is 5 bytes
    maxlength = 5;
  }
  String data;
  char a;
  unsigned long start = millis();
  while (millis() - start < timeout)
  {
    while (m_puart->available() > 0)
    {
      a = m_puart->read();
      if (a == '\0')
        continue;

      data += a;
      length ++;
      if(length >= maxlength)
      {
        if (data.indexOf(target,length-maxlength) != -1)
        {
          return data;
        }
        else if (data.indexOf("+IPD,",length-maxlength) != -1){
          if(checkIPD(data)){
            data = "";
          }
        }
      }
    }
  }
  return data;
}

String ESP8266::recvString(String target1, String target2, uint32_t timeout)
{
  int length = 0;
  int maxlength;
  maxlength = target1.length();
  if(maxlength<=target2.length()){
    maxlength = target2.length();
  }
  if(maxlength<=5){  // "+IPD," is 5 bytes
    maxlength = 5;
  }
  String data;
  char a;
  unsigned long start = millis();

  while (millis() - start < timeout)
  {
    while (m_puart->available() > 0)
    {
      a = m_puart->read();

      if (a == '\0')
        continue;

      data += a;
      length ++;
      if (data.indexOf(target1,length-1) != -1)
      {
        return data;
      }
      else if (data.indexOf(target2,length-1) != -1)
      {
        return data;
      }
      else if (data.indexOf("+IPD,",length-1) != -1){
        if(checkIPD(data)){
          data = "";
        }
      }
      // if (data.indexOf(target1) != -1)
      // {
      //   return data;
      // }
      // else if (data.indexOf(target2) != -1)
      // {
      //   return data;
      // }
      // else if (checkIPD(data))
      // {
      //   data = "";
      // }
    }
  }

  return data;
}

String ESP8266::recvString(String target1, String target2, String target3, uint32_t timeout)
{
  int length = 0;
  int maxlength;
  maxlength = target1.length();
  if(maxlength<=target2.length()){
    maxlength = target2.length();
  }
  if(maxlength<=target3.length()){
    maxlength = target3.length();
  }
  if(maxlength<=5){  // "+IPD," is 5 bytes
    maxlength = 5;
  }
  String data;
  char a;
  unsigned long start = millis();

  while (millis() - start < timeout)
  {
    while (m_puart->available() > 0)
    {
      a = m_puart->read();

      if (a == '\0')
        continue;

      data += a;
      length ++;
      if (data.indexOf(target1,length-1) != -1)
      {
        return data;
      }
      else if (data.indexOf(target2,length-1) != -1)
      {
        return data;
      }
      else if (data.indexOf(target3,length-1) != -1)
      {
        return data;
      }
      else if (data.indexOf("+IPD,",length-1) != -1){
        if(checkIPD(data)){
          data = "";
        }
      }
      // if (data.indexOf(target1) != -1)
      // {
      //   return data;
      // }
      // else if (data.indexOf(target2) != -1)
      // {
      //   return data;
      // }
      // else if (data.indexOf(target3) != -1)
      // {
      //   return data;
      // }
      // else if (checkIPD(data))
      // {
      //   data = "";
      // }
    }
  }

  return data;
}

bool ESP8266::recvFind(String target, uint32_t timeout)
{
  String data_tmp;

  data_tmp = recvString(target, timeout);

  if (data_tmp.indexOf(target) != -1)
  {
    return true;
  }

  return false;
}

bool ESP8266::recvFindAndFilter(String target, String begin, String end, String &data, uint32_t timeout)
{
  String data_tmp;

  data_tmp = recvString(target, timeout);

  if (data_tmp.indexOf(target) != -1)
  {
    int32_t index1 = data_tmp.indexOf(begin);
    int32_t index2 = data_tmp.indexOf(end);

    if (index1 != -1 && index2 != -1)
    {
      index1 += begin.length();
      data = data_tmp.substring(index1, index2);
      return true;
    }
    else if (index2 != -1)
    {
      data = data_tmp.substring(0, index2);
      return true;
    }

    index1 = data.indexOf("\r\n\r\nOK");

    if (index1 != -1)
    {
      data = data_tmp.substring(0, index1);
    }

    index1 = data.indexOf("\r\nOK");

    if (index1 != -1)
    {
      data = data_tmp.substring(0, index1);
    }
  }

  data = data_tmp;

  return false;
}

bool ESP8266::eAT(void)
{
  rx_empty();
  m_puart->println(F("AT"));

  AT_LIB_LOGDEBUG(F("AT"));

  return recvFind("OK");
}

bool ESP8266::eATRST(void)
{
  rx_empty();

#if 0   //USE_ESP32_AT
  //The execution of this command will reset all parameters saved in flash, and restore the factory default settings of the module.
  // The chip will be restarted when this command is executed.
  m_puart->println(F("AT+RESTORE"));

  AT_LIB_LOGDEBUG(F("AT+RESTORE"));
  
  //return recvFind("OK");
  return recvFind("OK", 5000);
#else
  m_puart->println(F("AT+RST"));

  AT_LIB_LOGDEBUG(F("AT+RST"));
  
  return recvFind("OK");
#endif


}

bool ESP8266::eATGMR(String &version)
{
  rx_empty();
  delay(3000);
  m_puart->println(F("AT+GMR"));

  AT_LIB_LOGDEBUG(F("AT+GMR"));

  return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", version, 10000);
}

// Enters Deep-sleep Mode in time
bool ESP8266::eATGSLP(uint32_t time)
{
  rx_empty();
  m_puart->print(F("AT+GSLP="));
  m_puart->println(time);

  AT_LIB_LOGDEBUG(F("AT+GSLP="));

  return recvFind("OK");
}

// AT Commands Echoing (mode = 0: OFF, 1: ON)
bool ESP8266::eATE(uint8_t mode)
{
  rx_empty();
  m_puart->print(F("ATE"));
  m_puart->println(mode);

  AT_LIB_LOGDEBUG(F("ATE"));

  return recvFind("OK");
}

// Restores the Factory Default Settings
bool ESP8266::eATRESTORE(void)
{
  rx_empty();
  m_puart->println(F("AT+RESTORE"));

  AT_LIB_LOGDEBUG(F("AT+RESTORE"));

  return recvFind("OK");
}


bool ESP8266::eATSETUART(uint32_t baudrate, uint8_t pattern)
{
  rx_empty();

  if (pattern > 3 || pattern < 1)
  {
    return false;
  }

  // ESP32-AT supports all _CUR, _DEF
  // AT+UART_DEF=<baudrate>,<databits>,<stopbits>,<parity>,<flow control>
  // AT+UART_CUR=115200,8,1,0,0
  switch (pattern)
  {
    case 1:
    // Deprecated => using _CUR
    //m_puart->print(F("AT+UART="));
    //AT_LIB_LOGDEBUG(F("AT+UART="));
    //break;
    case 2:
      m_puart->print(F("AT+UART_CUR="));
      AT_LIB_LOGDEBUG(F("AT+UART_CUR="));
      break;
    case 3:
      m_puart->print(F("AT+UART_DEF="));
      AT_LIB_LOGDEBUG(F("AT+UART_DEF="));
      break;
  }

  m_puart->print(baudrate);
  m_puart->print(F(","));
  m_puart->print(8);
  m_puart->print(F(","));
  m_puart->print(1);
  m_puart->print(F(","));
  m_puart->print(0);
  m_puart->print(F(","));
  m_puart->println(0);

  if (recvFind("OK", 5000))
  {
    return true;
  }
  else
  {
    return false;
  }
}

// Get the Wi-Fi Mode (Station/SoftAP/Station+SoftAP)
// ESP32-AT not support _CUR and _DEF here
bool ESP8266::qATCWMODE(uint8_t *mode, uint8_t pattern)
{
  String str_mode;
  bool ret;

  if (!mode || !pattern)
  {
    return false;
  }
  rx_empty();

  switch (pattern)
  {
#if !USE_ESP32_AT
    case 1 :
      m_puart->println(F("AT+CWMODE_DEF?"));
      AT_LIB_LOGDEBUG(F("AT+CWMODE_DEF?"));
      break;
    case 2:
      m_puart->println(F("AT+CWMODE_CUR?"));
      AT_LIB_LOGDEBUG(F("AT+CWMODE_CUR?"));
      break;
#endif
    default:
      m_puart->println(F("AT+CWMODE?"));
      AT_LIB_LOGDEBUG(F("AT+CWMODE?"));
      break;
  }

  ret = recvFindAndFilter("OK", ":", "\r\n\r\nOK", str_mode);

  if (ret)
  {
    *mode = (uint8_t)str_mode.toInt();
    return true;
  }
  else
  {
    return false;
  }
}

// Test if the Wi-Fi Mode (Station/SoftAP/Station+SoftAP) supported
bool ESP8266::eATCWMODE(String &list)
{
  rx_empty();
  m_puart->println(F("AT+CWMODE=?"));

  AT_LIB_LOGDEBUG(F("AT+CWMODE=?"));

  return recvFindAndFilter("OK", "+CWMODE:(", ")\r\n\r\nOK", list);
}

// Set the Wi-Fi Mode (Station/SoftAP/Station+SoftAP)
// ESP32-AT not support _CUR and _DEF here
bool ESP8266::sATCWMODE(uint8_t mode, uint8_t pattern)
{
  if (!pattern)
  {
    return false;
  }

  String data;

  rx_empty();

  switch (pattern)
  {
#if !USE_ESP32_AT
    case 1 :
      m_puart->print(F("AT+CWMODE_DEF="));
      AT_LIB_LOGDEBUG(F("AT+CWMODE_DEF="));
      break;
    case 2:
      m_puart->print(F("AT+CWMODE_CUR="));
      AT_LIB_LOGDEBUG(F("AT+CWMODE_CUR="));
      break;
#endif
    default:
      m_puart->print(F("AT+CWMODE="));
      AT_LIB_LOGDEBUG(F("AT+CWMODE="));
      break;
  }

  m_puart->println(mode);
  data = recvString("OK", "no change");

  if (data.indexOf("OK") != -1 || data.indexOf("no change") != -1)
  {
    return true;
  }

  return false;
}

// Get connected AP info
// ESP32-AT not support _CUR and _DEF here
bool ESP8266::qATCWJAP(String &ssid, uint8_t pattern)
{
  //bool ret;

  if (!pattern)
  {
    return false;
  }

  rx_empty();

  switch (pattern)
  {
#if !USE_ESP32_AT
    case 1 :
      m_puart->println(F("AT+CWJAP_DEF?"));
      AT_LIB_LOGDEBUG(F("AT+CWJAP_DEF?"));
      break;
    case 2:
      m_puart->println(F("AT+CWJAP_CUR?"));
      AT_LIB_LOGDEBUG(F("AT+CWJAP_CUR?"));
      break;
#endif
    default:
      m_puart->println(F("AT+CWJAP?"));
      AT_LIB_LOGDEBUG(F("AT+CWJAP?"));
      break;
  }

  ssid = recvString("OK", "No AP");

  if (ssid.indexOf("OK") != -1 || ssid.indexOf("No AP") != -1)
  {
    return true;
  }

  return false;

}

// Connects to an AP
// ESP32-AT not support _CUR and _DEF here
bool ESP8266::sATCWJAP(String ssid, String pwd, uint8_t pattern)
{
  String data;

  if (!pattern)
  {
    return false;
  }

  rx_empty();

  switch (pattern)
  {
#if !USE_ESP32_AT
    case 1 :
      m_puart->print(F("AT+CWJAP_DEF=\""));
      AT_LIB_LOGDEBUG(F("AT+CWJAP_DEF=\""));
      break;
    case 2:
      m_puart->print(F("AT+CWJAP_CUR=\""));
      AT_LIB_LOGDEBUG(F("AT+CWJAP_CUR=\""));
      break;
#endif
    default:
      m_puart->print(F("AT+CWJAP=\""));
      AT_LIB_LOGDEBUG(F("AT+CWJAP=\""));
      break;
  }

  m_puart->print(ssid);
  m_puart->print(F("\",\""));
  m_puart->print(pwd);
  m_puart->println(F("\""));

#if USE_ESP32_AT
  data = recvString("OK", "ERROR", 10000);
#else
  data = recvString("OK", "FAIL", 10000);
#endif

  if (data.indexOf("OK") != -1)
  {
    return true;
  }

  return false;
}

// Lists the Available APs
bool ESP8266::eATCWLAP(String &list)
{
  String data;
  rx_empty();
  m_puart->println(F("AT+CWLAP"));

  AT_LIB_LOGDEBUG(F("AT+CWLAP"));

  return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", list, 15000);
}

// Disconnects from the AP
bool ESP8266::eATCWQAP(void)
{
  String data;

  rx_empty();
  m_puart->println(F("AT+CWQAP"));

  AT_LIB_LOGDEBUG(F("AT+CWQAP"));

  return recvFind("OK");
}

// Get Configuration Params of the ESP8266/ESP32 SoftAP
// ESP32-AT not support _CUR and _DEF here
bool ESP8266::qATCWSAP(String &List, uint8_t pattern)
{
  if (!pattern)
  {
    return false;
  }

  rx_empty();

  switch (pattern)
  {
#if !USE_ESP32_AT
    case 1 :
      m_puart->println(F("AT+CWSAP_DEF?"));
      AT_LIB_LOGDEBUG(F("AT+CWSAP_DEF?"));
      break;
    case 2:
      m_puart->println(F("AT+CWSAP_CUR?"));
      AT_LIB_LOGDEBUG(F("AT+CWSAP_CUR?"));
      break;
#endif
    default:
      m_puart->println(F("AT+CWSAP?"));
      AT_LIB_LOGDEBUG(F("AT+CWSAP?"));
      break;
  }

  return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", List, 10000);
}

// Set Configuration Params of the ESP8266/ESP32 SoftAP
// ESP32-AT not support _CUR and _DEF here
bool ESP8266::sATCWSAP(String ssid, String pwd, uint8_t chl, uint8_t ecn, uint8_t pattern)
{
  String data;

  if (!pattern)
  {
    return false;
  }

  rx_empty();

  switch (pattern)
  {
#if !USE_ESP32_AT
    case 1 :
      m_puart->print(F("AT+CWSAP_DEF=\""));
      AT_LIB_LOGDEBUG(F("AT+CWSAP_DEF=\""));
      break;
    case 2:
      m_puart->print(F("AT+CWSAP_CUR=\""));
      AT_LIB_LOGDEBUG(F("AT+CWSAP_CUR=\""));
      break;
#endif
    default:
      m_puart->print(F("AT+CWSAP=\""));
      AT_LIB_LOGDEBUG(F("AT+CWSAP=\""));
      break;
  }

  m_puart->print(ssid);
  m_puart->print(F("\",\""));
  m_puart->print(pwd);
  m_puart->print(F("\","));
  m_puart->print(chl);
  m_puart->print(F(","));
  m_puart->println(ecn);

#if USE_ESP32_AT
  data = recvString("OK", 5000);
#else
  data = recvString("OK", "ERROR", 5000);
#endif

  if (data.indexOf("OK") != -1)
  {
    return true;
  }

  return false;
}

// Get IP of Stations to Which the ESP8266/ESP32 SoftAP is connected
bool ESP8266::eATCWLIF(String &list)
{
  String data;

  rx_empty();
  m_puart->println(F("AT+CWLIF"));

  AT_LIB_LOGDEBUG(F("AT+CWLIF"));

  return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", list);
}

// Get info if DHCP Enabled or Disabled
// ESP32-AT not support _CUR and _DEF here and has different command format
bool ESP8266::qATCWDHCP(String &List, uint8_t pattern)
{
  if (!pattern)
  {
    return false;
  }

  rx_empty();

  switch (pattern)
  {
#if !USE_ESP32_AT
    case 1 :
      m_puart->println(F("AT+CWDHCP_DEF?"));
      AT_LIB_LOGDEBUG(F("AT+CWDHCP_DEF?"));
      break;
    case 2:
      m_puart->println(F("AT+CWDHCP_CUR?"));
      AT_LIB_LOGDEBUG(F("AT+CWDHCP_CUR?"));
      break;
#endif
    default:
      m_puart->println(F("AT+CWDHCP?"));
      AT_LIB_LOGDEBUG(F("AT+CWDHCP?"));
      break;
  }

  return recvFindAndFilter("OK", "\r\r\n", "\r\nOK", List, 10000);
}

// Get info if DHCP Enabled or Disabled
// ESP32-AT not support _CUR and _DEF here and has different command format
// Be careful
// ESP32-AT => AT+CWDHCP=<operate>,<mode>
// • <operate>:
//    ‣ 0: disable
//    ‣ 1: enable
// • <mode>:
//    ‣ Bit0: Station DHCP
//    ‣ Bit1: SoftAP DHCP
//
// ESP8266-AT => AT+CWDHCP=<mode>,<en>
// • <mode>:
//    ‣ 0: Sets ESP8266 SoftAP
//    ‣ 1: Sets ESP8266 Station
//    ‣ 2: Sets both SoftAP and Station
// • <en>:
//    ‣ 0: disable DHCP
//    ‣ 1: enable DHCP

#if USE_ESP32_AT
// For ESP32-AT
// ESP32-AT => AT+CWDHCP=<operate>,<mode>
// • <operate>:
//    ‣ 0: disable
//    ‣ 1: enable
// • <mode>:
//    ‣ Bit0: Station DHCP
//    ‣ Bit1: SoftAP DHCP
bool ESP8266::sATCWDHCP(uint8_t op, uint8_t mode, uint8_t pattern)
{
  String data;

  if (!pattern)
  {
    return false;
  }

  rx_empty();

  switch (pattern)
  {
    default:
      m_puart->print(F("AT+CWDHCP="));
      AT_LIB_LOGDEBUG(F("AT+CWDHCP="));
  }

  m_puart->print(op);
  m_puart->print(F(","));
  m_puart->println(mode);
  data = recvString("OK", 2000);

  if (data.indexOf("OK") != -1)
  {
    return true;
  }

  return false;
}
#else
// For ESP8266
// ESP8266-AT => AT+CWDHCP=<mode>,<en>
// • <mode>:
//    ‣ 0: Sets ESP8266 SoftAP
//    ‣ 1: Sets ESP8266 Station
//    ‣ 2: Sets both SoftAP and Station
// • <en>:
//    ‣ 0: disable DHCP
//    ‣ 1: enable DHCP
bool ESP8266::sATCWDHCP(uint8_t mode, uint8_t en, uint8_t pattern)
{
  String data;

  if (!pattern)
  {
    return false;
  }

  rx_empty();

  switch (pattern)
  {
    case 1 :
      m_puart->print(F("AT+CWDHCP_DEF="));
      AT_LIB_LOGDEBUG(F("AT+CWDHCP_DEF="));
      break;
    case 2:
      m_puart->print(F("AT+CWDHCP_CUR="));
      AT_LIB_LOGDEBUG(F("AT+CWDHCP_CUR="));
      break;
    default:
      m_puart->print(F("AT+CWDHCP="));
      AT_LIB_LOGDEBUG(F("AT+CWDHCP="));
  }

  m_puart->print(mode);
  m_puart->print(F(","));
  m_puart->println(en);
  data = recvString("OK", "ERROR", 2000);

  if (data.indexOf("OK") != -1)
  {
    return true;
  }

  return false;
}
#endif

// ESP8266-AT => AT+CWDHCP=<mode>,<en>
// • <en>:
//    ‣ 0: disable DHCP
//    ‣ 1: enable DHCP
// Auto-Connects to the AP or Not
bool ESP8266::eATCWAUTOCONN(uint8_t en)
{
  rx_empty();

  if (en > 1)
  {
    return false;
  }

  m_puart->print(F("AT+CWAUTOCONN="));

  AT_LIB_LOGDEBUG(F("AT+CWAUTOCONN="));

  m_puart->println(en);

  return recvFind("OK");
}

// Get the MAC Address of the ESP8266/ESP32 Station
// ESP32-AT not support _CUR and _DEF here
bool ESP8266::qATCIPSTAMAC(String &mac, uint8_t pattern)
{
  rx_empty();

  if (!pattern)
  {
    return false;
  }

  switch (pattern)
  {
#if !USE_ESP32_AT
    case 1 :
      m_puart->println(F("AT+CIPSTAMAC_DEF?"));
      AT_LIB_LOGDEBUG(F("AT+CIPSTAMAC_DEF?"));
      break;
    case 2:
      m_puart->println(F("AT+CIPSTAMAC_CUR?"));
      AT_LIB_LOGDEBUG(F("AT+CIPSTAMAC_CUR?"));
      break;
#endif
    default:
      m_puart->println(F("AT+CIPSTAMAC?"));
      AT_LIB_LOGDEBUG(F("AT+CIPSTAMAC?"));
      break;
  }

  return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", mac, 2000);
}

// Set the MAC Address of the ESP8266/ESP32 Station
// ESP32-AT not support _CUR and _DEF here
bool ESP8266::eATCIPSTAMAC(String mac, uint8_t pattern)
{
  rx_empty();

  if (!pattern)
  {
    return false;
  }

  switch (pattern)
  {
#if !USE_ESP32_AT
    case 1 :
      m_puart->print(F("AT+CIPSTAMAC_DEF="));
      AT_LIB_LOGDEBUG(F("AT+CIPSTAMAC_DEF="));
      break;
    case 2:
      m_puart->print(F("AT+CIPSTAMAC_CUR="));
      AT_LIB_LOGDEBUG(F("AT+CIPSTAMAC_CUR="));
      break;
#endif
    default:
      m_puart->print(F("AT+CIPSTAMAC="));
      AT_LIB_LOGDEBUG(F("AT+CIPSTAMAC="));
      break;
  }

  m_puart->print(F("\""));
  m_puart->print(mac);
  m_puart->println(F("\""));

  return recvFind("OK");
}

// Get the IP Address of the ESP8266/ESP32 Station
// ESP32-AT not support _CUR and _DEF here
bool ESP8266::qATCIPSTAIP(String &ip, uint8_t pattern)
{
  rx_empty();

  if (!pattern)
  {
    return false;
  }

  switch (pattern)
  {
#if !USE_ESP32_AT
    case 1 :
      m_puart->println(F("AT+CIPSTA_DEF?"));
      AT_LIB_LOGDEBUG(F("AT+CIPSTA_DEF?"));
      break;
    case 2:
      m_puart->println(F("AT+CIPSTA_CUR?"));
      AT_LIB_LOGDEBUG(F("AT+CIPSTA_CUR?"));
      break;
#endif
    default:
      m_puart->println(F("AT+CIPSTA?"));
      AT_LIB_LOGDEBUG(F("AT+CIPSTA?"));
      break;
  }

  return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", ip, 2000);
}

// Set the IP Address of the ESP8266/ESP32 Station
// ESP32-AT not support _CUR and _DEF here
bool ESP8266::eATCIPSTAIP(String ip, String gateway, String netmask, uint8_t pattern)
{

  rx_empty();

  if (!pattern)
  {
    return false;
  }

  switch (pattern)
  {
#if !USE_ESP32_AT
    case 1 :
      m_puart->print(F("AT+CIPSTA_DEF="));
      AT_LIB_LOGDEBUG(F("AT+CIPSTA_DEF="));
      break;
    case 2:
      m_puart->print(F("AT+CIPSTA_CUR="));
      AT_LIB_LOGDEBUG(F("AT+CIPSTA_CUR="));
      break;
#endif
    default:
      m_puart->print(F("AT+CIPSTA="));
      AT_LIB_LOGDEBUG(F("AT+CIPSTA="));
      break;
  }

  m_puart->print(F("\""));
  m_puart->print(ip);
  m_puart->print(F("\",\""));
  m_puart->print(gateway);
  m_puart->print(F("\",\""));
  m_puart->print(netmask);
  m_puart->println(F("\""));

  return recvFind("OK");
}

// Get the IP Address of the ESP8266/ESP32 SoftAP
// ESP32-AT not support _CUR and _DEF here
bool ESP8266::qATCIPAP(String &ip, uint8_t pattern)
{
  rx_empty();

  if (!pattern)
  {
    return false;
  }

  switch (pattern)
  {
#if !USE_ESP32_AT
    case 1 :
      m_puart->println(F("AT+CIPAP_DEF?"));
      AT_LIB_LOGDEBUG(F("AT+CIPAP_DEF?"));
      break;
    case 2:
      m_puart->println(F("AT+CIPAP_CUR?"));
      AT_LIB_LOGDEBUG(F("AT+CIPAP_CUR?"));
      break;
#endif
    default:
      m_puart->println(F("AT+CIPAP?"));
      AT_LIB_LOGDEBUG(F("AT+CIPAP?"));
      break;
  }

  return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", ip, 2000);
}

// Set the IP Address of the ESP8266/ESP32 SoftAP
// ESP32-AT not support _CUR and _DEF here
bool ESP8266::eATCIPAP(String ip, uint8_t pattern)
{
  rx_empty();

  if (!pattern)
  {
    return false;
  }

  switch (pattern)
  {
#if !USE_ESP32_AT
    case 1 :
      m_puart->print(F("AT+CIPAP_DEF="));
      AT_LIB_LOGDEBUG(F("AT+CIPAP_DEF="));
      break;
    case 2:
      m_puart->print(F("AT+CIPAP_CUR="));
      AT_LIB_LOGDEBUG(F("AT+CIPAP_CUR="));
      break;
#endif
    default:
      m_puart->print(F("AT+CIPAP="));
      AT_LIB_LOGDEBUG(F("AT+CIPAP="));
      break;
  }

  m_puart->print(F("\""));
  m_puart->print(ip);
  m_puart->println(F("\""));

  return recvFind("OK");
}

// Starts SmartConfig
// <type>:
// ‣ 1: ESP-TOUCH
// ‣ 2: AirKiss
// ‣ 3: ESP-TOUCH+AirKiss
bool ESP8266::eCWSTARTSMART(uint8_t type)
{
  rx_empty();
  m_puart->print(F("AT+CWSTARTSMART="));

  AT_LIB_LOGDEBUG(F("AT+CWSTARTSMART="));
  m_puart->println(type);

  return recvFind("OK");
}

// Stops SmartConfig
bool ESP8266::eCWSTOPSMART(void)
{
  rx_empty();
  m_puart->println(F("AT+CWSTOPSMART"));

  AT_LIB_LOGDEBUG(F("AT+CWSTOPSMART"));

  return recvFind("OK");
}

// Gets the Connection Status
bool ESP8266::eATCIPSTATUS(String &list)
{
  String data;

  delay(100);
  rx_empty();
  m_puart->println(F("AT+CIPSTATUS"));

  AT_LIB_LOGDEBUG(F("AT+CIPSTATUS"));

  return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", list);
}

// Establishes TCP Connection, UDP Transmission or SSL Connection
// Single connection (AT+CIPMUX=0)
// For ESP8266 SSL
// SSL connection needs a large amount of memory; otherwise, it may cause system reboot. Use
// AT+CIPSSLSIZE=<size> to enlarge the SSL buffer size.
bool ESP8266::sATCIPSTARTSingle(String type, String addr, uint32_t port)
{
  String data;

  rx_empty();
  m_puart->print(F("AT+CIPSTART=\""));

  AT_LIB_LOGDEBUG(F("AT+CIPSTART=\""));

  m_puart->print(type);
  m_puart->print(F("\",\""));
  m_puart->print(addr);
  m_puart->print(F("\","));
  m_puart->println(port);

#if USE_ESP32_AT
  data = recvString("OK", 10000);

  if (data.indexOf("OK") != -1)
  {
    return true;
  }
#else
  data = recvString("OK", "ERROR", "ALREADY CONNECT", 10000);

  if (data.indexOf("OK") != -1 || data.indexOf("ALREADY CONNECT") != -1)
  {
    return true;
  }
#endif

  return false;
}

// Establishes TCP Connection, UDP Transmission or SSL Connection
// Multiple Connections (AT+CIPMUX=1)
// For ESP8266 SSL
// SSL connection needs a large amount of memory; otherwise, it may cause system reboot. Use
// AT+CIPSSLSIZE=<size> to enlarge the SSL buffer size.
bool ESP8266::sATCIPSTARTMultiple(uint8_t mux_id, String type, String addr, uint32_t port)
{
  String data;

  rx_empty();
  m_puart->print(F("AT+CIPSTART="));

  AT_LIB_LOGDEBUG(F("AT+CIPSTART="));

  m_puart->print(mux_id);
  m_puart->print(F(",\""));
  m_puart->print(type);
  m_puart->print(F("\",\""));
  m_puart->print(addr);
  m_puart->print(F("\","));
  m_puart->println(port);

#if USE_ESP32_AT
  data = recvString("OK", 10000);

  if (data.indexOf("OK") != -1)
  {
    return true;
  }
#else
  data = recvString("OK", "ERROR", "ALREADY CONNECT", 10000);

  if (data.indexOf("OK") != -1 || data.indexOf("ALREADY CONNECT") != -1)
  {
    return true;
  }
#endif

  return false;
}

// Sends Data of designated length.
// Single connection: (+CIPMUX=0)     => AT+CIPSEND=<length>
bool ESP8266::sATCIPSENDSingle(const uint8_t *buffer, uint32_t len)
{
  rx_empty();
  m_puart->print(F("AT+CIPSEND="));

  AT_LIB_LOGDEBUG(F("AT+CIPSEND="));

  m_puart->println(len);

  if (recvFind(">", 5000))
  {
    rx_empty();

    for (uint32_t i = 0; i < len; i++)
    {
      m_puart->write(buffer[i]);
    }

    return recvFind("SEND OK", 10000);
  }

  return false;
}

// Sends Data of designated length.
// Multiple connections: (+CIPMUX=1)  => AT+CIPSEND=<link ID>,<length>
bool ESP8266::sATCIPSENDMultiple(uint8_t mux_id, const uint8_t *buffer, uint32_t len)
{
  rx_empty();
  m_puart->print(F("AT+CIPSEND="));

  AT_LIB_LOGDEBUG(F("AT+CIPSEND="));

  m_puart->print(mux_id);
  m_puart->print(F(","));
  m_puart->println(len);

  if (recvFind(">", 5000))
  {
    rx_empty();

    for (uint32_t i = 0; i < len; i++)
    {
      m_puart->write(buffer[i]);
    }

    return recvFind("SEND OK", 10000);
  }

  return false;
}

// Sends Data of designated length.
// Single connection: (+CIPMUX=0)     => AT+CIPSEND=<length>
bool ESP8266::sATCIPSENDSingleFromFlash(const uint8_t *buffer, uint32_t len)
{
  rx_empty();
  m_puart->print(F("AT+CIPSEND="));

  AT_LIB_LOGDEBUG(F("AT+CIPSEND="));

  m_puart->println(len);

  if (recvFind(">", 5000))
  {
    rx_empty();

    for (uint32_t i = 0; i < len; i++)
    {
      m_puart->write((char) pgm_read_byte(&buffer[i]));
    }

    return recvFind("SEND OK", 10000);
  }
  return false;
}

// Sends Data of designated length.
// Multiple connections: (+CIPMUX=1)  => AT+CIPSEND=<link ID>,<length>
bool ESP8266::sATCIPSENDMultipleFromFlash(uint8_t mux_id, const uint8_t *buffer, uint32_t len)
{
  rx_empty();
  m_puart->print(F("AT+CIPSEND="));

  AT_LIB_LOGDEBUG(F("AT+CIPSEND="));

  m_puart->print(mux_id);
  m_puart->print(F(","));
  m_puart->println(len);

  if (recvFind(">", 5000))
  {
    rx_empty();

    for (uint32_t i = 0; i < len; i++)
    {
      m_puart->write((char) pgm_read_byte(&buffer[i]));
    }

    return recvFind("SEND OK", 10000);
  }

  return false;
}

// Closes TCP/UDP/SSL Connection for multiple connections
// AT+CIPCLOSE=<link ID>
bool ESP8266::sATCIPCLOSEMultiple(uint8_t mux_id)
{
  String data;

  rx_empty();
  m_puart->print(F("AT+CIPCLOSE="));

  AT_LIB_LOGDEBUG(F("AT+CIPCLOSE="));

  m_puart->println(mux_id);

#if 1
  data = recvString("OK", 5000);

  if (data.indexOf("OK") != -1)
  {
    return true;
  }
#else
  data = recvString("OK", "link is not", 5000);

  if (data.indexOf("OK") != -1 || data.indexOf("link is not") != -1)
  {
    return true;
  }
#endif

  return false;
}

// Closes TCP/UDP/SSL Connection for single connections
bool ESP8266::eATCIPCLOSESingle(void)
{
  rx_empty();
  m_puart->println(F("AT+CIPCLOSE"));

  AT_LIB_LOGDEBUG(F("AT+CIPCLOSE"));

  return recvFind("OK", 5000);
}

// Gets the Local IP Address
bool ESP8266::eATCIFSR(String &list)
{
  rx_empty();
  m_puart->println(F("AT+CIFSR"));

  AT_LIB_LOGDEBUG(F("AT+CIFSR"));

  return recvFindAndFilter("OK", "\r\r\n", "\r\n\r\nOK", list);
}

// Enables/Disables Multiple Connections
bool ESP8266::sATCIPMUX(uint8_t mode)
{
  String data;

  rx_empty();
  m_puart->print(F("AT+CIPMUX="));

  AT_LIB_LOGDEBUG(F("AT+CIPMUX="));

  m_puart->println(mode);

  //data = recvString("OK", "Link is builded");
  data = recvString("OK");

  if (data.indexOf("OK") != -1)
  {
    return true;
  }

  return false;
}

// Deletes/Creates TCP Server
// • <mode>:
// ‣ 0: deletes server.
// ‣ 1: creates server.
// • <port>: port number; 333 by default.
// • A TCP server can only be created when multiple connections are activated (AT+CIPMUX=1)
bool ESP8266::sATCIPSERVER(uint8_t mode, uint32_t port)
{
  String data;

  if (mode)
  {
    rx_empty();
    m_puart->print(F("AT+CIPSERVER=1,"));

    AT_LIB_LOGDEBUG(F("AT+CIPSERVER=1,"));

    m_puart->println(port);

#if 1
    data = recvString("OK");

    if (data.indexOf("OK") != -1)
    {
      return true;
    }
#else
    data = recvString("OK", "no change");

    if (data.indexOf("OK") != -1 || data.indexOf("no change") != -1)
    {
      return true;
    }
#endif

    return false;
  }
  else
  {
    rx_empty();
    m_puart->println(F("AT+CIPSERVER=0"));

    AT_LIB_LOGDEBUG(F("AT+CIPSERVER=0"));

    return recvFind("\r\r\n");
  }
}

// Sets Transmission Mode
// ‣ 0: normal transmission mode.
// ‣ 1: UART-Wi-Fi passthrough mode (transparent transmission), which can only be enabled in TCP/SSL
//      single connection mode or in UDP mode when the remote IP and port do not change.
bool ESP8266::sATCIPMODE(uint8_t mode)
{
  String data;

  if (mode > 1)
  {
    return false;
  }

  rx_empty();
  m_puart->print(F("AT+CIPMODE="));

  AT_LIB_LOGDEBUG(F("AT+CIPMODE="));

  m_puart->println(mode);

  //data = recvString("OK", "Link is builded", 2000);
  data = recvString("OK", 2000);

  if (data.indexOf("OK") != -1 )
  {
    return true;
  }

  return false;
}

// Saves the Transparent Transmission Link in Flash
// • <mode>:
// ‣ 0: ESP8266/ESP32 will NOT enter UART-Wi-Fi passthrough mode on power-up.
// ‣ 1: ESP8266/ESP32 will enter UART-Wi-Fi passthrough mode on power-up.
bool ESP8266::eATSAVETRANSLINK(uint8_t mode, String ip, uint32_t port)
{

  String data;

  rx_empty();
  m_puart->print(F("AT+SAVETRANSLINK="));

  AT_LIB_LOGDEBUG(F("AT+SAVETRANSLINK="));

  m_puart->print(mode);
  m_puart->print(F(",\""));
  m_puart->print(ip);
  m_puart->print(F("\","));
  m_puart->println(port);

  //data = recvString("OK", "ERROR", 2000);
  data = recvString("OK", 2000);

  if (data.indexOf("OK") != -1 )
  {
    return true;
  }

  return false;
}

// Ping Packets
bool ESP8266::eATPING(String ip)
{
  rx_empty();
  m_puart->print(F("AT+PING="));

  AT_LIB_LOGDEBUG(F("AT+PING="));

  m_puart->print(F("\""));
  m_puart->print(ip);
  m_puart->println(F("\""));

  return recvFind("OK", 2000);
}

// Sets the TCP Server Timeout
bool ESP8266::sATCIPSTO(uint32_t timeout)
{
  rx_empty();
  m_puart->print(F("AT+CIPSTO="));

  AT_LIB_LOGDEBUG(F("AT+CIPSTO="));

  m_puart->println(timeout);

  return recvFind("OK");
}

//KH Added
/////////////
uint32_t ESP8266::recv(uint8_t *buffer, uint32_t buffer_size, uint32_t timeout)
{
  return recvPkg(buffer, buffer_size, NULL, timeout, NULL);
}

uint32_t ESP8266::recv(uint8_t mux_id, uint8_t *buffer, uint32_t buffer_size, uint32_t timeout)
{
  uint8_t id;
  uint32_t ret;
  
  ret = recvPkg(buffer, buffer_size, NULL, timeout, &id);
  
  if (ret > 0 && id == mux_id) 
  {
    return ret;
  }
  
  return 0;
}

uint32_t ESP8266::recv(uint8_t *coming_mux_id, uint8_t *buffer, uint32_t buffer_size, uint32_t timeout)
{
  return recvPkg(buffer, buffer_size, NULL, timeout, coming_mux_id);
}

/*----------------------------------------------------------------------------*/
/* +IPD,<id>,<len>:<data> */
/* +IPD,<len>:<data> */

uint32_t ESP8266::recvPkg(uint8_t *buffer, uint32_t buffer_size, uint32_t *data_len, uint32_t timeout, uint8_t *coming_mux_id)
{
  String data;
  char a;
  int32_t index_PIPDcomma = -1;
  int32_t index_colon = -1; /* : */
  int32_t index_comma = -1; /* , */
  int32_t len = -1;
  int8_t id = -1;
  bool has_data = false;
  uint32_t ret;
  unsigned long start;
  uint32_t i;

  if (buffer == NULL)
  {
    return 0;
  }

  start = millis();
  
  while (millis() - start < timeout) 
  {
    if (m_puart->available() > 0) 
    {
      a = m_puart->read();
      data += a;
    }

    index_PIPDcomma = data.indexOf("+IPD,");
    
    if (index_PIPDcomma != -1) 
    {
      index_colon = data.indexOf(':', index_PIPDcomma + 5);
      
      if (index_colon != -1) 
      {
        index_comma = data.indexOf(',', index_PIPDcomma + 5);
        
        /* +IPD,id,len:data */
        if (index_comma != -1 && index_comma < index_colon) 
        {
          id = data.substring(index_PIPDcomma + 5, index_comma).toInt();
          
          if (id < 0 || id > 4) 
          {
            return 0;
          }
          
          len = data.substring(index_comma + 1, index_colon).toInt();
          
          if (len <= 0) 
          {
            return 0;
          }
        } 
        else 
        { 
          /* +IPD,len:data */
          len = data.substring(index_PIPDcomma + 5, index_colon).toInt();
          
          if (len <= 0) 
          {
            return 0;
          }
        }
        
        has_data = true;
        break;
      }
    }
  }

  if (has_data) 
  {
    i = 0;
    ret = (uint32_t) len > buffer_size ? buffer_size : (uint32_t) len;
    start = millis();
    
    while (millis() - start < 3000) 
    {
      while (m_puart->available() > 0 && i < ret) 
      {
        a = m_puart->read();
        buffer[i++] = a;
      }
      
      if (i == ret) 
      {
        rx_empty();
        
        if (data_len) 
        {
          *data_len = len;
        }
        
        if (index_comma != -1 && coming_mux_id) 
        {
          *coming_mux_id = id;
        }
        
        return ret;
      }
    }
  }
  return 0;
}
///////////////

#endif    // __ESP_AT_LIB_IMPL_H__

