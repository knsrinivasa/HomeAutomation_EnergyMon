/*
 ============================================================================
 Name        : NILM_Simu_V2.c
 Author      : skn2kor
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "main_inf.h"

/*****************************************************************************************************************/


int main(void)
{
	uint8 sched_speed = OS_SCHED_SLOW_DOWN_RATE ;
	system("cls") ;

	puts("\n\t==================================\n\t\tNILM Simulator V03\n\t==================================\n\n");
	printf("Schuduler running at speed [ 1 : %d ]\n", sched_speed) ;

//	Log_File_Remover() ;
//	Get_PQ_File ();
//	Log_head_writer();

//	simu_session_hdlr() ;

	// Scheduler
	os_sched();

	return EXIT_SUCCESS;
}


/*****************************************************************************************************************/
