#include <EtherCard.h>

#include "config.h"



// Output state of the driver
byte IOState;

// tcp/ip send and receive buffer
byte Ethernet::buffer[500];


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

}





