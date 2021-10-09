/****************************************************************************************************************************
  ESP_AT_Lib.h - Dead simple ESP8266/ESP32-AT-command wrapper
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

#ifndef __ESP_AT_LIB_H__
#define __ESP_AT_LIB_H__

#define ESP_AT_LIB_VERSION              "ESP_AT_Lib v1.4.0"

#ifndef ESP_AT_LIB_DEBUG_OUTPUT
  #define ESP_AT_LIB_DEBUG_OUTPUT       Serial
#endif

#ifndef ESP_AT_LIB_DEBUG
  #define ESP_AT_LIB_DEBUG              false
#endif

// Default to use ESP8266-AT
// _CUR and _DEF only valid for ESP8266-AT
#ifndef USE_ESP32_AT  
  #define USE_ESP32_AT              false
#endif

#if USE_ESP32_AT
  #warning Using ESP32 AT command in ESP_AT_Lib
#else
  #warning Using ESP8266 AT command in ESP_AT_Lib
#endif

#include "Arduino.h"
#include "ESP_AT_Debug.h"
#include "ESP_Board_Config.h"

#define  DEFAULT_PATTERN  3

#if ( defined(ARDUINO_AVR_ADK) || defined(ARDUINO_AVR_MEGA) || defined(ARDUINO_AVR_MEGA2560) )
  #if defined(SERIAL_TX_BUFFER_SIZE)
    #undef SERIAL_TX_BUFFER_SIZE
    #define SERIAL_TX_BUFFER_SIZE     256
  #endif
  
  #if defined(SERIAL_RX_BUFFER_SIZE)
    #undef SERIAL_RX_BUFFER_SIZE
    #define SERIAL_RX_BUFFER_SIZE     256
  #endif
#else
#if defined(SERIAL_TX_BUFFER_SIZE)
    #undef SERIAL_TX_BUFFER_SIZE
    #define SERIAL_TX_BUFFER_SIZE     2048
  #endif
  
  #if defined(SERIAL_RX_BUFFER_SIZE)
    #undef SERIAL_RX_BUFFER_SIZE
    #define SERIAL_RX_BUFFER_SIZE     2048
  #endif
#endif


/**
   Provide an easy-to-use way to manipulate ESP_AT.
*/

// KH, Still use ESP8266 class to avoid mods in old libs
class ESP8266
{
  public:

    typedef void (*onData)(uint8_t mux_id, uint32_t len, void* ptr);

    /*
       Constructor.

       @param uart - an pointer of Serial object.
       @warning parameter baud depends on the AT firmware. 9600 is an common value.
    */

    ESP8266(Stream* uart = &Serial);

    Stream* getUart()
    {
      return m_puart;
    }
    
    void changeUart(Stream* uart = &Serial)
    {
      m_puart = uart;
    }


    void setOnData(onData cbk, void* ptr)
    {
      m_onData = cbk;
      m_onDataPtr = ptr;
    }

    void run();

    /**
       Verify ESP_AT whether live or not.

       Actually, this method will send command "AT" to ESP_AT and waiting for "OK".

       @retval true - alive.
       @retval false - dead.
    */
    bool kick(void);

    /**
       Restart ESP_AT by "AT+RST".

       This method will take 3 seconds or more.

       @retval true - success.
       @retval false - failure.
    */
    bool restart(void);

    /**
       Get the version of AT Command Set.

       @return the string of version.
    */
    String getVersion(void);

    /**
       Start function of deep sleep.

        @param time - the sleep time.
        @retval true - success.
        @retval false - failure.
        @note the feature requires hardware support.
    */
    bool deepSleep(uint32_t time);

    /**
       Switch the echo function.

        @param mode - 1 start echo -0 stop echo
        @retval true - success.
        @retval false - failure.

    */
    bool setEcho(uint8_t mode);

    /**
         Restore factory.
         @retval true - success.
         @retval false - failure.
         @note  The operation can lead to restart the machine.
    */
    bool restore(void);

    /**
       Set up a serial port configuration.

       @param pattern -1 send "AT+UART=", -2 send "AT+UART_CUR=", -3 send "AT+UART_DEF=".
       @param baudrate - the uart baudrate.
       @retval true - success.
       @retval false - failure.
       @note  Only allows baud rate design, for the other parameters:databits- 8,stopbits -1,parity -0,flow control -0 .
    */
    bool setUart(uint32_t baudrate, uint8_t pattern);

    /**
       Set operation mode to station.

       @param   pattern1 -1, send "AT+CWMODE_DEF?",-2,send "AT+CWMODE_CUR?",-3,send "AT+CWMODE?".
       @param   pattern2 -1, send "AT+CWMODE_DEF=",-2,send "AT+CWMODE_CUR=",-3,send "AT+CWMODE=".
       @retval true - success.
       @retval false - failure.

    */
    bool setOprToStation(uint8_t pattern1 = DEFAULT_PATTERN, uint8_t pattern2 = DEFAULT_PATTERN);

    /**
       Get the model values list.

       @return the list of model.
    */
    String getWifiModeList(void);

    /**
       Set operation mode to softap.
       @param   pattern1 -1, send "AT+CWMODE_DEF?",-2,send "AT+CWMODE_CUR?",-3,send "AT+CWMODE?".
       @param   pattern2 -1, send "AT+CWMODE_DEF=",-2,send "AT+CWMODE_CUR=",-3,send "AT+CWMODE=".

       @retval true - success.
       @retval false - failure.
    */
    bool setOprToSoftAP(uint8_t pattern1 = DEFAULT_PATTERN, uint8_t pattern2 = DEFAULT_PATTERN);

    /**
       Set operation mode to station + softap.
       @param   pattern1 -1, send "AT+CWMODE_DEF?",-2,send  "AT+CWMODE_CUR?",-3,send "AT+CWMODE?".
       @param   pattern2 -1, send "AT+CWMODE_DEF=",-2,send "AT+CWMODE_CUR=",-3,send "AT+CWMODE=".

       @retval true - success.
       @retval false - failure.
    */
    bool setOprToStationSoftAP(uint8_t pattern1 = DEFAULT_PATTERN, uint8_t pattern2 = DEFAULT_PATTERN);

    /**
       Get the operation mode.
       @param   pattern1 -1, send "AT+CWMODE_DEF?",-2,send  "AT+CWMODE_CUR?",-3,send "AT+CWMODE?".

       @retval 0 - failure.
       @retval 1 - mode Station.
       @retval 2 - mode AP.
       @retval 3 - mode AP + station.
    */
    uint8_t getOprMode(uint8_t pattern1 = DEFAULT_PATTERN);

    /**
       Search available AP list and return it.

       @return the list of available APs.
       @note This method will occupy a lot of memeory(hundreds of Bytes to a couple of KBytes).
        Do not call this method unless you must and ensure that your board has enough memery left.
    */
    String getAPList(void);

    /**
       Search and returns the current connect AP.

       @param pattern -1, send "AT+CWJAP_DEF?",-2,send "AT+CWJAP_CUR?",-3,send "AT+CWJAP?".
       @return the ssid of AP connected now.
    */
    String getNowConecAp(uint8_t pattern = DEFAULT_PATTERN);

    /**
       Join in AP.

       @param pattern -1 send "AT+CWJAP_DEF=" -2 send "AT+CWJAP_CUR=" -3 send "AT+CWJAP=".
       @param ssid - SSID of AP to join in.
       @param pwd - Password of AP to join in.
       @retval true - success.
       @retval false - failure.
       @note This method will take a couple of seconds.
    */
    bool joinAP(String ssid, String pwd, uint8_t pattern = DEFAULT_PATTERN);

    /**
       Leave AP joined before.

       @retval true - success.
       @retval false - failure.
    */
    bool leaveAP(void);

    /**
       Set SoftAP parameters.

       @param pattern -1 send "AT+CWSAP_DEF=" -2 send "AT+CWSAP_CUR=" -3 send "AT+CWSAP=".
       @param ssid - SSID of SoftAP.
       @param pwd - PASSWORD of SoftAP.
       @param chl - the channel (1 - 13, default: 7).
       @param ecn - the way of encrypstion (0 - OPEN, 1 - WEP,
        2 - WPA_PSK, 3 - WPA2_PSK, 4 - WPA_WPA2_PSK, default: 4).
       @retval true - success.
       @retval false - failure.
       @note This method should not be called when station mode.
    */
    bool setSoftAPParam(String ssid, String pwd, uint8_t chl = 7, uint8_t ecn = 4, uint8_t pattern = DEFAULT_PATTERN);

    /**
       get SoftAP parameters.

       @param pattern -1 send "AT+CWSAP_DEF?" -2 send "AT+CWSAP_CUR?" -3 send "AT+CWSAP?".
       @note This method should not be called when station mode.
    */
    String getSoftAPParam(uint8_t pattern = DEFAULT_PATTERN);

    /**
       Get the IP list of devices connected to SoftAP.

       @return the list of IP.
       @note This method should not be called when station mode.
    */
    String getJoinedDeviceIP(void);

    /**
       Get the current state of DHCP.

       @param pattern -1 send "AT+CWDHCP_DEF?" -2 send "AT+CWDHCP_CUR?"  -3 send "AT+CWDHCP?".
       @return the state of DHCP.

    */
    String getDHCP(uint8_t pattern = DEFAULT_PATTERN);

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
    bool setDHCP(uint8_t op, uint8_t mode, uint8_t pattern = DEFAULT_PATTERN);

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
    bool setDHCP(uint8_t mode, uint8_t en, uint8_t pattern = DEFAULT_PATTERN);
#endif

    /**
      make boot automatically connected.
      @param en -1 enable  -0 disable.
      @retval true - success.
      @retval false - failure.
    */
    bool setAutoConnect(uint8_t en);

    /**
      Get the station's MAC address.
      @param pattern -1 send "AT+CIPSTAMAC_DEF?=" -2 send "AT+CIPSTAMAC_CUR?" -3 send "AT+CIPSTAMAC?".
      @return mac address.
      @note This method should not be called when ap mode.
    */
    String getStationMac(uint8_t pattern = DEFAULT_PATTERN);

    /**
      Set the station's MAC address.
      @param pattern -1 send "AT+CIPSTAMAC_DEF=" -2 send "AT+CIPSTAMAC_CUR=" -3 send "AT+CIPSTAMAC=".
      @param mac - the mac address of station.
      @retval true - success.
      @retval false - failure.
    */
    bool setStationMac(String mac, uint8_t pattern = DEFAULT_PATTERN);

    /**
      Get the station's IP.
      @param pattern -1 send "AT+CIPSTA_DEF?" -2 send "AT+CIPSTA_CUR?" -3 send "AT+CIPSTA?".
      @return the station's IP.
      @note This method should not be called when ap mode.
    */
    String getStationIp(uint8_t pattern = DEFAULT_PATTERN);

    /**
      Set the station's IP.
      @param pattern -1 send "AT+CIPSTA_DEF=" -2 send "AT+CIPSTA_CUR=" -3 send "AT+CIPSTA=".
      @param ip - the ip of station.
      @param gateway -the gateway of station.
      @param netmask -the netmask of station.
      @retval true - success.
      @retval false - failure.
      @note This method should not be called when ap mode.
    */
    bool setStationIp(String ip, String gateway, String netmask, uint8_t pattern = DEFAULT_PATTERN);

    /**
      Get the AP's IP.
      @param pattern -1 send "AT+CIPAP_DEF?" -2 send "AT+CIPAP_CUR?" -3 send "AT+CIPAP?".
      @return ap's ip.
      @note This method should not be called when station mode.

    */
    String getAPIp(uint8_t pattern = DEFAULT_PATTERN);

    /**
      Set the AP IP.
      @param pattern -1 send "AT+CIPAP_DEF=" -2 send "AT+CIPAP_CUR=" -3 send "AT+CIPAP=".
      @param ip - the ip of AP.
      @retval true - success.
      @retval false - failure.
      @note This method should not be called when station mode.
    */
    bool setAPIp(String ip, uint8_t pattern = DEFAULT_PATTERN);

    /**
      start smartconfig.
      @param type -1:ESP_TOUCH  -2:AirKiss.
      @retval true - success.
      @retval false - failure.
    */
    bool startSmartConfig(uint8_t type);

    /**
      stop smartconfig.

      @retval true - success.
      @retval false - failure.
    */
    bool stopSmartConfig(void);

    /**
       Get the current status of connection(UDP and TCP).

       @return the status.
    */
    String getIPStatus(void);

    /**
       Get the IP address of ESP_AT.

       @return the IP list.
    */
    String getLocalIP(void);

    /**
       Enable IP MUX(multiple connection mode).

       In multiple connection mode, a couple of TCP and UDP communication can be builded.
       They can be distinguished by the identifier of TCP or UDP named mux_id.

       @retval true - success.
       @retval false - failure.
    */
    bool enableMUX(void);

    /**
       Disable IP MUX(single connection mode).

       In single connection mode, only one TCP or UDP communication can be builded.

       @retval true - success.
       @retval false - failure.
    */
    bool disableMUX(void);

    /**
       Create TCP connection in single mode.

       @param addr - the IP or domain name of the target host.
       @param port - the port number of the target host.
       @retval true - success.
       @retval false - failure.
    */
    bool createTCP(String addr, uint32_t port);

    /**
       Release TCP connection in single mode.

       @retval true - success.
       @retval false - failure.
    */
    bool releaseTCP(void);

    /**
       Register UDP port number in single mode.

       @param addr - the IP or domain name of the target host.
       @param port - the port number of the target host.
       @retval true - success.
       @retval false - failure.
    */
    bool registerUDP(String addr, uint32_t port);

    /**
       Unregister UDP port number in single mode.

       @retval true - success.
       @retval false - failure.
    */
    bool unregisterUDP(void);

    /**
       Create TCP connection in multiple mode.

       @param mux_id - the identifier of this TCP(available value: 0 - 4).
       @param addr - the IP or domain name of the target host.
       @param port - the port number of the target host.
       @retval true - success.
       @retval false - failure.
    */
    bool createTCP(uint8_t mux_id, String addr, uint32_t port);

    /**
       Release TCP connection in multiple mode.

       @param mux_id - the identifier of this TCP(available value: 0 - 4).
       @retval true - success.
       @retval false - failure.
    */
    bool releaseTCP(uint8_t mux_id);

    /**
       Register UDP port number in multiple mode.

       @param mux_id - the identifier of this TCP(available value: 0 - 4).
       @param addr - the IP or domain name of the target host.
       @param port - the port number of the target host.
       @retval true - success.
       @retval false - failure.
    */
    bool registerUDP(uint8_t mux_id, String addr, uint32_t port);

    /**
       Unregister UDP port number in multiple mode.

       @param mux_id - the identifier of this TCP(available value: 0 - 4).
       @retval true - success.
       @retval false - failure.
    */
    bool unregisterUDP(uint8_t mux_id);

    /**
       Set the timeout of TCP Server.

       @param timeout - the duration for timeout by second(0 ~ 28800, default:180).
       @retval true - success.
       @retval false - failure.
    */
    bool setTCPServerTimeout(uint32_t timeout = 180);

    /**
       Start TCP Server(Only in multiple mode).

       After started, user should call method: getIPStatus to know the status of TCP connections.
       The methods of receiving data can be called for user's any purpose. After communication,
       release the TCP connection is needed by calling method: releaseTCP with mux_id.

       @param port - the port number to listen(default: 333).
       @retval true - success.
       @retval false - failure.

       @see String getIPStatus(void);
       @see uint32_t recv(uint8_t *coming_mux_id, uint8_t *buffer, uint32_t len, uint32_t timeout);
       @see bool releaseTCP(uint8_t mux_id);
    */
    bool startTCPServer(uint32_t port = 333);

    /**
       Stop TCP Server(Only in multiple mode).

       @retval true - success.
       @retval false - failure.
    */
    bool stopTCPServer(void);

    /**
      Set the module transfer mode

       @retval true - success.
       @retval false - failure.
    */
    bool setCIPMODE(uint8_t mode);

    /**
       Start Server(Only in multiple mode).

       @param port - the port number to listen(default: 333).
       @retval true - success.
       @retval false - failure.

       @see String getIPStatus(void);
       @see uint32_t recv(uint8_t *coming_mux_id, uint8_t *buffer, uint32_t len, uint32_t timeout);
    */
    bool startServer(uint32_t port = 333);

    //KH Mod
    ///////////
    uint32_t recv(uint8_t *buffer, uint32_t buffer_size, uint32_t timeout);

    uint32_t recv(uint8_t mux_id, uint8_t *buffer, uint32_t buffer_size, uint32_t timeout);

    uint32_t recv(uint8_t *coming_mux_id, uint8_t *buffer, uint32_t buffer_size, uint32_t timeout);

    uint32_t recvPkg(uint8_t *buffer, uint32_t buffer_size, uint32_t *data_len, uint32_t timeout, uint8_t *coming_mux_id);

    //////////

    /**
       Stop Server(Only in multiple mode).

       @retval true - success.
       @retval false - failure.
    */
    bool stopServer(void);
    /**
       Save the passthrough links

       @retval true - success.
       @retval false - failure.
    */
    bool saveTransLink (uint8_t mode, String ip, uint32_t port);

    /**
       PING COMMAND.

       @retval true - success.
       @retval false - failure.
    */
    bool setPing(String ip);

    /**
       Send data based on TCP or UDP builded already in single mode.

       @param buffer - the buffer of data to send.
       @param len - the length of data to send.
       @retval true - success.
       @retval false - failure.
    */
    bool send(const uint8_t *buffer, uint32_t len);

    /**
       Send data based on one of TCP or UDP builded already in multiple mode.

       @param mux_id - the identifier of this TCP(available value: 0 - 4).
       @param buffer - the buffer of data to send.
       @param len - the length of data to send.
       @retval true - success.
       @retval false - failure.
    */
    bool send(uint8_t mux_id, const uint8_t *buffer, uint32_t len);

    /**
       Send data based on TCP or UDP builded already in single mode.

       @param buffer - the buffer of data to send from flash memeory.
       @param len - the length of data to send.
       @retval true - success.
       @retval false - failure.
    */
    bool sendFromFlash(const uint8_t *buffer, uint32_t len);

    /**
       Send data based on one of TCP or UDP builded already in multiple mode.

       @param mux_id - the identifier of this TCP(available value: 0 - 4).
       @param buffer - the buffer of data to send from flash memeory.
       @param len - the length of data to send.
       @retval true - success.
       @retval false - failure.
    */
    bool sendFromFlash(uint8_t mux_id, const uint8_t *buffer, uint32_t len);

  private:

    /*
       Empty the buffer or UART RX.
    */
    void rx_empty(void);

    /*
       Receive data from uart. Return all received data if target found or timeout.
    */
    String recvString(String target, uint32_t timeout = 1000);

    /*
       Receive data from uart. Return all received data if one of target1 and target2 found or timeout.
    */
    String recvString(String target1, String target2, uint32_t timeout = 1000);

    /*
       Receive data from uart. Return all received data if one of target1, target2 and target3 found or timeout.
    */
    String recvString(String target1, String target2, String target3, uint32_t timeout = 1000);

    /*
       Receive data from uart and search first target. Return true if target found, false for timeout.
    */
    bool recvFind(String target, uint32_t timeout = 1000);

    /*
       Receive data from uart and search first target and cut out the substring between begin and end(excluding begin and end self).
       Return true if target found, false for timeout.
    */
    bool recvFindAndFilter(String target, String begin, String end, String &data, uint32_t timeout = 1000);

    /*
       Receive a package from uart.

       @param buffer - the buffer storing data.
       @param buffer_size - guess what!
       @param data_len - the length of data actually received(maybe more than buffer_size, the remained data will be abandoned).
       @param timeout - the duration waitting data comming.
       @param coming_mux_id - in single connection mode, should be NULL and not NULL in multiple.
    */
    uint32_t checkIPD(String& data);


    bool eAT(void);
    bool eATRST(void);
    bool eATGMR(String &version);
    bool eATGSLP(uint32_t time);
    bool eATE(uint8_t mode);
    bool eATRESTORE(void);
    bool eATSETUART(uint32_t baudrate, uint8_t pattern);

    bool qATCWMODE(uint8_t *mode, uint8_t pattern = 3);
    bool eATCWMODE(String &list) ;
    bool sATCWMODE(uint8_t mode, uint8_t pattern = 3);
    bool qATCWJAP(String &ssid, uint8_t pattern = 3) ;
    bool sATCWJAP(String ssid, String pwd, uint8_t pattern = 3);
    bool eATCWLAP(String &list);
    bool eATCWQAP(void);
    bool qATCWSAP(String &List, uint8_t pattern = 3);
    bool sATCWSAP(String ssid, String pwd, uint8_t chl, uint8_t ecn, uint8_t pattern = 3);
    bool eATCWLIF(String &list);
    bool qATCWDHCP(String &List, uint8_t pattern = 3);
    bool sATCWDHCP(uint8_t mode, uint8_t en, uint8_t pattern = 3);
    bool eATCWAUTOCONN(uint8_t en);
    bool qATCIPSTAMAC(String &mac, uint8_t pattern = 3);
    bool eATCIPSTAMAC(String mac, uint8_t pattern = 3);
    bool qATCIPSTAIP(String &ip, uint8_t pattern = 3);
    bool eATCIPSTAIP(String ip, String gateway, String netmask, uint8_t pattern = 3);
    bool qATCIPAP(String &ip, uint8_t pattern = 3);
    bool eATCIPAP(String ip, uint8_t pattern = 3);
    bool eCWSTARTSMART(uint8_t type);
    bool eCWSTOPSMART(void);


    bool eATCIPSTATUS(String &list);
    bool sATCIPSTARTSingle(String type, String addr, uint32_t port);
    bool sATCIPSTARTMultiple(uint8_t mux_id, String type, String addr, uint32_t port);
    bool sATCIPSENDSingle(const uint8_t *buffer, uint32_t len);
    bool sATCIPSENDMultiple(uint8_t mux_id, const uint8_t *buffer, uint32_t len);
    bool sATCIPSENDSingleFromFlash(const uint8_t *buffer, uint32_t len);
    bool sATCIPSENDMultipleFromFlash(uint8_t mux_id, const uint8_t *buffer, uint32_t len);
    bool sATCIPCLOSEMultiple(uint8_t mux_id);
    bool eATCIPCLOSESingle(void);
    bool eATCIFSR(String &list);
    bool sATCIPMUX(uint8_t mode);
    bool sATCIPSERVER(uint8_t mode, uint32_t port = 333);
    bool sATCIPMODE(uint8_t mode);
    bool eATSAVETRANSLINK(uint8_t mode, String ip, uint32_t port);
    bool eATPING(String ip);
    bool sATCIPSTO(uint32_t timeout);

    /*
       +IPD,len:data
       +IPD,id,len:data
    */

    Stream *m_puart; /* The UART to communicate with ESP_AT */
    onData m_onData;
    void*  m_onDataPtr;
};

#include "ESP_AT_Lib_Impl.h"

#endif /* #ifndef __ESP_AT_LIB_H__ */

