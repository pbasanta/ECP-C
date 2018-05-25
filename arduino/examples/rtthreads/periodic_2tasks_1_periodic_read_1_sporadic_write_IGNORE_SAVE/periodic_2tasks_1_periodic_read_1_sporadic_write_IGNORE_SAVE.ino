
#include <rtthreads.h>

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);     //LED
}


static THREAD * signaler= NULL; 
static int incoming_byte = 0; 
static int result_signal1 = 0; 
static int result_signal2 = 0; 
static int result_signal3 = 0; 
void sporadic_task1() {
  static uint32_t counter=0;
  counter++;
  Serial.print("[T1] THREAD 1------\n   counter:\t");  
  Serial.print(thread_get_time());  
  Serial.print("\n inc byte:\t");  
  Serial.print(incoming_byte, DEC);
  Serial.print(" ");  
  Serial.println((char)incoming_byte);
  Serial.print("res: --------------\n");  
  Serial.println(result_signal1,DEC);
  Serial.println(result_signal2,DEC);
  Serial.println(result_signal3,DEC);
}

void periodic_task2() {
  if (Serial.available() > 0) 
  {
    incoming_byte = Serial.read();
    result_signal1=thread_signal(signaler); //BANG
    delay(50);
    result_signal2=thread_signal(signaler); //BANG
    delay(50);
    result_signal3=thread_signal(signaler); //BANG
    //Serial.print("[T2] Resultado: ");
    //Serial.println("");
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

  Serial.print("Simple example with 1 sporadic task and another periodic \n\n");  
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


  THREAD* th1=sporadic_thread_create(&sporadic_task1, NULL, NULL, 200, 30, 0, 100000, 100000, 10000000, (uint8_t) (SIMPLERTFRAMEWORK_OVER_IGNORE_POLICY|SIMPLERTFRAMEWORK_SAVE_POLICY), 0, &error_handler_task1); 
  signaler=th1; 
  THREAD* th2=periodic_thread_create(&periodic_task2, NULL, NULL, 200, 40, 0, 1000000, 1000000, 10000000, &error_handler_task2); 
  threads_subsystem_start();


}



