
#include "sensor_data.h"

/*********************************************************
** The sensor_dictionary is used to send the sensors    **
** Data outside, and sync the remote PC with the        **
** meaning of each sensor                               **
*********************************************************/
sensor sensor_dictionary[sensor_max_num] = {
		{"velocity",1,0,0,120 },{"s2",2,0,0,8000 },
		{"s3",3,0,0,255 },{"s4",4,0,0,255 },
		{"wing_position",5,0,-100,100 },{"s6",6,0,0,255 },
		{"s7",7,0,0,255 },{"s8",8,0,0,255 },
		{"braking",9,0,0,90 },{"throttle",10,0,0,90 },
		{"SteeringWheel",11,0,-90,90 },{"speed_wheel1",12,0,0,255 },
		{"speed_wheel2",13,0,0,255 },{"speed_wheel3",14,0,0,255 },
		{"speed_wheel4",15,0,0,255 },{"s16",16,0,0,255 },
		//{"s17",17,0,0,255 },{"s18",18,0,0,255 },
		//{"s19",19,0,0,255 },{"s20",20,0,0,255 },
		{"TPS1",21,0,0,255 },{"TPS2",22,0,0,255 },
		//{"s23",23,0,0,255 },{"s24",24,0,0,255 },
		//{"s25",25,0,0,255 },{"s26",26,0,0,255 },
		//{"s27",27,0,0,255 },{"s28",28,0,0,255 },
		//{"s29",29,0,0,255 },{"s30",30,0,0,255 },
		//{"s31",31,0,0,255 },{"s32",32,0,0,255 },
		//{"s33",33,0,0,255 },{"s34",34,0,0,255 },
		//{"s35",35,0,0,255 },{"s36",36,0,0,255 },
		//{"s37",37,0,0,255 },{"s38",38,0,0,255 },
		/*{"s39",39,0,0,255 },*/{"gyro_data_x",40,0,0,255 },
		{"gyro_data_y",41,0,0,255 },{"gyro_data_z",42,0,0,255 },
		{"euler_data_h",43,0,0,255 },{"euler_data_p",44,0,-180,180 },
		{"euler_data_r",45,0,-180,180 },{"linear_accel_data_x",46,0,0,255 },
		{"linear_accel_data_y",47,0,0,255 },{"linear_accel_data_z",48,0,0,255 },
		{"IMU_TEMPERATURE",49,0,0,255 },{"s50",50,0,0,255 },
		{"Wing_Mode",51,0,0,255 },{"Throttle_Mode",52,0,0,255 }
};