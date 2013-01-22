#include <OneWire.h>
#include <dht11.h>


int Ether_Ping = 10; // Pin of the ethernet card
int DS18S20_Pin = 2; //DS18S20 Signal pin on digital 2
int DHT11_Pin = 3; // Pin for the DHT11
int Control_Pin = 5; // Pin of the controlled device

//Temperature chip i/o
OneWire ds(DS18S20_Pin); // on digital pin 2

//dht11 i/o
dht11 DHT11;
byte IOSTate;

void setup(void) {
  Serial.begin(9600);
  pinMode(Control_Pin, OUTPUT);
}

void loop(void) {
  checkOneWire();
  checkDHT11();
  digitalWrite(Control_Pin, IOState);
  IOState = !IOState;
  delay(2000); //just here to slow down the output so it is easier to read

}

/**
 * Check the onewire bus for DS18S20, and display the celsius
 */
void checkOneWire() {
  float temperature = getTemp();
  Serial.println(temperature);

}

float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
    //no more sensors on chain, reset search
    ds.reset_search();
    return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    return -1000;
  }
  
  Serial.print("Found device: ");
  for (byte i=0; i<8; i++) {
    if (addr[i]<16) Serial.print("0");
    Serial.print(addr[i], HEX);
  }
  Serial.println("");

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.print("Device is not recognized");
    return -1000;
  }
  
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Read Scratchpad


  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;

}

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

