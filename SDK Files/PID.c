
#include "PID.h"

pid* pid_create(pid* pid, float* input, float* output, float* setpoint, float kp, float ki, float kd)
{
	pid->input = input;
	pid->output = output;
	pid->setpoint = setpoint;
	pid->sumError = 0;
	pid->lastInput = 0;
	pid_limits(pid, 0, 255);

	pid_tune(pid, kp, ki, kd);

	return pid;
}

void pid_compute(pid* pid)
{

	
	float in = *(pid->input);
	// Compute error
	float error = (*(pid->setpoint)) - in;
	// Compute integral
	pid->sumError += (pid->Ki * error);
	if (pid->sumError > pid->maxOutput)
		pid->sumError = pid->maxOutput;
	else if (pid->sumError < pid->minOutput)
		pid->sumError = pid->minOutput;
	// Compute differential on input
	float dinput = in - pid->lastInput;
	// Compute PID output
	float out = pid->Kp * error + pid->sumError - pid->Kd * dinput;
	// Apply limit to output value
	if (out > pid->maxOutput)
		out = pid->maxOutput;
	else if (out < pid->minOutput)
		out = pid->minOutput;
	// Output to pointed variable
	(*pid->output) = out;
	// Keep track of some variables for next execution
	pid->lastInput = in;
}

void pid_tune(pid* pid, float kp, float ki, float kd)
{
	// Check for validity
	if (kp < 0 || ki < 0 || kd < 0)
		return;

	pid->Kp = kp;
	pid->Ki = ki ;
	pid->Kd = kd ;

}


void pid_limits(pid* pid, float min, float max)
{
	if (min >= max) return;
	pid->minOutput = min;
	pid->maxOutput = max;
	if (pid->sumError > pid->maxOutput)
			pid->sumError = pid->maxOutput;
	else if (pid->sumError < pid->minOutput)
			pid->sumError = pid->minOutput;
}







