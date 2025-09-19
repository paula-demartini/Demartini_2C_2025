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

/*==================[macros and definitions]=================================*/

bool HOLD=true;
bool CONTROL=true;

/*==================[internal data definition]===============================*/
TaskHandle_t mideDistancia_task_handle = NULL; //??
TaskHandle_t revisaTeclas_task_handle = NULL;
/*==================[internal functions declaration]=========================*/

static void mideDistancia_Task(void){

	while (1) {

		if (CONTROL) {

			uint16_t distancia=HcSr04ReadDistanceInCentimeters(); //mide
			led_t vector_LEDS[3]={LED_1, LED_2, LED_3}; //vector de led_t

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

		vTaskDelay(150/portTICK_PERIOD_MS);

	}

}

static void revisaTeclas_task (void) {

	while (1) {

		uint16_t teclas=SwitchesRead();

		switch (teclas) {

			case SWITCH_1:

			CONTROL=!CONTROL;

			break;

			case SWITCH_2:
			
			HOLD=!HOLD;

			break;

		}

		vTaskDelay(25/portTICK_PERIOD_MS);
	}

}



void app_main(void){
    LedsInit(); //inicializa los leds
	LcdItsE0803Init(); //inicializa los displays(?)
	HcSr04Init(GPIO_3, GPIO_2); //inicializa el sensor
    xTaskCreate(&mideDistancia_Task, "Mide la distancia, maneja los leds y muestra por display", 2048, NULL, 5, &mideDistancia_task_handle);
    xTaskCreate(&revisaTeclas_task, "Revisa las teclas", 2048, NULL, 5, &revisaTeclas_task_handle);
}



/*==================[external functions definition]==========================*/

/*==================[end of file]============================================*/