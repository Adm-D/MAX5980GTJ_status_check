#include "main.h"

uint8_t ports_status[5];
extern uint16_t status_check_delay;
char uart_data[UART_BUF_LEN];

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART2_UART_Init(void);

int main(void)
{

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));

  LL_GPIO_AF_DisableRemap_SWJ();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C2_Init();
  MX_USART2_UART_Init();

  while (1)
  {
		/*For default state case (AUTO = 1, A0 = 1, A1 = 1, A2 = 1, A3 = 1)*/
		if(status_check_delay == 1000){
			/*Status registers reading from MAX5980GTJ_STATUS_REGISTER*/
			Handle_I2C_Master(MAX5980GTJ_STATUS_REGISTER, ports_status, sizeof(ports_status));
			/*Transmission of status registers by UART*/
			for(uint8_t i = 0; i != 4; ++i){
				/*Port status register, DET_ST (0Ch[2:0] - 0Fh[2:0])*/
				switch(ports_status[i]&0x07)
				{
					case 0x00: {snprintf(uart_data, UART_BUF_LEN, "PORT(%d) - Detection status unknown\n", i+1);SendBuf_UART2(uart_data);break;}
					case 0x01: {snprintf(uart_data, UART_BUF_LEN, "PORT(%d) - Positive DC supply connected at the port (V(AGND)-V(OUT)<1V)\n", i+1);SendBuf_UART2(uart_data);break;}
					case 0x02: {snprintf(uart_data, UART_BUF_LEN, "PORT(%d) - High capacitance at the port (>8.5μF(typ))\n", i+1);SendBuf_UART2(uart_data);break;}
					case 0x03: {snprintf(uart_data, UART_BUF_LEN, "PORT(%d) - Low resistance at the port (R(DET)<15kΩ)\n", i+1);SendBuf_UART2(uart_data);break;}
					case 0x04: {snprintf(uart_data, UART_BUF_LEN, "PORT(%d) - Detection pass (15kΩ>R(DET)>33kΩ)\n", i+1);SendBuf_UART2(uart_data);break;}
					case 0x05: {snprintf(uart_data, UART_BUF_LEN, "PORT(%d) - High resistance at the port (R(DET)>33kΩ)\n", i+1);SendBuf_UART2(uart_data);break;}
					case 0x06: {snprintf(uart_data, UART_BUF_LEN, "PORT(%d) - Open port (I(OUT)<10μA)\n", i+1);SendBuf_UART2(uart_data);break;}
					case 0x07: {snprintf(uart_data, UART_BUF_LEN, "PORT(%d) - Low impedance to V(EE) at the port (V(OUT)-V(EE)<2V)\n", i+1);SendBuf_UART2(uart_data);break;}
				}
				/*Port status register, CLASS (0Ch[6:4] - 0Fh[6:4])*/
				switch((ports_status[i]&0x70)>>4)
				{
					case 0x00: {snprintf(uart_data, UART_BUF_LEN, "PORT(%d) - Class unknown\n", i+1);SendBuf_UART2(uart_data);break;}
					case 0x06: {snprintf(uart_data, UART_BUF_LEN, "PORT(%d) - Class 0\n", i+1);SendBuf_UART2(uart_data);break;}
					case 0x07: {snprintf(uart_data, UART_BUF_LEN, "PORT(%d) - Current limit\n", i+1);SendBuf_UART2(uart_data);break;}
					default: 	 {snprintf(uart_data, UART_BUF_LEN, "PORT(%d) - Class %d\n", i+1, (ports_status[i]&0x70)>>4);SendBuf_UART2(uart_data);break;}
				}
				/*Power status register, PWR_EN bits (10h[3:0])*/
				if(ports_status[4]&(1<<i))
					{snprintf(uart_data, UART_BUF_LEN, "PORT(%d) - Power is enabled\n", i+1);SendBuf_UART2(uart_data);}
				else if(!(ports_status[4]&(1<<i)))
					{snprintf(uart_data, UART_BUF_LEN, "PORT(%d) - Power is disabled\n", i+1);SendBuf_UART2(uart_data);}
				/*Power status register, PGOOD bits (10h[7:4])*/
				if((ports_status[4]>>4)&(1<<i))
					{snprintf(uart_data, UART_BUF_LEN, "PORT(%d) - V(OUT)-V(EE)>PG(TH)\n", i+1);SendBuf_UART2(uart_data);}
				else if(!((ports_status[4]>>4)&(1<<i)))
					{snprintf(uart_data, UART_BUF_LEN, "PORT(%d) - V(OUT)-V(EE)≤PG(TH) or a fault condition occurs\n", i+1);SendBuf_UART2(uart_data);}
			}
		status_check_delay=0;
		} 
  }
}

void Pause_by_cycle(uint32_t duration){//(duration = 1000000) almost equals 0.5 sec
	for(uint32_t i = 0; i != duration; ++i){}
}

void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_0)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE)
  {

  }
  LL_Init1msTick(8000000);
  LL_SetSystemCoreClock(8000000);
}

static void MX_I2C2_Init(void)
{
	LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
  /**I2C2 GPIO Configuration
  PB10   ------> I2C2_SCL
  PB11   ------> I2C2_SDA
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10|LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

  /** I2C Initialization
  */
  LL_I2C_DisableOwnAddress2(I2C2);
  LL_I2C_DisableGeneralCall(I2C2);
  LL_I2C_EnableClockStretching(I2C2);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.ClockSpeed = 100000;
  I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C2, &I2C_InitStruct);
  LL_I2C_SetOwnAddress2(I2C2, 0);
}

static void MX_USART2_UART_Init(void)
{
  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  /**USART2 GPIO Configuration
  PA2   ------> USART2_TX
  PA3   ------> USART2_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART2, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART2);
  LL_USART_Enable(USART2);
}

static void MX_GPIO_Init(void)
{
  /* GPIO Ports Clock Enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
}
