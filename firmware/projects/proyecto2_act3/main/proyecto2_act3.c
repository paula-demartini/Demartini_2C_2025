/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
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
#include "uart_mcu.h"

/*==================[macros and definitions]=================================*/

#define timerPeriod_us 1000000 //1 segundo

bool HOLD=true; 
bool CONTROL=true;
led_t vector_LEDS[3]={LED_1, LED_2, LED_3}; //vector de led_t

/*==================[internal data definition]===============================*/

TaskHandle_t UART_task = NULL;
TaskHandle_t mideDistancia_task_handle = NULL; //porque la tarea técnicamente todavía no existe

void notifMaker(void* param); //declarar antes de definir -así el timer encuentra la función-

timer_config_t timer = { 

	.timer = TIMER_A,
	.period = timerPeriod_us,
	.func_p = notifMaker,
	.param_p = NULL,

};

//idk por qué la otra sintaxis no funciona

/*==================[internal functions declaration]=========================*/

static void mideDistancia_Task(void){

	while (1) {

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //espera la notificación

		if (CONTROL) {

			uint16_t distancia=HcSr04ReadDistanceInCentimeters(); //mide
			UartSendString(UART_PC, (char *) UartItoa(distancia,10)); //no es necesario una tarea aparte para la transmisión serie
			UartSendString(UART_PC, " cm\r\n"); //enviar dos strings seguidos  con la función es como enviar uno solo

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

serial_config_t UART = {

	.port=UART_PC,
	.baud_rate=9600, //cómo calculo los bauds?
	.func_p=NULL,
	.param_p=NULL,

};

void app_main(void){
    LedsInit(); //inicializa los leds
	LcdItsE0803Init(); //inicializa los displays(?)
	HcSr04Init(GPIO_3, GPIO_2); //inicializa el sensor
	SwitchesInit(); //inicializa las teclas
	TimerInit(&timer); //inicializa el timer
	TimerStart(timer.timer); //arranca el timer
	UartInit(&UART); //inicializa la UART

//las interrupciones también podrían llamarse dentro de la definición de la tarea, antes del bucle

	SwitchActivInt(SWITCH_1, atiendeControl, NULL); //interrupción que atiende la tecla 1 (CONTROL)
	SwitchActivInt(SWITCH_2, atiendeHold, NULL); //interrupción que atiende la tecla 2 (HOLD)
    xTaskCreate(&mideDistancia_Task, "Mide la distancia, maneja los leds y muestra por display", 2048, NULL, 5, &mideDistancia_task_handle);

//las funciones que atienden las interrupciones no se llaman manualmente, lo hace el micro cuando se produce la interrupción

}

/*==================[external functions definition]==========================*/

/*==================[end of file]============================================*/

/*==================[internal functions declaration]=========================*/
