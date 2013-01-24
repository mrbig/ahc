/**
 * This file contains the codes of the zabbix agent
 */

/**
 * Handle incoming requests for the zabbix agent
 * Parse the requested item, and call the appropriate handler
 *
 * @param word pos position in the tcp packet
 */
static void serviceZabbixRequest(word pos) {
#if DSERIAL
  Serial.println("got packet on 10050");
  char * data = (char *) Ethernet::buffer + pos;
  Serial.println(data);
#endif
  byte i;
  char header[9];
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR("ZBXD"));
  for (i=0; i<9; i++) {
    switch (i) {
      case 0:
        header[i] = 0x01; break;
      case 1:
        header[i] = 5; break;
      default:
        header[i] = 0;
    }
  }
  bfill.emit_raw(header, 9);
  bfill.emit_p(PSTR("demo1"));
  ether.httpServerReply(bfill.position());
}
