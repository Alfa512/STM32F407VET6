#ifndef _I2C_TASK_H_
#define _I2C_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_rtc.h"
#include "ssd1306.h"
#include "stdio.h"
#include "string.h"

#include "ccm.h"

// Defines

#define I2C_TASK_STACK_SIZE 256

extern StackType_t i2cTask_stack[I2C_TASK_STACK_SIZE] CCM_RAM;  // Put task stack in CCM
extern StaticTask_t i2cTask_buffer CCM_RAM;  // Put TCB in CCM

// Task control functions
void i2cTaskBefore(void);
void i2cTaskMain(void*);
void i2cSetTimeModifier(char*);
void i2cSetMPUData(char*);
void i2cTaskAfter(void);

void RTC_Config(void);


#endif
