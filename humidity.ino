#include <dht11.h>

//dht11 i/o library
dht11 DHT11;

/**
 * Return the current humidity
 * @return the humidity or -1000 if there was an error
 */
float getHumidity() {
  if (checkDHT11())
    return (float)DHT11.humidity;
  else
    return -1000;
}

/**
 * Contact the DHT11 and read the latest value
 * When true is returned, the the values can be read from
 * the DHT11 variable.
 * @return true on success
 */
boolean checkDHT11() {
  int chk = DHT11.read(DHT11_Pin);

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
    Serial.println((float)DHT11.humidity, 2);
  
    Serial.print(F("Temperature (oC): "));
    Serial.println((float)DHT11.temperature, 2);
  }
#endif
  return chk == DHTLIB_OK;

}
