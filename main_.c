#include "FreeRTOS.h"
#include "task.h"
#include "basic_io.h"
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>


int32_t driver_0, driver_1, driver_2, driver_3 = 0;
uint8_t delayMult = 1;
/*
 1 - O protótipo da função de uma tarefa deve sempre retornar void, ou seja, a tarefa não possui retorno.
 2 - A função de uma tarefa deve receber um parâmetro de ponteiro void. 
*/
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTask3(void *pvParameters);
void vTask4_prints(void *pvParameters);

/*
 Denição da estrutura da função
*/

// definição da task1 (guinada)
void vTask1(void *pvParameters)
{
    // comparador que vai ser utilizado para a guinada 
    const uint8_t isHorario = 0b001;
    
    // Recebe o parametro da função como um inteiro de 64 bits (exageiro demais)
    uint64_t param = (uint64_t) pvParameters;

    // for infinito
    for(;;)
    {
        vPrintString("Guinada\n");

        // verifica se isHorario esta settado, caso não esteja, é anti-horario
        if(isHorario&param){
            driver_0++;
            driver_1--;
            driver_2++;
            driver_3--;
        } else {
            driver_0--;
            driver_1++;
            driver_2--;
            driver_3++;
        }

        // delay de 10ms, o delayMult é para auxiliar durante o debug das tasks
        vTaskDelay(10*delayMult);
    }

    // quando sair do for (ctrl-C), deleta a task
    vTaskDelete(NULL);
}

// mesma coisa da vTask1
void vTask2(void* pvParameters)
{
    const uint8_t isFront = 0b010;
    uint64_t param = (uint64_t) pvParameters;

    for(;;)
    {
        vPrintString("Arfagem\n");
        if(isFront&param){
            driver_0--;
            driver_1--;
            driver_2++;
            driver_3++;
        } else {
            driver_0++;
            driver_1++;
            driver_2--;
            driver_3--;
        }
        vTaskDelay(40*delayMult);
    }
	vTaskDelete(NULL);
}

// mesma coisa da vTask1
void vTask3(void* pvParameters)
{
    const uint8_t isLeft = 0b100;
    uint64_t param = (uint64_t) pvParameters;

    for(;;)
    {
        vPrintString("Rolagem\n");
        if(isLeft&param){
            driver_0++;
            driver_1--;
            driver_2--;
            driver_3++;
        } else {
            driver_0--;
            driver_1++;
            driver_2++;
            driver_3--;
        }
        vTaskDelay(20*delayMult);
    }
	vTaskDelete(NULL);
}

// task para printar o estado do param e dos motores
void vTask4_prints(void *pvParameters){
    for(;;){
        printf("param:[%"PRIu64"]\n", (uint64_t)pvParameters);
        printf("drive_0:[%"PRId32"]\n", driver_0);
        printf("drive_1:[%"PRId32"]\n", driver_1);
        printf("drive_2:[%"PRId32"]\n", driver_2);
        printf("drive_3:[%"PRId32"]\n\n", driver_3);

        // faz parse do parametro para um unsigned int de 64 bits, inverte os bits dele e da parte para um pointeiro para void, e modifica o valor do ponteiro usado para passar o parametro
        pvParameters = (void*) ~(uint64_t)pvParameters;
        vTaskDelay(10*delayMult);
    }
    vTaskDelete(NULL);
}

int main_(void)
{
    uint8_t orientacao_manobras = 0b111;
	xTaskCreate(vTask1, "Guinada", 1000, (void*)orientacao_manobras, 1, NULL);
	xTaskCreate(vTask2, "Arfagem", 1000, (void*)orientacao_manobras, 1, NULL);
	xTaskCreate(vTask3, "Rolagem", 1000, (void*)orientacao_manobras, 1, NULL);
	xTaskCreate(vTask4_prints, "Prints", 1000, (void*)orientacao_manobras, 1, NULL);

	// Inicia o escalonador de tarefas
	vTaskStartScheduler();

	for (;;);
	return 0;
}
