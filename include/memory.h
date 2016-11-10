/*
   memory.h
   This module manages dynamic memory requested by the user. For now, this dynamic
   memory is used exclusively for task memory allocation.
 */
#ifndef MEMORY_H_
#define MEMORY_H_
/************************INCLUDES*********************************/
#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "OS.h"

/************************DEFINITIONS*********************************/
#define TASK_SIZE 14
#define MAX_TASKS 14
#define PAGE_SIZE 8
#define NUM_PAGES ((TASK_SIZE*MAX_TASKS)/PAGE_SIZE)+1

#define ALARM_SIZE 8
#define MAX_ALARMS 4
#define NUM_PAGES_ALARMS ((ALARM_SIZE*MAX_ALARMS)/PAGE_SIZE)+1
#define ALARMS


/************************PRIVATE VARIABLES*********************************/


/************************PUBLIC FUNCTIONS*********************************/
char *task_malloc();
void task_free(char *ptr);

#ifdef ALARMS

char *alarm_malloc();
void alarm_free(char *ptr);

#endif

#endif   /* MEMORY_H_ */