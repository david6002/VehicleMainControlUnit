
#ifndef sensor_data_HEADER
#define sensor_data_HEADER

#include "free_rtos_Semaphores.h"

#define sensor_max_num 60
#define sensor_max_name_lengh 50

typedef struct
{
	char sensor_name[sensor_max_name_lengh];
	int  sensor_id;
	int  sensor_data;
	int  min_value;
	int  max_value;
} sensor;

extern sensor sensor_dictionary[];

void TCP_application_thread(void *data_array);
void Send_Data_Thread(void *data_array);
void I2C_Slave_Recieve(void *data_array);
void I2C_Master_Recieve(void *data_array);
void CanBus_Recieve(void *data_array);


#endif // !sensor_data_HEADER



