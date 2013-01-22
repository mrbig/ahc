/**
 * Handle webserver codes
 */

// used as cursor while filling the buffer
static BufferFiller bfill;

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
 * Check the ethernet for incoming data
 * Handle if anything is received.
 * This should be called from each loop in the
 * main loop
 */
void checkEthernet() {
  word pos = ether.packetLoop(ether.packetReceive());
  // check if valid tcp data is received
  if (pos) {
    bfill = ether.tcpOffset();
    char * data = (char *) Ethernet::buffer + pos;
#if DSERIAL
    Serial.println(data);
#endif
    // Check the input and prepare the returned content
    if (strncmp("GET / HTTP", data, 10) == 0)
      homePage(bfill);
    else if (strncmp("GET /?io=", data, 8) == 0)
      updateIO(bfill, data);
    else
      send404(bfill);
      
    // send web page data
    ether.httpServerReply(bfill.position());
  }
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
    "<h1>Lámpa távirányítás</h1>"
    "<form>"
  ), okHeader, htmlHeader);
  printRadio(
    buf,
    PSTR("Be:"),
    PSTR("io"),
    PSTR("1"),
    IOState
  );
  printRadio(
    buf,
    PSTR("Ki:"),
    PSTR("io"),
    PSTR("0"),
    !IOState
  );
  buf.emit_p(PSTR(
    "</form>$F"), htmlFooter);
}

static int getIntArg(const char* data, const char* key, int value =-1) {
  char temp[10];
  if (ether.findKeyVal(data + 6, temp, sizeof temp, key) > 0)
      value = atoi(temp);
  return value;
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
  Serial.print("Received new io state");
  Serial.println(d, DEC);
#endif
  if (d>=0 && d<=1) {
    IOState = d;
    digitalWrite(Control_Pin, IOState);
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

