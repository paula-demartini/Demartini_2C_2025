/*! @mainpage Proyecto 2: actividad 4
 * @section genDesc General Description
 *
 * Se digitaliza una señal analógica generada con un potenciómetro y se transmiten los resultados por UART a la PC. Luego, se convierte el archivo digital de un ECG en una señal analógica y se la visualiza en el osciloscopio.
 *
 * @section hardConn Hardware Connection
 * 
 * |  Potenciómetro |   EDU-ESP  |
 * |:--------------:|:-----------|
 * | 	+3,3V 	 	| 	+3,3V 	 |
 * | 	Middle 	 	| 	CH1 	 |
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

/** @fn void notifMaker(void)
* @brief Función que crea las notificaciones para las tareas
* @return void
*/
void notifMaker(void* param); //declarar antes de definir -así el timer encuentra la función-

/*==================[internal data definition]===============================*/

TaskHandle_t ADC_task_handle = NULL;

timer_config_t timer_ADC = { //contador para recoger muestras de la entrada (estado del pote)

	.timer = TIMER_A,
	.period = timerPeriod_us,
	.func_p = notifMaker,		
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

/*==================[internal functions declaration]=========================*/

void notifMaker(void* param) {
    vTaskNotifyGiveFromISR(ADC_task_handle, pdFALSE);   /* Envía una notificación a la tarea asociada (pasarle el handle, no el nombre de la función)*/
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


/*==================[external functions definition]==========================*/

void app_main(void){

	AnalogInputInit(&ANALOG);
	TimerInit(&timer_ADC); 
	TimerStart(timer_ADC.timer); 
	UartInit(&UART);
	xTaskCreate(&ADC_task, "Mide la entrada analógica, la convierte a digital y la muestra por UART", 2048, NULL, 5, &ADC_task_handle);

}

/*==================[end of file]============================================*/