
#include <rtthreads.h>

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);     //LED
}



void periodic_info() {
  static uint32_t counter=0;
  counter++;
  Serial.print("[T2] THREAD 1------\n   counter:\t");  
  Serial.println(counter);  
}

void error_handler(int erno)
{
  Serial.print("[T2] Error------\t");  
  while(1){
  }
}

void loop() { 
  
  Serial.print("Simple example  \n\n");  
  delay(1000); 
  /*
    Thread creation.
    Stack size =200 bytes 
    Priority=20
    Offset=0
    T=D= 1 second
    Cmax= 0.5 seconds
    error function
  */
  THREAD* th1=periodic_thread_create(&periodic_info, NULL, NULL, 200, 30, 0, 1000000, 1000000, 500000,&error_handler); 
  threads_subsystem_start();


}
