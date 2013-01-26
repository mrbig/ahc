#include <EtherCard.h>

#include "config.h"



// Output state of the driver
static byte IOState;

// This stores the current working mode of the io
#define IOMODE_AUTO 2
#define IOMODE_ON 1
#define IOMODE_OFF 0
static byte IOMode = IOMODE_OFF;




// tcp/ip send and receive buffer
byte Ethernet::buffer[768];


/**
 * Setup the hardware in and outputs
 */
void setup(void) {
#if DSERIAL
  Serial.begin(57600);
  Serial.println("booting...");
#endif
  pinMode(Control_Pin, OUTPUT);
  
  // Setup the ethernet card
  if (ether.begin(sizeof Ethernet::buffer, mymac, 10) == 0) 
    Serial.println( "Failed to access Ethernet controller");
  
  ether.staticSetup(myip);

#if DSERIAL
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);  
  ether.printIp("DNS: ", ether.dnsip);  
#endif
}

/**
 * This is the main loop
 */
void loop(void) {

  checkEthernet();
  IOController(false);

}

/**
 * Check the humidity at every five second, a decide wether the
 * IO should be turned on or off
 * @param boolean force if set to true, then skips the time check
 */
void IOController(boolean force) {
  static unsigned long lastMillis = 0;
  unsigned long currentMillis = millis();
  float humidity;
  
  if (!force && (currentMillis - lastMillis < 5000)) return;
  
  lastMillis = currentMillis;
  
#if DSERIAL
  Serial.println("Checking IO state");
#endif

  humidity = getTemp(); // for debugging
  
  // In auto mode update the state depending 
  // on the current humidity value
  if (IOMode == IOMODE_AUTO) {
    if (humidity <= -1000) {
      // If there was an error then turn off the IO
      IOState = 0;
    }
    else if (humidity <= MIN_HUM) {
      IOState = 1;
    }
    else if (humidity >= MAX_HUM) {
      IOState = 0;
    }
  } else {
    // In manual mode set to the current value
    IOState = IOMode;
  }
  
  digitalWrite(Control_Pin, IOState);
  
}





