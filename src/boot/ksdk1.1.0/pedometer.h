#ifdef WARP_BUILD_ENABLE_DEVMMA8451Q
volatile WarpI2CDeviceState			deviceMMA8451QState;
#endif

void calibratePedometer();
int movingAverage(int16_t *window, long *sum, uint16_t pos, uint8_t numSamples, int16_t nextNum);
void pedometer();

uint8_t numReadings = 50;

int16_t acceleration[3];
// int16_t X_acc[20];
// int16_t Y_acc[20];
// int16_t Z_acc[20];

int16_t X_acc;
int16_t Y_acc;
int16_t Z_acc;

int16_t X_acc_Prev;
int16_t Y_acc_Prev;
int16_t Z_acc_Prev;

int16_t minX;
int16_t maxX;
int16_t minY;
int16_t maxY;
int16_t minZ;
int16_t maxZ;

uint8_t activeAxis=1;

int16_t threshold=0;

int tempXSum = 0;
int tempYSum = 0;
int tempZSum = 0;

long sumX=0;
long sumY=0;
long sumZ=0;
int16_t average=0;
int16_t previous=0;
uint16_t delay1=20; //  20 ms delay
uint16_t delay2=20; //  50 ms delay
bool flag = false;
int X_avg=0, Y_avg=0, Z_avg=0;
uint16_t steps =  0;
uint16_t I2cPullupValue = 32768;