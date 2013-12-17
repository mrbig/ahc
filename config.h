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
#define DSERIAL 0 // set to 1 to show incoming request on serial port

// This is the targe humidity intervall
#define DEFAULT_HUMIDITY_MIN 28
#define DEFAULT_HUMIDITY_MAX 30

// How often the humidity is checked (in milliseconds)
#define CHECK_INTERVAL 5000

// Count of checks to calculate the average humidity
#define CHECK_COUNT 10

/**
 * ethernet configuration
 */
// ethernet interface ip address
static byte myip[] = { 192,168,1,200 };
// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };

// This id is used to identify our data in the eeprom
#define EEPROM_ID 0x42

/**
 * Pinout configuration
 */
// Pin of the ethernet card
byte Ether_Pin = 10;

// DS18S20 Signal pin on digital 2
byte DS18S20_Pin = 2;
 // Pin for the DHT21
byte DHT_Pin = 3;
 // Pin of the controlled device
byte Control_Pin = 7;

// Pin for the manual off switch
byte Switch_Pin = 4;

// How long to wait for the switch to debounce, before
// accepting it's state.
#define DEBOUNCE_DELAY 50

