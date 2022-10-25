/*
 * U_PiCalc_HS2022 TW.c
 *
 * Created: 20.03.2018 18:32:07
 * Author : appaie
 */ 

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "avr_compiler.h"
#include "pmic_driver.h"
#include "TC_driver.h"
#include "clksys_driver.h"
#include "sleepConfig.h"
#include "port_driver.h"

#include "FreeRTOS.h"
#include "task.h"

//Function Tasks
void vDisplayTask(void* pvParameters);
void vTimerTask(void* pvParameters);
void vControllerTask(void* pvParameters);
void vLeibnizTask(void* pvParameters);
void vNilakanthaTask(void* pvParameters);


#include "queue.h"
#include "event_groups.h"

EventGroupHandle_t egEventBits = NULL;
#define STRTSTP			0x01
#define RESET			0x02
#define	COMBINE			0x04
#define LEVEL			0x08
#define BREAK			0x10
#define ALGO			0x40
#define BUTTON			0xFF


#include "stack_macros.h"

#include "mem_check.h"

#include "init.h"
#include "utils.h"
#include "errorHandler.h"
#include "NHD0420Driver.h"

#include "ButtonHandler.h"

TaskHandle_t	leibniz;
TaskHandle_t	nilakantha;
TaskHandle_t	time;

uint16_t lot = 0;				//unsigned integer 16bit = lenght of time
float pi = 1;					//start var of calculating PI

//Initialize Main

int main(void)
{
	vInitClock();
	vInitDisplay();
	
	xTaskCreate(vControllerTask, (const char *) "control_tsk", configMINIMAL_STACK_SIZE+150, NULL, 3, NULL);			
	xTaskCreate(vDisplayTask, (const char *) "display_tsk", configMINIMAL_STACK_SIZE+100, NULL, 1, NULL);				
	xTaskCreate(vLeibnizTask, (const char *) "leibniz_tsk", configMINIMAL_STACK_SIZE, NULL, 0, &leibniz);				
	xTaskCreate(vNilakanthaTask, (const char *) "nilakantha_tsk", configMINIMAL_STACK_SIZE, NULL, 0, &nilakantha);				
	xTaskCreate(vTimerTask, (const char *) "time_tsk", configMINIMAL_STACK_SIZE, NULL, 2, &time);						
	
	vDisplayClear();
	vDisplayWriteStringAtPos(0,0,"PI-Calc V2.0");
	vDisplayWriteStringAtPos(1,0,"appaie TS TSE 2009");
	vDisplayWriteStringAtPos(2,0,"Leibniz|Nilakantha");
	vDisplayWriteStringAtPos(3,0,"start|n/a|n/a|n/a");																		
	vTaskStartScheduler();																	
	return 0;
}

//Display Function

void vDisplayTask(void* pvParameters)
{
	char piact[12] = "";
	char Algorithm[15] = "";
	for (;;)
	{
		xEventGroupClearBits(egEventBits, BREAK);											
		xEventGroupWaitBits(egEventBits, COMBINE, false, true, portMAX_DELAY);			
		if (xEventGroupGetBits(egEventBits) & ALGO)
		{
			sprintf(Algorithm,"%.12s","Leibniz");
		}
		else
		{
			sprintf(Algorithm,"%.12s","Nilakantha");
		}
		sprintf(piact,"%.7f",pi);
																
		xEventGroupClearBits(egEventBits, COMBINE);								
		xEventGroupSetBits(egEventBits, BREAK);
													
		vDisplayClear();															
		vDisplayWriteStringAtPos(0,0,"Mode: %s", Algorithm);
		if (xEventGroupGetBits(egEventBits) & ALGO)
		{
			vDisplayWriteStringAtPos(1,0,"Time: %ds", lot/1000);
		}
		else
		{
			vDisplayWriteStringAtPos(1,0,"Time: %dms", lot);
		}					
		vDisplayWriteStringAtPos(2,0, "PI: %s", piact);								
		vDisplayWriteStringAtPos(3,0, "play|stp|reset|swtch");
									
		vTaskDelay(500);																
	}
}

//ButtonControlling Function

void vControllerTask(void* pvParameters)
{
	egEventBits = xEventGroupCreate();
	initButtons();																			
	for(;;)
	{
		updateButtons();
		if(getButtonPress(BUTTON1) == SHORT_PRESSED)
		{									
			xEventGroupSetBits(egEventBits, STRTSTP);									
			xEventGroupClearBits(egEventBits, LEVEL);										
			vTaskResume(time);
		}
		if(getButtonPress(BUTTON2) == SHORT_PRESSED)
		{																					
			xEventGroupClearBits(egEventBits, STRTSTP);									
		}
		if(getButtonPress(BUTTON3) == SHORT_PRESSED)
		{																					
			pi = 1;																			
			xEventGroupSetBits(egEventBits, RESET);
			xEventGroupClearBits(egEventBits, LEVEL);
			lot = 0;																		
			vTaskResume(time);
		}
		if(getButtonPress(BUTTON4) == SHORT_PRESSED)
		{																					
			xEventGroupSetBits(egEventBits, RESET);								
			xEventGroupClearBits(egEventBits, LEVEL);
			
			eTaskState state = eTaskGetState(leibniz);									
			if(state == eSuspended)
			{
				vTaskSuspend(nilakantha);
				vTaskResume(leibniz);
			}
			else
			{
				vTaskSuspend(leibniz);
				vTaskResume(nilakantha);
			}
			lot = 0;																		
			vTaskResume(time);
		}
		vTaskDelay(10);																		
	}
}

//Leibniz Function

void vLeibnizTask(void* pvParameters)
{																																					
	float help = 1;
	uint32_t n = 3;																			
	long compare = 0;
	const long pisix = 314159;															
	xEventGroupSetBits(egEventBits, ALGO);											
	for(;;)
	{
		if (xEventGroupGetBits(egEventBits) & RESET)
		{																				
			help =1;
			n = 3;
			xEventGroupSetBits(egEventBits, ALGO);								
			xEventGroupClearBits(egEventBits, 0x03);										
			pi = 1;
			xEventGroupSetBits(egEventBits, COMBINE);
		}
		if(xEventGroupGetBits(egEventBits) & STRTSTP)
		{
			if(xEventGroupGetBits(egEventBits) & BREAK)
			{																			
				help = help - (1.0/n);											
				n = n + 2;																
				help = help + (1.0/n);
				pi = help*4;															
				n = n + 2;
			}
			else
			{
				xEventGroupSetBits(egEventBits, COMBINE);									
			}
		}
		compare = pi * 100000;															
		if (compare == pisix)
		{
			xEventGroupSetBits(egEventBits,LEVEL);										
		}
	}
}

//Nilakantha Function

void vNilakanthaTask(void* pvParameters)											//one of the fastest series to calculate PI		//https://www.hackster.io/momososo/nilakantha-series-89e939
{																				//https://www.geeksforgeeks.org/calculate-pi-using-nilkanthas-series/
	vTaskSuspend(nilakantha);
	/*double Pi = 0;*/
	float PIH = 0;
	double n = 2;
	double sign = 1;
	long compare = 0;																	
	const long pisix = 314159;															
	for(;;)
	{
		if (xEventGroupGetBits(egEventBits) & RESET)
		{																				
			sign = 1;																	
			n = 2;																		
			xEventGroupClearBits(egEventBits, ALGO);									
			xEventGroupClearBits(egEventBits, 0x03);									
			pi = 1;
			xEventGroupSetBits(egEventBits, COMBINE);									
		}
		if(xEventGroupGetBits(egEventBits) & STRTSTP)
		{
			if(xEventGroupGetBits(egEventBits) & BREAK)
			{
				PIH = PIH + (sign/(((n)*(n+1)*(n+2))));
				sign = sign * (-1);
				n += 2;
				pi = (PIH*4)+3;
			}
			else
			{
				xEventGroupSetBits(egEventBits, COMBINE);
			}
		}
		compare = pi * 100000;
		vTaskDelay(10);															//because of Nilakantha is so efficient the Timer Frequency doesnt keep up with the speet of wich PI is calculated    //thats why i put a delay inside, evade Idea create Counter that counts how many times the delay(100) is used and after the calculation is done subtract the counted run through
		
		if (compare == pisix)
		{
			xEventGroupSetBits(egEventBits,LEVEL);
		}
	}
}

//Timer Function

void vTimerTask(void* pvParameters)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 10;
	xLastWakeTime = xTaskGetTickCount();
	uint16_t strt = 0;
	uint16_t stp = 0;
	uint16_t pause = 0;
	uint16_t pause1 = 0;
	uint16_t pause10 = 0;
	int help = 0;
	for (;;)
	{
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		lot = 0;
		if (strt == 0) {
			xEventGroupWaitBits(egEventBits, STRTSTP, false, true, portMAX_DELAY);
			strt = xTaskGetTickCountFromISR();
			pause = 0;
		}
		if ((xEventGroupGetBits(egEventBits) & STRTSTP) != 1)
		{
			if (pause1 == 0)
			{
				pause1 = xTaskGetTickCountFromISR();
				help = 1;
			}
		}
		if (help == 1)
		{
			if ((xEventGroupGetBits(egEventBits) & STRTSTP))
			{
				pause10 = xTaskGetTickCountFromISR();
				pause = pause10 - pause1 + pause;
				help = 0;
				pause1 = 0;
				pause10 = 0;
			}
		}
		stp = xTaskGetTickCountFromISR();
		lot = stp - strt - pause;
		if (xEventGroupGetBits(egEventBits) & LEVEL)
		{
			strt = 0;
			stp = 0;
			pause = 0;
			pause1 = 0;
			pause10 = 0;
			vTaskSuspend(time);
		}
		else if (xEventGroupGetBits(egEventBits) & RESET)
		{
			strt = 0;
			stp = 0;
			pause = 0;
			pause1 = 0;
			pause10 = 0;
			help = 0;
			lot = 0;
			vTaskSuspend(time);
		}
		else
		{
			
		}
	}
}

