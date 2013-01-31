#include <dht.h>

//dht11 i/o library
dht DHT;

/**
 * Return the current humidity
 * @return the humidity or -1000 if there was an error
 */
float getHumidity() {
  if (checkDHT())
    return (float)DHT.humidity;
  else
    return -1000;
}

/**
 * Contact the DHT and read the latest value
 * When true is returned, the the values can be read from
 * the DHT11 variable.
 * @return true on success
 */
boolean checkDHT() {
  int chk = DHT.read22(DHT_Pin);

#if DSERIAL
  if (chk != DHTLIB_OK) {
    switch (chk)
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
  return chk == DHTLIB_OK;

}
