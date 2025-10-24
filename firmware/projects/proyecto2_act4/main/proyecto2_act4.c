/*! @mainpage Proyecto 2: actividad 4
 * @section genDesc General Description
 *
 * Se digitaliza una señal analógica generada con un potenciómetro y se transmiten los resultados por UART a la PC. Luego, se convierte el archivo digital de un ECG en una señal analógica y se la visualiza en el osciloscopio.
 *
 * @section hardConn Hardware Connection
 * 
 * |  Potenciómetro |   EDU-ESP  |
 * |:--------------:|:-----------|
 * | 	Vcc 	 	| 	+3,3V 	 |
 * | 	Centro 	 	| 	CH1 	 |
 * | 	GND 	 	| 	GND      |
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
#include "gpio_mcu.h"
#include "analog_io_mcu.h"
#include "timer_mcu.h"
#include "uart_mcu.h"

/*==================[macros and definitions]=================================*/

/** @def timerPeriod_us
* @brief Periodo del timer en [us]
*/
#define timerPeriod_us 2000 //2 ms

/** @def timerPeriod_us
* @brief Periodo del timer en [us]
*/
#define timerPeriodECG_us 4000 //4 ms

/** @fn void notifMaker(void)
* @brief Función que crea la notificación para la tarea ADC
* @return void
*/
void notifyADC(void* param); //declarar antes de definir -así el timer encuentra la función-

/** @fn void notifMaker(void)
* @brief Función que crea la notificación para la tarea ECG
* @return void
*/
void notifyECG(void* param); 

/*==================[internal data definition]===============================*/

TaskHandle_t ADC_task_handle = NULL;
TaskHandle_t ECG_task_handle = NULL;

timer_config_t timer_ADC = { //contador para recoger muestras de la entrada (estado del pote)

	.timer = TIMER_A,
	.period = timerPeriod_us,
	.func_p = notifyADC,		
	.param_p = NULL,

};

timer_config_t timer_ECG = { //contador para recoger muestras de la entrada (estado del pote)

	.timer = TIMER_B,
	.period = timerPeriodECG_us,
	.func_p = notifyECG,		
	.param_p = NULL,

};

//timer_ADC es el timer para la recolección de datos de la entrada analógica y la conversión a digital 

analog_input_config_t ANALOG = {

	.input = CH1,			/*!< Inputs: CH0, CH1, CH2, CH3 */
	.mode = ADC_SINGLE,		/*!< Mode: single read or continuous read */
	.func_p = NULL,			/*!< Pointer to callback function for convertion end (only for continuous mode) */
	.param_p = NULL,			/*!< Pointer to callback function parameters (only for continuous mode) */
	.sample_frec = NULL,	/*!< Sample frequency min: 20kHz - max: 2MHz (only for continuous mode)  */

};

//los últimos tres parámetros creo que directamente s epuede obviar

serial_config_t UART = {

	.port=UART_PC,
	.baud_rate=115200, 
	.func_p=NULL,  //función que atiende una interrupción de la UART
	.param_p=NULL,

};

uint8_t ECG[] = {
17,17,17,17,17,17,17,17,17,17,17,18,18,18,17,17,17,17,17,17,17,18,18,18,18,18,18,18,17,17,16,16,16,16,17,17,18,18,18,17,17,17,17,
18,18,19,21,22,24,25,26,27,28,29,31,32,33,34,34,35,37,38,37,34,29,24,19,15,14,15,16,17,17,17,16,15,14,13,13,13,13,13,13,13,12,12,
10,6,2,3,15,43,88,145,199,237,252,242,211,167,117,70,35,16,14,22,32,38,37,32,27,24,24,26,27,28,28,27,28,28,30,31,31,31,32,33,34,36,
38,39,40,41,42,43,45,47,49,51,53,55,57,60,62,65,68,71,75,79,83,87,92,97,101,106,111,116,121,125,129,133,136,138,139,140,140,139,137,
133,129,123,117,109,101,92,84,77,70,64,58,52,47,42,39,36,34,31,30,28,27,26,25,25,25,25,25,25,25,25,24,24,24,24,25,25,25,25,25,25,25,
24,24,24,24,24,24,24,24,23,23,22,22,21,21,21,20,20,20,20,20,19,19,18,18,18,19,19,19,19,18,17,17,18,18,18,18,18,18,18,18,17,17,17,17,
17,17,17
};

/*==================[internal functions declaration]=========================*/

void notifyADC(void* param) {
    vTaskNotifyGiveFromISR(ADC_task_handle, pdFALSE);   /* Envía una notificación a la tarea asociada (pasarle el handle, no el nombre de la función)*/
};

void notifyECG(void* param) {
    vTaskNotifyGiveFromISR(ECG_task_handle, pdFALSE);   /* Envía una notificación a la tarea asociada (pasarle el handle, no el nombre de la función)*/
};

void static ADC_task (void *pvParameter) {
	
	uint16_t value_mv;

	while (1) {

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //espera la notificación (tarea queda en BLOCKING)
		AnalogInputReadSingle(CH1, &value_mv); //lee la entrada de CH1 y la convierte en un valor binario que guarda en value_mv
		UartSendString(UART_PC, (char *) UartItoa(value_mv,10)); 
		UartSendString(UART_PC, "\r\n"); 

	}

};

void static ECG_task (void *pvParameter) {

	static uint8_t i;
	
	while (1) {

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //espera la notificación (tarea queda en BLOCKING)
		AnalogOutputWrite(ECG[i]);
		i++;
		if (i == sizeof(ECG)) {
			i = 0;
		}

	}

};


/*==================[external functions definition]==========================*/

void app_main(void){

	AnalogInputInit(&ANALOG);
	AnalogOutputInit();
	TimerInit(&timer_ADC); 
	TimerStart(timer_ADC.timer); 
	TimerInit(&timer_ECG); 
	TimerStart(timer_ECG.timer); 
	UartInit(&UART);
	xTaskCreate(&ADC_task, "Mide la entrada analógica, la convierte a digital y la muestra por UART", 2048, NULL, 5, &ADC_task_handle);
	xTaskCreate(&ECG_task, "Lee los datos del ECG digital y los convierte a una señal analógica", 2048, NULL, 5, &ECG_task_handle);

}

/*==================[end of file]============================================*/