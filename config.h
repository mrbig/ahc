#define SERIAL 1 // set to 1 to show incoming request on serial port

/**
 * ethernet configuration
 */
// ethernet interface ip address
static byte myip[] = { 192,168,1,200 };
// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };


/**
 * Pinout configuration
 */
// Pin of the ethernet card
int Ether_Pin = 10;

// DS18S20 Signal pin on digital 2
int DS18S20_Pin = 3;
 // Pin for the DHT11
int DHT11_Pin = 4;
 // Pin of the controlled device
int Control_Pin = 5;
