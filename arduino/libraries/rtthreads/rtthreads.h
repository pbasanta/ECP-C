#ifndef rtthreads_h
#define rtthreads_h

#define __STDC_LIMIT_MACROS
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <util/atomic.h>

#define SIMPLERTFRAMEWORK_STACK_SIZE 200
#define SIMPLERTFRAMEWORK_MAX_PRIO   64
#define SIMPLERTFRAMEWORK_NORM_PRIO   2
#define SIMPLERTFRAMEWORK_OVERRUN_PRIO 3
#define SIMPLERTFRAMEWORK_MIN_PRIO    0


#define SIMPLERTFRAMEWORK_BLOCKED 0x02
#define SIMPLERTFRAMEWORK_ACTIVE 0x01
#define SIMPLERTFRAMEWORK_SLEPT 0x00
#define SIMPLERTFRAMEWORK_SEM_BLOCKED 0x04


#define SIMPLERTFRAMEWORK_ATS_SIGNAL       0x04
#define SIMPLERTFRAMEWORK_COST_OVERUN      0x03
#define SIMPLERTFRAMEWORK_MEMORY_EXHAUSTED 0x02
#define SIMPLERTFRAMEWORK_DEADLINE_MISS    0x01
#define SIMPLERTFRAMEWORK_PERIOD_MISS      0x00

#define SIMPLERTFRAMEWORK_SAVE_POLICY   0x01
#define SIMPLERTFRAMEWORK_EXCEPT_POLICY 0x02
#define SIMPLERTFRAMEWORK_IGNORE_POLICY 0x03
#define SIMPLERTFRAMEWORK_REPLACE_POLICY 0x04

#define SIMPLERTFRAMEWORK_OVER_SAVE_POLICY   0x10
#define SIMPLERTFRAMEWORK_OVER_EXCEPT_POLICY 0x20
#define SIMPLERTFRAMEWORK_OVER_IGNORE_POLICY 0x30
#define SIMPLERTFRAMEWORK_OVER_REPLACE_POLICY 0x40


typedef struct queue_t QUEUE;
typedef struct thread_t THREAD;
typedef struct semaphore_t SEMAPHORE;



/* 
 * periodic_thread_create
 */
THREAD * periodic_thread_create(void (*entry)(void), 
				void *params_and_data, 
				uint8_t * stack, 
				uint16_t stack_size, 
				uint8_t priority,  
				uint32_t offset_us,
				uint32_t period_us, 
				uint32_t max_deadline_us,
				uint32_t  max_cost_us, 
				void (*error_handler)(int errno));

/* 
 * sporadic_thread_create
 */
THREAD * sporadic_thread_create(void (*entry)(void), 
				void *params_and_data, 
                                uint8_t * stack, 
                                uint16_t stack_size, 
                                uint8_t priority,  
                                uint32_t offset_us,
                                uint32_t period_us, 
                                uint32_t max_deadline_us,
                                uint32_t max_cost_us, 
				uint8_t mit_policy, 
				uint32_t queue_length_max,
                                void (*error_handler)(int errno));

/* 
 * aperiodic_thread_create
 */
THREAD * aperiodic_thread_create(void (*entry)(void),
				void *params_and_data, 
                                uint8_t * stack, 
                                uint16_t stack_size, 
                                uint8_t priority, 
                                uint32_t offset_us,
                                uint32_t max_deadline_us,
                                uint32_t max_cost_us,
				uint8_t over_policy, 
				uint32_t queue_length_max,
                                void (*error_handler)(int errno));

int thread_remove(THREAD* who);

/*
 * Its a blocking operation necessary to run tasks
 * 

 */
void threads_subsystem_start();



/*
 * Its a blocking operation necessary to run tasks
 */
void *  thread_get_params();

/*
 * Its a blocking operation necessary to run tasks
 */
//void thread_unblock(THREAD * th);
//void thread_block(THREAD * th);
int thread_signal(THREAD* th);

/*******************************************/
/* Simple ATS model */
int thread_ATS_signal(THREAD* th);
void thread_ATS_enable();
void thread_ATS_disable();
void thread_ATS_set_maximum(int32_t max);
int32_t thread_ATS_get_maximum();

/*******************************************/
/* Simple non blocking queue*/
/**/
QUEUE* thread_queue_create(size_t howmany); 
void   thread_queue_delete(QUEUE * qeueu); 

void* thread_queue_nb_get_element(QUEUE* qeuue); 
int32_t thread_queue_nb_set_element(QUEUE* qeuue, void* element); 
int32_t thread_queue_stored(QUEUE* qeuue); 
int32_t thread_queue_size(QUEUE* qeuue); 



#define SIMPLERTFRAMEWORK_ATOMIC_SECTION ATOMIC_BLOCK(ATOMIC_RESTORESTATE)

/*
 * Semaphore
 */

SEMAPHORE* thread_semaphore_create(int initialization);
void thread_semaphore_P(SEMAPHORE* sem);
void thread_semaphore_V(SEMAPHORE* sem);
void thread_semaphore_free(SEMAPHORE* sem);



/*
 * Sleep 
 */
void thread_sleep(uint32_t time_in_us);



/*
 */
void* thread_malloc(size_t size);
void thread_free(void* ptr);


/*
 * Yield 
 */
void thread_yield();

/*
 * Real-time threads clock 
 */
uint32_t thread_get_time();
uint32_t thread_get_error_in_time();


/*
 * Interrupts
 */
//void attach_thread_to_interrupt(uint8_t interrupt, THREAD *thr, int MODE);
void attach_interrupt(uint8_t interrupt, void (*handler)(void), int MODE);
void detach_interrupt(uint8_t interrupt);




/*
 * Memory access
 */

uint8_t	thread_read_byte(uint32_t position);
float	thread_read_float(uint32_t position); 
uint16_t thread_read_uint16_t(uint32_t position); 
uint32_t thread_read_uint32_t(uint32_t position); 

void  thread_write_byte(uint32_t position, uint8_t data);
void  thread_write_float(uint32_t position, float data); 
void  thread_write_uint16_t(uint32_t position, uint16_t data); 
void  thread_write_uint32_t(uint32_t position, uint32_t data); 




#endif