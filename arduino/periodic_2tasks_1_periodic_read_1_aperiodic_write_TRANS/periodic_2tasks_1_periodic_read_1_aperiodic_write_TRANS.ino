
#include <rtthreads.h>

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);     //LED
}


static THREAD * signaler= NULL; 
static int incoming_byte = 0; 

void aperiodic_task1() {
  static uint32_t counter=0;
  
  SIMPLERTFRAMEWORK_ATOMIC_SECTION{ 
    counter++;
  }
  Serial.print("[T1] THREAD 1------\n   counter:\t");  
  Serial.print(counter);  
  Serial.print("\n   inc byte:\t");  
  Serial.print(incoming_byte, DEC);
  Serial.print(" ");  
  Serial.println((char)incoming_byte);
}

void periodic_task2() {
  if (Serial.available() > 0) 
  {
    incoming_byte = Serial.read();
    thread_signal(signaler);
    thread_signal(signaler);
  }
 
}

void error_handler_task1(int erno)
{
   Serial.print("[T1] Error------\t \n  erno:"); 
  Serial.println(erno);   
  while(1){
  }
}

void error_handler_task2(int erno)
{
  Serial.print("[T2] Error------\t \n  erno:"); 
  Serial.println(erno);   
  while(1){
  }
}

void loop() { 
  
  Serial.print("Simple example with 1 aperiodic task and another periodic \n\n");  
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

//  THREAD* th1=aperiodic_thread_create(&aperiodic_task1, NULL, NULL, 200, 20, 0, 1000000, 500000, SIMPLERTFRAMEWORK_OVER_SAVE_POLICY, 2, &error_handler_task1); 
  THREAD* th1=aperiodic_thread_create(&aperiodic_task1, NULL, NULL, 200, 20, 0, 1000000, 500000, SIMPLERTFRAMEWORK_OVER_SAVE_POLICY, 2, NULL); 
  signaler=th1; 
  THREAD* th2=periodic_thread_create(&periodic_task2, NULL, NULL, 200, 30, 0, 1000000, 1000000, 500000, &error_handler_task2); 
  threads_subsystem_start();


}
