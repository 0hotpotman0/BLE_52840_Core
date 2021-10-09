/*
  Non-blocking Write

  This example demonstrates how to perform non-blocking writes
  to a file on a SD card. The file will contain the current millis()
  value every 10ms. If the SD card is busy, the data will be buffered
  in order to not block the sketch.

  NOTE: myFile.availableForWrite() will automatically sync the
        file contents as needed. You may lose some unsynced data
        still if myFile.sync() or myFile.close() is not called.

  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK  - pin 10
 ** CS   - pin 9
 ** SD POWER - pin 14

  This example code is in the public domain.
*/

#include <SD.h>

// file name to use for writing
const char filename[] = "demo.txt";

// File object to represent file
File txtFile;

// string to buffer output
String buffer;

unsigned long lastMillis = 0;

void setup() {
  // SD module power supply
  pinMode(PIN_SD_POWER, OUTPUT);//power
  digitalWrite(PIN_SD_POWER, HIGH);
  Serial.begin(9600);
  while (!Serial);

  // reserve 1kB for String used as a buffer
  buffer.reserve(1024);

  // init the SD card
  if (!SD.begin(PIN_SD_CS)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1) yield();
  }
  Serial.println("initialization done.");

  // If you want to start from an empty file,
  // uncomment the next line:
  // SD.remove(filename);

  // try to open the file for writing
  txtFile = SD.open(filename, FILE_WRITE);
  if (!txtFile) {
    Serial.print("error opening ");
    Serial.println(filename);
    while (1) yield();
  }

  // add some new lines to start
  txtFile.println();
  txtFile.println("Hello World!");
}

void loop() {
  // check if it's been over 10 ms since the last line added
  unsigned long now = millis();
  if ((now - lastMillis) >= 10) {
    // add a new line to the buffer
    buffer += "Hello ";
    buffer += now;
    buffer += "\r\n";

    lastMillis = now;
  }

  // check if the SD card is available to write data without blocking
  // and if the buffered data is enough for the full chunk size
  unsigned int chunkSize = txtFile.availableForWrite();
  if (chunkSize && buffer.length() >= chunkSize) {
    // write to file 
    if(txtFile.write(buffer.c_str(), chunkSize))
    {
      Serial.println("write success");
    }
    else{
      Serial.println("write failed");      
    }
    // remove written data from buffer
    buffer.remove(0, chunkSize);
  }
}
