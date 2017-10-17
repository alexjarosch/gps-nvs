#include <TinyGPS++.h>
#include <Wire.h>
#include <ADXL345.h>
#include <SPI.h>
#include <SD.h>

/*
  This program logs the accelerometer and GNSS data to an SD card along
  with controlling a second arduino to log raw data
  Software Version: 1.0
*/

// define an error led
int led = 13;
int pulse = 7;

// accelerometer
ADXL345 adxl; //variable adxl is an instance of the ADXL345 librar

// The TinyGPS++ object
TinyGPSPlus gps;

void setup()
{
  // this is important to avoid many startup files
  pinMode(pulse, OUTPUT); // file switch pulse
  digitalWrite(pulse, LOW); // set the interrupt to low
  pinMode(10, OUTPUT); //sd card
  pinMode(led, OUTPUT); //led
  
  // wait for the binary setup to finish
  delay(12000);
  
  // open a default serial
  Serial.begin(115200);
  delay(5000);
  // config port 1 to 9600 baud NMEA protocol
  Serial.println("$PORZA,1,9600,1*75");
  delay(5000);
  Serial.end();
  
  // open the usable port
  Serial.begin(9600);
  
  if (!SD.begin(4)) {
    digitalWrite(led, HIGH);
    delay(1000);
    digitalWrite(led, LOW);
    return;
  }
  
  adxl.powerOn(); // power up the accelerometer

  //set activity/ inactivity thresholds (0-255)
  adxl.setActivityThreshold(75); //62.5mg per increment
  adxl.setInactivityThreshold(75); //62.5mg per increment
  adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?
 
  //look of activity movement on this axes - 1 == on; 0 == off 
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);
 
  //look of inactivity movement on this axes - 1 == on; 0 == off
  adxl.setInactivityX(1);
  adxl.setInactivityY(1);
  adxl.setInactivityZ(1);
 
  //look of tap movement on this axes - 1 == on; 0 == off
  adxl.setTapDetectionOnX(0);
  adxl.setTapDetectionOnY(0);
  adxl.setTapDetectionOnZ(1);
 
  //set values for what is a tap, and what is a double tap (0-255)
  adxl.setTapThreshold(50); //62.5mg per increment
  adxl.setTapDuration(15); //625us per increment
  adxl.setDoubleTapLatency(80); //1.25ms per increment
  adxl.setDoubleTapWindow(200); //1.25ms per increment
 
  //set values for what is considered freefall (0-255)
  adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
 
  //setting all interrupts to take place on int pin 1
  //I had issues with int pin 2, was unable to reset it
  adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );
 
  //register interrupt actions - 1 == on; 0 == off  
  adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
  adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
  adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);

  // NMEA string to clear broadcast and define required broadcast
  Serial.println("$PORZB*55");
  delay(500);
  Serial.println("$PONME,2,6,1*40");
  delay(500);
  Serial.println("$PORZB,GGA,1,RMC,1*48");
  delay(500);
  
}

void loop()
{
  // define logging interval
  if (gps.time.second() == 00) {
      
    int x,y,z;
    adxl.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z
    
    File dataFile = SD.open("nmealog.txt", FILE_WRITE);
    // construct the logging message with print commands within the file, easier than assembling 
    // a string
    if (dataFile) {
      if (gps.date.isValid()) {
        if (gps.date.day() < 10) dataFile.print(F("0"));
        dataFile.print(gps.date.day());
        dataFile.print(F("."));
        if (gps.date.month() < 10) dataFile.print(F("0"));
        dataFile.print(gps.date.month());
        dataFile.print(F("."));
        dataFile.print(gps.date.year());
      }
      else {
        dataFile.print(F("DD:MM:YYYY"));
      }
        
      dataFile.print(F(" "));
      
      // add gps time
      if (gps.time.isValid()) {
        if (gps.time.hour() < 10) dataFile.print(F("0"));
        dataFile.print(gps.time.hour());
        dataFile.print(F(":"));
        if (gps.time.minute() < 10) dataFile.print(F("0"));
        dataFile.print(gps.time.minute());
        dataFile.print(F(":"));
        if (gps.time.second() < 10) dataFile.print(F("0"));
        dataFile.print(gps.time.second());
      }
      else {
        dataFile.print(F("hh:mm:ss"));
      }
      
      dataFile.print(F(" "));
      
      if (gps.location.isValid())
      {
        dataFile.print(gps.location.rawLatDegrees());
        dataFile.print(F("."));
        dataFile.print(gps.location.rawLatBillionths());
        dataFile.print(F(" "));
        dataFile.print(gps.location.rawLngDegrees());
        dataFile.print(F("."));
        dataFile.print(gps.location.rawLngBillionths());
        dataFile.print(F(" "));
        dataFile.print(gps.altitude.meters(), 3);
      }
      else
      {
        dataFile.print(F("INVALID-COORDS"));
      }
      
      dataFile.print(F(" "));
      
      dataFile.print(gps.satellites.value());
      dataFile.print(F(" "));
      dataFile.print(gps.hdop.value());
      dataFile.print(F(" "));
      
      dataFile.print(x);
      dataFile.print(F(" "));
      dataFile.print(y);
      dataFile.print(F(" "));
      dataFile.print(z);
      dataFile.println();
      dataFile.close();
    }
    else {
      digitalWrite(led, HIGH);
      delay(1000);
      digitalWrite(led, LOW);
      delay(1000);
      digitalWrite(led, HIGH);
      delay(1000);
      digitalWrite(led, LOW);
    }
  }
  
  // define file switch for binary logging  
  if (gps.time.hour() == 23 && gps.time.minute() == 59 && gps.time.second() == 42) {
    digitalWrite(led, HIGH);
    digitalWrite(pulse, HIGH);
    Serial.println("File switch pulse sent");
  }
  else {
    digitalWrite(led, LOW);
    digitalWrite(pulse, LOW);
  }
  smartDelay(1000);
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (Serial.available())
      gps.encode(Serial.read());
  } while (millis() - start < ms);
}

