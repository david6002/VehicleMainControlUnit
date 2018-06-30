
/*******************************************************************************
 * Include
 ******************************************************************************/
#include <stdio.h>
#include "xparameters.h"
#include "netif/xadapter.h"
#include "platform_config.h"
#include "xil_printf.h"
#include "sensor_data.h"
#include "free_rtos_Semaphores.h"
#include "semphr.h"


#if LWIP_DHCP==1
#include "lwip/dhcp.h"
#endif

#ifdef XPS_BOARD_ZCU102
#ifdef XPAR_XIICPS_0_DEVICE_ID
int IicPhyReset(void);
#endif
#endif

int main_thread();
void print_echo_app_header();


void lwip_init();

#if LWIP_DHCP==1
extern volatile int dhcp_timoutcntr;
err_t dhcp_start(struct netif *netif);
#endif

#define THREAD_STACKSIZE 1024

static struct netif server_netif;
struct netif *echo_netif;

void
print_ip(char *msg, struct ip_addr *ip)
{
	xil_printf(msg);
	xil_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip),
			ip4_addr3(ip), ip4_addr4(ip));
}

void
print_ip_settings(struct ip_addr *ip, struct ip_addr *mask, struct ip_addr *gw)
{

	print_ip("Board IP: ", ip);
	print_ip("Netmask : ", mask);
	print_ip("Gateway : ", gw);
}

int main()
{
	sys_thread_new("main_thrd", (void(*)(void*))main_thread, 0,
	                THREAD_STACKSIZE,
	                DEFAULT_THREAD_PRIO);
	vTaskStartScheduler();
	while(1);
	return 0;
}

void network_thread(void *p)
{
    struct netif *netif;
    struct ip_addr ipaddr, netmask, gw;
#if LWIP_DHCP==1
    int mscnt = 0;
#endif
    /* the mac address of the board. this should be unique per board */
    unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

    netif = &server_netif;

#if LWIP_DHCP==0
    /* initliaze IP addresses to be used */
    IP4_ADDR(&ipaddr,  192, 168, 1, 10);
    IP4_ADDR(&netmask, 255, 255, 255,  0);
    IP4_ADDR(&gw,      192, 168, 1, 254);
#endif

    /* print out IP settings of the board */
    xil_printf("\r\n\r\n");
    xil_printf("-----lwIP TCP/UDP server modified by David ------\n\r");

#if LWIP_DHCP==0
    print_ip_settings(&ipaddr, &netmask, &gw);
    /* print all application headers */
#endif

#if LWIP_DHCP==1
	ipaddr.addr = 0;
	gw.addr = 0;
	netmask.addr = 0;
#endif
    /* Add network interface to the netif_list, and set it as default */
    if (!xemac_add(netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR)) {
        xil_printf("Error adding N/W interface\r\n");
        return;
    }
    netif_set_default(netif);

    /* specify that the network if is up */
    netif_set_up(netif);

    /* start packet receive thread - required for lwIP operation */
    sys_thread_new("xemacif_input_thread", (void(*)(void*))xemacif_input_thread, netif,
            THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);

#if LWIP_DHCP==1
    dhcp_start(netif);
    while (1) {
		vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);
		dhcp_fine_tmr();
		mscnt += DHCP_FINE_TIMER_MSECS;
		if (mscnt >= DHCP_COARSE_TIMER_SECS*1000) {
			dhcp_coarse_tmr();
			mscnt = 0;
		}
	}
#else
    xil_printf("\r\n");
    xil_printf("%20s %6s %s\r\n", "Server", "Port", "Connect With..");
    xil_printf("%20s %6s %s\r\n", "--------------------", "------", "--------------------");

    print_echo_app_header();
    xil_printf("\r\n");
    vTaskDelete(NULL);
#endif
    return;
}

int main_thread()
{
	xSemaphore_I2C_Slave = xSemaphoreCreateBinary();
	if (xSemaphore_I2C_Slave == NULL)
		printf("xSemaphore_I2C_Slave creation failed.\r\n");
	xSemaphore_I2C_Master = xSemaphoreCreateBinary();
	if (xSemaphore_I2C_Master == NULL)
		printf("xSemaphore_I2C_Master creation failed.\r\n");
	xSemaphore_Canbus = xSemaphoreCreateBinary();
	if (xSemaphore_Canbus == NULL)
		printf("xSemaphore_Canbus creation failed.\r\n");
	sensor *p_sensor_data_array = NULL;
	p_sensor_data_array = malloc((sensor_max_num+1) *sizeof(sensor));
	//test
	p_sensor_data_array->sensor_id = 1;
	p_sensor_data_array->sensor_data = 1;

	//test end
	#if LWIP_DHCP==1
		printf("LWIP_DHCP==1");
		int mscnt = 0;
	#endif

	#ifdef XPS_BOARD_ZCU102
		IicPhyReset();
	#endif

	/* initialize lwIP before calling sys_thread_new */
    lwip_init();

    /* any thread using lwIP should be created using sys_thread_new */
    sys_thread_new("NW_THRD", network_thread, NULL,
		THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);

#if LWIP_DHCP==1
    while (1) {
	vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);
		if (server_netif.ip_addr.addr) {
			xil_printf("DHCP request success\r\n");
			print_ip_settings(&(server_netif.ip_addr), &(server_netif.netmask), &(server_netif.gw));
			print_echo_app_header();
			xil_printf("\r\n");
			sys_thread_new("TCP_Thread", TCP_application_thread, p_sensor_data_array,
					THREAD_STACKSIZE,
					DEFAULT_THREAD_PRIO);
			sys_thread_new("Send_Data_Thread", Send_Data_Thread, p_sensor_data_array,
					THREAD_STACKSIZE,
					DEFAULT_THREAD_PRIO);
			sys_thread_new("I2C_Salve_Thread", I2C_Slave_Recieve, p_sensor_data_array,
			        		THREAD_STACKSIZE,
			        		DEFAULT_THREAD_PRIO);
			sys_thread_new("I2C_Master_Thread", I2C_Master_Recieve, p_sensor_data_array,
							THREAD_STACKSIZE,
							DEFAULT_THREAD_PRIO);

			break;
		}
		mscnt += DHCP_FINE_TIMER_MSECS;
		if (mscnt >= 10000) {
			xil_printf("ERROR: DHCP request timed out\r\n");
			xil_printf("Configuring default IP of 192.168.1.10\r\n");
			IP4_ADDR(&(server_netif.ip_addr),  192, 168, 1, 10);
			IP4_ADDR(&(server_netif.netmask), 255, 255, 255,  0);
			IP4_ADDR(&(server_netif.gw),  192, 168, 1, 1);
			print_ip_settings(&(server_netif.ip_addr), &(server_netif.netmask), &(server_netif.gw));
			/* print all application headers */
			xil_printf("\r\n");
			xil_printf("%20s %6s %s\r\n", "Server", "Port", "Connect With..");
			xil_printf("%20s %6s %s\r\n", "--------------------", "------", "--------------------");

			print_echo_app_header();
			xil_printf("\r\n");
			sys_thread_new("TCP_Thread", TCP_application_thread, p_sensor_data_array,
					THREAD_STACKSIZE,
					DEFAULT_THREAD_PRIO);
			sys_thread_new("Send_Data_Thread", Send_Data_Thread, p_sensor_data_array,
					THREAD_STACKSIZE,
					DEFAULT_THREAD_PRIO);
			sys_thread_new("I2C_Salve_Thread", I2C_Slave_Recieve, p_sensor_data_array,
					THREAD_STACKSIZE,
					DEFAULT_THREAD_PRIO);
			sys_thread_new("I2C_Master_Thread", I2C_Master_Recieve, p_sensor_data_array,
					THREAD_STACKSIZE,
					DEFAULT_THREAD_PRIO);

			break;
		}

		free(p_sensor_data_array);
	}
#else
    {
    	sys_thread_new("TCP_Thread", TCP_application_thread, p_sensor_data_array,
    			THREAD_STACKSIZE,
    			DEFAULT_THREAD_PRIO);
		sys_thread_new("Send_Data_Thread", Send_Data_Thread, p_sensor_data_array,
				THREAD_STACKSIZE,
				DEFAULT_THREAD_PRIO);
		sys_thread_new("I2C_Salve_Thread", I2C_Slave_Recieve, p_sensor_data_array,
				THREAD_STACKSIZE,
				DEFAULT_THREAD_PRIO);
		sys_thread_new("I2C_Master_Thread", I2C_Master_Recieve, p_sensor_data_array,
				THREAD_STACKSIZE,
				DEFAULT_THREAD_PRIO);
    }
#endif
    vTaskDelete(NULL);
    return 0;
}
