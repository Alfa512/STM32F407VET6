// Macro to use CCM (Core Coupled Memory) in STM32F4
#ifndef CCM_RAM
#define CCM_RAM __attribute__((section(".ccmram")))
#endif
