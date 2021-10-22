
#include <LoRaWan.h>


unsigned char buffer[128] = {0, };


void setup(void)
{
    Serial.begin(115200);
    
    lora.init();
    
    lora.initP2PMode(433, SF12, BW125, 8, 8, 20);
}

void loop(void)
{
    short length = 0;
    short rssi = 0;
    
    memset(buffer, 0, 128);
    length = lora.receivePacketP2PMode(buffer, 128,  &rssi, 1);
    
    if(length)
    {
        Serial.print("Length is: ");
        Serial.println(length);
        Serial.print("RSSI is: ");
        Serial.println(rssi);
        Serial.print("Data is: ");
        for(unsigned char i = 0; i < length; i ++)
        {
            Serial.print("0x");
            Serial.print(buffer[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }
}
