/**
 * This file contains the codes of the zabbix agent
 */

// This is the type definition for the callback functions
typedef void(* zabbix_cb)(BufferFiller &buf, String &cmd);

// These are the valid commands handled by the system
static const char cmd1[] PROGMEM = "agent.ping";
static const char cmd2[] PROGMEM = "agent.version";

// mapping of the command strings
static const char* zabbix_commands[] = {cmd1, cmd2};
// and the command callbacks
static zabbix_cb zabbix_callbacks[] = {&agent_ping, &agent_version};


/**
 * Handle the ping request
 */
static void agent_ping(BufferFiller &buf, String &cmd) {
  Serial.println("callback agent.ping");
  sendZabbixResponse(buf, "1");
}

/**
 * Handle version check
 */
void agent_version(BufferFiller &buf, String &cmd) {
  Serial.println("callback agent.Version");
  sendZabbixResponse(buf, "demo02");
}

/**
 * Handle incoming requests for the zabbix agent
 * Parse the requested item, and call the appropriate handler
 *
 * @param BufferFiller buf the buffer filler to be used for output
 * @param word pos position in the tcp packet
 */
static void serviceZabbixRequest(BufferFiller &buf, word pos) {

  byte i;
  String cmd = String((char *) Ethernet::buffer + pos);
  String check;
  const char *ptr;
  char c;

#if DSERIAL
  Serial.println("got packet on 10050");
  Serial.println(cmd);
#endif

  // 2: this is the size of a pointer
  for (i=0; i<(sizeof zabbix_commands)/2; i++) {
    // read from the eeprom
    check = "";
    ptr = zabbix_commands[i];
    while ((c = pgm_read_byte(ptr++))) {
      check += c;
    }
    check += '\n'; // every command ends with a newline
    // now check contains the command name
    
    // Check it agains the current command
    if (cmd == check) {
      zabbix_callbacks[i](buf, cmd);
    }
  }
  

  

}

/**
 * Sends a response to the client
 *
 * @param BufferFiller buf the buffer filler to be used for output
 * @param const char* the response to be sent
 */
void sendZabbixResponse(BufferFiller &buf, const char* response) {
  char header[9];
  byte i, len;

  len = strlen(response);
  
  // Create the header
  for (i=0; i<9; i++) {
    switch (i) {
      case 0:
        header[i] = 0x01; break;
      case 1:
        header[i] = len; break;
      default:
        header[i] = 0;
    }
  }
  Serial.print("response: ");
  Serial.print(len);
  Serial.println(response);
  
  buf = ether.tcpOffset();
  buf.emit_p(PSTR("ZBXD"));
  buf.emit_raw(header, 9);
  buf.emit_raw(response, len);
  ether.httpServerReply(buf.position());
}
