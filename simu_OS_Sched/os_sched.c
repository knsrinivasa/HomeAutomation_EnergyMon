
#include "main_macros.h"
#include "os_sched.h"

#define		SIMU_OS_DEBUG_PRINTF	DISABLED


/*********************************************************************************************************/

/**
*********************************************************************************************************
*                                           OS_Table_Sri2ms()
*
* Description :  Execution of function available in 2 ms table
*
* Argument(s) :  none
*
* Return(s)   :  none.
*
*********************************************************************************************************
*/
void  OS_Table_SriIni(void)
{
    uint8 i;

#if (SIMU_OS_DEBUG_PRINTF == ENABLED)
    printf("In Ini Task handler\n") ;
#endif

    for( i = 0; i< (sizeof(OS_TASKSri_PROCINI)/sizeof(OS_TASKSri_PROCINI[0])); i++)
    {
       (*OS_TASKSri_PROCINI [i]) ();
    }
}

/*********************************************************************************************************/

/**
*********************************************************************************************************
*                                           OS_Table_Sri2ms()
*
* Description :  Execution of function available in 2 ms table
*
* Argument(s) :  none
*
* Return(s)   :  none.
*
*********************************************************************************************************
*/
void  OS_Table_Sri2ms(void)
{
    uint8 i;

#if (SIMU_OS_DEBUG_PRINTF == ENABLED)
    printf("In 2ms Task handler\n") ;
#endif

    for( i = 0; i< (sizeof(OS_TASKSri2MS)/sizeof(OS_TASKSri2MS[0])); i++)
    {
       (*OS_TASKSri2MS [i]) ();
    }
}

/**
*********************************************************************************************************
*                                           OS_Table_Sri10ms()
*
* Description :  Execution of function available in 10 ms table
*
* Argument(s) :  none
*
* Return(s)   :  none.
*
*********************************************************************************************************
*/
void  OS_Table_Sri10ms(void)
{
    uint8 i;
#if (SIMU_OS_DEBUG_PRINTF == ENABLED)
    printf("\tIn 10ms Task handler\n") ;
#endif

    for( i = 0; i< (sizeof(OS_TASKSri10MS)/sizeof(OS_TASKSri10MS[0])); i++)
    {
       (*OS_TASKSri10MS [i]) ();
    }
}

/**
*********************************************************************************************************
*                                           OS_Table_Sri20ms()
*
* Description :  Execution of function available in 20 ms table
*
* Argument(s) :  none
*
* Return(s)   :  none.
*
*********************************************************************************************************
*/
void  OS_Table_Sri20ms(void)
{
    uint8 i;
#if (SIMU_OS_DEBUG_PRINTF == ENABLED)
    printf("\t\tIn 20ms Task handler\n") ;
#endif

    for( i = 0; i< (sizeof(OS_TASKSri20MS)/sizeof(OS_TASKSri20MS[0])); i++)
    {
       (*OS_TASKSri20MS [i]) ();
    }
}

/**
*********************************************************************************************************
*                                           OS_Table_Sri100ms()
*
* Description :  Execution of function available in 100 ms table
*
* Argument(s) :  none
*
* Return(s)   :  none.
*
*********************************************************************************************************
*/
void  OS_Table_Sri100ms(void)
{
    uint8 i;
#if (SIMU_OS_DEBUG_PRINTF == ENABLED)
    printf("\t\t\tIn 100ms Task handler\n") ;
#endif

    for( i = 0; i< (sizeof(OS_TASKSri100MS)/sizeof(OS_TASKSri100MS[0])); i++)
    {
       (*OS_TASKSri100MS [i]) ();
    }
}

/**
*********************************************************************************************************
*                                           OS_Table_Sri200ms()
*
* Description :  Execution of function available in 200 ms table
*
* Argument(s) :  none
*
* Return(s)   :  none.
*
*********************************************************************************************************
*/
//#if OS_HMI_EN != FALSE
void  OS_Table_Sri200ms(void)
{
    uint8 i;
#if (SIMU_OS_DEBUG_PRINTF == ENABLED)
    printf("\t\t\t\t\tIn 200ms Task handler\n") ;
#endif

    for( i = 0; i< (sizeof(OS_TASKSri200MS)/sizeof(OS_TASKSri200MS[0])); i++)
    {
       (*OS_TASKSri200MS [i]) ();
    }
}
//#endif

/**
*********************************************************************************************************
*                                           OS_Table_Sri1000mSec()
*
* Description :  Execution of function available in 1000mSec table
*
* Argument(s) :  none
*
* Return(s)   :  none.
*
*********************************************************************************************************
*/
void  OS_Table_Sri1000ms(void)
{
    uint8 i;
#if (SIMU_OS_DEBUG_PRINTF == ENABLED)
    printf("\t\t\t\t\t\tIn 1000ms Task handler\n") ;
#endif
    for( i = 0; i< (sizeof(OS_TASKSri1000MS)/sizeof(OS_TASKSri1000MS[0])); i++)
    {
       (*OS_TASKSri1000MS [i]) ();
    }
}



/***********************************************************************************************************/
/***********************************************************************************************************/

void delay(double dly)
{

	/* save start clock tick */
	const clock_t start = clock();
	clock_t current;
	do {
		/* get current clock tick */
		current = clock();
		printf("CLOCKS_PER_SEC= %d\t\tCurrent clock = %d\t\tTime diff = %d\n",
				CLOCKS_PER_SEC, current, (current - start));
		/* break loop when the requested number of seconds have elapsed */
	} while ((double) (current - start) / CLOCKS_PER_SEC < dly);
}


/*****************************************************************************************************************/

//int os_sched( float *index, float *signal_P, float *signal_Q, int N )
int os_sched( void )
{
	clock_t start_clock_2ms, current_clock_2ms, diff_clock_2ms;
	clock_t start_clock_10ms, current_clock_10ms, diff_clock_10ms;
	clock_t start_clock_20ms, current_clock_20ms, diff_clock_20ms;
	clock_t start_clock_100ms, current_clock_100ms, diff_clock_100ms;
	clock_t start_clock_200ms, current_clock_200ms, diff_clock_200ms;
	clock_t start_clock_1000ms, current_clock_1000ms, diff_clock_1000ms;

	uint8 ini_proc_st ;

	puts("In scheduler\n");

	start_clock_2ms = clock();

	while (1)
	{

/*-------------------------------------------------------------*/
		ini_proc_st = Get_Ini_Proc_Sched_st() ;

		if( ini_proc_st == PROC_INIT )
		{
			//Log_head_writer () ;
			OS_Table_SriIni();
			Set_Ini_Proc_Sched_st(INI_SCHEDULED) ;
		}

/*-------------------------------------------------------------*/

		current_clock_2ms = clock();
		diff_clock_2ms = (current_clock_2ms - start_clock_2ms);

		if ( ( diff_clock_2ms >= (TASKS_2MS * OS_SCHED_SLOW_DOWN_RATE) ) ) // && (diff_clock_2ms < MS_10))
		{
			OS_Table_Sri2ms();
			start_clock_2ms = current_clock_2ms;
		}

/*-------------------------------------------------------------*/

		current_clock_10ms = clock();
		diff_clock_10ms = (current_clock_10ms - start_clock_10ms);

		if ( ( diff_clock_10ms >= (TASKS_10MS * OS_SCHED_SLOW_DOWN_RATE) ) ) //  && (diff_clock_10ms < MS_20))
		{
			OS_Table_Sri10ms();
			start_clock_10ms = current_clock_10ms;
		}


/*-------------------------------------------------------------*/

		current_clock_20ms = clock();
		diff_clock_20ms = (current_clock_20ms - start_clock_20ms);

		if ( ( diff_clock_20ms >= (TASKS_20MS * OS_SCHED_SLOW_DOWN_RATE) ) ) //  && (diff_clock_20ms < MS_100))
		{
			OS_Table_Sri20ms();
			start_clock_20ms = current_clock_20ms;
		}


/*-------------------------------------------------------------*/

		current_clock_100ms = clock();
		diff_clock_100ms = (current_clock_100ms - start_clock_100ms);

		if ( ( diff_clock_100ms >= (TASKS_100MS * OS_SCHED_SLOW_DOWN_RATE) ) ) // && (diff_clock_100ms < MS_200))
		{
			OS_Table_Sri100ms();
			start_clock_100ms = current_clock_100ms;
		}


/*-------------------------------------------------------------*/

		current_clock_200ms = clock();
		diff_clock_200ms = (current_clock_200ms - start_clock_200ms);

		if ( ( diff_clock_200ms >= (TASKS_200MS * OS_SCHED_SLOW_DOWN_RATE) ) ) // && (diff_clock_200ms < MS_1000))
		{
			OS_Table_Sri200ms();
			start_clock_200ms = current_clock_200ms;
		}


/*-------------------------------------------------------------*/

		current_clock_1000ms = clock();
		diff_clock_1000ms = (current_clock_1000ms - start_clock_1000ms);

		if ( ( diff_clock_1000ms >= (TASKS_1000MS * OS_SCHED_SLOW_DOWN_RATE) ) ) // && (diff_clock_1000ms < MS_2000))
		{
			OS_Table_Sri1000ms();
			start_clock_1000ms = current_clock_1000ms;
		}

/*-------------------------------------------------------------*/

	}


	puts("Sched exit\n");


	return 0 ;
}

