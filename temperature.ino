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
 *
 * This file reads the temperature from the DS18B20
 */
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
    Serial.println(F("CRC is not valid!"));
#endif
    return -1000;
  }
  
#if DSERIAL
  Serial.print(F("Found device: "));
  for (byte i=0; i<8; i++) {
    if (addr[i]<16) Serial.print(F("0"));
    Serial.print(addr[i], HEX);
  }
  Serial.println("");
#endif

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
#if DSERIAL
    Serial.print(F("Device is not recognized"));
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

