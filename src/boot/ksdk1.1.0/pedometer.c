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
    int16_t arrNumbersX[4] = {0};
    int16_t arrNumbersY[4] = {0};
    int16_t arrNumbersZ[4] = {0};
    uint16_t filterCounter = 0;
    uint8_t smoothingNumber = 4;
    while (1)
    {
        maxX = -10000;
        minX = 10000;
        maxY = -10000;
        minZ = 10000;
        maxZ = -10000;
        minZ = 10000;
        for (uint8_t i=0; i<numReadings; i++)
        {
            // Read sensor data
            getSensorDataMMA8451Q(acceleration);
            OSA_TimeDelay(delay1);

            // Perform moving average filtering on the samples
            X_acc[i] = movingAverage(arrNumbersX, &sumX, filterCounter, smoothingNumber, acceleration[0] - X_avg);
            Y_acc[i] = movingAverage(arrNumbersY, &sumY, filterCounter, smoothingNumber, acceleration[1] - Y_avg);
            Z_acc[i]  = movingAverage(arrNumbersZ, &sumZ, filterCounter, smoothingNumber, acceleration[2] - Z_avg);

            if (X_acc[i] >= maxX)
            {
                maxX = X_acc[i];
            }
            if (X_acc[i] < minX)
            {
                minX = X_acc[i];
            }
            if (Y_acc[i] >= maxY)
            {
                maxY = Y_acc[i];
            }
            if (Y_acc[i] < minY)
            {
                minY = Y_acc[i];
            }
            if (Z_acc[i] >= maxZ)
            {
                maxZ = Z_acc[i];
            }
            if (Z_acc[i] < minZ)
            {
                minZ = Z_acc[i];
            }

            filterCounter++;
            filterCounter = filterCounter % smoothingNumber;

            switch (activeAxis) {
                case 1:
                    average = (X_acc[i]);  //+ X_acc[i-1])/2;
                    break;
                case 2:
                    average = (Y_acc[i] );//+ Y_acc[i-1])/2;
                    break;
                case 3:
                    average = (Z_acc[i] );// + Z_acc[i-1])/2;
                    break;
                default:
                    average = (Z_acc[i]);// + Z_acc[i-1])/2;
                    break;
            }

            SEGGER_RTT_printf(0, "Average: %d \n", (int)average);

            SEGGER_RTT_printf(0, "Threshold: %d \n", (int)threshold);

            // SEGGER_RTT_printf(0, "Active axis: %d \n", (int)activeAxis);

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

        }

        // findMin_Max(X_acc, numReadings, &minX, &maxX);
        // findMin_Max(Y_acc, numReadings, &minY, &maxY);
        // findMin_Max(Z_acc, numReadings, &minY, &maxY);

        if (((maxX - minX) >= (maxY - minY)) && ((maxX - minX) >= (maxZ - minZ)))
        {
            activeAxis = 1;
            threshold = (maxX + minX)/2;
        } else if (((maxY - minY) >= (maxX - minX)) && ((maxY - minY) >= (maxZ - minZ)))
        {
            activeAxis = 2;
            threshold = (maxY + minY)/2;
        } else 
        {
            activeAxis = 3;
            threshold = (maxZ + minZ)/2;
        }

        OSA_TimeDelay(delay2);
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

    int16_t sum_X = 0, sum_Y=0 , sum_Z= 0;
    maxX = -10000;
    minX = 10000;
    maxY = -10000;
    minZ = 10000;
    maxZ = -10000;
    minZ = 10000;

    for (uint8_t i=0; i<numReadings; i++)
    {
        getSensorDataMMA8451Q(acceleration);
        OSA_TimeDelay(delay1);
        OSA_TimeDelay(delay1);
        X_acc[i] = acceleration[0];
        sum_X += X_acc[i];
        Y_acc[i] = acceleration[1];
        sum_Y += Y_acc[i];
        Z_acc[i] = acceleration[2];
        sum_Z += Z_acc[i];

        if (X_acc[i] > maxX)
        {
            maxX = X_acc[i];
        }
        if (X_acc[i] < minX)
        {
            minX = X_acc[i];
        }
        if (Y_acc[i] > maxY)
        {
            maxY = Y_acc[i];
        }
        if (Y_acc[i] < minY)
        {
            minY = Y_acc[i];
        }
        if (Z_acc[i] > maxZ)
        {
            maxZ = Z_acc[i];
        }
        if (Z_acc[i] < minZ)
        {
            minZ = Z_acc[i];
        }
    }

    X_avg = sum_X/numReadings;
    Y_avg = sum_Y/numReadings;
    Z_avg = sum_Z/numReadings;

    maxX = maxX - X_avg;
    minX = minX - X_avg;
    maxY = maxY - Y_avg;
    minY = minY - Y_avg;
    maxZ = maxZ - Z_avg;
    minZ = minZ - Z_avg;

    if (((maxX - minX) >= (maxY - minY)) && ((maxX - minX) >= (maxZ - minZ)))
    {
        activeAxis = 1;
        threshold = (maxX + minX)/2;
    } else if (((maxY - minY) >= (maxX - minX)) && ((maxY - minY) >= (maxZ - minZ)))
    {
        activeAxis = 2;
        threshold = (maxY + minY)/2;
    } else 
    {
        activeAxis = 3;
        threshold = (maxZ + minZ)/2;
    }

    SEGGER_RTT_printf(0, "X_avg: %d\t Y_avg: %d\t Z_avg: %d\n", (int)X_avg, (int)Y_avg, (int)Z_avg);

    devSSD1331init();

}

int movingAverage(int16_t *window, long *sum, uint16_t pos, uint8_t numSamples, int16_t nextNum)
{
  //Subtract the oldest number from the prev sum, add the new number
  *sum = *sum - window[pos] + nextNum;
  //Assign the nextNum to the position in the array
  window[pos] = nextNum;
  //return the average
  return *sum / numSamples;
}