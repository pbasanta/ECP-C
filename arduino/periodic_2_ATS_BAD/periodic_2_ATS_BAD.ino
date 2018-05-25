

#include <rtthreads.h>

void setup() 
{
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);     //LED
}

THREAD* th1=NULL;
THREAD* th2=NULL;
int handler=0;
int aux=0;
int aux_t=0;
void periodic_task1() 
{
  static uint32_t counter=0;
  counter++;
  Serial.print("[T1] THREAD 1------\n  current time(us): \t \t");  
  Serial.print(thread_get_time());  
  Serial.print("\n   error handler:\t");  
  Serial.println(handler);
  Serial.print("\n   auxt2:\t");  
  Serial.println(aux_t);
  
 /* aux=1;
  thread_ATS_enable();
  aux=2;
  //thread_ATS_disable();
  //thread_ATS_disable();
  aux=3;
   //thread_ATS_disable();*/
  thread_ATS_signal(th2);
}

void error_handler_task1(int what)
{

}


void periodic_task2() 
{

  thread_ATS_enable();
  handler++;
  handler++;
  //thread_ATS_disable();

}

void error_handler_task2(int what)
{
   //if(what==SIMPLERTFRAMEWORK_ATS_SIGNAL)
    //while(1){
          aux_t++;
    //}

  
}
void loop() { 
  
  Serial.print("Simple example\n  showing clock info + deviation \n");  
  delay(1000); 
  /*
   The tasks accesses to the information
  */
  th1=periodic_thread_create(&periodic_task1, NULL, NULL, 200, 20, 100000, 1000000, 1000000, 500000, &error_handler_task1); 
  th2=periodic_thread_create(&periodic_task2, NULL, NULL, 200, 30, 100, 1000000, 1000000, 500000, &error_handler_task2); 
  threads_subsystem_start();


}
