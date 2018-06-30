
#ifndef WingAlgo_HEADER
#define WingAlgo_HEADER


int steeringWheelToCornerRadius(int steeringWheel);
float steeringWheelToAckermanRatio(int steeringWheel);
int DFCalc(int friction, int driverMass, int rowRollAngle, int velocity, int rowSteeringWheel);
void DFToWingAngle(int active, int* wing1, int* wing2, int* wing3, int* wing4);
void anglesUpdater(int gyroPitch,int brakingSensor,int accelSensor,int steeringWheel,int friction,int driverMass,int rowRollAngle,int velocity,int* wing1, int* wing2, int* wing3, int* wing4);

#endif // !WingAlgo_HEADER
