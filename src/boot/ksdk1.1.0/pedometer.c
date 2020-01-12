// #include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "fsl_misc_utilities.h"
#include "fsl_device_registers.h"
#include "fsl_i2c_master_driver.h"
#include "fsl_spi_master_driver.h"
#include "fsl_rtc_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_power_manager.h"
#include "fsl_mcglite_hal.h"
#include "fsl_port_hal.h"

#include "gpio_pins.h"
#include "SEGGER_RTT.h"
#include "warp.h"

#include "devMMA8451Q.h"
#include "devSSD1331.h"
#include "pedometer.h"

/*
Function: pedometer 
* Executes the pedometer functionality
* This includes calling the calibration function, calculating the steps and displaying the steps
*/
void pedometer()
{
	calibratePedometer();

    while (1)
    {
        for (uint8_t i=1; i<numReadings; i++)
        {
            float tempResult = resultant[1];
            success = getSensorDataMMA8451Q(acceleration);
            X_acc[i] = acceleration[0];
            Y_acc[i] = acceleration[1];
            Z_acc[i] = acceleration[2];

            resultant[1] = sqrt(((X_acc[i] - X_avg) * (X_acc[i] - X_avg)) + ((Y_acc[i] - Y_avg) * (Y_acc[i] - Y_avg)) + ((Z_acc[i] - Z_avg) * (Z_acc[i] - Z_avg)));
            resultant[0] = tempResult;
            average = (resultant[1] + resultant[0]) / 2 ;
            SEGGER_RTT_printf(0, "Average: %d \n", (int)average);
            if (average > threshold && flag==false)
            {
                steps=steps+1;
                flag=true;
            
            }
            // Only once the step has ended, allow another step to start
            if (average < threshold  && flag==true)
            {
                flag = false;
                SEGGER_RTT_printf(0, "Steps: %d\n", steps);
                uint16_t prevVal = getCurrentDisplay();
                display(steps, prevVal);
            }

            OSA_TimeDelay(delay);
        }
    }
    
}

/*
Function: calibratePedometer
* Calibrate the pedometer
* Takes 20 readings of the acceleration from each axis and calculates the 
* average value
* This average value is used to remove the static offset from the acceleration readings
*/
void calibratePedometer()
{
    devSSD1331init();

    int sum_X = 0, sum_Y=0 , sum_Z= 0;

    for (uint8_t i=0; i<numReadings; i++)
    {
        success = getSensorDataMMA8451Q(acceleration);
        OSA_TimeDelay(delay);
        X_acc[i] = acceleration[0];
        sum_X += X_acc[i];
        Y_acc[i] = acceleration[1];
        sum_Y += Y_acc[i];
        Z_acc[i] = acceleration[2];
        sum_Z += Z_acc[i];
    }

    X_avg = sum_X/numReadingsF;
    Y_avg = sum_Y/numReadingsF;
    Z_avg = sum_Z/numReadingsF;

    SEGGER_RTT_printf(0, "X_avg: %d\t Y_avg: %d\t Z_avg: %d\n", (int)X_avg, (int)Y_avg, (int)Z_avg);

    resultant[0] = sqrt(((X_acc[numReadings-1] - X_avg) * (X_acc[numReadings-1] - X_avg)) + ((Y_acc[numReadings-1] - Y_avg) * (Y_acc[numReadings-1] - Y_avg)) + ((Z_acc[numReadings-1] - Z_avg) * (Z_acc[numReadings-1] - Z_avg)));

}

int movingAverage(int *window, long *sum, int pos, int numSamples, int nextNum)
{
  //Subtract the oldest number from the prev sum, add the new number
  *sum = *sum - window[pos] + nextNum;
  //Assign the nextNum to the position in the array
  window[pos] = nextNum;
  //return the average
  return *sum / numSamples;
}

    sum -= reads[ptr];
    sum += heartRateValue;
    reads[ptr] = heartRateValue;
    ave = sum / samp_siz;
    //Serial.println(heartRateValue);
    //Serial.println(ave);
    ptr++;
    ptr %= samp_siz;