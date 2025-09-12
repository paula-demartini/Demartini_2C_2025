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

/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/


void Enciende_LEDS_por_distancia (void) {

	uint16_t distancia=HcSr04ReadDistanceInCentimeters(); //mide

	led_t vector_LEDS[3]={LED_1, LED_2, LED_3};

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



void app_main (void) {

	LedsInit();
	Enciende_LEDS_por_distancia();

}


/*==================[external functions definition]==========================*/

/*==================[end of file]============================================*/