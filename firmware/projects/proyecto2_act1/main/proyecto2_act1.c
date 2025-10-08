/*! @mainpage Proyecto 2: actividad 1
 * @section genDesc General Description
 *
 * Se encienden los leds según los intervalos indicados y se muestra por display la medición del sensor de ultrasonido. Además, se implementan CONTROL y HOLD con tecla 1 y tecla 2 respectivamente.
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

/*==================[macros and definitions]=================================*/

/*==================[macros and definitions]=================================*/

/** @def HOLD
* @brief Booleano para congelar el display
*/
bool HOLD=true;

/** @def CONTROL
* @brief Booleano de control para detener o reanudar la medición
*/
bool CONTROL=true;

/*==================[internal data definition]===============================*/

TaskHandle_t mideDistancia_task_handle = NULL; 
TaskHandle_t revisaTeclas_task_handle = NULL;

/*==================[internal functions declaration]=========================*/

/** @fn static void mideDistancia_Task(void)
* @brief Tarea que enciende los leds según la distancia medida y muestra por display
* @return void
*/
static void mideDistancia_Task(void);

/** @fn static void revisaTeclas_task(void)
* @brief Tarea que revisa las teclas y modifica los booleanos de control HOLD y CONTROL según corresponda
* @return void
*/
static void revisaTeclas_task (void);

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

		vTaskDelay(1000/portTICK_PERIOD_MS); //refresco de medición cada 1s (en ms)

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

		vTaskDelay(250/portTICK_PERIOD_MS);
	}

}

void app_main(void){
    LedsInit(); //inicializa los leds
	LcdItsE0803Init(); //inicializa los displays(?)
	SwitchesInit(); //inicializa las teclas
	HcSr04Init(GPIO_3, GPIO_2); //inicializa el sensor
    xTaskCreate(&mideDistancia_Task, "Mide la distancia, maneja los leds y muestra por display", 2048, NULL, 5, &mideDistancia_task_handle);
    xTaskCreate(&revisaTeclas_task, "Revisa las teclas", 2048, NULL, 5, &revisaTeclas_task_handle);
}


/*==================[external functions definition]==========================*/

/*==================[end of file]============================================*/