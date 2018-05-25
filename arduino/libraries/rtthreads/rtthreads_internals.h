#ifndef rtthreads_internals_h
#define rtthreads_internals_h


typedef struct thread_t {
  /*struct thread_t * runq_prev;         previous thread in run queue */
  struct thread_t * runq_next;        /*next thread in run queue */
  uint8_t         * sp;               /* thread stack pointer */
  uint8_t         * stack;            /* start of thread stack */
  uint8_t           priority;         /* thread priority */
  uint8_t           block_pattern;    /* =0x00 periodic, =0x01 blocking */ 
  /**/
  uint8_t           ats_active;       	/* =0x00 no, =0x01 atc */ 
  uint32_t          ats_pending;      	/* =0x00 no, =0x01 atc */ 
  uint32_t          ats_max;  	  	/* =0x01  atc */ 
  
  
  /**/
  uint8_t           status;           /*                               */
  uint16_t          stack_size;       /* thread stack size */
  uint32_t          ticks_to_wait;    /* sleep timer */
  uint8_t           carry_ticks_to_wait;  /* sleep timer carry*/
  uint32_t          min_period_ticks;      
  uint32_t          max_deadline_ticks;
  uint32_t          max_cost_ticks;
  uint32_t          current_cost_ticks;
  uint32_t          offset_ticks;
  /*queue management*/
  uint8_t 	    mit_policy;
  int32_t 	    queue_length_max;
  int32_t 	    queue_pending;
  uint32_t 	    queue_last_input;
  /******************/
  SEMAPHORE*        blocked_semaphore; /*NULL*/
  /*Executor & error handler*/
  void 		    (*executor)();
  void 		    *params_and_data; 
  void   	    (*error_handler)(int errno);
  void              * parm;         /* parameter pointer */
  /**/
  uint8_t           aux1;         /* thread priority */
  uint8_t           aux2;        /* =0x00 periodic, =0x01 blocking */ 
} THREAD;

THREAD*  thread_internals_threadlist();

typedef struct semaphore_t
{int value;  //Initialization a cero
}
SEMAPHORE;

typedef struct queue_t{
  int32_t stored;
  int32_t howmany; //Maximum number of elements in the queue
  int32_t read_index; //Maximum number of elements in the queue
  int32_t write_index; //Maximum number of elements in the queue
} QUEUE;

typedef struct queue_t_l{
  int32_t stored;
  int32_t howmany; //Maximum number of elements in the queue
  int32_t read_index; //Maximum number of elements in the queue
  int32_t write_index; //Maximum number of elements in the queue
  void* elements[0];
} QUEUE_L;


THREAD*  thread_internals_threadlist();

#endif