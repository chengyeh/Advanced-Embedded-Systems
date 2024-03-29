/*--Task_ReportData.c
 *
 *  Author:			Gary J. Minden
 *	Organization:	KU/EECS/EECS 690
 *  Date:			March 13, 2016 (B60313)
 *
 *  Description:	Transmits data via UART to PC.
 *
 *  Modification:	2016-09-22 (B60922)
 *  				Changed number of value to 4 from 2.
 *
 */

#include	<stddef.h>
#include	<stdbool.h>
#include	<stdint.h>
#include	<stdarg.h>

#include	"Drivers/UARTStdio_Initialization.h"
#include	"Drivers/uartstdio.h"
#include	"Tasks/Task_ReportData.h"

#include	"FreeRTOS.h"
#include	"task.h"
#include	"queue.h"

//
//	Define the ReportData Queue
//
extern QueueHandle_t ReportData_Queue = NULL;

//
//	Define the ReportData Task
//
extern void Task_ReportData( void *pvParameters ) {

	ReportData_Item				theReport;
	BaseType_t					ReportQueue_Status;
	ReportData_OutputFormat		ReportData_Format = Mathematica_List;

	//
	//	Ensure UARTStdio is initialized
	//
	UARTStdio_Initialization();

	//
	//	Define ReportData_Queue
	//
	ReportData_Queue = xQueueCreate( 5, sizeof( ReportData_Item ) );

	while ( 1 )	{

		//
		//	Try to read ReportItem from ReportData_Queue.
		//	If a ReportData_Item is returned, print the contents
		//		to the UART via UARTStdioPrintf
		//
		ReportQueue_Status = xQueueReceive( ReportData_Queue,
											&theReport,
											10 * portTICK_PERIOD_MS );

		if ( ReportQueue_Status == pdTRUE ) {
			switch ( ReportData_Format ) {

				//
				//	Output in Excel Comma Separated format
				//
				case Excel_CSV:
					UARTprintf( "%08d,%02d,%d,%d,%d,%d\n",
								theReport.TimeStamp, theReport.ReportName,
								theReport.ReportValue_0, theReport.ReportValue_1,
								theReport.ReportValue_2, theReport.ReportValue_3);
					break;

				//
				//	Output in Mathematica List format
				//
				case Mathematica_List:
					UARTprintf( "{ %08d, %02d, %d, %d, %d, %d },\n",
								theReport.TimeStamp, theReport.ReportName,
								theReport.ReportValue_0, theReport.ReportValue_1,
								theReport.ReportValue_2, theReport.ReportValue_3 );

					break;
			}
		}

	}
}
