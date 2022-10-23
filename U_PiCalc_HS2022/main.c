/*
 * U_PiCalc_HS2022.c
 *
 * Created: 20.03.2018 18:32:07
 * Author : -
 */ 

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "avr_compiler.h"
#include "pmic_driver.h"
#include "TC_driver.h"
#include "clksys_driver.h"
#include "sleepConfig.h"
#include "port_driver.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "stack_macros.h"

#include "mem_check.h"

#include "init.h"
#include "utils.h"
#include "errorHandler.h"
#include "NHD0420Driver.h"

#include "ButtonHandler.h"


void controllerTask(void* pvParameters);

int main(void)
{
	vInitClock();
	vInitDisplay();
	
	xTaskCreate( controllerTask, (const char *) "control_tsk", configMINIMAL_STACK_SIZE+150, NULL, 3, NULL);

	vDisplayClear();
	vDisplayWriteStringAtPos(0,0,"PI-CALC V1.0");
	vDisplayWriteStringAtPos(1,0,"MODE: IDLE");
	
	vTaskStartScheduler();
	return 0;

}

void controllerTask(void* pvParameters) {
	initButtons();
	for(;;) {
		updateButtons();
		if(getButtonPress(BUTTON1) == SHORT_PRESSED) {
			char pistring[12];
			sprintf(&pistring[0], "PI: %.8f", M_PI);
			vDisplayWriteStringAtPos(3,0, "%s", pistring);
		}
		if(getButtonPress(BUTTON2) == SHORT_PRESSED) {
			
		}
		if(getButtonPress(BUTTON3) == SHORT_PRESSED) {
			
		}
		if(getButtonPress(BUTTON4) == SHORT_PRESSED) {
			
		}
		if(getButtonPress(BUTTON1) == LONG_PRESSED) {
			
		}
		if(getButtonPress(BUTTON2) == LONG_PRESSED) {
			
		}
		if(getButtonPress(BUTTON3) == LONG_PRESSED) {
			
		}
		if(getButtonPress(BUTTON4) == LONG_PRESSED) {
			
		}
		vTaskDelay(10/portTICK_RATE_MS);
	}
}

void leibniztask(void* pvParameters) {
	float pi4 = 1;
	float pi = 0;
	uint32_t n = 3;
	for(;;){
		pi4 = pi4 -1.0/n + 1.0/(n+2);
		n = n+4;
		int piCalculatet;
		piCalculatet = pi4 * 4;
		
	}
}


/* Function to calculate PI
double calculatePI(double PI, double n,
double sign)
{
	// Add for 1000000 terms
	for (int i = 0; i <= 1000000; i++) {
		PI = PI + (sign * (4 / ((n) * (n + 1)
		* (n + 2))));
		
		// Addition and subtraction
		// of alternate sequences
		sign = sign * (-1);
		
		// Increment by 2 according to formula
		n += 2;
	}
	
	// Return the value of Pi
	return PI;
}

// Driver code
//void main()
//{
	
	// Initialize sum=3, n=2, and sign=1
	//double PI = 3, n = 2, sign = 1;
	
	// Function call
	//printf("The approximation of Pi is %0.8lf\n",calculatePI(PI, n, sign));
} */