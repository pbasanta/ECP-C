// Wire Slave Sender
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Sends data as an I2C/TWI slave device
// Refer to the "Wire Master Reader" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#define MASTER_ID 10
#define SLAVE1_ID 12
#define SLAVE2_ID 14

#include <Wire.h>

int brightness = 0;

void setup()
{
  Wire.begin(SLAVE1_ID);                // join i2c bus with address #2
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event
  pinMode(13, OUTPUT);     
  digitalWrite(13, LOW);
}
byte counter='a';
void loop()
{

  delay(10000); //10 seconds
 //analogWrite(13, HIGH);
 counter++;
 if (counter=='p')
 counter='a';
  Wire.beginTransmission(MASTER_ID);
      Wire.write("Alarm S1:");    // sending data
      Wire.write(counter);
      Wire.write("");
  Wire.endTransmission();
  
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
 Wire.write("s1:hl "); // respond with message of 6 bytes
}

void receiveEvent(int howmany)
{
  while(Wire.available())    // slave may send less than requested
  { 
    char c = Wire.read(); // receive a byte as character
    //Serial.print(c);         // print the character
  }
  
   if(brightness==0){
  digitalWrite(13,HIGH);
  brightness=1;
 }
 else
 {digitalWrite(13,LOW);
   brightness=0;
 }

}
