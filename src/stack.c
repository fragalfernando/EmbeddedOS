/*
   stack.c
   This module implements a stack of tasks.
 */

/************************INCLUDES*********************************/
#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "OS.h"
#include "memory.h"
#include "stack.h"

/*************************PUBLIC FUNCTIONS*********************************/

/*                ----  void push_task_stack(task *ptr_task) ----
 * --- INPUT PARAMETERS:  task *ptr_task
 * --- OUTPUT PARAMETERS: NONE
 * --- RETURNS: NOTHING
 *  
 *  This function pushes a task into the corresponding stack(according to priority).
 *  The priority of the task is obtained internally and the task is set in the array
 *  of stacks indexed by priority.
 */
void push_task_stack(task *ptr_task)
{
	stacks[ptr_task->priority].size++;
	ptr_task->next=stacks[ptr_task->priority].top;
	stacks[ptr_task->priority].top=ptr_task;	
}

/*                ----  task *pop_task_stack(char stack_id) ----
 * --- INPUT PARAMETERS:  char stack_id
 * --- OUTPUT PARAMETERS: NONE
 * --- RETURNS: *task pointer 
 *  
 *  This function retries the task a the top of the stack in the stack array indexed by stack_id
 * 
 */
task *pop_task_stack(char stack_id)
{

  task *temp_task=stacks[stack_id].top;	
  if(stacks[stack_id].size==0)
	  return NULL;
  
  stacks[stack_id].top=(stacks[stack_id].top)->next;
  stacks[stack_id].size--;
  return temp_task;
}
