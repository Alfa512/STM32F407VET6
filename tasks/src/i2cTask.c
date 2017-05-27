#include "i2cTask.h"
#include "stm32f4xx_rtc.h"
#include "time.h"

#define RTC_CLOCK_SOURCE_LSE 

StackType_t i2cTask_stack[I2C_TASK_STACK_SIZE] CCM_RAM;  // Put task stack in CCM
StaticTask_t i2cTask_buffer CCM_RAM;  // Put TCB in CCM

RTC_InitTypeDef RTC_InitStructure;
RTC_TimeTypeDef RTC_TimeStructure;
RTC_DateTypeDef RTC_DateStructure;
RTC_TimeTypeDef RTC_TimeStampStructure;
RTC_DateTypeDef RTC_TimeStampDateStructure;

uint32_t uwAsynchPrediv = 0;
uint32_t uwSynchPrediv = 0; 
uint32_t uwSecondfraction = 0;
uint8_t errorFlag = 0;
uint8_t accessFlag = 1;

char * errorMessage = "ERROR";
char * MPUData = "S.T.A.L.K.E.R.";
char currentTimeString[100];
char * myName = "by ALFA";
uint32_t trashInt = 10;

void i2cTaskBefore(void) {
    SSD1306_Init();
    RTC_Config();
}

uint8_t received_data[2];

void i2cUpdateScreen()
{
  while(1)
  {
    if (accessFlag == 1)
    {
      accessFlag = 0;
      SSD1306_UpdateScreen();
      accessFlag = 1;
      break;
    }
    vTaskDelay(trashInt);
  }
  
}

void i2cTaskMain(void* dummy) {

	/* main program loop */
	while(!errorFlag) {
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
    sprintf(currentTimeString, "%d:%d:%d", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
    SSD1306_Fill(SSD1306_COLOR_BLACK);
    SSD1306_GotoXY(10, 10);
    SSD1306_Puts(currentTimeString, &Font_11x18, SSD1306_COLOR_WHITE);
    //SSD1306_DrawCircle(15, 20, 15, SSD1306_COLOR_WHITE);
    SSD1306_DrawCircle(105, 37, 10, SSD1306_COLOR_WHITE);
    SSD1306_GotoXY(12, 32);

    sprintf(currentTimeString, "%d:%d:%d", RTC_DateStructure.RTC_Date, RTC_DateStructure.RTC_Month, RTC_DateStructure.RTC_Year);
    SSD1306_Puts(currentTimeString, &Font_11x18, SSD1306_COLOR_WHITE);
    //SSD1306_Puts(myName, &Font_11x18, SSD1306_COLOR_WHITE);

    SSD1306_GotoXY(12, 53);
    SSD1306_Puts(MPUData, &Font_7x10, SSD1306_COLOR_WHITE);
    //SSD1306_DrawRectangle(10, 27, 82, 25, SSD1306_COLOR_WHITE);
    i2cUpdateScreen();
    vTaskDelay(trashInt);
	}

  while(1);

  /* never reached */
  vTaskDelete(NULL);
}

void i2cSetTimeModifier(char* modifier) {
  SSD1306_GotoXY(100, 10);
  SSD1306_Puts(modifier, &Font_7x10, SSD1306_COLOR_WHITE);
  i2cUpdateScreen();
}

void i2cSetMPUData(char* modifier) {
  MPUData = modifier;
}


void i2cTaskAfter(void) {
}

void RTC_Config(void)
{
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);
 
#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
/* The RTC Clock may varies due to LSI frequency dispersion. */
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);

  /* Wait till LSI is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  
  /* ck_spre(1Hz) = RTCCLK(LSI) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
  uwSynchPrediv = 0xFF;
  uwAsynchPrediv = 0x7F;

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
  }

  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  /* ck_spre(1Hz) = RTCCLK(LSE) /(uwAsynchPrediv + 1)*(uwSynchPrediv + 1)*/
  uwSynchPrediv = 0xFF;
  uwAsynchPrediv = 0x7F;
    
#else
  #error Please select the RTC Clock source inside the main.c file
#endif /* RTC_CLOCK_SOURCE_LSI */

   /* Configure the RTC data register and RTC prescaler */
  RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
  RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
  
  /* Check on RTC init */
  if (RTC_Init(&RTC_InitStructure) == ERROR)
  {
    SSD1306_Puts(errorMessage, &Font_16x26, SSD1306_COLOR_WHITE);
    SSD1306_UpdateScreen();
    errorFlag = 1;
  }
  
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
  
  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();
  
  /* Enable The TimeStamp */
  RTC_TimeStampCmd(RTC_TimeStampEdge_Falling, ENABLE);    
}