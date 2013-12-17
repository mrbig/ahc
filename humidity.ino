/* Copyright (C) 2009 Nagy Attila Gabor <nagy.attila.gabor@gmail.com>
 *
 *     This file is part of AHC - AHC Humidity Control.
 *
 *  AHC is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  AHC is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with AHC.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <dht.h>

//dht2x i/o library
dht DHT;

static float aHumidity[CHECK_COUNT];
static byte avgPos = 0;

// last status of the dht check
static int dhtStatus = -1;

/**
 * Initialize the humidity component
 * We set every item in the average array to -1000
 */
void initHumidity() {
  for (byte i =0; i<CHECK_COUNT; i++) {
    aHumidity[i] = -1000;
  }
}

/**
 * Return the current humidity. We calculate the average
 * of the last CHECK_COUNT readings
 * @return the humidity or -1000 if there was an error
 */
float getHumidity() {
  float sum = 0;
  byte count, i;
  count = 0;
#if DSERIAL
  Serial.println(F("Calculating average:"));
#endif
  for (i=0; i<CHECK_COUNT; i++) {
#if DSERIAL
    Serial.println(aHumidity[i], 2);
#endif
    if (aHumidity[i] != -1000) {
      sum += aHumidity[i];
      count++;
    }
  }
  if (count > 0) {
#if DSERIAL
    Serial.print(F("Average: "));
    Serial.println(sum/count, 2);
#endif
    return sum/count;
  } else {
#if DSERIAL
    Serial.print(F("Average: -1000"));
#endif
    return -1000;
  }
}

/**
 * Return the state of the last humidity download
 */
int getHumidityStatus() {
  return dhtStatus;
}


/**
 * Contact the DHT and read the latest value
 * When true is returned, the the values can be read from
 * the DHT11 variable.
 * @return true on success
 */
boolean checkDHT() {
  dhtStatus = DHT.read22(DHT_Pin);

#if DSERIAL
  if (dhtStatus != DHTLIB_OK) {
    switch (dhtStatus)
    {
      case DHTLIB_ERROR_CHECKSUM:
                  Serial.println(F("Checksum error"));
                  break;
      case DHTLIB_ERROR_TIMEOUT:
                  Serial.println(F("Time out error"));
                  break;
      default:
                  Serial.println(F("Unknown error"));
                  break;
    }
  } else {
    Serial.print(F("Humidity (%): "));
    Serial.println((float)DHT.humidity, 2);
  
    Serial.print(F("Temperature (oC): "));
    Serial.println((float)DHT.temperature, 2);
  }
#endif
  if (dhtStatus == DHTLIB_OK) {
    aHumidity[avgPos] = (float)DHT.humidity;
  } else {
    aHumidity[avgPos] = -1000;
  }
  avgPos++;
  avgPos = avgPos % CHECK_COUNT;

  return dhtStatus == DHTLIB_OK;

}
