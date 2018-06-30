
#include <math.h>
//#include "WingAlgo.h"

int closeAngle1 = 100;//30;
int openAngle1 = 0;
int closeAngle2 = 100;//60;
int openAngle2 = 0;
int closeAngle3 = 50;//30;
int openAngle3 = 0;
int closeAngle4 = 50;//30;
int openAngle4 = 0;

float steeringWheelToAckermanRatio(int steeringWheel) {

	if (steeringWheel > 180 || steeringWheel < -180) {
		return 3.38;
	}
	else if (steeringWheel<7.64 && steeringWheel>-7.64) {
		return 4.0;
	}
	else if ((steeringWheel<19.10 && steeringWheel>7.64) || (steeringWheel<-7.64 && steeringWheel>-19.10)) {
		return 3.99;
	}
	else if ((steeringWheel<30.56 && steeringWheel>19.1) || (steeringWheel<-19.1 && steeringWheel>-30.56)) {
		return 3.98;
	}
	else if ((steeringWheel<38.20 && steeringWheel>30.56) || (steeringWheel<-30.56 && steeringWheel>-38.20)) {
		return 3.96;
	}
	else if ((steeringWheel<45.84 && steeringWheel>38.20) || (steeringWheel<-38.20 && steeringWheel>-45.84)) {
		return 3.95;
	}
	else if ((steeringWheel<57.3 && steeringWheel>45.84) || (steeringWheel<-45.84 && steeringWheel>-57.30)) {
		return 3.92;
	}
	else if ((steeringWheel<68.75 && steeringWheel>57.3) || (steeringWheel<-57.3 && steeringWheel>-68.75)) {
		return 3.87;
	}
	else if ((steeringWheel<76.39 && steeringWheel>68.75) || (steeringWheel<-68.75 && steeringWheel>-76.39)) {
		return 3.84;
	}
	else if ((steeringWheel<87.85 && steeringWheel>76.39) || (steeringWheel<-76.39 && steeringWheel>-87.85)) {
		return 3.77;
	}
	else if ((steeringWheel<95.49 && steeringWheel>87.85) || (steeringWheel<-87.85 && steeringWheel>-95.49)) {
		return 3.71;
	}
	else if ((steeringWheel<103.13 && steeringWheel>95.49) || (steeringWheel<-95.49 && steeringWheel>-103.13)) {
		return 3.64;
	}
	else if ((steeringWheel<110 && steeringWheel>103.13) || (steeringWheel<-103.13 && steeringWheel>-110)) {
		return 3.53;
	}
	else if ((steeringWheel<114.59 && steeringWheel>110) || (steeringWheel<-110 && steeringWheel>-114.59)) {
		return 3.38;
	}
	else {
		return 3.38;
	}
}

int steeringWheelToCornerRadius(int steeringWheel) {
	int steeringWheelSigned = (int)((steeringWheel*1.41) - 180);
	float vehicleLength = 1.61;
	float ratio = steeringWheelToAckermanRatio(steeringWheelSigned);
	float wheelAngle = steeringWheelSigned / ratio;
	float wheelAngleSin = sin(wheelAngle);
	return (int)(vehicleLength / wheelAngleSin);
}




/*
friction 0->255,[0->2]
driverMass 0->255,
rowRollAngle 
velocity 0->255 km/h
rowSteeringWheel 0->255
*/
int DFCalc(int friction, int driverMass, int rowRollAngle, int velocity,int radius) {
	float g = 9.81;
	float s_rowRollAngle = sin(rowRollAngle);
	int vehicleMass = 230;
	int mass = vehicleMass + driverMass;
	float numerator = (mass*(pow(velocity,2.0))*cos(rowRollAngle));
	float denumerator = (radius*((friction / 127.5) - s_rowRollAngle));
	float isNeeded= (numerator / denumerator)-mass*g;
	if (isNeeded > 0) {
		//xil_printf("DF: %d\n",isNeeded);
		return 1;
	}
	else {
		return 0;
	}

}

void DFToWingAngle(int active,int* wing1, int* wing2, int* wing3, int* wing4) {

	if (active) {
		*wing1 = closeAngle1;
		*wing2 = closeAngle2;
		*wing3 = closeAngle3;
		*wing4 = closeAngle4;
		return;
	}
	else{
		*wing1 = openAngle1;
		*wing2 = openAngle2;
		*wing3 = openAngle3;
		*wing4 = openAngle4;
	}
	return;
}

void pitchStab(int gyroPitch, int* wing1, int* wing2, int* wing3, int* wing4) {
	int gyroPitchTHPlus = 0;
	int gyroPitchTHMinus = 0;

	if (gyroPitch > gyroPitchTHPlus) {//rear higher
		*wing1 = *wing1 + 1;
		*wing2 = *wing2 + 1;
		*wing3 = *wing3 - 1;
		*wing4 = *wing4 - 1;
	}
	else if (gyroPitch < gyroPitchTHMinus) {//rear higher
		*wing1 = *wing1 - 1;
		*wing2 = *wing2 - 1;
		*wing3 = *wing3 + 1;
		*wing4 = *wing4 + 1;
	}
	if (*wing1 > closeAngle1) {
		*wing1 = closeAngle1;
	}
	if (*wing2 > closeAngle2) {
		*wing2 = closeAngle2;
	}
	if (*wing3 > closeAngle3) {
		*wing3 = closeAngle3;
	}
	if (*wing4 > closeAngle4) {
		*wing4 = closeAngle4;
	}
	if (*wing1 < openAngle1) {
		*wing1 = openAngle1;
	}
	if (*wing2 < openAngle2) {
		*wing2 = openAngle2;
	}
	if (*wing3 < openAngle3) {
		*wing3 = openAngle3;
	}
	if (*wing4 < openAngle4) {
		*wing4 = openAngle4;
	}
	return;
}
/*
brakingSensor 0->255 [0%->100%]
accelSensor 0->255 [0%->100%]
wing1 rear high
wing2 rear low
wing3 front right
wing4 front left
*/
void anglesUpdater(int gyroPitch,int brakingSensor,int accelSensor,int steeringWheel,int friction,int driverMass,int rowRollAngle,int velocity,int* wing1, int* wing2, int* wing3, int* wing4) {
	float brakingTH = 0.7;
	float accelTH = 0.7;
	
	int radius = steeringWheelToCornerRadius(steeringWheel);
	int active = DFCalc(friction, driverMass, rowRollAngle, velocity, radius);
	if (active > 0) {
		DFToWingAngle(1, wing1, wing2, wing3, wing4); //all wings are closed
	}
	else {
		if (radius> 50 || radius<-50){
			DFToWingAngle(0, wing1, wing2, wing3, wing4);//all wings are open
			return;
		}
		else {
			if (brakingSensor > (255 * brakingTH)) {
				DFToWingAngle(1, wing1, wing2,wing3,wing4);//all wings are closed
				return;
			}
			else if (accelSensor > (255 * accelTH)) {
				DFToWingAngle(0, wing1, wing2, wing3, wing4); //all wings are open
				return;
			}
			else {
				pitchStab(gyroPitch, wing1, wing2, wing3, wing4);
				return;
			}
		}
	}

}
