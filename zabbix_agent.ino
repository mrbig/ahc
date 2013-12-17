/* Copyright (C) 2009 Nagy Attila Gabor <nagy.attila.gabor@gmail.com>
 *
 *     This file is part of AHC - AHC Humidity Control.
 *
 *  TrackMii is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  TrackMii is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with AHC.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
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
static const char cmd3[] PROGMEM = "ahc.iostate";
static const char cmd4[] PROGMEM = "ahc.temperature";
static const char cmd5[] PROGMEM = "ahc.humidity";
static const char cmd6[] PROGMEM = "ahc.memory.free";
static const char cmd7[] PROGMEM = "ahc.dht.status";
static const char cmd8[] PROGMEM = "ahc.trg.min";
static const char cmd9[] PROGMEM = "ahc.trg.max";

/**
 * Configuration of the commands and the callbacks
 */
static const ZabbixConfig zabbix_config[] = {
  {cmd1, &zbx_agent_ping},
  {cmd2, &zbx_agent_version},
  {cmd3, &zbx_ahc_iostate},
  {cmd4, &zbx_ahc_temperature},
  {cmd5, &zbx_ahc_humidity},
  {cmd6, &zbx_ahc_memory},
  {cmd7, &zbx_ahc_dhtStatus},
  {cmd8, &zbx_ahc_targetHumidityMin},
  {cmd9, &zbx_ahc_targetHumidityMax}
};

/**
 * Handle the ping request
 */
void zbx_agent_ping(BufferFiller &buf, String &cmd) {
  sendZabbixResponse(buf, "1");
}

/**
 * Return the current io state
 */
void zbx_ahc_iostate(BufferFiller &buf, String &cmd) {
  char buffer[2];
  buffer[0] = IOState + '0';
  buffer[1] = 0;
  sendZabbixResponse(buf, buffer);
}

/**
 * Return the current temperature
 */
void zbx_ahc_temperature(BufferFiller &buf, String &cmd) {
  char buffer[16];
  float temp = getTemp();
  if (temp > -1000 && temp != 0) {
    dtostrf(temp, 4, 2, buffer);
    sendZabbixResponse(buf, buffer);
  } else {
    sendZabbixError(buf);
  }
}

/**
 * Return the current humidity
 */
void zbx_ahc_humidity(BufferFiller &buf, String &cmd) {
  char buffer[16];
  float hum = getHumidity();
  if (hum > -1000) {
    dtostrf(hum, 4, 2, buffer);
    sendZabbixResponse(buf, buffer);
  } else {
    sendZabbixError(buf);
  }
}

/**
 * Return the last humidity download status
 */
void zbx_ahc_dhtStatus(BufferFiller &buf, String &cmd) {
  char buffer[6];
  sprintf(buffer, "%d", abs(getHumidityStatus()));
  sendZabbixResponse(buf, buffer);
}


/**
 * Return the available memory
 */
void zbx_ahc_memory(BufferFiller &buf, String &cmd) {
  char buffer[6];
  sprintf(buffer, "%d", memoryTest());
  sendZabbixResponse(buf, buffer);
}

/**
 * Return the minimum target humidity
 */
void zbx_ahc_targetHumidityMin(BufferFiller &buf, String &cmd) {
  char buffer[5];
  sprintf(buffer, "%d", targetHumidity_min);
  sendZabbixResponse(buf, buffer);
}

/**
 * Return the maximum target humidity
 */
void zbx_ahc_targetHumidityMax(BufferFiller &buf, String &cmd) {
  char buffer[5];
  sprintf(buffer, "%d", targetHumidity_max);
  sendZabbixResponse(buf, buffer);
}

/**
 * Handle version check
 */
void zbx_agent_version(BufferFiller &buf, String &cmd) {
  sendZabbixResponse(buf, "ahc 0.1");
}


/**
 * Handle incoming requests for the zabbix agent
 * Parse the requested item, and call the appropriate handler
 *
 * @param BufferFiller buf the buffer filler to be used for output
 * @param word pos position in the tcp packet
 */
void serviceZabbixRequest(BufferFiller &buf, word pos) {

  byte i;
  String cmd = String((char *) Ethernet::buffer + pos);
  String check;
  const char *ptr;
  char c;

#if DSERIAL
  Serial.println(F("got packet on 10050"));
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
      return;
    }
  }
  
  sendZabbixResponse(buf, "ZBX_NOTSUPPORTED");
  

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
#if DSERIAL
  Serial.print(F("response: ("));
  Serial.print(len);
  Serial.print(F(")"));
  Serial.println(response);
#endif
  
  buf = ether.tcpOffset();
  buf.emit_p(PSTR("ZBXD"));
  buf.emit_raw(header, 9);
  buf.emit_raw(response, len);
  ether.httpServerReply(buf.position());
}

/**
 * Send an ZBX_ERROR to the zabbix client
 *
 * @param BufferFiller buf the buffer filler to be used for output
 */
void sendZabbixError(BufferFiller &buf) {
  sendZabbixResponse(buf, "ZBX_ERROR");
}
