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
/**
 * This is the ethernet handler
 */

// used as cursor while filling the buffer
static BufferFiller bfill;

/**
 * Check the ethernet for incoming data
 * Handle if anything is received.
 * This should be called from each loop in the
 * main loop
 */
void checkEthernet() {
  word plen = ether.packetReceive();
  word pos;
  
  // check if valid tcp data is received
  if (plen) {
    // do basic checks, and handle webserver
    if ((pos = ether.packetLoop(plen))) {
      serviceHttpRequest(bfill, pos);
    }
    // check the zabbix port
    else if ((pos = ether.accept(10050, plen))) {
      serviceZabbixRequest(bfill, pos);
    }
  }
}

