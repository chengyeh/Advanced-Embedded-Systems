/*
 * Task_mpu9150.h
 *
 * Created on: Sep 18, 2017
 * Author: Cheng-Yeh Lee, Andrew Megaris
 */

//initialization flag
extern bool isMPU9150Initialized;

//function prototypes
extern void MPU9150Init();
extern void MPU9150Callback(void *pvCallbackData, uint_fast8_t ui8Status);
extern void Task_MPU9150(void *pvParameters);


/*
 * extern bool isMPU9150Initialized = false;
 * must be declared in the main file for the MPU9150
 * initialization functions.
 */
