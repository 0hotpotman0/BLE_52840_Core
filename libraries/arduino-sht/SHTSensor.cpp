/*
 *  Copyright (c) 2018, Sensirion AG <andreas.brauchli@sensirion.com>
 *  Copyright (c) 2015-2016, Johannes Winkelmann <jw@smts.ch>
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *      * Neither the name of the Sensirion AG nor the names of its
 *        contributors may be used to endorse or promote products derived
 *        from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <inttypes.h>
#include <Wire.h>
#include <Arduino.h>

#include "SHTSensor.h"


//
// class SHTSensorDriver
//

SHTSensorDriver::~SHTSensorDriver()
{
}

bool SHTSensorDriver::readSample()
{
  return false;
}


//
// class SHTI2cSensor
//

const uint8_t SHTI2cSensor::EXPECTED_DATA_SIZE   = 6;

bool SHTI2cSensor::readFromI2c(uint8_t i2cAddress,
                               const uint8_t *i2cCommand,
                               uint8_t commandLength, uint8_t *data,
                               uint8_t dataLength,
                               uint8_t duration)
{
  Wire.beginTransmission(i2cAddress);
  for (int i = 0; i < commandLength; ++i) {
    if (Wire.write(i2cCommand[i]) != 1) {
      return false;
    }
  }

  if (Wire.endTransmission() != 0) {
    return false;
  }

  delay(duration);

  Wire.requestFrom(i2cAddress, dataLength);

  // check if the same number of bytes are received that are requested.
  if (Wire.available() != dataLength) {
    return false;
  }

  for (int i = 0; i < dataLength; ++i) {
    data[i] = Wire.read();
  }
  return true;
}

uint8_t SHTI2cSensor::crc8(const uint8_t *data, uint8_t len)
{
  // adapted from SHT21 sample code from
  // http://www.sensirion.com/en/products/humidity-temperature/download-center/

  uint8_t crc = 0xff;
  uint8_t byteCtr;
  for (byteCtr = 0; byteCtr < len; ++byteCtr) {
    crc ^= data[byteCtr];
    for (uint8_t bit = 8; bit > 0; --bit) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x31;
      } else {
        crc = (crc << 1);
      }
    }
  }
  return crc;
}


bool SHTI2cSensor::readSample()
{
  uint8_t data[EXPECTED_DATA_SIZE];
  uint8_t cmd[mCmd_Size];

  cmd[0] = mI2cCommand >> 8;
  //is omitted for SHT4x Sensors
  cmd[1] = mI2cCommand & 0xff;

  if (!readFromI2c(mI2cAddress, cmd, mCmd_Size, data,
                   EXPECTED_DATA_SIZE, mDuration)) {
    return false;
  }

  // -- Important: assuming each 2 byte of data is followed by 1 byte of CRC

  // check CRC for both RH and T
  if (crc8(&data[0], 2) != data[2] || crc8(&data[3], 2) != data[5]) {
    return false;
  }

  // convert to Temperature/Humidity
  uint16_t val;
  val = (data[0] << 8) + data[1];
  mTemperature = mA + mB * (val / mC);

  val = (data[3] << 8) + data[4];
  mHumidity = mX + mY * (val / mZ);

  return true; 
  
}

//
// class SHTC1Sensor
//

class SHTC1Sensor : public SHTI2cSensor
{
public:
    SHTC1Sensor()
        // clock stretching disabled, high precision, T first
        : SHTI2cSensor(0x70, 0x7866, 15, -45, 175, 65535, 0, 100, 65535, 2)
    {
    }
};


//
// class SHT3xSensor
//

class SHT3xSensor : public SHTI2cSensor
{
private:
  static const uint16_t SHT3X_ACCURACY_HIGH    = 0x2400;
  static const uint16_t SHT3X_ACCURACY_MEDIUM  = 0x240b;
  static const uint16_t SHT3X_ACCURACY_LOW     = 0x2416;

  static const uint8_t SHT3X_ACCURACY_HIGH_DURATION   = 15;
  static const uint8_t SHT3X_ACCURACY_MEDIUM_DURATION = 6;
  static const uint8_t SHT3X_ACCURACY_LOW_DURATION    = 4;

public:
  static const uint8_t SHT3X_I2C_ADDRESS_44 = 0x44;
  static const uint8_t SHT3X_I2C_ADDRESS_45 = 0x45;

  SHT3xSensor(uint8_t i2cAddress = SHT3X_I2C_ADDRESS_44)
      : SHTI2cSensor(i2cAddress, SHT3X_ACCURACY_HIGH,
                     SHT3X_ACCURACY_HIGH_DURATION,
                     -45, 175, 65535, 0, 100, 65535, 2)
  {
  }

  virtual bool setAccuracy(SHTSensor::SHTAccuracy newAccuracy)
  {
    switch (newAccuracy) {
      case SHTSensor::SHT_ACCURACY_HIGH:
        mI2cCommand = SHT3X_ACCURACY_HIGH;
        mDuration = SHT3X_ACCURACY_HIGH_DURATION;
        break;
      case SHTSensor::SHT_ACCURACY_MEDIUM:
        mI2cCommand = SHT3X_ACCURACY_MEDIUM;
        mDuration = SHT3X_ACCURACY_MEDIUM_DURATION;
        break;
      case SHTSensor::SHT_ACCURACY_LOW:
        mI2cCommand = SHT3X_ACCURACY_LOW;
        mDuration = SHT3X_ACCURACY_LOW_DURATION;
        break;
      default:
        return false;
    }
    return true;
  }
};


//
// class SHT4xSensor
//

class SHT4xSensor : public SHTI2cSensor
{
private:
  static const uint16_t SHT4X_ACCURACY_HIGH    = 0xFD00;
  static const uint16_t SHT4X_ACCURACY_MEDIUM  = 0xF600;
  static const uint16_t SHT4X_ACCURACY_LOW     = 0xE000;

  static const uint8_t SHT4X_ACCURACY_HIGH_DURATION   = 10;
  static const uint8_t SHT4X_ACCURACY_MEDIUM_DURATION = 4;
  static const uint8_t SHT4X_ACCURACY_LOW_DURATION    = 2;

public:
  static const uint8_t SHT4X_I2C_ADDRESS_44 = 0x44;
  static const uint8_t SHT4X_I2C_ADDRESS_45 = 0x45;

  SHT4xSensor(uint8_t i2cAddress = SHT4X_I2C_ADDRESS_44)
      : SHTI2cSensor(i2cAddress, SHT4X_ACCURACY_HIGH,
                     SHT4X_ACCURACY_HIGH_DURATION,
                     -45, 175, 65535, -6, 125, 65535, 1)
  {
  }

  virtual bool setAccuracy(SHTSensor::SHTAccuracy newAccuracy)
  {
    switch (newAccuracy) {
      case SHTSensor::SHT_ACCURACY_HIGH:
        mI2cCommand = SHT4X_ACCURACY_HIGH;
        mDuration = SHT4X_ACCURACY_HIGH_DURATION;
        break;
      case SHTSensor::SHT_ACCURACY_MEDIUM:
        mI2cCommand = SHT4X_ACCURACY_MEDIUM;
        mDuration = SHT4X_ACCURACY_MEDIUM_DURATION;
        break;
      case SHTSensor::SHT_ACCURACY_LOW:
        mI2cCommand = SHT4X_ACCURACY_LOW;
        mDuration = SHT4X_ACCURACY_LOW_DURATION;
        break;
      default:
        return false;
    }
    return true;
  }
};


//
// class SHT3xAnalogSensor
//

float SHT3xAnalogSensor::readHumidity()
{
  float max_adc = (float)((1 << mReadResolutionBits) - 1);
  return -12.5f + 125 * (analogRead(mHumidityAdcPin) / max_adc);
}

float SHT3xAnalogSensor::readTemperature()
{
  float max_adc = (float)((1 << mReadResolutionBits) - 1);
  return -66.875f + 218.75f * (analogRead(mTemperatureAdcPin) / max_adc);
}


//
// class SHTSensor
//

const SHTSensor::SHTSensorType SHTSensor::AUTO_DETECT_SENSORS[] = {
  SHT3X,
  SHT3X_ALT,
  SHTC1,
  SHT4X
};
const float SHTSensor::TEMPERATURE_INVALID = NAN;
const float SHTSensor::HUMIDITY_INVALID = NAN;

bool SHTSensor::init()
{
  if (mSensor != NULL) {
    cleanup();
  }

  switch(mSensorType) {
    case SHT3X:
      mSensor = new SHT3xSensor();
      break;

    case SHT3X_ALT:
      mSensor = new SHT3xSensor(SHT3xSensor::SHT3X_I2C_ADDRESS_45);
      break;

    case SHTW1:
    case SHTW2:
    case SHTC1:
    case SHTC3:
      mSensor = new SHTC1Sensor();
      break;
    case SHT4X:
      mSensor = new SHT4xSensor();
      break;
    case AUTO_DETECT:
    {
      bool detected = false;
      for (unsigned int i = 0;
           i < sizeof(AUTO_DETECT_SENSORS) / sizeof(AUTO_DETECT_SENSORS[0]);
           ++i) {
        mSensorType = AUTO_DETECT_SENSORS[i];
        delay(40); // TODO: this was necessary to make SHT4x autodetect work; revisit to find root cause
        if (init()) {
          detected = true;
          break;
        }
      }
      if (!detected) {
        cleanup();
      }
      break;
    }
  }

  // to finish the initialization, attempt to read to make sure the communication works
  // Note: readSample() will check for a NULL mSensor in case auto detect failed
  return readSample();
}

bool SHTSensor::readSample()
{
  if (!mSensor || !mSensor->readSample())
    return false;
  mTemperature = mSensor->mTemperature;
  mHumidity = mSensor->mHumidity;
  return true;
}

bool SHTSensor::setAccuracy(SHTAccuracy newAccuracy)
{
  if (!mSensor)
    return false;
  return mSensor->setAccuracy(newAccuracy);
}

void SHTSensor::cleanup()
{
  if (mSensor) {
    delete mSensor;
    mSensor = NULL;
  }
}
