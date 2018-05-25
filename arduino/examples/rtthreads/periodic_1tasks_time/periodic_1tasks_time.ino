

#include <rtthreads.h>

void setup() 
{
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);     //LED
}


void periodic_task1() 
{
  static uint32_t counter=0;
  counter++;
  Serial.print("[T1] THREAD 1------\n  current time(us): \t \t");  
  Serial.print(thread_get_time());  
  Serial.print("\n   and error(us):\t");  
  Serial.println(thread_get_error_in_time());
}

void error_handler_task1(int erno)
{
  Serial.print("[T1] Error------\t");  
  while(1){
  }
}



void loop() { 
  
  Serial.print("Simple example\n  showing clock info + deviation \n");  
  delay(1000); 
  /*
   The tasks accesses to the information
  */
  THREAD* th1=periodic_thread_create(&periodic_task1, NULL, NULL, 200, 30, 0, 1000000, 1000000, 500000, &error_handler_task1); 
  threads_subsystem_start();


}
