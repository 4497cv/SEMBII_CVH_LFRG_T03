/*
 * rtos_i2c.c
 *
 *  Created on: 13 oct 2019
 *      Author: Luis Fernando
 */

#include "rtos_i2c.h"

volatile uint8_t g_MasterCompletionFlag = 0;
i2c_master_handle_t master_handle_t;

static void i2c_master_callback(I2C_Type *base, i2c_master_handle_t *handle,
        status_t status, void * userData)
{
	//PRINTF("SUCCESS\n");
	if (status == kStatus_Success)
	{
		g_MasterCompletionFlag = true;

	}
}

void I2Cinit()
{
	CLOCK_EnableClock(kCLOCK_PortB);
	CLOCK_EnableClock(kCLOCK_I2c0);

	port_pin_config_t config_i2c =
	{ kPORT_PullUp, kPORT_SlowSlewRate, kPORT_PassiveFilterDisable,
		        kPORT_OpenDrainEnable, kPORT_LowDriveStrength, kPORT_MuxAlt2,
		        kPORT_UnlockRegister,
	};

	PORT_SetPinConfig(PORTB, 2, &config_i2c);
	PORT_SetPinConfig(PORTB, 3, &config_i2c);

	i2c_master_config_t masterConfig;

	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = 100000;
	I2C_MasterInit(I2C0, &masterConfig, CLOCK_GetFreq(kCLOCK_BusClk));
}

void i2c_send(uint8_t data)
{
	uint8_t i2c_data = data;
	i2c_master_transfer_t master_to_slave_transfer;

	I2C_MasterTransferCreateHandle(I2C0, &master_handle_t,i2c_master_callback, NULL);

	master_to_slave_transfer.slaveAddress = I2C_SLAVE_ADDR;
	master_to_slave_transfer.direction = kI2C_Write;
	master_to_slave_transfer.subaddress = /**/;
	master_to_slave_transfer.subaddressSize = /**/;
	master_to_slave_transfer.data = &i2c_data;
	master_to_slave_transfer.dataSize = /**/;
	master_to_slave_transfer.flags = kI2C_TransferDefaultFlag;


	I2C_MasterTransferNonBlocking(I2C0, &master_handle_t,&master_to_slave_transfer);
	I2Cwritedelay();

	while (!g_MasterCompletionFlag)
	{
		/*
		 * Do nothing
		 */
	}
	g_MasterCompletionFlag = false;
	I2Cwritedelay();
}
