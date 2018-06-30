
//----------------- FreeRTOS -------------------
#include <stdio.h>
#include <string.h>
#include <unistd.h>


//----------------- Lwip -----------------------
#include "lwip/err.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"

#include "lwip/sockets.h"
#include "netif/xadapter.h"
#include "lwipopts.h"
#include "xil_printf.h"
#include "FreeRTOS.h"
#include "task.h"

//----------------- AXI -------------------
#include "xparameters.h"
#include "xgpio.h"
#include "xstatus.h"
#include "xil_types.h"

//----------------- Other files -----------------
#include "sensor_data.h"
#include "WingAlgo.h"
#include "ThrottleAlgo.h"
#include "PID.h"


void Send_Data_Thread(void *data_array)
{

	sensor *data = data_array;

	/*********************** Initialization ***********************/
	//--------------- Begin AXI --------------------------------------
	XGpio Gpio_32_bit_in; // XGPIO instance that will be used to work with 32 from PL
	XGpio Gpio_32_bit_out; // XGPIO instance that will be used to work with 32 from PL


	char *sensor_data = (char*)malloc(sensor_max_num * sensor_max_name_lengh * sizeof(char)) ;// 50 chars max for sensor

	// SWITCHes initialization
	XGpio_Initialize(&Gpio_32_bit_out, XPAR_AXI_GPIO_PS_TO_PL_1_DEVICE_ID);
	XGpio_Initialize(&Gpio_32_bit_in, XPAR_AXI_GPIO_PL_TO_PS_1_DEVICE_ID);
	XGpio_SetDataDirection(&Gpio_32_bit_in, 1, 0xffffffff);
	XGpio_SetDataDirection(&Gpio_32_bit_in, 2, 0xffffffff);
	XGpio_SetDataDirection(&Gpio_32_bit_out, 1, 0x0);
	XGpio_SetDataDirection(&Gpio_32_bit_out, 2, 0x0);

	//-------------------------UDP Send-----------------------
	struct udp_pcb *broadcast_pcb;
	struct ip_addr forward_ip;
	#define out_buf_size 100
	#define fwd_port 5001
	#define transmit_port 5001

	struct udp_pcb *ptel_pcb = udp_new();
	int udp_port = fwd_port;
	int sensor_count = 0;
	int time_count = 0;

	udp_bind(ptel_pcb, IP_ADDR_ANY, udp_port);
	IP4_ADDR(&forward_ip, 192, 168,   1, 255);
	broadcast_pcb = udp_new();
	usleep(5000000);//wait 5 sec to start
	//lion
	pid mypid;
	pid* throttlePid;
	float throttleCmd;
	float throttlePosition;
	float output;
	float kp= 2.5;
	float ki= 0.35;
	float kd= 0.00;
	throttlePid=pid_create(&mypid, &throttlePosition, &output, &throttleCmd, kp, ki,kd);

	while(1)
	{
		xSemaphoreGive(xSemaphore_I2C_Slave);
		xSemaphoreGive(xSemaphore_I2C_Master);
		if (xSemaphoreTake(xSemaphore_I2C_Slave, portMAX_DELAY) == pdTRUE &&
			xSemaphoreTake(xSemaphore_I2C_Master, portMAX_DELAY) == pdTRUE)
		{
			/* send UDP massage every 100 ms */
			if (time_count == 5) // 5 * 20ms = 100
			{
				sprintf(sensor_data,"%d:%d;",data->sensor_id,data->sensor_data);
				for (sensor_count = 0 ; sensor_count < sensor_max_num ; sensor_count++ )
				{
					data++;
					sprintf(sensor_data + strlen(sensor_data),"%d:%d;",data->sensor_id,data->sensor_data);
				}

				int sample_size = strlen(sensor_data);

				struct pbuf * p = pbuf_alloc(PBUF_TRANSPORT, sample_size, PBUF_REF);
				p->payload = sensor_data;
				p->len = p->tot_len = sample_size;

				udp_sendto(broadcast_pcb, p, &forward_ip, fwd_port); //dest port
				pbuf_free(p);

				data = data - sensor_count;
				time_count = 0;
			}

			/* wing algorithm */
			int* wing1 = 0; int* wing2 = 0; int* wing3 = 0; int* wing4 = 0;
			int gyroPitch=0;
			int brakingSensor=((data + 9)->sensor_data);
			int accelSensor=((data + 10)->sensor_data);
			int steeringWheel=((data + 11)->sensor_data);
			int friction=0.8;
			int driverMass=70;
			int rowRollAngle=0;
			int velocity=((data + 1)->sensor_data);
			int wing_mode = ((data + 51)->sensor_data);
			int Throttle_mode = ((data + 52)->sensor_data);
			int Throttle_mode_data = ((data + 52)->max_value);
			int TPS1 = ((data + 21)->sensor_data);
			int TPS2 = ((data + 22)->sensor_data);

			/* send to Wing Algorithm */
			anglesUpdater(gyroPitch,brakingSensor,accelSensor,steeringWheel,friction,driverMass,rowRollAngle,velocity,wing1,wing2,wing3,wing4);

			/* send Wing Algorithm data to AXI*/
			unsigned int data1_to_send = (int)((*wing1));
			data1_to_send += (int)((*wing2)) << 8;
			data1_to_send += (int)((*wing3)) << 16;
			data1_to_send += (int)((*wing4)) << 24;


			/* repair min/max values of tps sensors */
			if (TPS1 > 30 && TPS2 < 231)
				throttlePosition = (int)((((data + 22)->sensor_data) - 31) *90/(231 - 31)) ;
			else if ((data + 22)->sensor_data < 31)
				throttlePosition = 0 ;
			else if ((data + 22)->sensor_data > 230)
				throttlePosition = 90;


			throttleCmd = ThrottlePostion(accelSensor,accelSensor,brakingSensor,Throttle_mode,Throttle_mode_data);

			/* pid tuning according to error */
			if (abs(throttleCmd - throttlePosition)>5 ){
				pid_tune(throttlePid, kp, ki, kd);
			}
			else{
				pid_tune(throttlePid, 0.1, 0.1, 0.0);
			}
			pid_compute(throttlePid);

			/* send output to Fpga PID  */
			unsigned int data2_to_send = (int)output;
		    /* send Error to PID */
			data2_to_send += (int)(throttleCmd - throttlePosition) << 8;

			XGpio_DiscreteWrite(&Gpio_32_bit_out, 1,data1_to_send);
			XGpio_DiscreteWrite(&Gpio_32_bit_out, 2,data2_to_send);


			//unsigned int return_data = XGpio_DiscreteRead(&Gpio_32_bit_in,1);

			xSemaphoreGive(xSemaphore_I2C_Master);
			xSemaphoreGive(xSemaphore_I2C_Slave);
			time_count ++;
			usleep(20000);// send info every 0.02 sec
		}
		else
			xil_printf("Send data waiting semaphore\r\n");
		}
	free(sensor_data);
}
