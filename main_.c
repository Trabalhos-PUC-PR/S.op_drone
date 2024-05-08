#include "FreeRTOS.h"
#include "task.h"
#include "basic_io.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "semphr.h"

const uint8_t MAX_SPEED = 10;

volatile SemaphoreHandle_t s_driver;
volatile SemaphoreHandle_t s_controls;
uint8_t orientacao_manobras = 0b011; 
volatile int8_t driver_0, driver_1, driver_2, driver_3 = 0;
uint8_t delayMult = 10;

/*
 1 - O protótipo da função de uma tarefa deve sempre retornar void, ou seja, a tarefa não possui retorno.
 2 - A função de uma tarefa deve receber um parâmetro de ponteiro void. 
*/
void vTask1_guinada(void *pvParameters);
void vTask2_arfagem(void *pvParameters);
void vTask3_rolagem(void *pvParameters);
void vTask4_control(void *pvParameters);
void vTask5_printer(void *pvParameters);

void acc_driver(volatile int8_t *driver);
void dec_driver(volatile int8_t *driver);

void printb(uint8_t v);
void printb(uint8_t v) {
    uint8_t i, s = 1<<((sizeof(v)<<3)-1);
    for (i = s; i; i>>=1) printf("%d", v & i || 0 );
}

/*
 Denição da estrutura da função
*/

// definição da task1 (guinada)
void vTask1_guinada(void *pvParameters)
{
    // Mascara utilizada para checagem do estado da ação (no caso, Guinada)
    const uint8_t isHorario = 0b001;
    
    // Recebe o parametro da função como um inteiro de 8 bits 
    volatile uint8_t* param =  pvParameters;

    // for infinito
    for(;;)
    {
        // verifica se isHorario está de acordo com a máscara, e sim, é Guinada em sentido horario, caso contrario, anti-horario
        xSemaphoreTake(s_driver, portMAX_DELAY);
        if(isHorario&(*param)){
            acc_driver(&driver_0);
            dec_driver(&driver_1);
            acc_driver(&driver_2);
            dec_driver(&driver_3);
        } else {
            dec_driver(&driver_0);
            acc_driver(&driver_1);
            dec_driver(&driver_2);
            acc_driver(&driver_3);
        }
        xSemaphoreGive(s_driver);

        // delay de 10ms, o delayMult é para auxiliar durante o debug das tasks
        vTaskDelay(10*delayMult);
    }

    // quando sair do for (ctrl-C), deleta a task
    vTaskDelete(NULL);
}

// mesma coisa da vTask1
void vTask2_arfagem(void* pvParameters)
{
    const uint8_t isFront = 0b010;
    volatile uint8_t* param = pvParameters;

    for(;;)
    {
        xSemaphoreTake(s_driver, portMAX_DELAY);
        if(isFront&(*param)){
            dec_driver(&driver_0);
            dec_driver(&driver_1);
            acc_driver(&driver_2);
            acc_driver(&driver_3);
        } else {
            acc_driver(&driver_0);
            acc_driver(&driver_1);
            dec_driver(&driver_2);
            dec_driver(&driver_3);
        }
        xSemaphoreGive(s_driver);
        vTaskDelay(40*delayMult);
    }
	vTaskDelete(NULL);
}

// mesma coisa da vTask1
void vTask3_rolagem(void* pvParameters)
{
    const uint8_t isLeft = 0b100;
    volatile uint8_t* param = pvParameters;

    for(;;)
    {
        xSemaphoreTake(s_driver, portMAX_DELAY);
        if(isLeft&(*param)){
            acc_driver(&driver_0);
            dec_driver(&driver_1);
            dec_driver(&driver_2);
            acc_driver(&driver_3);
        } else {
            dec_driver(&driver_0);
            acc_driver(&driver_1);
            acc_driver(&driver_2);
            dec_driver(&driver_3);
        }
        xSemaphoreGive(s_driver);
        vTaskDelay(20*delayMult);
    }
	vTaskDelete(NULL);
}

void vTask4_control(void *pvParameters){
    volatile uint8_t* param = pvParameters;
    for(;;)
    {
        xSemaphoreTake(s_driver, portMAX_DELAY);
        *param ^= (rand() % 8);
        xSemaphoreGive(s_driver);
        vTaskDelay(90*delayMult);
    }
	vTaskDelete(NULL);
}

void vTask5_printer(void *pvParameters){
    for(;;)
    {
        xSemaphoreTake(s_driver, portMAX_DELAY);
        printf("param[%d]\n", orientacao_manobras%8);
        printf("Motores:\n\t0:[%d]\t1:[%d]\n\n\t2:[%d]\t3:[%d]\n", driver_0, driver_1, driver_2, driver_3);
        xSemaphoreGive(s_driver);
        vTaskDelay(100*delayMult);
    }
	vTaskDelete(NULL);
}

void acc_driver(volatile int8_t *driver){
    if(*driver < MAX_SPEED){
        *driver+=1;
    }
    return;
}
void dec_driver(volatile int8_t *driver){
    if(*driver > -MAX_SPEED){
        *driver-=1;
    }
    return;
}

int main_(void)
{
    vSemaphoreCreateBinary(s_driver);

	xTaskCreate(vTask1_guinada, "Guinada", 1000, &orientacao_manobras, 1, NULL);
	xTaskCreate(vTask2_arfagem, "Arfagem", 1000, &orientacao_manobras, 1, NULL);
	xTaskCreate(vTask3_rolagem, "Rolagem", 1000, &orientacao_manobras, 1, NULL);
	xTaskCreate(vTask4_control, "Control", 1000, &orientacao_manobras, 0, NULL);
	xTaskCreate(vTask5_printer, "Printer", 1000, NULL, 0, NULL);
    
	// Inicia o escalonador de tarefas
	vTaskStartScheduler();

	for (;;);
	return 0;
}
