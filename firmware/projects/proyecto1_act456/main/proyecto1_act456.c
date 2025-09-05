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
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

void  conversor_a_BCD (uint32_t data, uint8_t digits, uint8_t *BCD_digitos) //BCD_digits es el puntero a un vector vacío [0,1,2]

/*Función que recibe en una variable de 32 bits el número a convertir a BCD en binario natural, la cantidad de dìgitos 
y el puntero que guarda la dirección de memoria de la estructura que va a almacenar dichos dígitos*/

{

	for (int i = 0; i<digits; i++) {
 
		BCD_digits[i] = data%10; //se queda con el resto de la división por 10 (dividimos por 10 para convertir de natural a BCD)
		data = data/10; //se queda con el resultado de la divisón entera por dios

/*		int centenas = (data/100)%10  ;
		int decenas = (data/10)%10  ;
		int unidades = data %10 ;
*/

	}

} //guarda los números al revés, es decir, si llega el 123 en el vector queda [3,2,1]

struct GPIO_DATA {

	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/

} ;

/* numero ya está en BCD */

void  distribuye_num (int8_t numero, struct GPIO_DATA *vector) { //vector es un vector de punteros a con las direcciones de memoria a 4 estructuras del tipo GPIO que guardan el pin y el estado de dicho pin (entrada)

	int8_t filtro = 00000001;

	for (int i = 0, i<4 ; i++) {

		int b = numero & filtro; //en b queda el bit de interés (analiza de a uno)
		numero = numero/2;

		if (b==1) {

			GPIOOn(vector[i].pin);

		} else { //b sólo puede ser cero o uno

			GPIOOff(vector[i].pin);

		}

	}

}

struct GPIO_DATA vector[4]; //definición del vector de estructuras

vector[0].pin=GPIO_20;
vector[0].dir=GPIO_OUTPUT;
vector[1].pin=GPIO_21;
vector[1].dir=GPIO_OUTPUT;
vector[2].pin=GPIO_22;
vector[2].dir=GPIO_OUTPUT;
vector[3].pin=GPIO_23;
vector[3].dir=GPIO_OUTPUT;

struct GPIO_DATA vector_LCD[3]; //definición del vector de estructuras para seleccionar el display

vector_LCD[0].pin=GPIO_19;
vector_LCD[0].dir=GPIO_OUTPUT;
vector_LCD[1].pin=GPIO_18;
vector_LCD[1].dir=GPIO_OUTPUT;
vector_LCD[2].pin=GPIO_9;
vector_LCD[2].dir=GPIO_OUTPUT;

/* vector1 es igual a vector de la función anterior y vector2 tiene las direcciones del display correspondiente */

void coordina_displays (int32_t dato, int8_t digitos, struct GPIO_DATA *vector1, struct GPIO_DATA *vector2) { 

	uint8_t ArregloBCD[digitos]; //define un vector vacío de enteros
	conversor_a_BCD (dato, digitos, ArregloBCD); //convierte a BCD
	for (i = 0; i<digitos; i++) {

		distribuye_num (ArregloBCD[i], vector1) //distribuye los 4 bits en las 4 entradas
		GPIOOn(vector2[i].pin); //lo prende y lo apaga; le da un pulso
		GPIOOff(vector2[i].pin);

	}

}

uint32_t numero_de_prueba=123; //lo interpreta en binario :)

void app_main(void) { 

	coordina_displays (numero_de_prueba, 3, vector, vector_LCD);

}


/*==================[external functions definition]==========================*/

/*==================[end of file]============================================*/




