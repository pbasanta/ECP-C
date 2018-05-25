#include <rtthreads.h>
//#include <ChibiOS_AVR.h>
//#include <utility/hal.h>
//#include <SerialPort.h>



//SerialPort<0, 63, 63> NewSerial;
void setup(){
  
  Serial.begin(9600);
  
  //halInit();
  //NewSerial.begin(9600);
  //SerialDriver sdp;
  
  while(1==1){

  while (Serial.available()>0)
   {
    Serial.write(Serial.read());
    } 
  }
}
  
void loop(){
}



