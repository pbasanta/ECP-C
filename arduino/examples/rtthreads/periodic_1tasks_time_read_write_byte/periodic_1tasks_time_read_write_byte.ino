

#include <rtthreads.h>

void setup() 
{
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);     //LED
}


void periodic_task1() 
{
  static uint8_t counter=0;
  static uint16_t counter16=0;
  static uint32_t counter32=0;
  static float  float_counter=0.0;
  
  //uint8_t aux_count=counter+1;
  //uint16_t aux_count16=counter16+1;
  //Write operation
  thread_write_byte((uint32_t)&counter, counter+1);
  thread_write_uint16_t((uint32_t)&counter16, counter16+2);
  thread_write_uint32_t((uint32_t)&counter32, counter32+4);
  thread_write_float((uint32_t)&float_counter, float_counter+8.0);  
  
  Serial.print("[T1] THREAD 1------\n  current time(us): \t \t");  
  Serial.print(thread_get_time());  

  Serial.print("\n   counter(s):\n");  
  //Read operation
  uint8_t aux_count8_2=thread_read_byte((uint32_t)&counter);
  uint16_t aux_count16_2=thread_read_uint16_t((uint32_t)&counter16);
  uint32_t aux_count32_2=thread_read_uint32_t((uint32_t)&counter32);
  float aux_float_2=thread_read_float((uint32_t)&float_counter);
  Serial.println(aux_count8_2);
  Serial.println(aux_count16_2);
  Serial.println(aux_count32_2);
 // Serial.println(aux_float_2);
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
