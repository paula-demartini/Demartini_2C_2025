/*! @mainpage Proyecto 1: actividades 4, 5 y 6
 *
 * @section genDesc General Description
 *
 * Resolución de las actividades 4, 5 y 6 del primer proyecto de electrónica programable
 *
 * @section hardConn Hardware Connection
 *
 * |    Periférico  |   EDU-ESP   	|
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

/*==================[macros and definitions]=================================*/

/** @def numero_de_prueba 
* @brief Número de prueba (120)
*/
#define numero_de_prueba 120

/** @def vector 
* @brief Vector que guarda la información (pin y dir) de los GPIO de cada conversor BCD-7seg [D0, D1, D2 y D3]
*/
struct GPIO_DATA vector[4];

/** @def vector_LCD
* @brief Vector que guarda la información (pin y dir) de los GPIO de selección de los 3 conversores BCD-7seg [SEL_1, SEL_2, y SEL_3]
*/
struct GPIO_DATA vector_LCD[3];

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/** @fn void conversor_a_BCD(uint32_t data, uint8_t digits, uint8_t *BCD_digits)
* @brief Convierte el número que se le pasa en binario natural a BCD
* @param data Número binario a convertir
* @param digits Cantidad de dígitos de salida
* @param BCD_digits Arreglo donde se van a guardar dichos dígitos de salida
* @return void
*/
void  conversor_a_BCD (uint32_t data, uint8_t digits, uint8_t *BCD_digits);

/** @fn void distribuye_bits(int8_t numero, struct GPIO_DATA *vector)
* @brief Distribuye los bits de un BCD en las 4 entradas del conversor BCD-7seg
* @param numero Número en BCD
* @param vector Vector de estructuras GPIO_DATA para mapear los bits a la entrada del conversor
* @return
*/
void  distribuye_bits (int8_t numero, struct GPIO_DATA *vector);

/** @fn void coordina_displays (int32_t data, int8_t digits, struct GPIO_DATA *vector1, struct GPIO_DATA *vector2)
* @brief Reparte un máximo de 3 dígitos por display
* @param data Número en binario natural
* @param digits Cantidad de dígitos de salida
* @param vector1 Vector de estructuras GPIO_DATA para mapear los bits a la entrada del conversor 
* @param vector2 Vector de estructuras GPIO_DATA para selección el conversor
* @return
*/
void coordina_displays (int32_t data, int8_t digits, struct GPIO_DATA *vector1, struct GPIO_DATA *vector2);



void  conversor_a_BCD (uint32_t data, uint8_t digits, uint8_t *BCD_digits) //BCD_digits es el puntero a un vector vacío [ , , ] donde los elementos recibirán las posiciones o índices 0,1,2

/*Función que recibe en una variable de 32 bits el número en binario natural a convertir a BCD, la cantidad de dìgitos de salida 
y el puntero que guarda la dirección de memoria del vector (o arreglo) que va a almacenar dichos dígitos*/

{

	for (int i = 0; i<digits; i++) {
 
		BCD_digits[i] = data%10; //se queda con el resto de la división por 10 (dividimos por 10 para convertir de natural a BCD)
		data = data/10; //se queda con el resultado de la divisón entera por dos

	}

} //guarda los números al revés, es decir, si llega el 123 en el vector queda [3,2,1]

struct GPIO_DATA { //definición de la estructura

	gpio_t pin;			/*!< GPIO pin number (qué puerto)*/
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT (si el puerto es de entrada o salida)*/

};

struct GPIO_DATA vector[4]; //definición del vector de estructuras (entre corchetes el número de lugares o elementos)
/*Como tenemos los números en BCD son 4 bits por dígito */

void  distribuye_bits (int8_t numero, struct GPIO_DATA *vector) { //numero ya está en BCD

/*vector es un puntero al vector con 4 estructuras del tipo GPIO que guardan el pin y el estado de dicho pin (in/out)*/

	int8_t filtro = 00000001; //para la máscara (AND)

	for (int i = 0; i<4 ; i++) {

		int b = numero & filtro; //en b queda el bit de interés (analiza de a uno)
		numero = numero/2; //corre los bits de numero a la derecha, rellenando con ceros el MSB

		if (b==1) {

			GPIOOn(vector[i].pin);

		} else { //b sólo puede ser cero o uno

			GPIOOff(vector[i].pin);

		}

	}

}

struct GPIO_DATA vector_LCD[3]; //definición del vector de estructuras para seleccionar el display

/* vector1 es igual a vector de la función anterior y vector2 tiene las direcciones del display correspondiente */

void coordina_displays (int32_t data, int8_t digits, struct GPIO_DATA *vector1, struct GPIO_DATA *vector2) { 

/*vector1 es vector y vector2 es vector_LCD*/

	uint8_t ArregloBCD[digits]; //define un vector vacío de enteros con "digits" elementos
	conversor_a_BCD (data, digits, ArregloBCD); //convierte a BCD y guarda cada dígito en ArregloBCD

	for (int i = 0; i<digits; i++) {

		distribuye_bits (ArregloBCD[i], vector1); //distribuye los 4 bits de un dígito en las 4 entradas del display
		GPIOOn(vector2[i].pin); //lo prende y lo apaga; le da un pulso al display correspondiente para encenderlo
		GPIOOff(vector2[i].pin);

	}

}


void app_main(void) { 

	GPIOInit(GPIO_9, GPIO_OUTPUT);
	GPIOInit(GPIO_18, GPIO_OUTPUT);
	GPIOInit(GPIO_19, GPIO_OUTPUT);
	GPIOInit(GPIO_20, GPIO_OUTPUT);
	GPIOInit(GPIO_21, GPIO_OUTPUT);
	GPIOInit(GPIO_22, GPIO_OUTPUT);
	GPIOInit(GPIO_23, GPIO_OUTPUT);

	vector_LCD[0].pin=GPIO_19;
	vector_LCD[0].dir=GPIO_OUTPUT;
	vector_LCD[1].pin=GPIO_18;
	vector_LCD[1].dir=GPIO_OUTPUT;
	vector_LCD[2].pin=GPIO_9;
	vector_LCD[2].dir=GPIO_OUTPUT;

	vector[0].pin=GPIO_20; //carga en el campo pin de la estructura GPIO_DATA en el primer lugar "GPIO_20"
	vector[0].dir=GPIO_OUTPUT;
	vector[1].pin=GPIO_21;
	vector[1].dir=GPIO_OUTPUT;
	vector[2].pin=GPIO_22;
	vector[2].dir=GPIO_OUTPUT;
	vector[3].pin=GPIO_23;
	vector[3].dir=GPIO_OUTPUT;
	
//	uint32_t numero_de_prueba=120; //lo interpreta en binario :)
	coordina_displays (numero_de_prueba, 3, vector, vector_LCD);

}

/*==================[external functions definition]==========================*/

/*==================[end of file]============================================*/




