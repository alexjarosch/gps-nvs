#include <SdFat.h>
SdFat sd;
SdFile myFile;

// Software Version: 1.0

// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int chipSelect = 4;

int led = 13;
volatile int state = 1;

// define data buffer
const int bufferlen = 1024; // has worked with 1024 and 1000 and 768
char inputBuffer[bufferlen];

// define a cancel all data output message
// uint8_t cancel_all[4]={0x10,0x0E,0x10,0x03};

// different logging intervals
uint8_t request_raw[5]={0x10,0xF4,0x0A,0x10,0x03}; // 1 seconds
// uint8_t request_raw[5]={0x10,0xF4,0x14,0x10,0x03}; // 2 seconds
// uint8_t request_raw[5]={0x10,0xF4,0x32,0x10,0x03}; // 5 seconds
// uint8_t request_raw[5]={0x10,0xF4,0x64,0x10,0x03}; // 10 seconds
// uint8_t conf_com1[10]={0x10,0x0B,0x01,0x80,0x25,0x00,0x00,0x02,0x10,0x03}; // set com 1 to 9600 baud NMEA
uint8_t conf_com2[10]={0x10,0x0B,0x02,0x80,0x25,0x00,0x00,0x04,0x10,0x03}; // set com 2 to 9600 baud BINR

int filecount = 0;
char logfile[9];

void setup()
{
  pinMode(led, OUTPUT);
  // initialize the receiver for 9600 baud comms
  digitalWrite(led, HIGH);
  Serial.begin(115200,SERIAL_8O1);
  while (!Serial) {} // wait for Leonardo ;)
  delay(5000);
  // use the next two lines if you want to configure port 1 
  // from the binary Arduino.
//  Serial.write(conf_com1,sizeof(conf_com1));
//  delay(5000);
  Serial.write(conf_com2,sizeof(conf_com2));
  delay(5000);
  Serial.end();
  digitalWrite(led, LOW);
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600, SERIAL_8O1);
  while (!Serial) {} // wait for Leonardo ;)
  
  // Initialize SdFat or print a detailed error message and halt
  // Use half speed like the native library.
  // change to SPI_FULL_SPEED for more performance.
  if (!sd.begin(chipSelect, SPI_FULL_SPEED)) sd.initErrorHalt();
  // count the existing files
  while (myFile.openNext(sd.vwd(), O_READ)) {
    filecount = filecount + 1;
    myFile.close();
  }
  // attach an interrupt to switch file names
  attachInterrupt(0, filestate, RISING);
 
}

void loop() {
  
  if (state == 1) {
    if (myFile.isOpen()) {
      myFile.close();
    }
    filecount = filecount + 1;
    sprintf(logfile, "l%04d.dat", filecount);
    // open the file for write at end like the Native SD library
    if (!myFile.open(logfile, O_WRITE | O_CREAT | O_SYNC)) {
      sd.errorHalt("opening test.txt for write failed");
    }
    state = 0;
  }
  
  if (state == 0) {
    // request RAW stream on x sec interval
    Serial.write(request_raw,sizeof(request_raw));
  }
  
  while (Serial.available() > 0 && state == 0) {
    Serial.readBytes(inputBuffer, bufferlen);
    digitalWrite(led, HIGH);
    myFile.write(inputBuffer, bufferlen);
    digitalWrite(led, LOW);
  }

}

void filestate()
{
  state = 1;
}
