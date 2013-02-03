#include <dht.h>

//dht2x i/o library
dht DHT;

// last status of the dht check
static int dhtStatus = -1;

/**
 * Return the current humidity
 * @return the humidity or -1000 if there was an error
 */
float getHumidity() {
  if (dhtStatus == DHTLIB_OK)
    return (float)DHT.humidity;
  else
    return -1000;
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
  return dhtStatus == DHTLIB_OK;

}
