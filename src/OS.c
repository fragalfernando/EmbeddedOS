/*
   OS.c
   This module implements all the task management functions such as :
     +Scheduling
     +Task activation, chaining, and termination
     +Registers backup when an interruption occurs
 */
/************************INCLUDES*********************************/
#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "stack.h"
#include "memory.h"
#include "OS.h"
#include "alarms.h"
#define PWM1 PTAD_PTAD0
#define PWM2 PTAD_PTAD1

/************************PRIVATE VARIABLES*********************************/
task_stack stacks[MAX_PRIORITY];
task  current_task;

char cur_AC;
char cur_SR;
char cur_X;

/************************PRIVATE FUNCTIONS*********************************/



/*                ----  static unsigned int *get_return_pc_task (void) ----
 * --- INPUT PARAMETERS:  NONE
 * --- OUTPUT PARAMETERS: NONE
 * --- RETURNS: *unsigned int      TO_DO: CHANGE FOR *void
 *  
 * This function returns the PC of a task. This is achieved by retrieving the PC from the stack.
 */
static unsigned int *get_return_pc_task (void) 
{

  unsigned int **stack_pointer;
  #asm    
    nop
    ais #8
    tsx
    ais #-8
    sthx stack_pointer
  #endasm;
  
  return *stack_pointer;
}

/*                ----  static unsigned int *get_return_pc_task_int (void) ----
 * --- INPUT PARAMETERS:  NONE
 * --- OUTPUT PARAMETERS: NONE
 * --- RETURNS: *unsigned int      TO_DO: CHANGE FOR *void
 *  
 * This function returns the PC of a task that was interrupted. 
 * This is achieved by retrieving the PC from the stack. The number of memory locations away from the
 * stack pointer  where the PC is located in the stack changes from the previous implementation.
 */
static unsigned int *get_return_pc_task_int (void) 
{

  unsigned int **stack_pointer;
  #asm    
    nop
    ais #11
    tsx
    ais #-11
    sthx stack_pointer
  #endasm;
    
   return *stack_pointer;
}

/*                ----  static char get_return_ac_task_int (void) ----
 * --- INPUT PARAMETERS:  NONE
 * --- OUTPUT PARAMETERS: NONE
 * --- RETURNS: char AC     
 *  
 * This function returns the AC of a task that was interrupted. 
 * This is achieved by retrieving the AC from the stack. 
 */
static char get_return_ac_task_int (void) 
{

  char *AC;
  #asm    
    nop
    ais #9
    tsx
    ais #-9
    sthx AC
  #endasm;
    
   return *AC;
}
/*                ----  static char get_return_ccr_task_int (void) ----
 * --- INPUT PARAMETERS:  NONE
 * --- OUTPUT PARAMETERS: NONE
 * --- RETURNS: char CCR     
 *  
 * This function returns the CCR of a task that was interrupted. 
 * This is achieved by retrieving the CCR from the stack. 
 */
static char get_return_ccr_task_int (void) 
{

  char *CCR;
  #asm    
    nop
    ais #7
    tsx
    ais #-7
    sthx CCR
  #endasm;
    
   return *CCR;
}

/*                ----  static void scheduler() ----
 * --- INPUT PARAMETERS:  NONE
 * --- OUTPUT PARAMETERS: NONE
 * --- RETURNS: void     
 *  
 * This functions decides what is the next task to be executed, sets the stack pointer,
 * the program counter and the current_task to the task that is next to be executed.
 */
void scheduler()
{
	int i;
	task *current;
	for(i=0;i<5;i++)
	{
	  if(stacks[i].size>0)
	  {
		  current=pop_task_stack(i);
		  current_task=*current;
		  if(!current->dynamic_task)
		    task_free(current);		  
		  if(current_task.SP)  
		  {
			  SET_SP(current_task.SP);  //This is a multiple line macro, thus the braces are needed 
			                            //even if it seems as a single line statement.
		  }
          #asm
		  lda current_task.AC
		  ldhx current_task.HX
          #endasm;
		  if(running_alarms>0)
		  {
	          registro_OC+=next_OC;
			  TPM1C0SC_CH0IE=1;   
		  }

		  //SRTISC=0x51;
		  
		  SET_PC(current_task.PC_backup);
	  }
	}
}


/************************PUBLIC FUNCTIONS*********************************/
/*                ----   void recover_interrupt() ----
 * --- INPUT PARAMETERS:  NONE
 * --- OUTPUT PARAMETERS: NONE
 * --- RETURNS: void     
 *  
 * This function recovers the PC and the SP of the task that was interrupted
 */
void recover_interrupt()
{
  current_task.PC_backup=get_return_pc_task_int();
  current_task.SP=(char*)current_task.SP+7; 
  current_task.AC=get_return_ac_task_int();
  current_task.CCR=get_return_ccr_task_int();
}

/************************PUBLIC FUNCTIONS*********************************/


/*                ----   void activate_task(task *new_task) ----
 * --- INPUT PARAMETERS: task *new_task
 * --- OUTPUT PARAMETERS: NONE
 * --- RETURNS: void     
 *  
 * This function pushes the current task into the stack as well as the task passed as a parameter,
 * then it calls the scheduler which will decide which is the next task to be executed.
 */
void activate_task(task *new_task)
{
  volatile task *cur_task=task_malloc();
  
  //save current task
  cur_task->priority=current_task.priority;
  cur_task->auto_start=current_task.auto_start;
  cur_task->dynamic_task=current_task.dynamic_task;
  cur_task->PC_backup=get_return_pc_task();
  cur_task->next=current_task.next;
  cur_task->SP=current_task.SP;
  cur_task->AC=current_task.AC;
  cur_task->HX=current_task.HX;
  cur_task->CCR=current_task.CCR;
 //push both the current and new tasks to the task stack
  push_task_stack(cur_task);
  push_task_stack(new_task);
  
  scheduler();
  
}

/*                ----   void activate_task_int(task *new_task) ----
 * --- INPUT PARAMETERS: task *new_task
 * --- OUTPUT PARAMETERS: NONE
 * --- RETURNS: void     
 *  
 * This function pushes the current task into the stack as well as the task passed as a parameter,
 * then it calls the scheduler which will decide which is the next task to be executed. The difference
 * between this function and activate_task is that the PC of the interrupted function doesn't need
 * to be retrieved since RECOVER_INTERRUPT() already backups the registers.
 */

void activate_task_int(task *new_task)
{
  volatile task *cur_task=task_malloc();
  
  //save current task
  cur_task->priority=current_task.priority;
  cur_task->auto_start=current_task.auto_start;
  cur_task->dynamic_task=current_task.dynamic_task;
  cur_task->PC_backup=current_task.PC_backup;
  cur_task->next=current_task.next;
  cur_task->SP=current_task.SP;
  cur_task->AC=current_task.AC;
  cur_task->HX=current_task.HX;
  cur_task->CCR=current_task.CCR;
 //push both the current and new tasks to the task stack
  push_task_stack(cur_task);
  push_task_stack(new_task);
  
  scheduler();
  
}


void save_currrent_task()
{
  volatile task *cur_task=task_malloc();
  
  //save current task
  cur_task->priority=current_task.priority;
  cur_task->auto_start=current_task.auto_start;
  cur_task->dynamic_task=current_task.dynamic_task;
  cur_task->PC_backup=current_task.PC_backup;
  cur_task->next=current_task.next;
  cur_task->SP=current_task.SP;
  cur_task->AC=current_task.AC;
  cur_task->HX=current_task.HX;
  cur_task->CCR=current_task.CCR;
 //push both the current and new tasks to the task stack
  push_task_stack(cur_task);
}




/*                ----   void terminate_task(void) ----
 * --- INPUT PARAMETERS: NONE
 * --- OUTPUT PARAMETERS: NONE
 * --- RETURNS: void     
 *  
 * This function  just calls the scheduler. Since the current task is not backed up, the 
 * next task to be executed will be called by the scheduler.
 */
void terminate_task()
{
  scheduler();	
}


/*                ----   chain_task(task *new_task) ----
 * --- INPUT PARAMETERS: NONE
 * --- OUTPUT PARAMETERS: NONE
 * --- RETURNS: void     
 *  
 * This functions pushes the task passed as a parameter to the stack, and the calls the scheduler.
 */
void chain_task(task *new_task)
{
	push_task_stack(new_task);
	scheduler();
}


/*                ----   void fill_task(task *task_ptr, char priority, char auto_start, ----
		          ----                        void *fun_ptr,void *pc_backup)            ----
 * --- INPUT PARAMETERS:task *task_ptr,char priority, char auto_start,void *fun_ptr,void *pc_backup
 * --- OUTPUT PARAMETERS: NONE
 * --- RETURNS: void     
 *  
 * This functions fills a task with the specified parameters
 */

void fill_task(task *task_ptr, char priority, char auto_start, 
		      void *pc_backup,unsigned int dyn_task)
{
	task_ptr->priority=priority;
	task_ptr->auto_start=auto_start;
	task_ptr->PC_backup=pc_backup;
	task_ptr->SP=0x00;
	task_ptr->dynamic_task=dyn_task;
}
