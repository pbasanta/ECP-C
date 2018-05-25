

#include <rtthreads.h>
#include <rtthreads_internals.h>

void setup() 
{
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);     //LED
}

void print_stack(THREAD* th){
  
  
  Serial.print("STACK POINTER--------: ");
  Serial.print(((unsigned long)(th->sp)),HEX);
  
  for(int i=0; i<th->stack_size;i++)
  {
    if(i%20==0)
    {
    Serial.println("");
    Serial.print(((unsigned long)(th->stack+th->stack_size-i-1)),HEX);
    Serial.print(":\t");
    }
    Serial.print(th->stack[th->stack_size-i-1],HEX);
    Serial.print("\t");
  }
  
}

/*
*/
void periodic_task1();
void periodic_task2();
void error_handler_task1(int erno);
void error_handler_task2(int erno);
THREAD* th2;

void periodic_pattern(); //Periodic pattern de la implementacion


void print_debug_symbols()
{
    Serial.print("\n[--] ApplicationDefined INFO ----------------------------------------\n");
    Serial.print("print_debug_symbols= 0x\t");              
    Serial.println(((long int)&print_debug_symbols),HEX);
    Serial.print("periodic_task1= 0x\t");              
    Serial.println(((long int)&periodic_task1),HEX);
    Serial.print("error_handler_task1= 0x\t");              
    Serial.println(((long int)&error_handler_task1),HEX);
    Serial.print("error_handler_task1= 0x\t");              
    Serial.println(((long int)&error_handler_task2),HEX);
    Serial.print("periodic_pattern= 0x\t");              
    Serial.println(((long int)&periodic_pattern),HEX);

}


/*
*/
void periodic_task1() 
{
  THREAD* th=thread_internals_threadlist();
  static int counter=0;
   /*Serial.print("[T1] THREAD 1------\n  current time(us): \t \t");  
  Serial.print(thread_get_time());  
  Serial.print("\n   and error(us):\t");  
  Serial.println(thread_get_error_in_time());*/
  
  if(counter==0)
  {
    thread_ATS_signal(th2);
    counter++;
  }
  
  print_debug_symbols();
  
  
  while(th!=NULL)
  {
    Serial.print("\n[--] THREAD INFO ----------------------------------------\n");
    Serial.print("uint8_t * sp= 0x\t");               /* thread stack pointer */
    Serial.println((long int)th->sp,HEX);
    Serial.print("uint8_t * stack= 0x");            /* start of thread stack */
    Serial.println((long int)th->stack,HEX);
    Serial.print("uint8_t   priority= ");
    Serial.println((long int)th->priority,DEC);
    Serial.print("uint8_t   block_pattern= 0x");
    Serial.println((long int)th->block_pattern,HEX);
    Serial.print("uint8_t   status= 0x");
    Serial.println((long int)th->status,HEX);
    Serial.print("uint16_t  stack_size= ");
    Serial.println((int)th->stack_size,DEC);    
    Serial.print("uint32_t  ticks_to_wait= ");
    Serial.println((long int)th->ticks_to_wait,DEC);    
    Serial.print("uint8_t   carry_ticks_to_wait= ");
    Serial.println((long int)th->carry_ticks_to_wait,DEC);      
    Serial.print("uint32_t  min_period_ticks = ");
    Serial.println((long int)th->min_period_ticks,DEC);      
    Serial.print("uint32_t  max_deadline_ticks = ");
    Serial.println((long int)th->max_deadline_ticks,DEC);      
    Serial.print("uint32_t  max_cost_ticks = ");
    Serial.println((long int)th->max_cost_ticks,DEC);      
    Serial.print("uint32_t  current_cost_ticks = ");
    Serial.println((long int)th->current_cost_ticks,DEC);      
    Serial.print("uint32_t  offset_ticks= ");
    Serial.println((long int)th->offset_ticks,DEC);      
    Serial.print("uint8_t   mit_policy= 0x");
    Serial.println((long int)th->offset_ticks,HEX);      
    Serial.print("uint32_t  queue_length_max");
    Serial.println((long int)th->queue_length_max,DEC);      
    Serial.print("uint32_t  queue_pending");
    Serial.println((long int)th->queue_pending,DEC);      
    Serial.print("uint32_t  queue_last_input");
    Serial.println((long int)th->queue_last_input,DEC); 
    Serial.print("void      (*executor)()= 0x"); 
    Serial.println((long int)th->executor,HEX);    
    Serial.print("void      *params_and_data= 0x"); 
    Serial.println((long int)th->params_and_data,HEX);    
    Serial.print("void      (*error_handler)(int errno)= 0x"); 
    Serial.println((long int)th->error_handler,HEX);    
    Serial.print("void      *parm = 0x"); 
    Serial.println((long int)th->parm,HEX);
    Serial.print("uint8_t           ats_active=DEC");
    Serial.println((long int)th->ats_active,DEC);
    Serial.print("uint8_t           ats_pending=DEC"); 
    Serial.println((long int)th->ats_pending,DEC);
    Serial.print("uint8_t           ats_max_pending=DEC"); 
    Serial.println((long int)th->ats_max_pending,DEC);
    Serial.print("void      th->aux1 = 0x"); 
    Serial.println((long int)th->aux1,HEX);
    Serial.print("void      th->aux2 = 0x"); 
    Serial.println((long int)th->aux2,HEX);    
    
    print_stack(th);
    th=th->runq_next; 
          
  }
  
  
}

void error_handler_task1(int erno)
{
  Serial.print("[T1] Error------\t");  
  while(1){
  }
}

void periodic_task2() 
{
thread_ATS_enable();
}

void error_handler_task2(int erno)
{
  while(1){
  }
}


void loop() { 
  
  Serial.print("Simple example\n  showing clock info + deviation \n");  
  delay(1000); 
  /*
   The tasks accesses to the information
  */
  THREAD* th1=periodic_thread_create(&periodic_task1, NULL, NULL, 200, 30, 100000, 10000000, 10000000, 50000000, NULL); 
  th2=periodic_thread_create(&periodic_task2, NULL, NULL, 101, 20, 0, 10000000, 10000000, 5000000, &error_handler_task2); 
  threads_subsystem_start();


}
