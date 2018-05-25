

#include <rtthreads.h>

void setup() 
{
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);     //LED
}

THREAD* th1=NULL;
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
  Serial.print("\n   auxt:\t");  
  Serial.println(aux_t);
  
  aux=0;
  

  thread_ATS_enable();
  aux=1;
  //thread_ATS_disable();
  thread_ATS_signal(th1);
  thread_ATS_signal(th1);
  aux=2;
  thread_ATS_disable();
  aux=3;

}

void error_handler_task1(int what)
{
  //Serial.print("[T1] EATS------\t"); 

    handler++;
    aux_t=aux;
  /*while (1)
  {
  }
 */
 
}



void loop() { 
  
  Serial.print("Simple example\n  showing clock info + deviation \n");  
  delay(1000); 
  /*
   The tasks accesses to the information
  */
  th1=periodic_thread_create(&periodic_task1, NULL, NULL, 200, 30, 0, 1000000, 1000000, 500000, &error_handler_task1); 
  threads_subsystem_start();


}
