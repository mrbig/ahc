#include <OneWire.h>

//Temperature chip i/o
OneWire ds(DS18S20_Pin);



/**
 * Read the temperature from the first DS18S20 on the bus
 * @return float the temperature in DEG Celsius
 */
float getTemp(){

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
    //no more sensors on chain, reset search
    ds.reset_search();
    return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
#if DSERIAL
    Serial.println("CRC is not valid!");
#endif
    return -1000;
  }
  
#if DSERIAL
  Serial.print("Found device: ");
  for (byte i=0; i<8; i++) {
    if (addr[i]<16) Serial.print("0");
    Serial.print(addr[i], HEX);
  }
  Serial.println("");
#endif

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
#if DSERIAL
    Serial.print("Device is not recognized");
#endif
    return -1000;
  }
  
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end

  ds.reset();
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

