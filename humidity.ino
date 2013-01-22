#include <dht11.h>

//dht11 i/o library
dht11 DHT11;

/**
 * Contact the DHT11 and display the humidity and temperature read from it
 */
void checkDHT11() {
  int chk = DHT11.read(DHT11_Pin);
  if (chk != DHTLIB_OK) {
    switch (chk)
    {
      case DHTLIB_ERROR_CHECKSUM:
                  Serial.println("Checksum error");
                  break;
      case DHTLIB_ERROR_TIMEOUT:
                  Serial.println("Time out error");
                  break;
      default:
                  Serial.println("Unknown error");
                  break;
    }
    return;
  }

  Serial.print("Humidity (%): ");
  Serial.println((float)DHT11.humidity, 2);

  Serial.print("Temperature (oC): ");
  Serial.println((float)DHT11.temperature, 2);
}
