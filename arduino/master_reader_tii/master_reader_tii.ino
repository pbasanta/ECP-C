// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.

#define MASTER_ID 10
#define SLAVE1_ID 12
#define SLAVE2_ID 14

#include <Wire.h>


void setup()
{
  Wire.begin(MASTER_ID);        // join i2c bus (address optional for master)
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);  // start serial for output
  Serial.write(" \n peek, available: ");
  Serial.print(Wire.peek());
  Serial.print(Wire.available());
 Serial.write(" \n  ");
}

void task(int who, int send, int receive)
{
  long t1= micros();
  if(send!=0){
    Wire.beginTransmission(who);
    for(int i=0;i<send;i++)
    {
      Wire.write((byte)16);    // sending data
    }
    Wire.endTransmission();
  }
 int cuantos2=0;
 long t2= micros();
  if(receive!=0){
   cuantos2=Wire.requestFrom(who, receive);    // request 6 bytes from slave device #2
  }
 long t3= micros();
  int amount=0;


  Serial.print("\n Slave ");
  Serial.print(who);
  Serial.print("(s: ");
  Serial.print(send);
  Serial.print(" r: ");
  Serial.print(receive);
  Serial.print("). Time in us s - r: ");  
  Serial.print(t2-t1);
  Serial.print(" - ");
  Serial.print(t3-t2);
  Serial.print(". Amount: ");
  Serial.print(amount);
  Serial.print(". Cuantos: ");
  Serial.print(amount);
  Serial.print("\t ");
  //Flushing the buffer
  while(Wire.available())    // slave may send less than requested
  { 
    char c = Wire.read(); // receive a byte as character
    amount++;
    Serial.print(c);         // print the character
  }
  
  
}
int count=0;
void loop()
{
  if (count==4)
  {
  task(SLAVE1_ID,6,0);

  }
  ///task(SLAVE1_ID,1,0);
  task(SLAVE1_ID,0,10000);

  //task(SLAVE2_ID ,0,6);
  //task(SLAVE2_ID ,6,0);
  
  count++;
  if (count==5)
  {
    count=0;
  }  
  delay(10000);

}

//Useful for async traffic
void receiveEvent(int howMany){
  if(Wire.available()==howMany)
  {
  while(Wire.available())    // slave may send less than requested
  { 
    char c = Wire.read(); // receive a byte as character
   Serial.print(c);         // print the character
  }
  }

}
