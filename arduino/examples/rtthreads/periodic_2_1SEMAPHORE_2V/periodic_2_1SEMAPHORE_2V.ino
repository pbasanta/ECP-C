

#include <rtthreads.h>

void setup() 
{
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);     //LED
}

THREAD* th1=NULL;
THREAD* th2=NULL;
SEMAPHORE*   sem1=NULL;




int handler=0;
int aux=0;
int aux_t=0;
uint32_t qdata=NULL;

void periodic_task1() 
{
  static uint32_t counter=0;
  counter++;
  Serial.print("\n[T1] THREAD 1------\n  current time(us): \t \t");  
  Serial.print(thread_get_time());  
  Serial.print("\n   DATA:\t");  
  Serial.print(handler);  
  thread_semaphore_P(sem1);
  
 
}

void error_handler_task1(int what)
{

}


void periodic_task2() 
{

  //thread_ATS_enable();
  handler++;
  thread_semaphore_V(sem1);

 // qgen=qgen+2;
  //thread_ATS_disable();

}

void error_handler_task2(int what)
{
   //if(what==SIMPLERTFRAMEWORK_ATS_SIGNAL)
  
    aux_t=aux;
  
}
void loop() { 
  
  Serial.print("Simple example\n  showing clock info + deviation \n");  
  delay(1000); 
  /*
   The tasks accesses to the information
  */
  th1=periodic_thread_create(&periodic_task1, NULL, NULL, 200, 30, 0, 1000000, 1000000, 500000, &error_handler_task1); 
  sem1= thread_semaphore_create(0); //Semaphore initialized
  th2=periodic_thread_create(&periodic_task2, NULL, NULL, 200, 30, 100000, 1000000, 1000000, 500000, &error_handler_task2); 
  
  threads_subsystem_start();


}


