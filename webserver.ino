/**
 * Handle webserver codes
 */

// used as cursor while filling the buffer
static BufferFiller bfill;

// Prepared pieces of code
static const char okHeader[] PROGMEM = 
  "HTTP/1.0 200 OK\r\n"
  "Content-Type: text/html\r\n"
  "Pragma: no-cache\r\n"
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
#if SERIAL
    Serial.println(data);
#endif
    // Check the input and prepare the returned content
    if (strncmp("GET / HTTP", data, 10) == 0)
      homePage(bfill);
    else
      send404(bfill);
      
    // send web page data
    ether.httpServerReply(bfill.position());
  }
}



/**
 * Display the main page
 * @param BufferFiller buf the buffer used to write to the output
 */
static void homePage(BufferFiller& buf) {
  buf.emit_p(PSTR("$F\r\n"
    "<h1>Hello world</h1>"
  ), okHeader);
}
  
/**
 * Send a 404 reply
 * @param BufferFiller buf the buffer used to write to the output
 */
static void send404(BufferFiller& buf) {
  buf.emit_p(PSTR(
    "HTTP/1.0 404 Not found\r\n"
    "Content-Type: text/html"
    "\r\n"
    "<h1>404 Not found</h1>"));
}
