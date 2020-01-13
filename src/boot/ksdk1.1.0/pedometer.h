#ifdef WARP_BUILD_ENABLE_DEVMMA8451Q
volatile WarpI2CDeviceState			deviceMMA8451QState;
#endif

void calibratePedometer();
int movingAverage(int16_t *window, long *sum, uint16_t pos, uint8_t numSamples, int16_t nextNum);
void pedometer();

uint8_t numReadings = 20;

int16_t acceleration[3];
int16_t X_acc[20];
int16_t Y_acc[20];
int16_t Z_acc[20];

int16_t minX;
int16_t maxX;
int16_t minY;
int16_t maxY;
int16_t minZ;
int16_t maxZ;

uint8_t activeAxis=2;

int16_t threshold;

long sumX=0;
long sumY=0;
long sumZ=0;
int16_t average=0;
uint16_t delay1=1; //  1 ms delay
uint16_t delay2=50; //  50 ms delay
bool flag = false;
int X_avg, Y_avg, Z_avg;
uint16_t steps =  0;
uint16_t I2cPullupValue = 32768;