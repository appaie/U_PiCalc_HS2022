/*
 * U_PiCalc_HS2022.c
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
void vNilkanthaTask(void* pvParameters);


#include "queue.h"
#include "event_groups.h"

EventGroupHandle_t egEventBits = NULL;
#define STARTSTOPP		0x01
#define RESET_SHORT		0x02
#define PI_COLLECT		0x04
#define PI_EVEN			0x08
#define BREAK			0x10
#define ALGORITHMUS		0x40
#define BUTTON_ALL		0xFF


#include "stack_macros.h"

#include "mem_check.h"

#include "init.h"
#include "utils.h"
#include "errorHandler.h"
#include "NHD0420Driver.h"

#include "ButtonHandler.h"

TaskHandle_t	leibniz;
TaskHandle_t	nilkantha;
TaskHandle_t	zeit;

void vLeibnizTask(void* pvParameters)
{																							// Berechnung Pi/4																// initial state of vWallisschesTask Task shall be off
	float help = 1;
	uint32_t n = 3;																			// Nenner Leibnizzahl startwert; unsigned int 32Bit
	long compare = 0;
	const long pisix = 314159;															// Variable für zum vergleichen
	xEventGroupSetBits(egEventBits, ALGORITHMUS);											// Anfangs setzen, da dieser Anfangs nicht suspendet ist, damit richiges Angezeigt wird
	for(;;)
	{
		if (xEventGroupGetBits(egEventBits) & RESET_SHORT)
		{																					// Rücksetzaufgabe
			help =1;
			n = 3;
			xEventGroupSetBits(egEventBits, ALGORITHMUS);									// in der Resetfunktion, damit nicht jeder Durchlauf gesetzt wird
			xEventGroupClearBits(egEventBits, 0x03);										// Bits STARTSTOPP & RESET_SHORT Clearen
			pi = 1;
			xEventGroupSetBits(egEventBits, PI_COLLECT);
		}
		if(xEventGroupGetBits(egEventBits) & STARTSTOPP)
		{
			if(xEventGroupGetBits(egEventBits) & BREAK)
			{																				// wenn Pausenbit gesetzt dann rechnen erlaubt
				help = help - (1.0/n);												// 1.0 nötig dass es float ist, bei nur 1 istes Int
				n = n + 2;																// aufzählen da so Algorithmus funktioniert
				help = help + (1.0/n);
				pi = help*4;															// Rundenwert an Pi weitergeben für darstellung
				n = n + 2;
			}
			else
			{
				xEventGroupSetBits(egEventBits, PI_COLLECT);									// Wenn nicht am Berechnen/Break gesetzt, Freigabe für Datenabholung and DisplayTask geben
			}
		}
		compare = pi * 100000;															//damit die Nötigen Stellen ohne Komma nachfolgend verglichen werden können (long int schneidet nach Komma ab)
		if (compare == pisix)
		{
			xEventGroupSetBits(egEventBits,PI_EVEN);										// Bit setzen für übereinstimmung der Pi Werte
		}
	}
}