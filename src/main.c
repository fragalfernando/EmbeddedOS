#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "OS.h"
#include "memory.h"
#include "alarms.h"
#define PWM1 PTAD_PTAD0
#define PWM1_ENABLE PTADD_PTADD0

#define RX1 PTGD_PTGD4
#define RX2 PTDD_PTDD2
#define RX3 PTDD_PTDD3
#define RX4 PTDD_PTDD7
task *task1,*task2,*task3,*task4,*task5;
int global_variable;
alarm *alarm1, *alarm2,*alarm3,*alarm4;
int i1;

unsigned char rx_count1;
unsigned char rx_index1;
char rx_char1;
char temp;

unsigned char rx_count2;
unsigned char rx_index2;
char rx_char2;
char temp2;

unsigned char rx_count3;
unsigned char rx_index3;
char rx_char3;
char temp3;


unsigned char rx_count4;
unsigned char rx_index4;
char rx_char4;
char temp4;

mailbox mailbox1,mailbox2,mailbox3,mailbox4;

void read_pin1()
{
	temp=RX1;
	PWM1=1;
	mailbox1.lock=1;
	
	if(rx_count1>=0 && rx_count1<8)
	{
		
	    rx_char1=rx_char1>>1;
	    if(temp==1)
	    	rx_char1=rx_char1|0x80; 
	}
   if(rx_count1==8)
    {
    	mailbox1.buffer[rx_index1++]=rx_char1;
    	KBIPE_KBIPE4=1;
    	PWM1=0;
    	mailbox1.lock=0;
    	terminate_task();
    } 
    
    rx_count1++;
	 task2=task_malloc();
	alarm1=alarm_malloc();
	  fill_task(&read_pin1, 0, 0, 
		      &read_pin1,0);
	fill_alarm(alarm1,1,1,task2);
	resume_alarm(alarm1);
	PWM1=0;
	
	//TPM1C0SC_CH0IE=1;
	terminate_task();
}

void read_pin2()
{
	temp2=RX2;
	mailbox2.lock=1;
	if(rx_count2>=0 && rx_count2<8)
	{
		
	    rx_char2=rx_char2>>1;
	    if(temp2==1)
	    	rx_char2=rx_char2|0x80; 
	}
   if(rx_count2==8)
    {
    	mailbox2.buffer[rx_index2++]=rx_char2;
    	KBIPE_KBIPE5=1;
    	//PWM1=0;
    	terminate_task();
    } 
    
    rx_count2++;
	 task3=task_malloc();
	alarm2=alarm_malloc();
	  fill_task(task3, 0, 0, 
		      &read_pin2,0);
	fill_alarm(alarm2,1,1,task3);
	resume_alarm(alarm2);
	
	//TPM1C0SC_CH0IE=1;
	terminate_task();
}

void read_pin3()
{
	temp3=RX3;
	mailbox3.lock=1;
	if(rx_count3>=0 && rx_count3<8)
	{
		
	    rx_char3=rx_char3>>1;
	    if(temp3==1)
	    	rx_char3=rx_char3|0x80; 
	}
   if(rx_count3==8)
    {
    	mailbox3.buffer[rx_index3++]=rx_char3;
    	KBIPE_KBIPE6=1;
    	//PWM1=0;
    	mailbox3.lock=0;
    	terminate_task();
    } 
    
    rx_count3++;
	 task4=task_malloc();
	alarm3=alarm_malloc();
	  fill_task(task4, 0, 0, 
		      &read_pin3,0);
	fill_alarm(alarm3,1,1,task4);
	resume_alarm(alarm3);
	
	//TPM1C0SC_CH0IE=1;
	terminate_task();
}

void read_pin4()
{
	temp4=RX4;
	mailbox4.lock=1;
	if(rx_count4>=0 && rx_count4<8)
	{
		
	    rx_char4=rx_char4>>1;
	    if(temp4==1)
	    	rx_char4=rx_char4|0x80; 
	}
   if(rx_count4==8)
    {
    	mailbox4.buffer[rx_index4++]=rx_char4;
    	KBIPE_KBIPE7=1;
    	//PWM1=0;
    	mailbox4.lock=0;
    	terminate_task();
    } 
    
    rx_count4++;
	 task5=task_malloc();
	alarm4=alarm_malloc();
	  fill_task(task5, 0, 0, 
		      &read_pin4,0);
	fill_alarm(alarm4,1,1,task5);
	resume_alarm(alarm4);
	
	//TPM1C0SC_CH0IE=1;
	terminate_task();
}

void vfnMGC_init(void) {
	/* Antes 16MHz external clock source connected, ahora 4 Mhz External. */
	ICGC1 = 0x78; // Configura Cristal Externo con FLL x10 para tener 40 Mhz con cristal de 4 Mhz.
	ICGC2 = 0xB8;

	SOPT = 0x00; // Desactiva Watchdog

	// SRS = 0xC2;       // Fuses

	while (!ICGS1_REFST) {
	}; // Wait for Reference Status bit to update
	while (ICGS1_CLKST != 0b11) {
	}; // Wait for clock status bits to update 
	while (!ICGS1_LOCK) {
	}; // Wait for LOCK bit to set 
}

void wait_8ms(int ms){
 
 int i;
 SRTISC=0x01;
 for(i=0;i<ms;i++)
 {
	 while(!SRTISC_RTIF){__RESET_WATCHDOG();}
	 SRTISC_RTIACK=1;
	 SRTISC=0x01;
 }   
 		
}
void main(void) 
{
   
  PWM1_ENABLE=1;
  rx_count1=0;
  rx_index1=0;
  rx_char1=0;
  
  rx_count2=0;
  rx_index2=0;
  rx_char2=0;
  
  rx_count3=0;
  rx_index3=0;
  rx_char3=0;
  
  rx_count4=0;
  rx_index4=0;
  rx_char4=0;
  
  PTGPE_PTGPE4=1;
  PTDPE_PTDPE2=1;
  PTDPE_PTDPE3=1;
  PTDPE_PTDPE7=1;
  current_task.priority=3;
  current_task.auto_start=4;
  running_alarms=0;
  vfnMGC_init();

  
   OS_set_timer(BASE_1_US);
   OS_set_tics(4152);
    
  counter=0;
  wait_8ms(5);
  KBISC=0x06;
  KBIPE_KBIPE4=1;
   KBIPE_KBIPE5=1;
   KBIPE_KBIPE6=1;
   KBIPE_KBIPE7=1;
  KBISC_KBIE=1;
  wait_8ms(5);
  EnableInterrupts;
  while(1) {EnableInterrupts; /*KBISC=0x06*/; KBISC_KBIE=1;  /*__RESET_WATCHDOG();*/};

  for(;;) {
    __RESET_WATCHDOG();	/* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}



void interrupt VectorNumber_Vkeyboard1 KBI(void)
{
	KBISC_KBACK=1;
	KBIPE_KBIPE4=1;
	KBIPE_KBIPE5=1;
	KBIPE_KBIPE6=1;
	KBIPE_KBIPE7=1;
	for(i1=1;i1<250;i1++)
		asm(NOP;);
	if (RX1==0)
	{
		KBIPE_KBIPE4=0;
		alarm1=alarm_malloc();
		task1=task_malloc();
		fill_task(task1, 0, 0,
				&read_pin1,0);
		fill_alarm(alarm1,1,1,task1);
		start_alarm(alarm1);
		rx_count1=0;
		rx_char1=0;
	}  
    if(RX2==0)
    {  	
    	KBIPE_KBIPE5=0;
		alarm2=alarm_malloc();
		task3=task_malloc();
		fill_task(task3, 0, 0,
				&read_pin2,0);
		fill_alarm(alarm2,1,1,task3);
		start_alarm(alarm2);
		rx_count2=0;
		rx_char2=0;
    }  
    
    if(RX3==0)
    {  	
    	KBIPE_KBIPE6=0;
		alarm3=alarm_malloc();
		task4=task_malloc();
		fill_task(task4, 0, 0,
				&read_pin3,0);
		fill_alarm(alarm3,1,1,task4);
		start_alarm(alarm3);
		rx_count3=0;
		rx_char3=0;
    }  
    
    if(RX4==0)
    {  	
    	KBIPE_KBIPE7=0;
		alarm4=alarm_malloc();
		task5=task_malloc();
		fill_task(task5, 0, 0,
				&read_pin4,0);
		fill_alarm(alarm4,1,1,task5);
		start_alarm(alarm4);
		rx_count4=0;
		rx_char4=0;
    }  
	
}

