/**
 * Handle webserver codes
 */

// Prepared pieces of code
static const char okHeader[] PROGMEM = 
  "HTTP/1.0 200 OK\r\n"
  "Content-Type: text/html; charset=utf8\r\n"
  "Pragma: no-cache\r\n"
;

static const char redirect[] PROGMEM =
  "HTTP/1.0 302 found\r\n"
  "Location: /\r\n"
  "\r\n"
;

static const char htmlHeader[] PROGMEM =
  "<!DOCTYPE HTML>\n"
  "<html>"
  "<head>"
    "<title>parologtato v0.1</title>"
  "</head>"
  "<body>"
;
static const char htmlFooter[] PROGMEM =
  "</body></html>"
;

/**
 * Service any request on the webserver
 * @param BufferFiller buf the buffer filler to be used for output
 * @param word pos Position in the ethernet packet
 */
static void serviceHttpRequest(BufferFiller& buf, const word pos) {
  buf = ether.tcpOffset();
  char * data = (char *) Ethernet::buffer + pos;

#if DSERIAL
  Serial.println(data);
#endif

  // Check the input and prepare the returned content
  if (strncmp("GET / HTTP", data, 10) == 0)
    homePage(buf);
  else if (strncmp("GET /?io=", data, 8) == 0)
    updateIO(buf, data);
  else if (strncmp("GET /?min=", data, 9) == 0)
    updateTarget(buf, data);
  else
    send404(buf);

  // send web page data
  ether.httpServerReply(buf.position());
}

/**
 * Display a radio button with a label
 * @param BufferFiller buf the buffer filler to be used for output
 * @param const char * label label for the radio button
 * @param const char * name name of the radio button
 * @param const char * value value of the radio button
 * @param byte state wether the radio button should be marked on or off
 */
static void printRadio(BufferFiller& buf, const char* label, const char* name, const char * value, const byte state) {
  buf.emit_p(PSTR(
    "<label>$F<input type=\"radio\" name=\"$F\" value=\"$F\""
    ), label, name, value);
  if (state) {
    buf.emit_p(PSTR(" checked"));
  }
  buf.emit_p(PSTR(" onchange=\"this.form.submit()\"></label> "));
}
/**
 * Display the main page
 * @param BufferFiller buf the buffer used to write to the output
 */
static void homePage(BufferFiller& buf) {
  buf.emit_p(PSTR("$F\r\n"
    "$F"
    "<h1>Párologtató vezérlés</h1>"
    "<form>"
  ), okHeader, htmlHeader);
  printRadio(
    buf,
    PSTR("Auto:"),
    PSTR("io"),
    PSTR("2"),
    IOMode == IOMODE_AUTO
  );
  printRadio(
    buf,
    PSTR("Be:"),
    PSTR("io"),
    PSTR("1"),
    IOMode == IOMODE_ON
  );
  printRadio(
    buf,
    PSTR("Ki:"),
    PSTR("io"),
    PSTR("0"),
    IOMode == IOMODE_OFF
  );
  buf.emit_p(PSTR(
    "</form>"));
  buf.emit_p(PSTR("<form>"
    "<h2>elvárt páratartalom:</h2>"
    "<p><label>min: <input name=\"min\" size=\"2\" maxlength=\"2\" type=\"number\" min=\"20\" max=\"90\" pattern=\"[0-9]{2}\" value=\"$D\">%</label> "
    "<label>max: <input name=\"max\" size=\"2\" maxlength=\"2\" type=\"number\" min=\"20\" max=\"90\" pattern=\"[0-9]{2}\" value=\"$D\">%</label></p>"
    "<input type=\"submit\" value=\"ok\">"
    "</form>$F"), targetHumidity_min, targetHumidity_max, htmlFooter);
}

static int getIntArg(const char* data, const char* key, int value =-1) {
  char temp[10];
  if (ether.findKeyVal(data + 6, temp, sizeof temp, key) > 0)
      value = atoi(temp);
  return value;
}

/**
 * Update the target humidity value
 *
 * @param BufferFiller buf the buffer used to write to the output
 * @param const char* data the incoming tcp data
 */
static void updateTarget(BufferFiller& buf, const char* data) {
  byte min = getIntArg(data, "min");
  byte max = getIntArg(data, "max");
#if DSERIAL
  Serial.print(F("Received new target value, min: "));
  Serial.print(min, DEC);
  Serial.print(F(" max: "));
  Serial.println(max, DEC);
#endif
  if (checkTargetHumidity(min, max)) {
    targetHumidity_min = min;
    targetHumidity_max = max;
    IOController(true);
    // TODO: write to the eeprom
  }
  buf.emit_p(redirect);
}

/**
 * Check the target humidity values for correctness
 * @param byte min the minimum value
 * @param byte max the maximum value
 * @return boolean true if the values are correct
 */
static boolean checkTargetHumidity(const byte min, const byte max) {
  if (min<20) return false;
  if (max>90) return false;
  if (min>max) return false;
  return true;
}

/**
 * Check the io state and update it
 *
 * @param BufferFiller buf the buffer used to write to the output
 * @param const char* data the incoming tcp data
 */
static void updateIO(BufferFiller& buf, const char* data) {
  byte d = getIntArg(data, "io");
#if DSERIAL
  Serial.print(F("Received new io state"));
  Serial.println(d, DEC);
#endif
  if (d>=0 && d<=2) {
    IOMode = d;
    IOController(true);
  }
  buf.emit_p(redirect);
}
  
/**
 * Send a 404 reply
 * @param BufferFiller buf the buffer used to write to the output
 */
static void send404(BufferFiller& buf) {
  buf.emit_p(PSTR(
    "HTTP/1.0 404 Not found\r\n"
    "Content-Type: text/html\r\n"
    "\r\n"
    "<h1>404 Not found</h1>"));
}

