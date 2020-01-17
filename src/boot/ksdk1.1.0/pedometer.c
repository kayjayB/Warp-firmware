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
    
    // Call the calibration  function
	calibratePedometer(&X_avg, &Y_avg, &Z_avg);

    tempXSum = 0;
    tempYSum = 0;
    tempZSum = 0;

    // Read in 50 samples to calculate the threshold for the first loop of the step counter
    for (uint8_t i=0; i<numReadings; i++)
    {
        // Read sensor data
        getSensorDataMMA8451Q(acceleration);

        X_acc = acceleration[0];
        Y_acc = acceleration[1];
        Z_acc = acceleration[2];

        // In the first loop, set the minimum and maximum acceleration as the current sample
        if (i==0)
        {
            maxX = X_acc;
            minX = X_acc;
            maxY = Y_acc;
            minY = Y_acc;
            maxZ = Z_acc;
            minZ = Z_acc;
        }

        // Calculate the sum of the acceleration for each axis
        tempXSum += X_acc;
        tempYSum += Y_acc;
        tempZSum += Z_acc;

        // Assign the minimum and maximum acceleration for each axis
        // If the current sample is the minimum or maximum, update the minimum or maximum
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

        // 20 ms delay
        OSA_TimeDelay(delay1);
    }

    // Find the active axis by finding the axis with the greatest variation between minimum and maximum
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

    //  Find the offset acceleration for each axis
    X_avg = tempXSum/numReadings;
    Y_avg = tempYSum/numReadings;
    Z_avg = tempZSum/numReadings;

    //Assign the previous sample
    getSensorDataMMA8451Q(acceleration);
    X_acc_Prev = acceleration[0];
    Y_acc_Prev = acceleration[1];
    Z_acc_Prev= acceleration[2];

    while (1)
    {
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

            // In the first loop, set the minimum and maximum acceleration as the current sample
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

            // Assign the minimum and maximum acceleration for each axis
            // If the current sample is the minimum or maximum, update the minimum or maximum
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

            // Update the index for the moving avergae filter
            filterCounter++;
            filterCounter = filterCounter % smoothingNumber;

            // Based on the active axis, assign the current and previous sample to the variables that will be compared
            switch (activeAxis) {
                case 1:
                    average = (X_acc);
                    previous = X_acc_Prev;
                    break;
                case 2:
                    average = (Y_acc);
                    previous = Y_acc_Prev;
                    break;
                case 3:
                    average = (Z_acc);
                    previous = Z_acc_Prev;
                    break;
                default:
                    average = (Z_acc);
                    previous = Z_acc_Prev;
                    break;
            }

            SEGGER_RTT_printf(0, "%d, %d, %d, %d, %d, %d \n", X_acc, Y_acc, Z_acc, activeAxis, threshold, steps);

            // If the current sample crosses the threshold on a negative gradient, increment the step counter
            if ((previous > threshold) && (threshold > average) && (abs(average - previous)>200))
            {
                steps=steps+1;
                uint16_t prevVal = getCurrentDisplay();
                display(steps, prevVal);
            }

            // Shorter time delay for the last loop to compensate for the extra operations
            if (i==(numReadings-1))
            {
                OSA_TimeDelay(delay2);
            }
            else
                OSA_TimeDelay(delay1);

            // Assign the previous accelerations for the next loop
            X_acc_Prev = X_acc;
            Y_acc_Prev = Y_acc;
            Z_acc_Prev = Z_acc;

        }

        // Find the active axis by finding the axis with the greatest variation between minimum and maximum
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