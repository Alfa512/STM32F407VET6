#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include "stm32f4xx_conf.h"

#include "blinkTask.h"
#include "i2cTask.h"

#include "ccm.h"

int main(void) {
  SystemInit();
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

  blinkTaskBefore();
  xTaskCreateStatic(
    blinkTaskMain,
    "BLINK",
    BLINK_TASK_STACK_SIZE,
    NULL,
    1,
    blinkTask_stack,
    &blinkTask_buffer
  );

  xTaskCreateStatic(
    blinkTaskSlave,
    "BLINK2",
    BLINK_TASK_STACK_SIZE,
    NULL,
    1,
    blink2Task_stack,
    &blink2Task_buffer
  );

  i2cTaskBefore();
  xTaskCreateStatic(
    i2cTaskMain,
    "I2C",
    I2C_TASK_STACK_SIZE,
    NULL,
    1,
    i2cTask_stack,
    &i2cTask_buffer
  );

  vTaskStartScheduler();  // should never return

  for (;;);
}

void vApplicationTickHook(void) {
}

void vApplicationIdleHook(void) {
}

StaticTask_t xIdleTaskTCB CCM_RAM;
StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE] CCM_RAM;

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {

  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

static StaticTask_t xTimerTaskTCB CCM_RAM;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH] CCM_RAM;

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize) {
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
  *ppxTimerTaskStackBuffer = uxTimerTaskStack;
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName) {
  (void) pcTaskName;
  (void) pxTask;
  /* Run time stack overflow checking is performed if
     configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
     function is called if a stack overflow is detected. */
  taskDISABLE_INTERRUPTS();
  for(;;);
}

void vApplicationMallocFailedHook(void) {
  taskDISABLE_INTERRUPTS();
  for(;;);
}
