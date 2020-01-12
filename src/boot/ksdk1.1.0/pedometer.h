#ifdef WARP_BUILD_ENABLE_DEVMMA8451Q
volatile WarpI2CDeviceState			deviceMMA8451QState;
#endif

void calibratePedometer();
void pedometer();
int movingAverage(int *window, long *sum, int pos, int numSamples, int nextNum);

int numReadings = 20;
float numReadingsF = 20.0;

int16_t acceleration[3];
int16_t X_acc[20];
int16_t Y_acc[20];
int16_t Z_acc[20];
float resultant[2];
float average=0.0;
int delay=0500; //  50 ms delay
bool success = true;
bool flag = false;
float X_avg, Y_avg, Z_avg;
uint16_t steps =  0;
float threshold = 100;
uint16_t I2cPullupValue = 32768;