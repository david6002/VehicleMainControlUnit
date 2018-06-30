This folder contains the Vehicle main control unit SDK source files.
the files description:
1)	I2c_slave_Recieve.c
This file purpose is to receive data from 4 different MSP430G2533 chips via IIC shared bus as a slave.
This file runs the IIC_Slave_Recieve function, Which uses the “XIicPs_SlaveRecvPolled()” Xilinx function, that operates the Arm IIC Controller 0 in polled mode.
Each message from the Msp430g2533 Chip is constructed in the following way:
IIC message: 
Slave Address	0xFF
(Msg Synq)	First Sensor ID	First Sensor Data	Second Sensor ID	Second Sensor Data	···	N Sensor ID	N Sensor Data	N+1 Sensor ID	N+1 Sensor Data

The 4 different MSP430G2533 chips are configured as IIC Master, their config and collision protection are mentioned in the appendix.
The shared sensor array group index Associated With this thread are 1-30.

2)	I2c_Master_Recieve.c
This file purpose is to receive data from The Bosch BNO055 IMU chip via IIC bus as a master.
This file runs the IIC_Master_Recieve function, which uses the “XIicPs_MasterSendPolled ()” and “XIicPs_MasterRecvPolled ()” Xilinx functions, That Operates the Arm IIC Controller 1 in Polled mode.
In order to read data from the IMU using the official IIC protocol we had to use “XIicPs_SetOptions()” as will be mentioned in the appendix.
In order to read the required information from the IMU we used the BNO055 official driver:
o	bno055.c
The shared sensor array group Index associated with this thread are 40-49.

3)	Tcp_Recieve.c
This file purpose is to receive/send data to/from a remote laptop via Ethernet protocol (Using TCP).
This file runs the process_TCP_request() function, Which uses the lwIP , in order to receive Commands from the Laptop and to send the list of all the sensors  connected to the control unit.

4)	Send_Data_Thread
This file runs the Send_Data_Thread() function , which has several functionalities:
•	Sending all the sensors data to the  remote laptop Via Ethernet protocol (Using UDP).
Every 100ms this function sends the full sensors data using UDP via Ethernet.
Each message is constructed in the following way:
The Destination IP is 192.168.1.255/24 (Lan Broadcast).
The Destination Port is 5001.
The Data in the UDP message is constructed in the following way:
First Sensor ID	:	First Sensor Data	;	Second Sensor ID	:	Second Sensor Data	;	···	;	N Sensor ID	:	N
Sensor Data	;

•	Processing data used by the Dynamic Wings and Electronic Throttle
In order to control the Dynamic Wings and the Electronic Throttle Motors,
The Dynamic Wings and Electronic Throttle implementations are in the following files:
o	WingAlgo.c
o	ThrottleAlgo.c 
In this thread we run these files, Thus, receiving the required angle of each of the Electric Motors.
•	Sending data and commands to the FPGA  
Every 20ms this function sends All the Data and Commands to the FPGA Via AXI.
The data currently sent to the FPGA is:
o	Motors required angle.
o	Feedback from the Electric Motors (for the PID control)
       More Data Will be sent in the Future when more applications will 
       be developed.
5)     sensor_dictionary.c
this file contains all the sensors in the system. 