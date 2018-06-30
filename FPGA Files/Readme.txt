This folder contains the Vehicle main control unit fpga source files.
files description:
•	AXI_GPIO
provides an input/output interface to the AXI interface.
We use these IP’s to send Data between the Arm processor and the FPGA.
We have 2 different entities:
	•	Dual-channel 32-bit PL to PS  
	•	Dual-channel 32-bit PS to PL  
•	Split_AXI
This entity connects on one end to the AXI_GPIO and splits the multiple 32-bit channels to smaller logic vectors, which then connects to the other entity’s in the design. 
•	pwmGenerator
This Entity receives a value in the range 0 -255 and 125MHz clock signal.
The output is a signal with the proportional duty cycle percentage: (Input_Value*(100/255)).
The output signal's frequency is 500Hz (can be changed accordingly to the application).
•	IIC Fault Detect
Each of the Msp430G2533 is connected to a digital shared bus, called        Send_Flag.
They use the channel to sync transition, when the chip wants to send 
data via the IIC channel, he will check if the channel is low (not busy), only then 
he will set the Send_Flag to high and send the data.
This Entity monitors the Send_Flag bus, if one of the following occur:
o	The Send_Flag is On for more than 10ms
o	The Send_Flag is Off for more than 100ms
Then the Entity would reset all the Msp430G2533 chips.


