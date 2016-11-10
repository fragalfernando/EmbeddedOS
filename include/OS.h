/*
   OS.h
   This module implements all the task management functions such as :
     +Scheduling
     +Task activation, chaining, and termination
     +Registers backup when an interruption occurs
 */

#ifndef OS_H_
#define OS_H_
/************************INCLUDES*********************************/
#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "stack.h"
#include "memory.h"


/************************DEFINITIONS*********************************/
#define MAX_PRIORITY 5
#define MAILBOX_BUFFER_SIZE 50
/************************MACRO IMPLEMENTATIONS*********************************/


/*********This macro sets the PC to the address specified as a parameter*******/
#define SET_PC(address)        asm(ldhx address;);        \
                            asm(JMP ,X;);

/*********This macro bacups the current stack pointer before calling internally*******/
/*********activate_task with the parameter 'a' begin the task to be activated*********/
#define ACTIVATE_TASK(a)  asm(tsx;);        \
		                  asm(sthx current_task.SP;);        \
		                  activate_task(a);

/*********This macro backups the current stack pointer before calling internally***************/
/*********recover_interrupt which backups all register of the task that was interrupted*********/
  
#define RECOVER_PRE_INT()  asm(sthx current_task.HX;);    \
	                       asm(tsx); \
		                   asm(sthx current_task.SP;);   \
		                   recover_interrupt();

/*********This macro sets the Stack Pointer to the parameter 'sp'*******************************/
#define SET_SP(sp)         asm(ldhx sp;);        \
		                   asm(txs;);      

#define TERMINATE_TASK  terminate_task();

#define CHAIN_TASK(a)  chain_task(a);


/************************STRUCTURES*********************************/
 typedef struct tasks task;

typedef struct tasks
{
 char priority;
 char auto_start;
 /*REGISTERS*/
 void *SP;
 unsigned int dynamic_task;
 void* PC_backup;
 char AC;
 char CCR;
 unsigned int HX;
 /************/
 task *next;	
};




typedef struct mailbox
{
	char buffer[MAILBOX_BUFFER_SIZE];
	unsigned char task1;
	unsigned char task2;
	unsigned char lock;
}mailbox;

/************************PRIVATE VARIABLES*********************************/


/************************PUBLIC VARIABLES*********************************/
extern task  current_task;


/************************PRIVATE FUNCTIONS*********************************/
static unsigned int *get_return_pc_task (void);
static unsigned int *get_return_pc_task_int (void);
static char get_return_ac_task_int(void);
static char get_return_ccr_task_int(void);
 void scheduler();

/************************PUBLIC FUNCTIONS*********************************/
void recover_interrupt();
void activate_task(task *new_task);
void activate_task_int(task *new_task);
void terminate_task();
void chain_task(task *new_task);
void fill_task(task *task_ptr, char priority, char auto_start, 
		      void *pc_backup, unsigned int dyn_task);

void save_currrent_task();
#endif /* OS_H_ */
