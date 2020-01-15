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
    int16_t arrNumbersX[4] = {0};
    int16_t arrNumbersY[4] = {0};
    int16_t arrNumbersZ[4] = {0};
    uint16_t filterCounter = 0;
    uint8_t smoothingNumber = 4;
    
	calibratePedometer(&X_avg, &Y_avg, &Z_avg);

    tempXSum = 0;
    tempYSum = 0;
    tempZSum = 0;

    for (uint8_t i=0; i<numReadings; i++)
    {
        // Read sensor data
        getSensorDataMMA8451Q(acceleration);

        // Perform moving average filtering on the samples
        // X_acc = movingAverage(arrNumbersX, &sumX, filterCounter, smoothingNumber, acceleration[0]);// - X_avg));
        // Y_acc = movingAverage(arrNumbersY, &sumY, filterCounter, smoothingNumber, acceleration[1]);// - Y_avg));
        // Z_acc = movingAverage(arrNumbersZ, &sumZ, filterCounter, smoothingNumber, acceleration[2]);// - Z_avg));

        X_acc = acceleration[0];
        Y_acc = acceleration[1];
        Z_acc = acceleration[2];

        if (i==0)
        {
            maxX = X_acc;
            minX = X_acc;
            maxY = Y_acc;
            minY = Y_acc;
            maxZ = Z_acc;
            minZ = Z_acc;
        }

        // SEGGER_RTT_printf(0, "X_avg: %d\t Y_avg: %d\t Z_avg: %d\n", (int)X_avg, (int)Y_avg, (int)Z_avg);

        tempXSum += X_acc;
        tempYSum += Y_acc;
        tempZSum += Z_acc;

        if (X_acc >= maxX)
        {
            maxX = X_acc;
        }
        if (X_acc < minX)
        {
            minX = X_acc;
        }
        if (Y_acc >= maxY)
        {
            maxY = Y_acc;
        }
        if (Y_acc < minY)
        {
            minY = Y_acc;
        }
        if (Z_acc >= maxZ)
        {
            maxZ = Z_acc;
        }
        if (Z_acc < minZ)
        {
            minZ = Z_acc;
        }

        // filterCounter++;
        // filterCounter = filterCounter % smoothingNumber;

        OSA_TimeDelay(delay1);
    }

    if (((maxX - minX) >= (maxY - minY)) && ((maxX - minX) >= (maxZ - minZ)))
    {
        activeAxis = 1;
        threshold = (maxX + minX)/2;
    } else if (((maxY - minY) >= (maxX - minX)) && ((maxY - minY) >= (maxZ - minZ)))
    {
        activeAxis = 2;
        threshold = (maxY + minY)/2;
    } else if (((maxZ - minZ) >= (maxX - minX)) && ((maxZ - minZ) >= (maxY - minY)))
    {
        activeAxis = 3;
        threshold = (maxZ + minZ)/2;
    }
    X_avg = tempXSum/numReadings;
    Y_avg = tempYSum/numReadings;
    Z_avg = tempZSum/numReadings;

    getSensorDataMMA8451Q(acceleration);
    X_acc_Prev = acceleration[0];
    Y_acc_Prev = acceleration[1];
    Z_acc_Prev= acceleration[2];

    while (1)
    {
        tempXSum = 0;
        tempYSum = 0;
        tempZSum = 0;
        for (uint8_t i=0; i<numReadings; i++)
        {
            // Read sensor data
            getSensorDataMMA8451Q(acceleration);
            int tempX = acceleration[0] - X_avg;
            int tempY = acceleration[1] - Y_avg;
            int tempZ = acceleration[2] - Z_avg;

            // Perform moving average filtering on the samples
            X_acc = movingAverage(arrNumbersX, &sumX, filterCounter, smoothingNumber, tempX);
            Y_acc = movingAverage(arrNumbersY, &sumY, filterCounter, smoothingNumber, tempY);
            Z_acc = movingAverage(arrNumbersZ, &sumZ, filterCounter, smoothingNumber, tempZ);

            if (i==0)
            {
                maxX = X_acc;
                minX = X_acc;
                maxY = Y_acc;
                minY = Y_acc;
                maxZ = Z_acc;
                minZ = Z_acc;
            }

            // SEGGER_RTT_printf(0, "%d, %d, %d, %d, %d, %d \n", X_acc, Y_acc, Z_acc, tempX, tempY, tempZ);

            tempXSum += X_acc;
            tempYSum += Y_acc;
            tempZSum += Z_acc;

            if (X_acc >= maxX)
            {
                maxX = X_acc;
            }
            if (X_acc < minX)
            {
                minX = X_acc;
            }
            if (Y_acc >= maxY)
            {
                maxY = Y_acc;
            }
            if (Y_acc < minY)
            {
                minY = Y_acc;
            }
            if (Z_acc >= maxZ)
            {
                maxZ = Z_acc;
            }
            if (Z_acc < minZ)
            {
                minZ = Z_acc;
            }

            filterCounter++;
            filterCounter = filterCounter % smoothingNumber;

            switch (activeAxis) {
                case 1:
                    average = (X_acc);  //+ X_acc[i-1])/2;
                    previous = X_acc_Prev;
                    break;
                case 2:
                    average = (Y_acc);//+ Y_acc[i-1])/2;
                    previous = Y_acc_Prev;
                    break;
                case 3:
                    average = (Z_acc);// + Z_acc[i-1])/2;
                    previous = Z_acc_Prev;
                    break;
                default:
                    average = (Z_acc);// + Z_acc[i-1])/2;
                    previous = Z_acc_Prev;
                    break;
            }

            // SEGGER_RTT_printf(0, "%d, %d, %d, %d, %d \n", X_acc, Y_acc, Z_acc, activeAxis, threshold);

            // if (average > threshold && flag==false && abs((average - previous)>50))
            if ((previous > threshold) && (threshold > average) && (abs(average - previous)>100))
            {
                steps=steps+1;
                flag=true;
                // uint16_t prevVal = getCurrentDisplay();
                // display(steps, prevVal);
                SEGGER_RTT_printf(0, "Steps: %d \n", steps);
            }
            // Only once the step has ended, allow another step to start
            // if (average < threshold  && flag==true)
            // {
            //     flag = false;
            //     // SEGGER_RTT_printf(0, "Steps: %d\n", steps);
            //     uint16_t prevVal = getCurrentDisplay();
            //     display(steps, prevVal);
            // }

            OSA_TimeDelay(delay1);
            X_acc_Prev = X_acc;
            Y_acc_Prev = Y_acc;
            Z_acc_Prev = Z_acc;

        }

        if (((maxX - minX) >= (maxY - minY)) && ((maxX - minX) >= (maxZ - minZ)))
        {
            activeAxis = 1;
            threshold = (maxX + minX)/2;
        } else if (((maxY - minY) >= (maxX - minX)) && ((maxY - minY) >= (maxZ - minZ)))
        {
            activeAxis = 2;
            threshold = (maxY + minY)/2;
        } else if (((maxZ - minZ) >= (maxX - minX)) && ((maxZ - minZ) >= (maxY - minY)))
        {
            activeAxis = 3;
            threshold = (maxZ + minZ)/2;
        }
    }
    
}

/*
Function: calibratePedometer
* Calibrate the pedometer
* Takes 50 readings of the acceleration from each axis and calculates the 
* average value
* This average value is used to remove the static offset from the acceleration readings
*/
void calibratePedometer(int *X_avg, int *Y_avg, int *Z_avg)
{

    int16_t sum_X = 0, sum_Y=0 , sum_Z= 0;

    for (uint8_t i=0; i<numReadings; i++)
    {
        getSensorDataMMA8451Q(acceleration);
        OSA_TimeDelay(delay1);
        X_acc = acceleration[0];
        sum_X += X_acc;
        Y_acc = acceleration[1];
        sum_Y += Y_acc;
        Z_acc = acceleration[2];
        sum_Z += Z_acc;
    }

    *X_avg = sum_X/numReadings;
    *Y_avg = sum_Y/numReadings;
    *Z_avg = sum_Z/numReadings;

    SEGGER_RTT_printf(0, "X_avg: %d\t Y_avg: %d\t Z_avg: %d\n", (int)*X_avg, (int)*Y_avg, (int)*Z_avg);

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