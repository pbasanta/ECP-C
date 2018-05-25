// Wire Slave Sender
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Sends data as an I2C/TWI slave device
// Refer to the "Wire Master Reader" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

int brightness = 0;

void setup()
{
  Wire.begin(4);                // join i2c bus with address #2
  Wire.onRequest(requestEvent); // register event
  pinMode(13, OUTPUT);     
  digitalWrite(13, LOW);
}

void loop()
{
  delay(100);
 //analogWrite(13, HIGH);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
 Wire.write("hello "); // respond with message of 6 bytes
  
 if(brightness==0){
  digitalWrite(13,HIGH);
  brightness=1;
 }
 else
 {digitalWrite(13,LOW);
   brightness=0;
 }



}
