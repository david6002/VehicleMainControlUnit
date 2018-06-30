
//----------------- Common libs -------------------
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

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


//AXI:
#include "xparameters.h"
#include "xgpio.h"
#include "xstatus.h"

#include "sensor_data.h"



#define THREAD_STACKSIZE 1024
#define output_SIZE 1024

u16_t echo_port = 7;

static sensor *p_sensor_data_array = NULL; // global local sensor_data_array

void print_echo_app_header()
{
    xil_printf("%20s %6d %s\r\n", "TCP server",
                        echo_port,
                        "$ telnet 192.168.1.10 7");

}

/* thread spawned for each connection */
void process_TCP_request(void *p)
{
	int sd = (int)p;
		int RECV_BUF_SIZE = 2048;
		int n, nwrote;
		int output_data_Len;
		int sensor_count = 0;
		char recv_buf[RECV_BUF_SIZE];
		char output_data[output_SIZE] = "";


	while (1) {

		strcpy(output_data,"");// reset input
		/* read a max of RECV_BUF_SIZE bytes from socket */
		if ((n = read(sd, recv_buf, RECV_BUF_SIZE)) < 0) {
			xil_printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
			break;
		}

		/* break if the recved message = "quit" */
		if (!strncmp(recv_buf, "quit", 4))
			break;

		/* break if client closed connection */
		if (n <= 0)
			break;

		/* client is requesting sensor_dictionary */
		if (!strncmp(recv_buf, "CLIENT_CONNECT", strlen("CLIENT_CONNECT")))
		{
			/* Build string with dictionary of sensors */
			for (sensor_count = 0 ; sensor_count < sensor_max_num - 1 ; sensor_count++ )
			{
				sprintf(output_data + strlen(output_data),"%d:%s,%d,%d;",
						sensor_dictionary[sensor_count].sensor_id,
						sensor_dictionary[sensor_count].sensor_name,
						sensor_dictionary[sensor_count].min_value,
						sensor_dictionary[sensor_count].max_value
				);
			}
			sprintf(output_data + strlen(output_data),"<FIN>");
			xil_printf("command is %s\r\n",output_data);
		}
		
		/* Get Wings Mode
		Command looks like: WINGS_MODE:Wings_mode:Wings_data */
		else if (!strncmp(recv_buf, "WINGS_MODE", strlen("WINGS_MODE")))
		{
			snprintf(output_data,strlen(recv_buf),"Command %s was received,Allocation mode to memory...\n",recv_buf);
			int counter = 0;
			int sensor_id = 51,sensor_value = 0;
			char *p_line = NULL, seperate_str[] = ":", *next_token = NULL;//for spliting line

			counter = 0;
			p_line = strtok_r(recv_buf, seperate_str, &next_token);
			
			while (p_line != NULL && counter < 3){
				if (counter != 0)
				{
					sensor_value = atoi(p_line);
					if ((counter % 2) == 1)
						(p_sensor_data_array + sensor_id) -> sensor_data = sensor_value;
					else
						(p_sensor_data_array + sensor_id) -> max_value = sensor_value;

				}
				p_line = strtok_r(NULL, seperate_str, &next_token);
				counter++;
			}

		}
		
		/* Get Throttle Mode
		Command looks like: Throttle_MODE;Throttle_mode:Throttle_data */
		else if (!strncmp(recv_buf, "Throttle_MODE", strlen("Throttle_MODE")))
		{
			snprintf(output_data,strlen(recv_buf),"Command %s was received,Allocation mode to memory...\n",recv_buf);
			int counter = 0;
			int sensor_id = 52,sensor_value = 0;
			char *p_line = NULL, seperate_str[] = ":", *next_token = NULL;//for spliting line

			counter = 0;
			p_line = strtok_r(recv_buf, seperate_str, &next_token);
			
			while (p_line != NULL && counter < 3){
				if (counter != 0)
				{
					sensor_value = atoi(p_line);
					if ((counter % 2) == 1)
						(p_sensor_data_array + sensor_id) -> sensor_data = sensor_value;
					else
						(p_sensor_data_array + sensor_id) -> max_value = sensor_value;

				}
				p_line = strtok_r(NULL, seperate_str, &next_token);
				counter++;
			}

		}

		/* Simulation Mode - SECURITY RISK!  */
		/*
		else if (!strncmp(recv_buf, "SIM", strlen("SIM")))
		{
			int counter = 0;
			int sensor_id = 0,sensor_value = 0;
			char *p_line = NULL, seperate_str[] = ";:", *next_token = NULL;//for spliting line

			counter = 0;
			p_line = strtok_r(recv_buf, seperate_str, &next_token);
			if (xSemaphoreTake(xSemaphore_I2C_Master, portMAX_DELAY) == pdTRUE){
			while (p_line != NULL){
				if (counter != 0)
				{
					sensor_value = atoi(p_line);
					if ((counter % 2) == 1)
						sensor_id = sensor_value;
					else
						(p_sensor_data_array + sensor_id) -> sensor_data = sensor_value;


				}
				p_line = strtok_r(NULL, seperate_str, &next_token);

				counter++;
			}
			xSemaphoreGive(xSemaphore_I2C_Master);
			}
		}*/

		/* handle other request */
		else
			sprintf(output_data,"Command %s was received, illegal Command => Ignoring Command\r\n ",recv_buf);

		/* send FeedBack */
		output_data_Len = strlen(output_data);
		if ((nwrote = write(sd, output_data, output_data_Len)) < 0) {
			xil_printf("%s: ERROR responding to client echo request. received = %d, written = %d\r\n",
					__FUNCTION__, n, nwrote);
			xil_printf("Closing socket %d\r\n", sd);
			break;
		}
	}

	/* close connection */
	close(sd);
	vTaskDelete(NULL);
}


void TCP_application_thread(void *data_array)
{
	int sock, new_sd;
	struct sockaddr_in address, remote;
	int size;
	p_sensor_data_array = data_array;

	if ((sock = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return;

	address.sin_family = AF_INET;
	address.sin_port = htons(echo_port);
	address.sin_addr.s_addr = INADDR_ANY;

	if (lwip_bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
		return;

	lwip_listen(sock, 0);

	size = sizeof(remote);

	while (1) {
		if ((new_sd = lwip_accept(sock, (struct sockaddr *)&remote, (socklen_t *)&size)) > 0) {
			sys_thread_new("echos", process_TCP_request,
				(void*)new_sd,
				THREAD_STACKSIZE,
				DEFAULT_THREAD_PRIO);
		}
	}
}
