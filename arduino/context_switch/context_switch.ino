/*
  Context_switch tutorial, partially taken from FreeRTOS
 */
#include <inttypes.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define ARDUINO_DEBUG
#undef  ARDUINO_DEBUG

#define RTARDUINO_STACK_SIZE 200
#define RTARDUINO_MAX_PRIO   64
#define RTARDUINO_NORM_PRIO   2
#define RTARDUINO_MIN_PRIO    0

#define RTARDUINO_BLOCKED 0x02
#define RTARDUINO_ACTIVE 0x01
#define RTARDUINO_SLEPT 0x00

#define RTARDUINO_BLOCKED 0x02
#define RTARDUINO_ACTIVE 0x01
#define RTARDUINO_SLEPT 0x00

#define RTARDUINO_COST_OVERUN      0x03
#define RTARDUINO_MEMORY_EXHAUSTED 0x02
#define RTARDUINO_DEADLINE_MISS    0x01
#define RTARDUINO_PERIOD_MISS      0x00

typedef struct thread_t {
  struct thread_t * runq_prev;    /* previous thread in run queue */
  struct thread_t * runq_next;    /* next thread in run queue */

  uint8_t         * sp;           /* thread stack pointer */
  uint8_t         * stack;        /* start of thread stack */
  uint8_t           priority;     /* thread priority */
  uint8_t           flags;       
  uint16_t          stack_size;   /* thread stack size */
  uint32_t          ticks_to_wait;  /* sleep timer */
  uint8_t           carry_ticks_to_wait;  /* sleep timer carry*/
  /*Real-time parameters*/
  uint32_t          min_period_ticks;
  uint32_t          max_deadline_ticks;
  uint32_t          max_cost_ticks;
  uint32_t          current_cost_ticks;
  /*Executor & error handler*/
  void (*executor)();
  void (*error_handler)(int errno);
  void            * parm;         /* parameter pointer */
} 
THREAD;



/*
 Periodic threads
*/
THREAD * periodic_thread_create(void (*entry)(void), 
uint8_t * stack, uint16_t stack_size, 
uint8_t priority,  
uint32_t period_us, 
uint32_t max_deadline_us,
uint32_t  max_cost_us, 
void (*error_handler)(int errno));

/*
 Sporadic threads
*/
THREAD * sporadic_thread_create(void (*entry)(void), 
uint8_t * stack, uint16_t stack_size, 
uint8_t priority,  
uint32_t min_period_us, 
uint32_t max_deadline_us,
uint32_t max_cost_us, 
void (*error_handler)(int errno));

/*
 Thread base class
*/
THREAD * thread_create(void (*entry)(void), uint8_t * stack, uint16_t stack_size, uint8_t priority, void * parm);

void thread_yield();
void thread_block(THREAD * th);
void thread_unblock(THREAD * th);
void thread_usleep(uint32_t microseconds);
void threads_subsystem_init();
/** */

void tick_sleep(uint32_t ticks);
//void thread_sleep(uint16_t ms);



//void thread_stop(THREAD * th);

//void thread_kill(THREAD * th);
//void thread_yield(void);

/******************************************/


static uint32_t tick_counter_us=0;
static uint32_t tick_counter_previous_us=0;
static uint8_t  carry_tick_counter=0;

static uint32_t us_per_tick=128;
static uint32_t yield=0;
static uint32_t yield_max=1; //1,024 ms*yield_max
static THREAD*  thread_list= NULL;
static THREAD*  current_thread=NULL;
static THREAD*  thread_main_inf=NULL;

void periodic_pattern()
{
  THREAD* this_thread=current_thread;
  uint32_t thread_counter_us=tick_counter_us;
  uint32_t previous_thread_counter_us=thread_counter_us;
  uint8_t  carry_thread_counter=0;
  while(1){
    this_thread->current_cost_ticks=0; //start in zero
    this_thread->executor(); //Upcalls the cycle
    
    uint32_t ticks_aux=tick_counter_us;
    uint8_t carry_ticks_aux;
    if(ticks_aux<thread_counter_us){ //Overflow_test
      carry_ticks_aux=1 ;
    }
    else
    {
      carry_ticks_aux=0 ;
    }
    
    //Error handler process 
    if(this_thread->error_handler!=NULL){ 
        /* if((ticks_aux-thread_counter_us) > this_thread->min_period_ticks) //Carry safe operation
           {
            //PERIOD VIOLATION
            this_thread->error_handler(RTARDUINO_PERIOD_MISS);
           }*/
         if((ticks_aux-thread_counter_us) > this_thread->max_deadline_ticks) //Carry safe operation
           {
            //DEADLINE VIOLATION
            this_thread->error_handler(RTARDUINO_DEADLINE_MISS);
           }  
         if((this_thread->current_cost_ticks) > this_thread->max_cost_ticks) //Carry safe operation
           {
            //COST VIOLATION
            this_thread->error_handler(RTARDUINO_COST_OVERUN);
           }  
             
     } 
    //Calculating the next stage  
    previous_thread_counter_us=thread_counter_us;
    thread_counter_us+=this_thread->min_period_ticks;
    /*if(previous_thread_counter_us>thread_counter_us){
        carry_thread_counter=1;
    }
    else
    {
       carry_thread_counter=0;
    }*/
    
     tick_sleep(thread_counter_us-ticks_aux);  //It is safe
        
  }
}

/**
 * Periodic thread creation
 */
THREAD * periodic_thread_create(void (*entry)(void), 
uint8_t * stack, uint16_t stack_size, 
uint8_t priority,  
uint32_t period_us, 
uint32_t max_deadline_us,
uint32_t  max_cost_us, 
void (*error_handler)(int errno))
{
  //0.-Thread creation
  THREAD * base_th= thread_create(&periodic_pattern, stack, stack_size, priority, NULL);
  //1.-Periodic thread initialization
  base_th->executor=entry;
  base_th->error_handler=error_handler;
  base_th->min_period_ticks=period_us/us_per_tick;
  base_th->max_cost_ticks=max_cost_us/us_per_tick;
  base_th->max_deadline_ticks=max_deadline_us/us_per_tick;
}


/* ********* Implementation ************* */

//void do_switch(uint8_t *sp)__attribute__(("naked"));

/* ********* Implementation ************* */
//static THREAD* thread_list=NULL;

/* ********* Implementation ************* */

/* THREAD * thread_create(void (*entry)(void), uint8_t priority, void * parm)
 {
 return thread_create(entry, NULL, RTARDUINO_STACK_SIZE,  priority, parm);
 }
 
 THREAD * thread_create(void (*entry)(void),uint16_t stack_size, uint8_t priority, void * parm)
 {
 return thread_create(entry, NULL, stack_size,  priority, parm); 
 }
 
 */




THREAD * thread_create(void (*entry)(void), uint8_t * stack, uint16_t stack_size, uint8_t priority, void * parm)
{
  cli();
  THREAD* th= (THREAD*) malloc(sizeof(THREAD)); 
  sei();
  if (th==NULL)  
    return NULL;
  th->priority=priority;
  th->parm=parm; 
  th->flags=RTARDUINO_ACTIVE;

  if(stack!=NULL){
    th->stack=stack;
    th->stack_size=stack_size;
  }
  else{
    cli();
    th->stack= (uint8_t*) malloc(stack_size); 
    sei(); 
    if (th->stack==NULL) 
    { 
      free(th);
      return NULL;
    }

    th->stack_size=stack_size;
  }
  //Memory initialization


  memset(th->stack, 0x99, th->stack_size);
  th->sp=th->stack+th->stack_size-1; 
  *th->sp-- = ((uint16_t)entry) & 0xff;
  *th->sp-- = (((uint16_t)entry) >> 8) & 0xff;
  for(int i=0;i<33;i++){
    *th->sp-- =0x00;
  }


  /* Serial.println("<check>------");  
   Serial.print(((int)(uint8_t*)th->sp) & 0xff);  
   Serial.print("L");  
   Serial.print(((int)(uint8_t*)th->sp>>8) & 0xff); 
   Serial.println("H");   */

  /* Serial.println("pointer tu hola -----2");  
   Serial.print(((int)*(th->sp+2)) & 0xff);  
   Serial.print(".");  
   Serial.print(((int)*(th->sp+1)) & 0xff);  ; 
   Serial.println(".");
   */

  //IF s==null
  if(thread_list==NULL){
    cli();
    THREAD* thmain=(THREAD*) malloc(sizeof(THREAD)); 
    thread_main_inf=thmain;
    sei();
    thmain->priority=RTARDUINO_NORM_PRIO;
    thmain->parm=NULL;
    th->flags=RTARDUINO_ACTIVE;
    thread_list=thmain;
    thmain->runq_next=NULL;
    thmain->runq_prev=NULL;
    current_thread=thmain;
    thread_start(thmain);
  }
  th->runq_next=thread_list;
  thread_list->runq_prev=th;
  thread_list=th;
  return th;
}

static THREAD* sleeping_threads=NULL;
static THREAD* next_thread=NULL;
volatile uint8_t pxCurrentTCB_aux[2]={
  0,0};
volatile uint8_t* pxCurrentTCB=pxCurrentTCB_aux;
static int first_time =1;
void change_high() asm("change_high");
void change_high()
{
  //pxCurrentTCB
  // sp = sp + 2;

#ifdef ARDUINO_DEBUG
  Serial.println("SP inv");  
  Serial.print(((int) pxCurrentTCB_aux[0])& 0xff);  
  Serial.print(".");  
  Serial.print(((int) pxCurrentTCB_aux[1])& 0xff);  
  Serial.println(".");  
  delay(100);
#endif

  ///if (first_time==1){
  //Save old PCB 
  int aux=(int)pxCurrentTCB_aux[1]&0xFF;
  aux=aux<<8;
  aux=aux|((int)pxCurrentTCB_aux[0]&0xFF);
  current_thread->sp=(uint8_t*)aux;
  //Put new PCTB

  if(current_thread!=next_thread) {
    pxCurrentTCB_aux[0]=((int)next_thread->sp)&0xff; 
    pxCurrentTCB_aux[1]=((int)next_thread->sp>>8)&0xff;
  }
  current_thread=next_thread;

  // first_time=0;
  //}

#ifdef ARDUINO_DEBUG
  Serial.println("SP inv2");  
  Serial.print(((int) pxCurrentTCB_aux[0])& 0xff);  
  Serial.print(".");  
  Serial.print(((int) pxCurrentTCB_aux[1])& 0xff);  
  Serial.println(".");
  delay(100);
#endif
  //sp=next_thread->sp ;


  /*Serial.println("SP pre-3");  
   Serial.print(((int)(uint8_t*)next_thread->sp) & 0xff);  
   Serial.print(".");  
   Serial.print(((int)(uint8_t*)next_thread->sp>>8) & 0xff); 
   Serial.println(".");   
   */

}


void do_switch(){  
  // asm("cli");
  asm volatile("push r0");
  asm volatile("push r1");
  asm volatile("push r2");
  asm volatile("push r3");  
  asm volatile("push r4");  
  asm volatile("push r5");
  asm volatile("push r6");
  asm volatile("push r7");
  asm volatile("push r8");
  asm volatile("push r9");  
  asm volatile("push r10");
  asm volatile("push r11");
  asm volatile("push r12");
  asm volatile("push r13");  
  asm volatile("push r14");  
  asm volatile("push r15");
  asm volatile("push r16");
  asm volatile("push r17");
  asm volatile("push r18");
  asm volatile("push r19");  
  asm volatile("push r20");
  asm volatile("push r21");
  asm volatile("push r22");
  asm volatile("push r23");  
  asm volatile("push r24");  
  asm volatile("push r25");
  asm volatile("push r26");
  asm volatile("push r27");
  asm volatile("push r28");
  asm volatile("push r29");  
  asm volatile("push r30");
  asm volatile("push r31");
  asm volatile("in r0, 0x3f");
  asm volatile("push r0");

  asm volatile ("lds	r26, pxCurrentTCB");	
  asm volatile ("lds	r27, pxCurrentTCB + 1");
  asm volatile ("in	r0, 0x3d");
  asm volatile ("st     x+, r0");
  asm volatile ("in	r0, 0x3e");	
  asm volatile ("st	x+, r0");		

  /*asm volatile( "in r24, 0x3e"); //sp2h
   asm volatile( "in r25, 0x3d ");//sp2l
   asm volatile("push r25");
   asm volatile("push r24");
   asm volatile("st context_sp_hp, r24");
   asm volatile("st context_sp_lp, r24");*/


  asm volatile("call change_high");

  //change stacks
  //asm volatile("pop r25");
  //asm volatile("pop r24");
  //asm volatile( "out 0x3e, r24"); //sp2h
  //asm volatile( "out 0x3d, r25");  //sp2l    
  //Aqui deberan de estar los stacks ok.
  asm volatile ("lds	r26, pxCurrentTCB");	
  asm volatile ("lds	r27, pxCurrentTCB + 1");
  asm volatile ("ld     r0, x+");
  asm volatile ("out	0x3d, 0");
  asm volatile ("ld     r0, x+");
  asm volatile ("out	0x3e, r0");

  asm volatile("pop r0");
  asm volatile("out 0x3f, r0"); //Registro de estado
  asm volatile("pop r31");
  asm volatile("pop r30");
  asm volatile("pop r29");  
  asm volatile("pop r28");  
  asm volatile("pop r27");
  asm volatile("pop r26");
  asm volatile("pop r25");
  asm volatile("pop r24");
  asm volatile("pop r23");  
  asm volatile("pop r22");
  asm volatile("pop r21");
  asm volatile("pop r20");
  asm volatile("pop r19");  
  asm volatile("pop r18");  
  asm volatile("pop r17");
  asm volatile("pop r16");
  asm volatile("pop r15");
  asm volatile("pop r14");
  asm volatile("pop r13");  
  asm volatile("pop r12");
  asm volatile("pop r11");
  asm volatile("pop r10");
  asm volatile("pop r9");  
  asm volatile("pop r8");  
  asm volatile("pop r7");
  asm volatile("pop r6");
  asm volatile("pop r5");
  asm volatile("pop r4");
  asm volatile("pop r3");  
  asm volatile("pop r2");
  asm volatile("pop r1");
  asm volatile("pop r0");
  //  asm("reti");
}





void thread_start(struct thread_t * th){
  // uint8_t *ret=(th->sp+2);
  next_thread=th;
  do_switch();
}  
/*
* It performs the yield operation
*/
void thread_yield(int i){
  cli();
  THREAD * th=NULL;
  THREAD * th_candidate=thread_main_inf;
  if(thread_list==NULL) 
    return;
  th=current_thread;
  ///do{
  do
  {
    next_thread=th->runq_next;
    if(next_thread==NULL)
      next_thread=thread_list;
    th=next_thread;

    //Check if threads have to the awaken
    //Overflow taken into consideration
     if(th->flags==RTARDUINO_SLEPT){
      if(th->carry_ticks_to_wait==1)
       {
         if(carry_tick_counter==1)
           {
           th->carry_ticks_to_wait=0;
           }
       }
      if((th->carry_ticks_to_wait==0)&&(th->ticks_to_wait<tick_counter_us))
       {
          th->flags=RTARDUINO_ACTIVE;
        }
        
    }

    if((th->flags==RTARDUINO_ACTIVE)&&(th->priority>=th_candidate->priority)&&(next_thread!=current_thread)){
      th_candidate=th;
    } 
  } 
  while(next_thread!=current_thread); //The main is never blocked

    //  }while((current_thread!=next_thread)&&(next_thread->flags==RTARDUINO_BLOCKED));
  current_thread->current_cost_ticks+=(tick_counter_us-tick_counter_previous_us); //No problems with overflow
  
  next_thread=th_candidate;
  if(current_thread!=next_thread)
    thread_start(next_thread);

  sei();
}  
void thread_yield(){
  thread_yield(0);
}


void thread_block()
{
  cli();
  if(current_thread->flags==RTARDUINO_ACTIVE) {
    current_thread->flags=RTARDUINO_BLOCKED;
    thread_yield();
  }
  sei();
}

void thread_unblock(struct thread_t * th)
{

  cli();
  if(th->flags==RTARDUINO_BLOCKED){ 
    th->flags= RTARDUINO_ACTIVE;
    thread_yield();
  }
  sei();
}

void thread_ms(uint32_t time_ms)
{

  //tick_sleep(time_us);
  //sleeping_threads=current_thread;
  //sei();
}

void thread_usleep(uint32_t time_us)
{
  cli();
  current_thread->flags=RTARDUINO_SLEPT;
  time_us=time_us/us_per_tick; //Just to perform correction; 
  tick_sleep(time_us);
  //sleeping_threads=current_thread;
  sei();
}

void tick_sleep(uint32_t ticks)
{
  cli();
  current_thread->flags=RTARDUINO_SLEPT;
  //ticks=10000; //time_us/us_per_tick; //Just to perform correction; 
  uint32_t ticks_aux=tick_counter_us;
  current_thread->ticks_to_wait=ticks+ticks_aux;
  if(ticks_aux>current_thread->ticks_to_wait)
    {
      current_thread->carry_ticks_to_wait=1;
    }
   else
    {
      current_thread->carry_ticks_to_wait=0;
    }
  //sleeping_threads=current_thread;
  thread_yield();
  sei();
}




/*****************************************************************
 * TIMER
 ******************************************************************/

ISR(TIMER2_OVF_vect) {
  // Interrupt routine.
  //cli();
  tick_counter_us+=1;
  yield++;
  if(yield==yield_max){
    yield=0;
    if(tick_counter_previous_us>tick_counter_us) //Detecting the overflow
      {  
        carry_tick_counter=1;
      }
    else
      { 
        carry_tick_counter=0;
      }
    thread_yield();
    tick_counter_previous_us=tick_counter_us;
  }
  //sei();
}


/*  0 - 0 - 0 => No Clock
 0 - 0 - 1 => No Prescaling  [0.016 ms]
 0 - 1 - 0 => Prescaler=8  [0.128 ms]
 0 - 1 - 1 => Prescaler=32  [0.512 ms]
 1 - 0 - 0  => Prescaler=64  [1.024 ms]
 1 - 0 - 1  => Prescaler=128  [2.048 ms]
 1 - 1 - 0  => Prescaler=256  [4.096 ms]
 1 - 1 - 1  => Prescaler=1024  [16.384 ms]
 */

void enable_timer2() {
  //  int prescale  = 1024; //1-1-1
  //  int ocr2aval  = 256;
  //  int iinterval = prescale * (ocr2aval+1) / (F_CPU / 1.0e6); //64 us
  //tick_counter_tick_us=1;

#ifdef ARDUINO_DEBUG
  Serial.print("Interval "); 
  //Serial.print(tick_counter_tick_us); 
  Serial.println("us ");   
  delay(1000);
#endif

  cli();
  TCCR2A = (1<<COM2A1) | (0<<COM2A0) | (0<<COM2B1)| (0<<COM2B0) | (0<<3) | (0<<2) | (0<<WGM21) | (0<<WGM20);  
  TCCR2B = (0<<FOC2A) | (0<<FOC2B) | (0<<5) | (0<<4) | (0<<WGM22) | (0<<CS22) | (1<<CS21) | (0<<CS20); //Configurado a 128 us
  //OCR2A = ocr2aval;
  TIMSK2 = (0<<OCIE2A)| (1<<TOIE2); //pBV2 //divide entre 

  //TCCR2A = (1 << WGM21);
  //TCCR2B = (1 << CS21);
  //TCCR2B = (1<<CS22)|(1<<CS21)|(1<<CS20);
  //TIMSK2 = (1 << OCIE2A); //Compare
  // OCR2A = ocr2aval;
  sei();
}

void threads_subsystem_init(){
  if (current_thread==NULL)
    return;
  enable_timer2();
  while(1==1){
    thread_yield();
  }
}

/************************************************************************/
/* USE CASE                                                             */
/************************************************************************/
void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);     //LED
}

void periodic_pins(){
  //Serial.print("[T1] THREAD 1: tick \t");  
  //Serial.println(tick_counter_us);  
  //delay(1000);
  // thread_block();

  static int swing=0;
  if(swing==0){
    digitalWrite(13, HIGH);   // set the LED on
    swing=1;
  }

  else{
    swing=0;
    //thread_usleep(1000000);    // 1segundo
    //thread_usleep(3000000);    // 1segundo
    digitalWrite(13, LOW);    // set the LED off
  }

  //thread_usleep(1000000);    // 1segundo
  //      delay(1000);              // wait for a second
  // thread_usleep(1000000);    // 1segundo
  // do_switch();
  //thread_yield();
}

void periodic_info() {
  //  while(1){
  //Serial.print("Hola desde el hilo 2 \n");  

  //  thread_unblock(th);
  Serial.print("[T2] THREAD 1: tick \t");  
  Serial.println(tick_counter_us);  
  //delay(1000);
  //thread_usleep(4000000);    // 1segundo
  //thread_unblock(th);
  //}
}

void printHola() {
  while(1){
    //Serial.print("[T1] THREAD 1: tick \t");  
    //Serial.println(tick_counter_us);  
    //delay(1000);
    thread_block();
    digitalWrite(13, HIGH);   // set the LED on
    //   delay(3000);              // wait for 3 seconds
    thread_usleep(1000000);    // 1segundo
    //thread_usleep(3000000);    // 1segundo
    digitalWrite(13, LOW);    // set the LED off
    //thread_usleep(1000000);    // 1segundo
    //      delay(1000);              // wait for a second
    thread_usleep(1000000);    // 1segundo
    // do_switch();
    //thread_yield();
  }
}

static   THREAD* th=NULL;
void printHola2() {
  while(1){
    //Serial.print("Hola desde el hilo 2 \n");  

    thread_unblock(th);
    Serial.print("[T2] THREAD 1: tick \t");  
    Serial.println(tick_counter_us);  
    //delay(1000);
    thread_usleep(4000000);    // 1segundo
    //thread_unblock(th);
  }
}

//static uint8_t stacks[3000]; 

void errfunction(int errno){
  while(1){
    //        cli();
    digitalWrite(13, HIGH);   // set the LED on
    //      sei();
  }
}
void loop() {  
  // do_switch();
  // asm("call do_switch");
  Serial.print("INI-\n\n");  
  delay(1000);
  //  enable_timer2();
  //  th= thread_create(&printHola, NULL, 300, 60,NULL);
  for(int i=40;i<41;i++)
  {
    //CODE, NULL, stacksize,priority, t, d, c, &errorhandler
    th= periodic_thread_create(&periodic_pins, NULL, 200, i, 100000, 100000, 0,&errfunction); //t=200 us
  }
  THREAD* th2= periodic_thread_create(&periodic_info, NULL, 200, 30, 100000, 1000000, 1000000,NULL); //t=30ms

  //THREAD* t2= thread_create(&printHola2, NULL, 300, 40,NULL);
  /* THREAD* th2= thread_create(&printHola2, NULL, 60, 4,NULL);
   THREAD* th3= thread_create(&printHola2, NULL, 60, 4,NULL);
   */
  Serial.println((int)th);  
  Serial.println((int)th->stack);  
  Serial.println((int)th->sp);  
  delay(100);
  /*Serial.println((int)th2);   
   Serial.println((int)th2->stack); 
   Serial.println((int)th3);  
   Serial.println((int)th3->stack); 
   THREAD* th2= thread_create(&printHola, &stacks[1000], 700, 4,NULL);
   
   */
  //thread_start(th);
  //enable_timer2();
  threads_subsystem_init(); //It blocks the main thread

  /* while(1){
   Serial.print("[M*] MAIN    : tick \t");    
   Serial.println(tick_counter_us);  
   delay(2000);
   // thread_yield();
   }*/

  if(th->stack!=NULL)
    free(th->stack);
  if(th!=NULL)
    free(th);
  // thread_start(th);
  // thread_start(th);
  delay(4000);

}






