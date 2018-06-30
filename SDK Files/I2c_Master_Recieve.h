
#ifndef SRC_I2C_MASTER_RECIEVE_H_
#define SRC_I2C_MASTER_RECIEVE_H_

#include "bno055.h"
#include "sensor_data.h"

#define	BNO055_I2C_BUS_WRITE_ARRAY_INDEX	((u8)1)

/************************** Function Prototypes *******************************/

int I2cPsMasterPolled(u16 DeviceId,sensor *data_array);

s8 BNO055_I2C_bus_read(u8 Address, u8 Register, u8 * Data, u8 ByteCount);

s8 BNO055_I2C_bus_write(u8 Address, u8 Register, u8 * Data, u8 ByteCount);

s32 bno055_data_readout(sensor *data_array);

#endif /* SRC_I2C_MASTER_RECIEVE_H_ */
