/*
 * alarms.h
 *
 *  Created on: 18/09/2014
 *      Author: Work
 */

#ifndef ALARMS_H_
#define ALARMS_H_
             
#define ALARMS
/************************INCLUDES*********************************/
#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "OS.h"
#include "memory.h"

/************************DEFINES*********************************/
#define  registro_OC TPM1C0V
#define timer TPM1CNT


/************************STRUCTURES*********************************/
typedef struct alarm
{
	unsigned int max_tics;
	unsigned int current_tics;
	unsigned char cycles;
	 task *alarm_task;
}alarm;

enum TIME_BASE {
  BASE_1_US=0,
  BASE_2_US=1,
  BASE_4_US=2,
  BASE_8_US=3,
  BASE_16_US=4,
  BASE_32_US=5,
  BASE_64_US=6,
  BASE_128_US=7,
};


/************************PUBLIC FUNCTIONS*********************************/
unsigned char start_alarm(alarm *alarm_ptr);
unsigned char resume_alarm(alarm *alarm_ptr);
unsigned char stop_alarm(alarm *alarm_ptr);
void fill_alarm(alarm *alarm_ptr,unsigned int max_tics,unsigned char cycles,struct task *alarm_task);
alarm *get_next_finished_alarm();
void update_alarms();
unsigned char OS_set_timer(unsigned char time_base_us);
unsigned char OS_set_tics(unsigned int number_cycles);
void OS_start_timer();
void OS_stop_timer();
void OS_resume_timer();

extern unsigned char running_alarms;
extern unsigned long next_OC;
extern unsigned int counter;

#endif /* ALARMS_H_ */
