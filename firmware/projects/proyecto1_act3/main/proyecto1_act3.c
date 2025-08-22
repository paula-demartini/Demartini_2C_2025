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
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/

/*==================[end of file]============================================*/

struct LEDS {

    int mode; //ON, OFF, TOGGLE
	int LED; //número de led
	int N; //cantidad de ciclos
	int T;  //periodo

};

enum {ON, OFF, TOGGLE}; //por default, le asigna 1,2 y 3 a ON, OFF y TOGGLE (así, con enum, me olvido del número y el orden de las instrucciones -uso directamente la etiqueta-)

void ManejaLEDS (struct LEDS *ptr){

	switch (ptr->mode) {

		case ON: //enciende LED
		LedOn(ptr->LED); //no es necesario anidar switches
		
/*		switch (ptr->LED) {

			case 1: //LED 1
			LedOn(1)

			break;

			case 2: //LED 2
			LedOn(2)

			break;

			case 3: //LED 3
			LedOn(3)

			break;
		} */

		break;

		case OFF: //apaga LED
        LedOff(ptr->LED);

		break;

		case TOGGLE: //parpadea LED
		for (int i=0; i<2*(ptr->N); i++) { //tengo que multiplicar la cantidad de ciclos;; also, i=0 porque la condición es estrictamente menor (para i=N=10 ya no lo recorre al for)
			LedToggle(ptr->LED);
			vTaskDelay(ptr->T/2 / portTICK_PERIOD_MS);
		}
		break;

	}

}

//
void app_main(void){
	LedsInit();
	struct LEDS leds = {TOGGLE, LED_3, 10, 500}; //creo una estructura LED llamada leds 
	ManejaLEDS(&leds); //llamo a la función ManejaLEDS y le paso el puntero de leds	
	while (1)
	{

	}
}
	