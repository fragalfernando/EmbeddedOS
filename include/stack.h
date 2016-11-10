/*
   stack.h
   This module implements a stack of tasks.
 */

#ifndef STACK_H_
#define STACK_H_
/************************INCLUDES*********************************/
#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "OS.h"
#include "memory.h"


/************************STRUCTURES*********************************/
typedef struct task_stacks
{
  char size;
  struct task *top;
}task_stack;

/**********************PUBLIC FUNCTIONS*******************************/
void push_task_stack(struct task *ptr_task);
struct task *pop_task_stack(char stack_id);

/***********************PUBLIC VARIABLES*************************************/
extern task_stack stacks[5];


#endif /* STACK_H_ */

