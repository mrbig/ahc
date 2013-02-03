#include <EtherCard.h>
#include <EEPROM.h>
#include "config.h"



// Output state of the driver
static byte IOState;

// This stores the current working mode of the io
#define IOMODE_AUTO 2
#define IOMODE_ON 1
#define IOMODE_OFF 0
static byte IOMode = IOMODE_OFF;


// Target humidity minimum (below this we will turn on the IO)
static byte targetHumidity_min = DEFAULT_HUMIDITY_MIN;

// Target humidity maximum (turn off the IO above this)
static byte targetHumidity_max = DEFAULT_HUMIDITY_MAX;


// tcp/ip send and receive buffer
byte Ethernet::buffer[880];


/**
 * Setup the hardware in and outputs
 */
void setup(void) {
#if DSERIAL
  Serial.begin(57600);
  Serial.println(F("booting..."));
#endif
  pinMode(Control_Pin, OUTPUT);
  
  // Setup the ethernet card
  if (ether.begin(sizeof Ethernet::buffer, mymac, 10) == 0) {
#if !DSERIAL
    Serial.begin(57600);
#endif
    Serial.println(F("Failed to access Ethernet controller"));
  }
  
  ether.staticSetup(myip);
  loadSettings();

#if DSERIAL
  ether.printIp(F("IP:  "), ether.myip);
  ether.printIp(F("GW:  "), ether.gwip);  
  ether.printIp(F("DNS: "), ether.dnsip);  
  Serial.print(F("Free memory "));
  Serial.println(memoryTest(), DEC);
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
  Serial.println(F("Checking IO state"));
#endif

  checkDHT();
  humidity = getHumidity();
  
  // In auto mode update the state depending 
  // on the current humidity value
  if (IOMode == IOMODE_AUTO) {
    if (humidity <= -1000) {
      // If there was an error then turn off the IO
      IOState = 0;
    }
    else if (humidity < targetHumidity_min) {
      IOState = 1;
    }
    else if (humidity >= targetHumidity_max) {
      IOState = 0;
    }
  } else {
    // In manual mode set to the current value
    IOState = IOMode;
  }
  
  digitalWrite(Control_Pin, IOState);
  
}

/**
 * Writes current settings to the EEPROM
 */
void saveSettings() {
  int ptr = 0;
  byte checksum = 0;
  EEPROM.write(ptr++, EEPROM_ID);
  EEPROM.write(ptr++, IOMode);
  EEPROM.write(ptr++, targetHumidity_min);
  EEPROM.write(ptr++, targetHumidity_max);
  checksum = IOMode + targetHumidity_min + targetHumidity_max;
  EEPROM.write(ptr++, checksum);
}

/**
 * Loads the current settings from the EEPROM
 */
void loadSettings() {
  int ptr = 0;
  byte checksum = 0;
  byte i;
  
  if (EEPROM.read(ptr++) != EEPROM_ID) {
#if DSERIAL
    Serial.println(F("Invalid EEPROM_ID found"));
#endif
    return;
  }
  
  // calculate the cheksum
  for (i=ptr; i<ptr+3; i++) {
    checksum += EEPROM.read(i);
  }
  
  if (checksum != EEPROM.read(i)) {
#if DSERIAL
    Serial.println(F("Checksum error, when reading from eeprom"));
#endif
    return;
  }

  IOMode = EEPROM.read(ptr++);
  targetHumidity_min = EEPROM.read(ptr++);
  targetHumidity_max = EEPROM.read(ptr++);
  
}

// this function will return the number of bytes currently free in RAM - for diagnostics only
int memoryTest()
{
  int byteCounter = 0; // initialize a counter
  byte *byteArray; // create a pointer to a byte array
  // More on pointers here: http://en.wikipedia.org/wiki/Pointer#C_pointers

  // use the malloc function to repeatedly attempt
  // allocating a certain number of bytes to memory
  // More on malloc here: http://en.wikipedia.org/wiki/Malloc
  while ( (byteArray = (byte*) malloc (byteCounter * sizeof(byte))) != NULL )
  {
    byteCounter++; // if allocation was successful, then up the count for the next try
    free(byteArray); // free memory after allocating it
  }

  free(byteArray); // also free memory after the function finishes
  return byteCounter; // send back the highest number of bytes successfully allocated
}

