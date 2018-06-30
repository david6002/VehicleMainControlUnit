
//----------------- FreeRTOS -------------------
#include <stdio.h>
#include <string.h>
#include <unistd.h>

//----------------- AXI -------------------
#include "xparameters.h"
#include "xgpio.h"
#include "xstatus.h"
#include "xiicps.h"
#include "xil_types.h"

//----------------- Other files -----------------
#include "bno055.h"
#include "I2c_Master_Recieve.h"
#include "free_rtos_Semaphores.h"


/************************** Constant Definitions ******************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define IIC_DEVICE_ID		XPAR_XIICPS_1_DEVICE_ID

#define IIC_SCLK_RATE		100000

/*
 * The following constant controls the length of the buffers to be sent
 * and received with the IIC.
 */
#define TEST_BUFFER_SIZE	120

/**************************** Type Definitions ********************************/


/************************** Variable Definitions ******************************/

XIicPs Iic1;		/**< Instance of the IIC Device */

/*
 * The following buffers are used in this example to send and receive data
 * with the IIC.
 */
u8 SendBuffer[TEST_BUFFER_SIZE];    /**< Buffer for Transmitting Data */
u8 RecvBuffer[TEST_BUFFER_SIZE];    /**< Buffer for Receiving Data */
struct bno055_t bno055;


/******************************************************************************/
/**
*
* Main function to call the polled master example.
*
* @param	None.
*
* @return	XST_SUCCESS if successful, XST_FAILURE if unsuccessful.
*
* @note		None.
*
*******************************************************************************/
void I2C_Master_Recieve(void *data_array)
{
	int Status = -1;

	xil_printf("IIC Master Polled Started \r\n");

	sensor *data = data_array;
	/*
	 * Run the Iic polled example in master mode, specify the Device
	 * ID that is specified in xparameters.h.
	 */
	Status = I2cPsMasterPolled(IIC_DEVICE_ID,data);
	if (Status != XST_SUCCESS) {
		xil_printf("IIC Master Polled Failed\r\n");
	}

	xil_printf("Successfully ran IIC Master Polled\r\n");
}

/*****************************************************************************/
/**
*
* This function sends data and expects to receive data from slave as modular
* of 64.
*
* This function uses interrupt-driven mode of the device.
*
* @param	DeviceId is the Device ID of the IicPs Device and is the
*		XPAR_<IICPS_instance>_DEVICE_ID value from xparameters.h
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
*******************************************************************************/
int I2cPsMasterPolled(u16 DeviceId, sensor *data_array)
{
	int Status;
	XIicPs_Config *Config;

	xil_printf("IIC Master start\n");
	bno055.dev_addr = BNO055_I2C_ADDR1;
	bno055.bus_write = BNO055_I2C_bus_write;
	bno055.bus_read = BNO055_I2C_bus_read;
	/*
	 * Initialize the IIC driver so that it's ready to use
	 * Look up the configuration in the config table,
	 * then initialize it.
	 */
	Config = XIicPs_LookupConfig(DeviceId);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XIicPs_CfgInitialize(&Iic1, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XIicPs_SelfTest(&Iic1);
	if (Status != XST_SUCCESS)
		return XST_FAILURE;

	/*
	 * Set the IIC serial clock rate.
	 */
	XIicPs_SetSClk(&Iic1, IIC_SCLK_RATE);

	//Communicate with bno055
	bno055_data_readout(data_array);

	return XST_SUCCESS;
}


s32 bno055_data_readout(sensor *data_array)
{
	/* Variable used to return value of
	communication routine*/
	s32 comres = BNO055_ERROR;
	u8 BNO055_CHIP_ID = 0;

	s8 temp = 1;
	u8 accel_self_test = 0;
	u8 gyro_self_test = 0;
	/* variable used to set the power mode of the sensor*/
	u8 power_mode = BNO055_INIT_VALUE;

	/***********read raw gyro data***********/
	/* variable used to read the gyro x data */
	s16 gyro_datax = BNO055_INIT_VALUE;
	/* variable used to read the gyro y data */
	s16 gyro_datay = BNO055_INIT_VALUE;
	 /* variable used to read the gyro z data */
	s16 gyro_dataz = BNO055_INIT_VALUE;
	 /* structure used to read the gyro xyz data */
	struct bno055_gyro_t gyro_xyz;

	/*************read raw Euler data************/
	 /*variable used to read the euler h data*/
	s16 euler_data_h = BNO055_INIT_VALUE;
	  //variable used to read the euler r data
	s16 euler_data_r = BNO055_INIT_VALUE;
	 //variable used to read the euler p data
	s16 euler_data_p = BNO055_INIT_VALUE;
	// structure used to read the euler hrp data
	struct bno055_euler_t euler_hrp;

	//***********read raw linear acceleration data**********
	// variable used to read the linear accel x data
	s16 linear_accel_data_x = BNO055_INIT_VALUE;
	// variable used to read the linear accel y data
	s16 linear_accel_data_y = BNO055_INIT_VALUE;
	// variable used to read the linear accel z data
	s16 linear_accel_data_z = BNO055_INIT_VALUE;
	// structure used to read the linear accel xyz data
	struct bno055_linear_accel_t linear_acce_xyz;

/*---------------------------------------------------------------------------*
 *********************** START INITIALIZATION ************************
 *  This API used to assign the value/reference of
 *	the following parameters
 *	I2C address
 *	Bus Write
 *	Bus read
 *	Chip id
 *	Page id
 *	Accel revision id
 *	Mag revision id
 *	Gyro revision id
 *	Boot loader revision id
 *	Software revision id
 *-------------------------------------------------------------------------*/
	usleep(5000000);//wait IMU to start
	/* Make sure we have the right device */
	s32 status = 0;
	status = BNO055_I2C_bus_read(BNO055_I2C_ADDR1,0,&BNO055_CHIP_ID,1);
	if(status == XST_FAILURE)
		xil_printf("imu not responding status is: %d\n",status);
	xil_printf("CHIP id is: %x\n",BNO055_CHIP_ID);

	BNO055_I2C_bus_read(BNO055_I2C_ADDR1,01,&BNO055_CHIP_ID,1);

	xil_printf("CHIP rev is: %x\n",BNO055_CHIP_ID);
	comres = bno055_init(&bno055);
/*	For initializing the BNO sensor it is required to the operation mode
	of the sensor as NORMAL
	Normal mode can set from the register
	Page - page0
	register - 0x3E
	bit positions - 0 and 1*/
	power_mode = BNO055_POWER_MODE_NORMAL;
	/* set the power mode as NORMAL*/
	comres += bno055_set_power_mode(power_mode);
/*----------------------------------------------------------------*
************************* END INITIALIZATION *************************
*-----------------------------------------------------------------*/

/************************* START READ RAW SENSOR DATA****************/



/*	Using BNO055 sensor we can read the following sensor data and
	virtual sensor data
	Sensor data:
		Accel
		Mag
		Gyro
	Virtual sensor data
		Euler
		Quaternion
		Linear acceleration
		Gravity sensor */
/*	For reading sensor raw data it is required to set the
	operation modes of the sensor
	operation mode can set from the register
	page - page0
	register - 0x3D
	bit - 0 to 3
	for sensor data read following operation mode have to set
	 * SENSOR MODE
		*0x01 - BNO055_OPERATION_MODE_ACCONLY
		*0x02 - BNO055_OPERATION_MODE_MAGONLY
		*0x03 - BNO055_OPERATION_MODE_GYRONLY
		*0x04 - BNO055_OPERATION_MODE_ACCMAG
		*0x05 - BNO055_OPERATION_MODE_ACCGYRO
		*0x06 - BNO055_OPERATION_MODE_MAGGYRO
		*0x07 - BNO055_OPERATION_MODE_AMG
		based on the user need configure the operation mode*/
	comres += bno055_set_operation_mode(BNO055_OPERATION_MODE_CONFIG);
	bno055_get_selftest_gyro(&gyro_self_test);
	xil_printf("gyro_self_test is: %d\n",gyro_self_test);
	bno055_get_selftest_accel(&accel_self_test);
	xil_printf("accel_self_test is: %d\n",accel_self_test);
	comres += bno055_set_operation_mode(BNO055_OPERATION_MODE_NDOF);
	comres += bno055_set_gyro_unit(BNO055_GYRO_UNIT_DPS);
	comres += bno055_set_euler_unit(BNO055_EULER_UNIT_DEG);

	while(1)
	{
	xSemaphoreGive(xSemaphore_I2C_Master);


	comres += bno055_read_gyro_x(&gyro_datax);
	comres += bno055_read_gyro_y(&gyro_datay);
	comres += bno055_read_gyro_z(&gyro_dataz);
	comres += bno055_read_gyro_xyz(&gyro_xyz);

	bno055_read_temp_data(&temp);

/************************ END READ RAW SENSOR DATA***************/

/************************ START READ RAW FUSION DATA ********
	For reading fusion data it is required to set the
	operation modes of the sensor
	operation mode can set from the register
	page - page0
	register - 0x3D
	bit - 0 to 3
	for sensor data read following operation mode have to set
	*FUSION MODE
		*0x08 - BNO055_OPERATION_MODE_IMUPLUS
		*0x09 - BNO055_OPERATION_MODE_COMPASS
		*0x0A - BNO055_OPERATION_MODE_M4G
		*0x0B - BNO055_OPERATION_MODE_NDOF_FMC_OFF
		*0x0C - BNO055_OPERATION_MODE_NDOF
		based on the user need configure the operation mode */
	//comres += bno055_set_operation_mode(BNO055_OPERATION_MODE_NDOF);
	/*Raw Euler H, R and P data can read from the register
	page - page 0
	register - 0x1A to 0x1E */
	comres += bno055_read_euler_h(&euler_data_h);
	comres += bno055_read_euler_r(&euler_data_r);
	comres += bno055_read_euler_p(&euler_data_p);
	comres += bno055_read_euler_hrp(&euler_hrp);

	/*Raw Linear accel X, Y and Z data can read from the register
	page - page 0
	register - 0x28 to 0x2D*/
	comres += bno055_read_linear_accel_x(&linear_accel_data_x);
	comres += bno055_read_linear_accel_y(&linear_accel_data_y);
	comres += bno055_read_linear_accel_z(&linear_accel_data_z);
	comres += bno055_read_linear_accel_xyz(&linear_acce_xyz);
/************************* END READ RAW FUSION DATA  ************/

	/* Insert into Sensors memory */
	if (xSemaphoreTake(xSemaphore_I2C_Slave, portMAX_DELAY) == pdTRUE)
		{
		(data_array + 40)->sensor_data = gyro_datax/16;
		(data_array + 41)->sensor_data = gyro_datay/16;
		(data_array + 42)->sensor_data = gyro_dataz/16;
		(data_array + 43)->sensor_data = euler_data_h/16;
		(data_array + 44)->sensor_data = euler_data_p/16;
		(data_array + 45)->sensor_data = euler_data_r/16;
		(data_array + 46)->sensor_data = linear_accel_data_x/100;
		(data_array + 47)->sensor_data = linear_accel_data_y/100;
		(data_array + 48)->sensor_data = linear_accel_data_z/100;
		(data_array + 49)->sensor_data = temp;
		}
	}
	return comres;
}


s8 BNO055_I2C_bus_read(u8 Address, u8 Register, u8 * Data, u8 ByteCount) {
  int Status; /** Wait until bus is idle to start another transfer.*/
  while (XIicPs_BusIsBusy(&Iic1)) { /* NOP */ } /** Set the IIC Repeated Start option.*/
  Status = XIicPs_SetOptions(&Iic1, XIICPS_REP_START_OPTION);
  if (Status != XST_SUCCESS) {
	  xil_printf("XIicPs_SetOptions error!\n\r");
	  return XST_FAILURE;
  } /* Send the buffer using the IIC and check for errors.*/
  Status = XIicPs_MasterSendPolled(&Iic1, &Register, 1, Address);
  if (Status != XST_SUCCESS) {
	  xil_printf("XIicPs_MasterSendPolled error!\n\r");
      return XST_FAILURE;
  }
  /** Clear the IIC Repeated Start option.*/
  Status = XIicPs_ClearOptions(&Iic1, XIICPS_REP_START_OPTION);
    if (Status != XST_SUCCESS) {
  	  xil_printf("XIicPs_MasterRecvPolled error!\n\r");
      return XST_FAILURE;
    }
  /** Receive the data.*/
  Status = XIicPs_MasterRecvPolled(&Iic1, Data, ByteCount, Address);
  if (Status != XST_SUCCESS) {
	  xil_printf("XIicPs_MasterRecvPolled error!\n\r");
	  return XST_FAILURE;
  }
  return XST_SUCCESS;
}

s8 BNO055_I2C_bus_write(u8 Address, u8 Register, u8 *Data, u8 ByteCount) {
  int Status; /** Wait until bus is idle to start another transfer.*/

  while (XIicPs_BusIsBusy(&Iic1)) { /* NOP */ } /** Set the IIC Repeated Start option.*/

  u8 data[2] = {Register,*Data};
  Status = XIicPs_MasterSendPolled(&Iic1, data, 2, Address);
  if (Status != XST_SUCCESS) {
	  xil_printf("XIicPs_MasterSendPolled error!\n\r");
      return XST_FAILURE;
  }
  return XST_SUCCESS;
}
