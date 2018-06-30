
//----------------- global -------------------
#include <stdio.h>
#include <string.h>
#include <unistd.h>

//----------------- FreeRTOS -------------------
#include "free_rtos_Semaphores.h"

//----------------- AXI -------------------
#include "xparameters.h"
#include "xgpio.h"
#include "xstatus.h"
#include "xil_types.h"


//----------------- I2c Driver -----------------
#include "xparameters.h"
#include "xiicps.h"
#include "xil_printf.h"

//----------------- Other files -----------------

#include "sensor_data.h"

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define IIC_DEVICE_ID		XPAR_XIICPS_0_DEVICE_ID

/* The slave address to send to and receive from.
 */
#define IIC_SLAVE_ADDR		0x50
#define IIC_SCLK_RATE		100000

/*
 * The following constant controls the length of the buffers to be sent
 * and received with the IIC
 */
#define I2C_BUFFER_SIZE	13

/**************************** Type Definitions ********************************/

/************************** Function Prototypes *******************************/

int I2cPsSlavePolled(u16 DeviceId,sensor *data_array);


/************************** Variable Definitions ******************************/

XIicPs Iic0;				/* Instance of the IIC Device */

/*
 * The following buffers are used in this example to send and receive data
 * with the IIC. These buffers are defined as global so that they are not
 * defined on the stack.
 */
u8 RecvBuffer[I2C_BUFFER_SIZE];	/* Buffer for Receiving Data */

/******************************************************************************/
/**
*
* Main function to call the polled slave example.
*
* @param	None.
*
* @return	XST_SUCCESS if successful, XST_FAILURE if unsuccessful.
*
* @note		None.
*
*******************************************************************************/
void I2C_Slave_Recieve(void *data_array)
	{
	int Status;

	sensor *data = data_array;
	xil_printf("IIC Slave Address is %x \r\n",IIC_SLAVE_ADDR);
	xil_printf("i2c sensor id is %d \r\n",data->sensor_id);
	xil_printf("i2c test is %d \r\n",data->sensor_data);

	/*
	 * Run the Iic polled slave example , specify the Device ID that is
	 * generated in xparameters.h.
	 */
	Status = I2cPsSlavePolled(IIC_DEVICE_ID , data);
	if (Status != XST_SUCCESS) {
		xil_printf("IIC Slave program failed\r\n");
		return XST_FAILURE;
	}

	xil_printf("Successfully ran IIC Slave Polled Example Test\r\n");
	return XST_SUCCESS;
	}

	/*****************************************************************************/
	/**
	*
	* This function does polled mode transfer in slave mode. It first sends to
	* master then receives.
	*
	* @param	DeviceId is the Device ID of the IicPs Device and is the
	*		XPAR_<IICPS_instance>_DEVICE_ID value from xparameters.h
	*
	* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
	*
	* @note		None.
	*
	*******************************************************************************/
	int I2cPsSlavePolled(u16 DeviceId , sensor *data_array)
	{
	int Status;
	XIicPs_Config *Config;
	int Index;
	int message_count = 0, sensor_id = 0 , message_error_counter = 0;
	int write_flag = 0;

	/*
	 * Initialize the IIC driver so that it's ready to use
	 * Look up the configuration in the config table,
	 * then initialize it.
	 */
	Config = XIicPs_LookupConfig(DeviceId);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XIicPs_CfgInitialize(&Iic0, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XIicPs_SelfTest(&Iic0);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	XIicPs_SetupSlave(&Iic0, IIC_SLAVE_ADDR);

	//allocate_data_array;
	for (Index = 0; Index <= sensor_max_num; Index ++)
	{
		data_array->sensor_id = Index;
		data_array->sensor_data = 0;
		data_array++;
	}
	data_array = data_array - Index;// return to original pointer

	while(1){

		xSemaphoreGive(xSemaphore_I2C_Slave);
		//xil_printf("write_flag is: %d\r\n",write_flag);
		if (write_flag == 0)
		{
			Status = XIicPs_SlaveRecvPolled(&Iic0, RecvBuffer,
						I2C_BUFFER_SIZE);
			if (Status != XST_SUCCESS) {
				xil_printf("XST_FAILURE\n");
			}
		}

		/*
		 * Verify received data is correct.
		 */
		if (xSemaphoreTake(xSemaphore_I2C_Slave, portMAX_DELAY) == pdTRUE)
		{
		write_flag = 0;
		//message_count = 0;
		for(Index = 0; Index < I2C_BUFFER_SIZE; Index ++)
		{
			int data = RecvBuffer[Index];
			if (data < 0 || data > 255) // check if data is valid
				continue;
			/* data = 255 => massage start */
			if (data == 255)//data == 255
				message_count = 1;
			else if (data <= sensor_max_num && (message_count % 2) == 1  )// sensor id
				{
				sensor_id = data;
				message_count ++;
				if ((data_array+sensor_id)->sensor_id != sensor_id)// data_array not sync
					{
					xil_printf("sensors dont match %d,%d\n",(data_array+sensor_id)->sensor_id,sensor_id);
					break;// data corrupted
					}
				}
			else if ((message_count % 2) == 0 && message_count > 0)// sensor data
				{
				(data_array+sensor_id)->sensor_data = data * 90 / 255;
				message_count ++;
				}
			else
				{
				//data is corrupted  - ignore
				xil_printf("Data i2c error , message_error_counter is: %d, ");
				message_error_counter++;

				}
			}
		}
		else
		{
			xil_printf("i2c waiting semaphore\r\n");
			write_flag = 1;//make loop wait for Semaphore
		}

	}
	return XST_SUCCESS;
	}



