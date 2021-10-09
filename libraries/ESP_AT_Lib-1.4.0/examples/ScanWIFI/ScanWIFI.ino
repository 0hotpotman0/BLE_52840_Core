/****************************************************************************************************************************
  ScanWiFi.ino
  For ESP8266/ESP32-AT-command running shields
  
  ESP_AT_Lib is a wrapper library for the ESP8266/ESP32 AT-command shields
  
  Based on and modified from ESP8266 https://github.com/esp8266/Arduino/releases
  Built by Khoi Hoang https://github.com/khoih-prog/ESP_AT_Lib
  Licensed under MIT license
  
  Data Format:
  +CWLAP:<ecn>,<ssid>,<rssi>,<mac>,<channel>,<freq offset>,<freq cali>,<pairwise_cipher>,<group_cipher>,<bgn>,<wps>
  <ecn>：加密方式
  	0：OPEN 
  	1：WEP 
  	2：WPA_PSK 
  	3：WPA2_PSK 
  	4：WPA_WPA2_PSK 
  	5：WPA2_Enterprise（目前AT不支持连接这种加密AP） 
  <ssid>：字符串参数，AP的SSID 
  <rssi>：信号强度
  <mac>：字符串参数，AP的MAC地址
  <channel>：信道号
  <freq	offset>：AP 频偏，单位：kHz。此数值除以 2.4，可得到 ppm 值 
  <freq	cali>：频偏校准值
  <pairwise_cipher>: 
  	0：CIPHER_NONE 
  	1：CIPHER_WEP40 
  	2：CIPHER_WEP104 
  	3：CIPHER_TKIP 
  	4：CIPHER_CCMP 
  	5：CIPHER_TKIP_CCMP 
  	6：CIPHER_UNKNOWN 
  <group_cipher>: 定义与 <pairwise_cipher> 相同
  <bgn>: 
  	bit0 代表 b 模式; bit1 代表 g 模式; bit2 代表 n 模式
  	若对应 bit 为 1，表示该模式使能；若对应 bit 为 0，则该模式未使能。
  <wps>：0，WPS 未使能；1，WPS 使能  
  
  @example ScanWiFi.ino
  @brief The ScanWiFi demo of library WeeESP8266.
 *****************************************************************************************************************************/

#include "ESP_AT_Lib.h"

// Your board <-> ESP_AT baud rate:
#define ESP_AT_BAUD       115200

ESP8266 wifi(&EspSerial);

void setup(void)
{
  Serial.begin(115200);
  while (!Serial);

  delay(1000);

#if defined(BOARD_NAME)
  Serial.println("\nStart ConnectWiFi on " + String(BOARD_NAME));
#else
  Serial.println("\nStart ConnectWiFi");
#endif

  Serial.println(ESP_AT_LIB_VERSION);

  // initialize serial for ESP module
  EspSerial.begin(ESP_AT_BAUD);

  Serial.print("FW Version: ");
  Serial.println(wifi.getVersion().c_str());


  if (wifi.setOprToStation()) 
  {
    Serial.println("Set STA Mode OK");
  } 
  else 
  {
    Serial.println("Set STA Mode failed");
  }
  wifi.leaveAP();
  delay(100);

  Serial.println("Setup done");
}

void loop(void)
{
    Serial.println("scan start");
 
    // WiFi.scanNetworks will return the number of networks found
    Serial.println(wifi.getAPList());
    Serial.println("scan done");
 
    // Wait a bit before scanning again
    delay(5000);
}
