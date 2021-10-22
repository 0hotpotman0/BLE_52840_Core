#include <PDM.h>
#include <SPI.h>
#include <SD.h>

// default number of output channels
static const char channels = 1;

// default PCM output frequency
static const int frequency = 16000;
#define SAMPLES (frequency*5) 
// Buffer to read samples into, each sample is 16-bits
short sampleBuffer[512];
short sampleBuffer1[SAMPLES] = {0};

// Number of audio samples read
volatile int samplesRead = 0;

void finalize_template(File &sFile);
void record();
void create_template(File &sFile);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  // Configure the data receive callback
  PDM.onReceive(onPDMdata);

  // Optionally set the gain
  // Defaults to 20 on the BLE Sense and 24 on the Portenta Vision Shield
  // PDM.setGain(30);

  if (!SD.begin(7)) {
    Serial.println("initialization failed!");
    while (1)yield();
  }
  else{
    Serial.println("SD initialization success!");    
  }
  // Initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate for the Arduino Nano 33 BLE Sense
  // - a 32 kHz or 64 kHz sample rate for the Arduino Portenta Vision Shield
  if (!PDM.begin(channels, frequency)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }
}
int sample_cnt = 0;
void loop() {
  // Wait for samples to be read
  if (samplesRead) {
    // Print samples to the serial monitor or plotter
    for (int i = 0; i < samplesRead; i++) {
      sampleBuffer1[sample_cnt++] = sampleBuffer[i];
      if(sample_cnt >= SAMPLES)
      {
          Serial.println("End of PDM");
          record(); 
          PDM.end();
          samplesRead = 0;
          return;
      }
    }
    
    // Clear the read count
    samplesRead = 0;
  }  
}
File myFile;
void record(){
  Serial.println("Finished sampling");
  static char print_buf[128] = {0};
  int r = sprintf(print_buf, "test%d.wav", millis());
  File sFile = SD.open(print_buf, FILE_WRITE);
  if (sFile) {
    Serial.print("Writing to "); // txt
    Serial.println(print_buf);
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening ");
    Serial.println(print_buf);
  }
  create_template(sFile);
  
  for (int i = 0; i < SAMPLES; i++) {
    
  //int16_t sample = filter.step(recording_buf[i]);
  short sample = sampleBuffer1[i];
  sFile.write(sample & 0xFF);
  sFile.write((sample >> 8) & 0xFF);

  //Serial.println(sample);
  }
  
  Serial.println("Finished writing");
  finalize_template(sFile);
}

void create_template(File &sFile)
{
  
struct soundhdr {
  char  riff[4];        /* "RIFF"                                  */
  long  flength;        /* file length in bytes                    */
  char  wave[4];        /* "WAVE"                                  */
  char  fmt[4];         /* "fmt "                                  */
  long  chunk_size;     /* size of FMT chunk in bytes (usually 16) */
  short format_tag;     /* 1=PCM, 257=Mu-Law, 258=A-Law, 259=ADPCM */
  short num_chans;      /* 1=mono, 2=stereo                        */
  long  srate;          /* Sampling rate in samples per second     */
  long  bytes_per_sec;  /* bytes per second = srate*bytes_per_samp */
  short bytes_per_samp; /* 2=16-bit mono, 4=16-bit stereo          */
  short bits_per_samp;  /* Number of bits per sample               */
  char  data[4];        /* "data"                                  */
  long  dlength;        /* data length in bytes (filelength - 44)  */
} wavh;

  // It's easy enough to initialize the strings
  strncpy(wavh.riff,"RIFF", 4);
  strncpy(wavh.wave,"WAVE", 4);
  strncpy(wavh.fmt,"fmt ", 4);
  strncpy(wavh.data,"data", 4);
  
  // size of FMT chunk in bytes
  wavh.chunk_size = 16;
  wavh.format_tag = 1; // PCM
  wavh.num_chans = 1; // mono
  wavh.srate = 16000;
  wavh.bytes_per_sec = (16000 * 1 * 16 * 1)/8;
  wavh.bytes_per_samp = 2;
  wavh.bits_per_samp = 16;
  wavh.dlength = 16000 * 2 *  1 * 16/2;
  
  sFile.seek(0);
  sFile.write((byte *)&wavh, 44);
}

void finalize_template(File &sFile)
{
  unsigned long fSize = sFile.size()-8;
  sFile.seek(4); 
  byte data[4] = {lowByte(fSize), highByte(fSize), fSize >> 16, fSize >> 24};
  sFile.write(data,4);
  byte tmp;
  sFile.seek(40);
  fSize = fSize - 36;
  data[0] = lowByte(fSize);
  data[1]= highByte(fSize);
  data[2]= fSize >> 16;
  data[3]= fSize >> 24;
  //sFile.write((byte*)data, 4);
  sFile.close();
}
/**
 * Callback function to process the data from the PDM microphone.
 * NOTE: This callback is executed as part of an ISR.
 * Therefore using `Serial` to print messages inside this function isn't supported.
 * */
void onPDMdata() {
  // Query the number of available bytes
  int bytesAvailable = PDM.available();

  // Read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);

  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}
