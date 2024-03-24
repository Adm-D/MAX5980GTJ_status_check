#include "main.h"
#include "stm32f1xx_it.h"

uint16_t status_check_delay;

void NMI_Handler(void)
{
  while (1){}
}

void HardFault_Handler(void)
{
  while (1){}
}

void MemManage_Handler(void)
{
  while (1){}
}

void BusFault_Handler(void)
{
  while (1){}
}

void UsageFault_Handler(void)
{
  while (1){}
}

void SysTick_Handler(void)
{
  status_check_delay++;
}
