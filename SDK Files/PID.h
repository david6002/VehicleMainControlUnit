
#ifndef PID_H
#define PID_H

#include <stdbool.h>
#include <stdint.h>

typedef struct
{	// Input, output and setpoint
	float * input; 
	float * output; 
	float * setpoint; 
	// Tuning parameters
	float Kp; 
	float Ki; 
	float Kd; 
	// Output minimum and maximum values
	float minOutput; 
	float maxOutput; 
	// Variables for PID algorithm
	float sumError; 
	float lastInput; 
}pid;

#ifdef	__cplusplus
extern "C" {
#endif

	pid* pid_create(pid* pid, float* input, float* output, float* setoint, float kp, float ki, float kd);

	void pid_compute(pid* pid);

	
	void pid_tune(pid* pid, float kp, float ki, float kd);


	void pid_limits(pid* pid, float min, float max);



#ifdef	__cplusplus
}
#endif

#endif
// End of Header file
