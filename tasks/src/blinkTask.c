#include "blinkTask.h"
#include "i2cTask.h"
#include "MPU9250.h"

StackType_t blinkTask_stack[BLINK_TASK_STACK_SIZE] CCM_RAM;  // Put task stack in CCM
StackType_t blink2Task_stack[BLINK_TASK_STACK_SIZE] CCM_RAM;  // Put task stack in CCM
StaticTask_t blinkTask_buffer CCM_RAM;  // Put TCB in CCM
StaticTask_t blink2Task_buffer CCM_RAM;  // Put TCB in CCM
RTC_TimeTypeDef RTC_TimeStructure;


const TickType_t xDelay = 100 / portTICK_PERIOD_MS;

void blinkTaskBefore(void) {
  blinkTask_initGPIO();
}

void blinkTaskSlave(void* dummy)
{
    uint32_t trashInt = 100;

  for (;;) {
    trashInt+=50; 
    if(trashInt>500) {
      trashInt = 100;
    }
    GPIO_WriteBit(GPIOF, GPIO_Pin_9, Bit_SET);
    GPIO_WriteBit(GPIOF, GPIO_Pin_10, Bit_SET);
    vTaskDelay(trashInt);
    GPIO_WriteBit(GPIOF, GPIO_Pin_10, Bit_RESET);
    GPIO_WriteBit(GPIOF, GPIO_Pin_9, Bit_RESET);
    vTaskDelay(trashInt);
  }
  vTaskDelete(NULL);
}

void blinkTaskMain(void* dummy) {

  /*
  pc.baud(9600);  

  //Set up I2C
  i2c.frequency(400000);  // use fast (400 kHz) I2C  
  
  pc.printf("CPU SystemCoreClock is %d Hz\r\n", SystemCoreClock);   
  
  t.start(); 
    uint8_t whoami = mpu9250.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);  // Read WHO_AM_I register for MPU-9250
  pc.printf("I AM 0x%x\n\r", whoami); pc.printf("I SHOULD BE 0x71\n\r");

  */

//MPU9250(long clock, 0, BITS_DLPF_CFG_188HZ, BITS_DLPF_CFG_188HZ);


  uint32_t t_delay = 200;
  /* main program loop */
  

  for (;;) {
    //uint32_t d1 = GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_3);
    uint32_t s1 = GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_6);
    uint32_t s2 = GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_7);
    uint32_t s3 = GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_5);
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
    if(s1 > 0)
    {
        RTC_TimeStructure.RTC_Hours++;
        if(RTC_TimeStructure.RTC_Hours >= 24)
          RTC_TimeStructure.RTC_Hours = 0;
        RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
        i2cSetTimeModifier("H");
    }
    if(s2 > 0)
    {
        RTC_TimeStructure.RTC_Minutes++;
        if(RTC_TimeStructure.RTC_Minutes >= 60)
          RTC_TimeStructure.RTC_Minutes = 0;
        RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
        i2cSetTimeModifier("M");
    }
    if(s3 > 0)
    {
        RTC_TimeStructure.RTC_Seconds++;
        if(RTC_TimeStructure.RTC_Seconds >= 60)
          RTC_TimeStructure.RTC_Seconds = 0;
        RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
        i2cSetTimeModifier("S");
    }

//GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_SET);
    SPI_I2S_SendData(SPI1, 0x00); //Передаем байт 0x93 через SPI1
while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET) //Передатчик занят?
;

GPIO_WriteBit(GPIOA, GPIO_Pin_4, Bit_RESET);
    if (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == SET) {
    uint16_t d1 = SPI_I2S_ReceiveData(SPI1);
    //d1 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
    //d1 = GPIO_ReadInputData(GPIOA);
    char text[20];
sprintf(text,"%u", d1);
i2cSetMPUData(text);
}
else
{
  uint16_t d1 = 512;
    char text[20];
sprintf(text,"%u", d1);
i2cSetMPUData(text);
}
  //RTC_TimeStructure.RTC_Minutes = timeinfo->tm_min;
  //RTC_TimeStructure.RTC_Seconds = timeinfo->tm_sec;

//RTC_TimeStructure.RTC_Hours

    /* delay */
    vTaskDelay(t_delay);
  }

  /* never reached */
  vTaskDelete(NULL);
}

void blinkTaskAfter(void) {
}

//Private functions

void blinkTask_initGPIO(void) {
  GPIO_InitTypeDef GPIO_InitStruct;
  SPI_InitTypeDef SPI_InitStructure;
  /*USART_InitTypeDef USART_InitStruct;*/

  /*RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);*/

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

  // Настраиваем ноги SPI1 для работы в режиме альтернативной функции
 
  GPIO_InitStruct.GPIO_Pin = /*GPIO_Pin_10 | GPIO_Pin_11 |*/ GPIO_Pin_10;
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOF, &GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Pin = /*GPIO_Pin_10 | GPIO_Pin_11 |*/ GPIO_Pin_9;
  GPIO_Init(GPIOF, &GPIO_InitStruct);

  //PF6
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOF, &GPIO_InitStruct);
 
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOF, &GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOF, &GPIO_InitStruct);

  //PF2
  /*GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOF, &GPIO_InitStruct);*/

  //PF3
  /*GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
  
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOF, &GPIO_InitStruct);*/
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
 
  // Настраиваем ноги SPI1 для работы в режиме альтернативной функции
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
 
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_5 | GPIO_Pin_6;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
 
  //Заполняем структуру с параметрами SPI модуля
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //полный дуплекс
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // передаем по 8 бит
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low; // Полярность и
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge; // фаза тактового сигнала
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; // Управлять состоянием сигнала NSS программно
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; // Предделитель SCK
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; // Первым отправляется старший бит
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; // Режим - мастер
  SPI_Init(SPI1, &SPI_InitStructure); //Настраиваем SPI1
  SPI_Cmd(SPI1, ENABLE); // Включаем модуль SPI1....
 
  // Поскольку сигнал NSS контролируется программно, установим его в единицу
  // Если сбросить его в ноль, то наш SPI модуль подумает, что
  // у нас мультимастерная топология и его лишили полномочий мастера.
  SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);

  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
}
