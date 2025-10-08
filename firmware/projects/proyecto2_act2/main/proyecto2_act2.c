/*! @mainpage Proyecto 2: actividad 2
 * @section genDesc General Description
 *
 * Se implementa la actividad 1 pero ahora con interrupciones y timer.
 *
 * @section hardConn Hardware Connection
 *
 * |    LCD         |   EDU-ESP   	|
 * |:--------------:|:--------------|
 * | 	D1  	 	| 	GPIO_20		|
 * | 	D2  	 	| 	GPIO_21		|
 * | 	D3  	 	| 	GPIO_22		|
 * | 	D4  	 	| 	GPIO_23		|
 * | 	SEL_1	 	| 	GPIO_19		|
 * | 	SEL_2	 	| 	GPIO_18		|
 * | 	SEL_3	 	| 	GPIO_9		|
 * | 	+5V 	 	|  	+5V     	|
 * | 	GND 	 	| 	GND 		|
 * 
 * |    HC-SR04     |   EDU-ESP   	|
 * |:--------------:|:--------------|
 * | 	ECHO 	 	| 	GPIO_3		|
 * | 	TRIGGER	 	| 	GPIO_2		|
 * | 	+5V 	 	| 	+5V 		|
 * | 	GND 	 	| 	GND     	|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2025 | Document creation		                         |
 *
 * @author Demartini Paula (paula.demartini@ingenieria.uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "gpio_mcu.h"
#include "hc_sr04.h"
#include "lcditse0803.h"
#include "switch.h"
#include "timer_mcu.h"
/*==================[macros and definitions]=================================*/

/** @def timerPeriod_us
* @brief Periodo del timer en [us]
*/
#define timerPeriod_us 1000000 //1 segundo

/** @def HOLD
* @brief Booleano para congelar el display
*/
bool HOLD=true;

/** @def CONTROL
* @brief Booleano de control para detener o reanudar la medición
*/
bool CONTROL=true;

/** @def vector_LEDS
* @brief Vector de LEDS
*/
led_t vector_LEDS[3]={LED_1, LED_2, LED_3}; //vector de led_t

/*==================[internal data definition]===============================*/

TaskHandle_t mideDistancia_task_handle = NULL; //porque la tarea técnicamente todavía no existe

timer_config_t timer = { 

	.timer = TIMER_A,
	.period = timerPeriod_us,
	.func_p = notifMaker,
	.param_p = NULL,

};

//idk por qué la otra sintaxis no funciona

/*==================[internal functions declaration]=========================*/

/** @fn static void mideDistancia_Task(void)
* @brief Tarea que enciende los leds según la distancia medida y muestra por display
* @return void
*/
static void mideDistancia_Task(void);

/** @fn void notifMaker(void)
* @brief Función que crea las notificaciones para las tareas
* @return void
*/
void notifMaker(void* param); //declarar antes de definir -así el timer encuentra la función-

/** @fn void atiendeHold(void)
* @brief Tarea que atiende la interrupción de tecla 2 (HOLD)
* @return void
*/
static void atiendeHold (void);

/** @fn void atiendeControl(void)
* @brief Tarea que atiende la interrupción de tecla 1 (CONTROL)
* @return void
*/
static void atiendeControl (void);

static void mideDistancia_Task(void){

	while (1) {

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //espera la notificación

		if (CONTROL) {

			uint16_t distancia=HcSr04ReadDistanceInCentimeters(); //mide

			if (HOLD) {
				LcdItsE0803Write(distancia); //muestra por display
			}
		
			if (distancia<=10) {

				for (int i=0 ; i<3 ; i++) {
					LedOff(vector_LEDS[i]);
				}

			} else if (10<distancia && distancia<=20) {

				for (int i=1 ; i<3 ; i++) {
					LedOff(vector_LEDS[i]);
				}
				LedOn(vector_LEDS[0]);

			} else if (20<distancia && distancia<=30) {

				for (int i=0 ; i<2 ; i++) {
					LedOn(vector_LEDS[i]);
				}
				LedOff(vector_LEDS[2]);

			} else { //distancia>30

				for (int i=0 ; i<3 ; i++) {
					LedOn(vector_LEDS[i]);
				}
			}

		}


	}

}

void notifMaker(void* param) {
    vTaskNotifyGiveFromISR(mideDistancia_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada (pasarle el handle, no el nombre de la función)*/
}

//Interrupciones

static void atiendeControl (void) {

	CONTROL=!CONTROL;

}

static void atiendeHold (void) {
	
	HOLD=!HOLD;

}


void app_main(void){
    LedsInit(); //inicializa los leds
	LcdItsE0803Init(); //inicializa los displays
	HcSr04Init(GPIO_3, GPIO_2); //inicializa el sensor
	SwitchesInit(); //inicializa las teclas
	TimerInit(&timer); //inicializa el timer
	TimerStart(timer.timer); //arranca el timer

//las interrupciones también podrían llamarse dentro de la definición de la tarea, antes del bucle

	SwitchActivInt(SWITCH_1, atiendeControl, NULL); //interrupción que atiende la tecla 1 (CONTROL) //&atiendeControl?
	SwitchActivInt(SWITCH_2, atiendeHold, NULL); //interrupción que atiende la tecla 2 (HOLD)
    xTaskCreate(&mideDistancia_Task, "Mide la distancia, maneja los leds y muestra por display", 2048, NULL, 5, &mideDistancia_task_handle);

//las funciones que atienden las interrupciones no se llaman manualmente, lo hace el micro cuando se produce la interrupción

}

/*==================[external functions definition]==========================*/

/*==================[end of file]============================================*/