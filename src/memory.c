/*
   memory.c
   This module manages dynamic memory requested by the user. For now, this dynamic
   memory is used exclusively for task memory allocation.
 */
/************************INCLUDES*********************************/
#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "OS.h"
#include "memory.h"


/************************PRIVATE VARIABLES*********************************/
const unsigned char pages_groups=(NUM_PAGES/8)+1;
char dyn_mem[TASK_SIZE*MAX_TASKS];  //Memory block used to provide dynamic memory
char page_directory[NUM_PAGES];  //page directory
char full_pages[(NUM_PAGES/8)+1]; //full_pages (i.e. Is a given page full?)


/************************PUBLIC FUNCTIONS*********************************/

/*                ----  char *task_malloc(void) ----
 * --- INPUT PARAMETERS:  NONE
 * --- OUTPUT PARAMETERS: NONE
 * --- RETURNS: *char (pointer to the start of an allocated memory block)
 *  
 *  This function returns a pointer to the start of an allocated memory section
 *  of the size of a task structure. Since blocks are always the same size, this
 *  implementation uses the first fit algorithm for memory allocation. Memory is organized 
 *  in pages and block of pages.
 * 
 */
char *task_malloc()
{
  	unsigned char page, offset,f_page,f_offset;
  	volatile unsigned char temp; 
  	offset=0;
  	page=0;
  	
  	for(f_page=0;f_page<pages_groups ;f_page++)
  	{	
  	  f_offset=0;
  	  temp=full_pages[f_page]; 
  	  if(temp==0xFF)
  		  continue;
  	  while(temp&0x01)
  	   {	  
  		  f_offset++;
  		  temp>>=1;
  	   }
  	   if(f_offset!=7)
  		   break;
  	}  
  	
  	page=PAGE_SIZE*f_page+f_offset;
    
  	temp=page_directory[page];
  	while(temp&0x01)
  	{
  	  offset++;
  	  temp>>=1;
  	}

  	page_directory[page]|=(0x01<<offset);  //mark it as not available
  	
  	if(page_directory[page]==0xFF)  //if the page is full we mark it as full
  		full_pages[f_page]|=(0x01<<f_offset);
  	
  	return (char*)(dyn_mem+PAGE_SIZE*page*TASK_SIZE+offset*TASK_SIZE);  	
}


/*                ----  void *task_free(char *ptr) ----
 * --- INPUT PARAMETERS:  *char (pointer to the start of an allocated memory block)
 * --- OUTPUT PARAMETERS: NONE
 * --- RETURNS: NONE
 *  
 *  This function frees a memory block previously allocated trhough task_malloc.
 * 
 */
void task_free(char *ptr)
{
  unsigned char f_page,f_offset;	
  unsigned char page=((unsigned char)(ptr-dyn_mem)/TASK_SIZE)/PAGE_SIZE;
  unsigned char offset=((unsigned char)(ptr-dyn_mem)/TASK_SIZE)%PAGE_SIZE;
  page_directory[page]&=~(0x01<<offset); 
  f_page=page/8;
  f_offset=page%8;
  full_pages[f_page]&=~(0x01<<f_offset);
}

#ifdef ALARMS

/*                ----  char *alarm_malloc(void) ----
 * --- INPUT PARAMETERS:  NONE
 * --- OUTPUT PARAMETERS: NONE
 * --- RETURNS: *char (pointer to the start of an allocated memory block)
 *  
 *  This function is contained in a conditional define to provide the user the
 *  option of not using alarms.
 *  
 *  Alarm malloc is used to provide the option of declaring and using alarms as local
 *  and global variables.
 *  
 *  This function returns a pointer to the start of an allocated memory section
 *  of the size of an alarm structure. Since blocks are always the same size, this
 *  implementation uses the first fit algorithm for memory allocation. Memory is organized 
 *  in pages and block of pages.
 * 
 */

char alarms_dyn_mem[ALARM_SIZE*MAX_ALARMS];  //Memory block used to provide dynamic memory
char alarms_page_directory[NUM_PAGES_ALARMS];  //page directory
char alarms_full_pages[(NUM_PAGES_ALARMS/8)+1]; //full_pages (i.e. Is a given page full?)
const unsigned char alarms_pages_groups=(NUM_PAGES/8)+1;


char *alarm_malloc()
 {
    unsigned char page, offset,f_page,f_offset;
	volatile unsigned char temp; 
	offset=0;
	page=0;
	
	for(f_page=0;f_page<alarms_pages_groups ;f_page++)
	{	
	  f_offset=0;
	  temp=alarms_full_pages[f_page]; 
	  if(temp==0xFF)
		  continue;
	  while(temp&0x01)
	   {	  
		  f_offset++;
		  temp>>=1;
	   }
	   if(f_offset!=7)
		   break;
	}  
	
	page=PAGE_SIZE*f_page+f_offset;

	temp=alarms_page_directory[page];
	while(temp&0x01)
	{
	  offset++;
	  temp>>=1;
	}

	alarms_page_directory[page]|=(0x01<<offset);  //mark it as not available
	
	if(alarms_page_directory[page]==0xFF)  //if the page is full we mark it as full
		alarms_full_pages[f_page]|=(0x01<<f_offset);
	
	return (char*)(alarms_dyn_mem+PAGE_SIZE*page*ALARM_SIZE+offset*ALARM_SIZE);  	
}

void alarm_free(char *ptr)
{
  unsigned char f_page,f_offset;	
  unsigned char page=((unsigned char)(ptr-alarms_dyn_mem)/ALARM_SIZE)/PAGE_SIZE;
  unsigned char offset=((unsigned char)(ptr-alarms_dyn_mem)/ALARM_SIZE)%PAGE_SIZE;
  alarms_page_directory[page]&=~(0x01<<offset); 
  f_page=page>>3;
  f_offset=page%8;
  alarms_full_pages[f_page]&=~(0x01<<f_offset);
}
#endif 

