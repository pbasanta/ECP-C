#include <rtthreads.h>
#include <ChibiOS_AVR.h>
#include <utility/hal.h>
#include <SerialPort.h>

#include <Arduino.h>

SerialPort<0, 63, 63> NewSerial;
void setup(){
  
  //Serial.begin(9600);
  
  halInit();
  NewSerial.begin(9600);
  //SerialDriver sdp;
  
  while(1==1){

  while (NewSerial.available()>0)
   {
    NewSerial.write(NewSerial.read());
    } 
  }
}
  
void loop(){
}



