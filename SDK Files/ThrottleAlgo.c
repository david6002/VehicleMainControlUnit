
#include <stdlib.h>
#include <math.h>

#define SensorsError_1  300 // Too much diffrance between the two sensors

int ThrottlePostion(int AccPedalSensor_1,int AccPedalSensor_2,int BrakeSensor,int mode,int mode_data)
{
	int sensorDiff = abs(AccPedalSensor_2 - AccPedalSensor_1);
	int sensorAvarage = (AccPedalSensor_1 + AccPedalSensor_2) / 2;
	
	if (sensorDiff > AccPedalSensor_1 /10 || sensorDiff >  AccPedalSensor_2 /10)
		return SensorsError_1;
	
	if (BrakeSensor > 10) // if the brakes pedal is pressed
		return 0;
	else if (mode == 0) // linear mode
		return (AccPedalSensor_1 * 90 /254);
	
	else if (mode == 1) // Limit max Position
	{
		if (sensorAvarage < (int)mode_data * 90/100)
			return AccPedalSensor_1;
		else
			return (int)(mode_data * 90/100);
	}
	
	else if (mode == 2) // Quick start
	{
		if (sensorAvarage > 10)
			return 90; // max
	}

	return AccPedalSensor_1;
		
}




