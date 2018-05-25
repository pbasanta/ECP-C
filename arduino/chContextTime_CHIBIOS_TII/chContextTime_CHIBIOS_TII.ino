// Connect a scope to pin 13
// Measure difference in time between first pulse with no context switch
// and second pulse started in thread 2 and ended in thread 1.

#include <ChibiOS_AVR.h>

#include <util/atomic.h>

#include <stdio.h>
#include <stdlib.h>

#include <util/delay.h>

const uint8_t LED_PIN = 13;
static long time_2=0;

static long time_1=0;



// Semaphore to trigger context switch
static Semaphore sem;

static long cost_offset=0;
static long cost_c1=0;
static long cost_wcet1=0;
static long period_p1=0;
static long deadline_d1=0;
static long ats_pending_1=0;
static long ats_active_1=0;
static void (*executor_1)(void)=NULL;
static void* params_and_data_1=NULL;
static void (*error_handler_1)(int errno)=NULL;
static long block_pattern1=0;
/*
offset_ticks=offset_us/us_per_tick;
  base_th->min_period_ticks=period_us/us_per_tick;
  base_th->max_cost_ticks=max_cost_us/us_per_tick;
  base_th->max_deadline_ticks=max_deadline_us/us_per_tick;
  base_th->block_pattern=0x00;

  */


static long* p_cost_c1=&cost_c1;
static long* p_cost_wcet1=&cost_wcet1;
static long* p_period_p1=&period_p1;
static long* p_deadline_d1=&deadline_d1;
static long* p_ats_pending_1=&ats_pending_1;
static long* p_ats_active_1=&ats_active_1;


static long cost_c2=0;
static long cost_wcet2=0;
static long period_p2=0;
static long deadline_d2=0;
static long ats_pending_2=0;
static long ats_active_2=0;

static long* p_cost_c2=&cost_c2;
static long* p_cost_wcet2=&cost_wcet2;
static long* p_period_p2=&period_p2;
static long* p_deadline_d2=&deadline_d2;
static long* p_ats_pending_2=&ats_pending_2;
static long* p_ats_active_2=&ats_active_2;

static long current_thread=0;
static long last_time=micros();
static void do_overun(){
  deadline_d1=0;
  cost_c1=0;
}

static void do_switch(){
  
   long aux_cost_add= micros();
   //
   *p_cost_c2+= aux_cost_add-last_time;
   last_time=aux_cost_add;
   //Checking for overruns
   
   for(int i=0; i<2;i++){
     if(deadline_d2>aux_cost_add)
       do_overun();
     if(cost_wcet2>aux_cost_add);
        do_overun();
     current_thread=1;   
     }
   if(*p_deadline_d1>aux_cost_add)
     do_overun();
   if(*p_cost_wcet1>aux_cost_add)
     do_overun();
     
   if((*p_ats_pending_1>0)&&(*p_ats_active_1==0))
     do_overun();
   
  
}


//------------------------------------------------------------------------------
// thread 1 - high priority thread to set pin low
// 64 byte stack beyond task switch and interrupt needs
static WORKING_AREA(waThread1, 104);

static msg_t Thread1(void *arg) {

  while (TRUE) {
    chSemWait(&sem);
    time_2=micros();
    digitalWrite(LED_PIN, LOW);
  }
  return 0;
}
//------------------------------------------------------------------------------
// thread 2 - lower priority thread to toggle LED and trigger thread 1
// 64 byte stack beyond task switch and interrupt needs
static WORKING_AREA(waThread2, 104);


static msg_t Thread2(void *arg) {
  pinMode(LED_PIN, OUTPUT);
  while (TRUE) {
    // first pulse to get time with no context switch
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_PIN, LOW);
    // start second pulse
    digitalWrite(LED_PIN, HIGH);
    // trigger context switch for task that ends pulse
    time_1=micros();
    do_switch();
    chSemSignal(&sem);
    // sleep until next tick (1024 microseconds tick on Arduino)
    Serial.print("TIME: \n");
    Serial.print(time_2-time_1);
    Serial.print("\n");
    
    while(1==1){
    }
    chThdSleep(10);
  }
  return 0;
}


byte stack[300];

//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
    
  chBegin(chSetup);
  while (1) {}
}



void  do_ats_1(){
}
  
  
//------------------------------------------------------------------------------
void chSetup() {
  // initialize semaphore
  int aux=1023;
  aux=aux/10;

  chSemInit(&sem, 0);
  
  long time_1=micros();
  chThdYield();
  long   time_2=micros();
  Serial.print("yield: ");
  Serial.print(time_2-time_1);
  Serial.print("\n");
  
  
   time_1=micros();
  micros();
  time_2=micros();
  Serial.print("error: ");
  Serial.print(time_2-time_1);
  Serial.print("\n");
  
  //Semaphore
  Semaphore* sem1;
  time_1=micros();
  sem1= (Semaphore*)malloc(sizeof(Semaphore));
  chSemInit(sem1, 1);
  time_2=micros();
  Serial.print("semaphore-alloc: ");
  Serial.print(time_2-time_1);
  Serial.print(" us and bytes:");
  Serial.print(sizeof(Semaphore));
  Serial.print("\n");

  Semaphore sem2;
  chSemInit(&sem2, 1);  
  
  time_1=micros();
  chSemWait(&sem2);
  time_2=micros();
  Serial.print("semaphore-V: ");
  Serial.print(time_2-time_1);
  Serial.print("\n");
  
  time_1=micros();
  chSemSignal(&sem2);
  time_2=micros();
  Serial.print("semaphore-P: ");
  Serial.print(time_2-time_1);
  Serial.print("\n");

  time_1=micros();
  chSemReset(sem1,0);;
  free(sem1);
  time_2=micros();
  Serial.print("semaphore-dealloc: ");
  Serial.print(time_2-time_1);
  Serial.print("\n");

  void* data;  
  time_1=micros();
  data=malloc(10);
  time_2=micros();
  Serial.print("tmalloc (10): ");
  Serial.print(time_2-time_1);
  Serial.print("\n");
  
  time_1=micros();
  free(data);
  time_2=micros();
  Serial.print("free (10): ");
  Serial.print(time_2-time_1);
  Serial.print("\n");
  
  ///Queue methods

  GenericQueue *q1=NULL;
  time_1=micros();
 
  q1=(GenericQueue*) malloc(sizeof(GenericQueue));
  uint8_t* buffer = NULL;
  buffer= (uint8_t*) malloc(10*sizeof(void*));
  //void chIQInit(InputQueue *iqp, uint8_t *bp, size_t size, qnotify_t infy);
  chIQInit(q1,buffer,10*sizeof(void*), NULL);
   time_2=micros();
   
  Serial.print("queue (10): Create ");
  Serial.print(time_2-time_1);
  Serial.print(" us and bytes: ");
  Serial.print(sizeof(GenericQueue));
  Serial.print(" + ");
  Serial.print((long)sizeof(void*));
  Serial.print(" per element\n");
  
  
  time_1=micros();
  //Writing somehting a  point with 16 elements
  chIQPutI(q1,((uint8_t)((long)q1>>8)));
  chIQPutI(q1,((uint8_t)((long)q1)&&0xFF));
  time_2=micros();
  Serial.print("queue (10): setElement ");
  Serial.print(time_2-time_1);
  Serial.print(" per element\n");
  
  
  time_1=micros();
  //Writing somehting a  point with 16 elements
  chIQPutI(q1,((uint8_t)((long)q1>>8)));
  chIQPutI(q1,((uint8_t)((long)q1)&&0xFF));
  time_2=micros();
  Serial.print("queue (10): setElement ");
  Serial.print(time_2-time_1);
  Serial.print(" per element\n");
  
  
  time_1=micros();
  void* ptr=NULL;
  ptr= (void*) ((byte)chOQGetI(q1)<<8+ (byte)chOQGetI(q1));
  time_2=micros();

  Serial.print("queue (10): getElement ");
  Serial.print(time_2-time_1);
  Serial.print(" per element\n");

  time_1=micros();
  free(q1);
  free(buffer);
  time_2=micros();
  Serial.print("queue(10):free");
  Serial.print(time_2-time_1);
  Serial.print("\n");
  
  
    
  int mem_at=0;
  time_1=micros();
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    mem_at++;
    mem_at--;
  }
  
  time_2=micros();
  Serial.print("ATOMIC_BLOCK");
  Serial.print(time_2-time_1);
  Serial.print("\n");
  
  
          
 
  time_1=micros();
  _delay_us(10.0D);
  time_2=micros();
  Serial.print("sleep: ");
  Serial.print(time_2-time_1);
  Serial.print("\n");
  

  
  time_1=micros();
  long taux_1 =chTimeNow();
  delay(10);
  time_2=micros();
  long taux_2=  chTimeNow();
  Serial.print("sleep: ");
  Serial.print(time_2-time_1);
  Serial.print(". Ticks en un sleep de 10 ms");
  Serial.print(taux_2-taux_1);
  Serial.print("\n");

  //ATS enable
  time_1=micros();
  *p_ats_active_1=1; //+1
  if(p_ats_pending_1!=0){
    do_ats_1();
  }
  time_2=micros();
  Serial.print("ATS-1: ");
  Serial.println(time_2-time_1);
 

  //ATS signal
  time_1=micros();
  if(p_ats_active_1!=0){
    do_ats_1();
  }
  time_2=micros();
  Serial.print("ATS-signal: ");
  Serial.println(time_2-time_1);
  //ATS disable
  time_1=micros();  
  p_ats_active_1=0;
  time_2=micros();
  Serial.print("ATS-disable: ");
  Serial.println(time_2-time_1);
  //
  

  // start high priority thread
  time_1=micros();
  Thread* th1=chThdCreateFromHeap(NULL,THD_WA_SIZE(228),NORMALPRIO+2,Thread1,NULL);
    //chThdCreateStatic(waThread1, sizeof(waThread1),
    //NORMALPRIO+2, Thread1, NULL);
  *p_cost_c1=102005/aux; //+2
  *p_cost_wcet1=10025/aux; //+2
  *p_period_p1=10025/aux; //+2
  *p_deadline_d1=10025/aux; //+2
  *p_ats_pending_1=0; //+1
  *p_ats_active_1=0; //+1

  //memset(stack, 0x99, 200); //Thread internal inicialization

  time_2=micros();
  Serial.print("THREAD CREATION: ");
  Serial.print(time_2-time_1);
  Serial.print("\n size: ");
  Serial.print(sizeof(Thread));
  
  Serial.print("\n");
  


  // start lower priority thread
  chThdCreateStatic(waThread2, sizeof(waThread2),
    NORMALPRIO+1, Thread2, NULL);
}
//------------------------------------------------------------------------------
// idle task not used
void loop() {}
