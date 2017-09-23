/*
 * Task_mpu9150.c
 *
 * Created on: Sep 18, 2017
 * Author: Cheng-Yeh Lee, Andrew Megaris
 */

#include	"inc/hw_ints.h"
#include	"inc/hw_memmap.h"
#include	"inc/hw_types.h"
#include	"inc/hw_uart.h"

#include	<stddef.h>
#include	<stdbool.h>
#include	<stdint.h>
#include	<stdarg.h>
#include<stdio.h>

#include	"driverlib/sysctl.h"
#include	"driverlib/pin_map.h"
#include	"driverlib/gpio.h"

#include	"FreeRTOS.h"
#include	"task.h"
#include"driverlib/timer.h"
#include	"Drivers/uartstdio.h"

#include"semphr.h"
#include"Task_I2C7_Handler.h"

#include	"sensorlib/ak8975.h"
#include"sensorlib/mpu9150.h"

#include	"Tasks/Task_ReportData.h"

#include	"Task_mpu9150.h"

//global variables
extern bool isMPU9150Initialized = false;
float fAccel[3];
float fGyro[3];
tMPU9150 sMPU9150;
volatile bool g_bMPU9150Done;
extern tI2CMInstance I2C7_Instance;
ReportData_Item	accelerometerReport;
ReportData_Item gyroscopeReport;
extern volatile uint32_t xPortSysTickCount;

extern void MPU9150Init(){

	//check if the MPU9150 is already initialized
	if (!(isMPU9150Initialized)){
		I2C7_Initialization();
		//Initialize the MPU9150
		uint_fast8_t stat9150;
		stat9150 = MPU9150Init(&sMPU9150, I2C7_Instance_Ref, 0x68, MPU9150Callback, 0);
		while(!g_bMPU9150Done){}

		//set the MPU9150 initialization flag to true
		isMPU9150Initialized = true;

		UARTprintf( ">>>>MPU9150Init; Status: %02X\n", stat9150 );
	}
}

extern void MPU9150Callback(void *pvCallbackData, uint_fast8_t ui8Status) {

	// See if an error occurred.s
	if (ui8Status != I2CM_STATUS_SUCCESS) {

		// An error occurred, so handle it here if required.
		UARTprintf( ">>>>I2C7 in 9150 callback Error: %02X\n", ui8Status );
	}

	// Indicate that the MPU9150 transaction has completed.
	g_bMPU9150Done = true;
}

extern void Task_MPU9150(void *pvParameters) {

	//initialize the MPU9150
	MPU9150Init();

	//FreeRTOS task loop
	while (1) {
		//Read the data from the MPU9150
		g_bMPU9150Done = false;
		MPU9150DataRead(&sMPU9150, MPU9150Callback, 0);
		while(!g_bMPU9150Done){}

		MPU9150DataAccelGetFloat(&sMPU9150, &fAccel[0], &fAccel[1], &fAccel[2]);

		//load the data into the accelerometer report
		accelerometerReport.TimeStamp = xPortSysTickCount;
		accelerometerReport.ReportName = 51;
		accelerometerReport.ReportValue_0 = 	(int)(fAccel[0] * 1000000);
		accelerometerReport.ReportValue_1 = 	(int)(fAccel[1] * 1000000);
		accelerometerReport.ReportValue_2 =	(int)(fAccel[2] * 1000000);
		accelerometerReport.ReportValue_3 = 	0;

		//send the report data to the data report task
		xQueueSend(ReportData_Queue, &accelerometerReport, 0 );

		MPU9150DataGyroGetFloat(&sMPU9150, &fGyro[0], &fGyro[1], &fGyro[2]);

		//load the data into the gyroscope report
		gyroscopeReport.TimeStamp = xPortSysTickCount;
		gyroscopeReport.ReportName = 52;
		gyroscopeReport.ReportValue_0 = 	(int)(fGyro[0] * 1000000);
		gyroscopeReport.ReportValue_1 = 	(int)(fGyro[1] * 1000000);
		gyroscopeReport.ReportValue_2 =	(int)(fGyro[2] * 1000000);
		gyroscopeReport.ReportValue_3 = 	0;

		//send the report data to the data report task
		xQueueSend(ReportData_Queue, &gyroscopeReport, 0 );

		//FreeRTOS task delay
		vTaskDelay((10000 * configTICK_RATE_HZ) / 10000);
	}
}


