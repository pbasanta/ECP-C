
#include <rtthreads.h>

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);     //LED
}



static int incoming_byte = 0; 

void periodic_task1() {
  int* params= (int*) thread_get_params();
  Serial.print("[T2] THREAD 1------\n   counter:\t");  
  Serial.print(*params);  
  (*params)++;
}



void error_handler_task1(int erno)
{
  Serial.print("[T2] Error------\t \n  erno:"); 
  Serial.println(erno);   
  while(1){
  }
}



void loop() { 
  
  Serial.print("Simple example with overun \n\n");  
  delay(1000); 
  /*
    Thread creation.
    Stack size =200 bytes 
    Priority=20
    Offset=0
    T=D= 1 second
    Cmax= 0.5 seconds. The task should be preempted !!!
    error function
  */
  int counter =0;
  THREAD* th1=periodic_thread_create(&periodic_task1, &counter, NULL, 200, 30, 10000, 1000000, 1000000, 500000,&error_handler_task1); 
 
  threads_subsystem_start();


}
