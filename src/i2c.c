#include "i2c.h"

volatile uint8_t data_index= 0;

//This function has to be used only when data_size >= 3 bytes
void Handle_I2C_Master(uint8_t register_address, uint8_t* register_data, uint8_t data_size)
{
	/*Acknowledge Enable*/
  LL_I2C_AcknowledgeNextData(I2C2, LL_I2C_ACK);
	/*Generate a Start condition just for sending register address*/
  LL_I2C_GenerateStartCondition(I2C2);
  /*Loop until Start Bit transmitted*/
  while(!LL_I2C_IsActiveFlag_SB(I2C2)){}
  /*Send PoE address with a write request */
  LL_I2C_TransmitData8(I2C2, MAX5980GTJ_DEFAULT_ADDRESS | I2C_REQUEST_WRITE);
	/*Loop until address acknowledgement received*/
  while(!LL_I2C_IsActiveFlag_ADDR(I2C2)){}
  /* Clear ADDR flag*/
  LL_I2C_ClearFlag_ADDR(I2C2);
	/*Check is a new byte should be sent and data register empty*/
	if(LL_I2C_IsActiveFlag_BTF(I2C2))
	{
		/*Write data in data register*/
		LL_I2C_TransmitData8(I2C2, register_address);
		/*Loop until byte acknowledgement*/
		while(!LL_I2C_IsActiveFlag_AF(I2C2));
	}
		
	/*Generate a Start condition again for register reading*/
	LL_I2C_GenerateStartCondition(I2C2);
  /*Loop until Start Bit transmitted*/
  while(!LL_I2C_IsActiveFlag_SB(I2C2)){}
  /*Send PoE address with a read request */
  LL_I2C_TransmitData8(I2C2, MAX5980GTJ_DEFAULT_ADDRESS | I2C_REQUEST_READ);
	/*Loop until address acknowledgement received*/
  while(!LL_I2C_IsActiveFlag_ADDR(I2C2)){}
  /* Clear ADDR flag*/
  LL_I2C_ClearFlag_ADDR(I2C2);
	/*Loop until register_data filled*/
	while(data_size > 0)
  {
		/*Acknowledge Disable*/
		if(data_size == 3) LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
		/* Generate Stop condition */
		if(data_size == 2) LL_I2C_GenerateStopCondition(I2C2);
    /*Check is a new byte received (including ACK pulse) and data register not empty*/
		while(!LL_I2C_IsActiveFlag_BTF(I2C2))
    {
      /*Read data from data register*/
      register_data[data_index++] = LL_I2C_ReceiveData8(I2C2);
      data_size--;
    }
  }
}
