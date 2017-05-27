#ifndef _BLINK_TASK_H_
#define _BLINK_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_spi.h"
#include "ssd1306.h"
//#include "MPU9250.h"

#include "ccm.h"

// Defines

#define BLINK_TASK_STACK_SIZE 256

// Task control functions
void blinkTaskBefore(void);
void blinkTaskMain(void*);
void blinkTaskSlave(void*);
void blinkTaskAfter(void);


void blinkTask_initGPIO(void);

// Public functions

// Public variables

extern StackType_t blinkTask_stack[BLINK_TASK_STACK_SIZE] CCM_RAM;  // Put task stack in CCM
extern StackType_t blink2Task_stack[BLINK_TASK_STACK_SIZE] CCM_RAM;  // Put task stack in CCM
extern StaticTask_t blinkTask_buffer CCM_RAM;  // Put TCB in CCM
extern StaticTask_t blink2Task_buffer CCM_RAM;  // Put TCB in CCM

#endif
