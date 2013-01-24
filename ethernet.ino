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

