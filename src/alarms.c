/************************INCLUDES*********************************/
#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "OS.h"
#include "memory.h"
#include "alarms.h"
#define PWM1 PTAD_PTAD0
#define PWM2 PTAD_PTAD1
/************************PRIVATE VARIABLES*********************************/
unsigned char last_alarm=0;
alarm *system_alarms[MAX_ALARMS];
alarm *temp_alarm;
unsigned char first_alarm;
unsigned char alarm_count=0;
unsigned char interrupted;
unsigned long next_OC;
char global_monitor;
char flag=0;
unsigned int counter=0;
/************************GLOBAL VARIABLES*********************************/

unsigned char running_alarms=0;
/************************PUBLIC FUNCTIONS*********************************/
unsigned char start_alarm(alarm *alarm_ptr)
{
  unsigned char i,status;
  for(i=0;i<MAX_ALARMS;i++)
  {
	  if(system_alarms[i]==NULL)
	  {
		  system_alarms[i]=alarm_ptr;
		  break;  
	  }	  
  }
  if(i>=MAX_ALARMS)
	  return 1;
  running_alarms++;
  if(running_alarms==1)
	  OS_start_timer();
  return 0;
}

unsigned char resume_alarm(alarm *alarm_ptr)
{
  unsigned char i,status;
  for(i=0;i<MAX_ALARMS;i++)
  {
	  if(system_alarms[i]==NULL)
	  {
		  system_alarms[i]=alarm_ptr;
		  break;  
	  }	  
  }
  if(i>=MAX_ALARMS)
	  return 1;
  running_alarms++;
  if(running_alarms==1)
	  OS_resume_timer();
  return 0;
}

unsigned char stop_alarm(alarm *alarm_ptr)
{
	unsigned char i;
	
	for(i=0;i<MAX_ALARMS;i++)
	{
		if(system_alarms[i]==alarm_ptr)
		{
			system_alarms[i]=NULL;
			alarm_free((char*)alarm_ptr);
			running_alarms--;
			return 0;
		}
	}
	return 1;
}

void fill_alarm(alarm *alarm_ptr,unsigned int max_tics,unsigned char cycles,struct task *alarm_task)
{
	alarm_ptr->max_tics=max_tics;
	alarm_ptr->cycles=cycles;
	alarm_ptr->alarm_task=alarm_task;
	alarm_ptr->current_tics=max_tics;
}

alarm *get_next_finished_alarm()
{
	unsigned char i;
	for(i=last_alarm;i<MAX_ALARMS;i++)
	{
		if(system_alarms[i]->current_tics==0)
		{
		  	last_alarm=i+1;
		  	return system_alarms[i];
		}
	}
	last_alarm=0;
	return NULL;
}

void update_alarms()
{
	unsigned char i;
	for(i=0;i<MAX_ALARMS;i++)
	{
		if(system_alarms[i]!=NULL)
		{
		  system_alarms[i]->current_tics--;
		  if(system_alarms[i]->current_tics==0)
			  system_alarms[i]->cycles--;  
		}
	}
}


unsigned char OS_set_timer(unsigned char time_base_us)
{
    if(time_base_us>7)
    	return 1;
    
	 TPM1SC=0b00001000;
	 TPM1SC+=time_base_us;
	 TPM1C0SC=0b00010000;
	 return 0;
}

unsigned char OS_set_tics(unsigned int number_cycles)
{
     next_OC=number_cycles;
	 return 0;
}

void OS_start_timer()
{
	 TPM1C0SC_CH0IE=1; 
	 registro_OC=next_OC+timer;
	 last_alarm=0;
}

void OS_resume_timer()
{
	 TPM1C0SC_CH0IE=1; 
	 registro_OC+=next_OC;
	 last_alarm=0;
}

void OS_stop_timer()
{
	TPM1C0SC_CH0IE=0; 
}

void interrupt VectorNumber_Vtpm1ch0 interrupt_OC(void)
{
	RECOVER_PRE_INT()
	
		PWM2=1;	
	TPM1C0SC_CH0F=0;//apagar bandera output compare
	update_alarms();
	PWM2=0;
    last_alarm=0;
    first_alarm=0;
	for(alarm_count=0;alarm_count<MAX_ALARMS;alarm_count++)
	  if(system_alarms[alarm_count]!=NULL)
		  break;
/*	if (alarm_count==MAX_ALARMS)
		OS_stop_timer();
*/	
	while((temp_alarm=get_next_finished_alarm())!=0x00)
	{
		counter++;
		if(counter==244)
			counter=0;
		
	  if(!first_alarm)
	  {
		  first_alarm=1;
		  save_currrent_task();
	  }
	push_task_stack(temp_alarm->alarm_task);
	
	if(temp_alarm->cycles==0)
	{
		(temp_alarm->alarm_task)->dynamic_task=0;
		
		alarm_free(temp_alarm);
		
		running_alarms--;
	}
	for(alarm_count=0;alarm_count<MAX_ALARMS;alarm_count++)
		if(system_alarms[alarm_count]==temp_alarm)
		{
			system_alarms[alarm_count]=NULL;
			break;
		}
		  
			
	}
    
	if(!first_alarm && running_alarms>0)
	{
		TPM1C0SC_CH0IE=1; 
        registro_OC+=next_OC;
	}

    
	scheduler();
	
	
	
 }
