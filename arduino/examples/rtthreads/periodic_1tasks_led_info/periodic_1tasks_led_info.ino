

#include <rtthreads.h>

void setup() 
{
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);     //LED
}



static int led_info=0;
void periodic_task1() 
{
  static uint32_t counter=0;
  counter++;
  Serial.print("[T1] THREAD 1------\n  led info(): \t \t");  
  Serial.print(led_info);  
  Serial.println("") ;
}

void error_handler_task1(int erno)
{
  Serial.print("[T1] Error------\t");  
  while(1){
  }
}


void led_handler(void)
{
  if(led_info==0) {
    led_info=1;
    }
  else {
    led_info=0;    
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
