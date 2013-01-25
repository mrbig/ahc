/**
 * This file contains the codes of the zabbix agent
 */

// This is the type definition for the callback functions
typedef void(* zabbix_cb)(BufferFiller &buf, String &cmd);

// This structure is used to configure the callbacks
typedef struct {
  const char * cmd; // Name of the item
  const zabbix_cb callback; // Callback handler
} ZabbixConfig;

// These are the valid commands handled by the system
static const char cmd1[] PROGMEM = "agent.ping";
static const char cmd2[] PROGMEM = "agent.version";


/**
 * Configuration of the commands and the callbacks
 */
static const ZabbixConfig zabbix_config[] = {
  {cmd1, &agent_ping},
  {cmd2, &agent_version}
};

/**
 * Handle the ping request
 */
static void agent_ping(BufferFiller &buf, String &cmd) {
  sendZabbixResponse(buf, "1");
}

/**
 * Handle version check
 */
static void agent_version(BufferFiller &buf, String &cmd) {
  sendZabbixResponse(buf, "ahc 0.1");
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

  // Iterate over the configuration array
  for (i=0; i<(sizeof zabbix_config)/(sizeof (ZabbixConfig)); i++) {
    // read from the eeprom
    check = "";
    ptr = zabbix_config[i].cmd;
    while ((c = pgm_read_byte(ptr++))) {
      check += c;
    }
    check += '\n'; // every command ends with a newline
    // now check contains the command name
    
    // Check it agains the current command
    if (cmd == check) {
      (zabbix_config[i].callback)(buf, cmd);
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
