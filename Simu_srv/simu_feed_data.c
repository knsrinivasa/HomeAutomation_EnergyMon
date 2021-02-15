/*
 * simu_feed_data.c
 *
 *  Created on: Nov 15, 2013
 *      Author: skn2kor
 */

#include "main_inf.h"

/*****************************************************************************************************************/

void feed_PQ_data(void)
{
	static uint32 signal_read_idx;
	uint8	Train_PQ_read_st, Run_PQ_read_st ;
//	char *header  = "Simu_run";

	Train_PQ_read_st = Get_Simu_Train_PQ_Read_St() ;
	Run_PQ_read_st =   Get_Simu_Run_PQ_Read_St() ;
//	printf("Train_PQ_read_st = %d\tRun_PQ_read_st = %d\t", Train_PQ_read_st, Run_PQ_read_st) ;

		if ( (Train_PQ_read_st == START_READ_TRAIN_PQ) || (Run_PQ_read_st == START_READ_RUN_PQ) )
	{
//		printf("******File Read\n") ;
		if(signal_read_idx<(Simu_read_data_Num) )
		{
			WattValue   = signal_P[signal_read_idx];
			VarValue    = signal_Q[signal_read_idx];

//			printf("\n ____________________________________________________________\n") ;
			printf("\tRead Idx = %d\tWattValue = %d\tVARValue = %d\n", signal_read_idx+1, WattValue, VarValue) ;
//			printf("|____________________________________________________________\n") ;


/*
			LOG_simu (header, WattValue, VarValue, signal_read_idx)  ;

			LOG(EventLog_P,"%d %d %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f\n",
					signal_read_idx ,
					WattValue,
					VarValue,
					0xF1,
					VarValue,
					0xF2,
					VarValue,
					0xF3,
					VarValue,
					0xFf,
					VarValue
			) ;
*/

			signal_read_idx++;
		}
		else
			Set_GUI_IP_Req_St (GUI_IP_REQUIRED) ;
	}

//		exit (0) ;

}

/*****************************************************************************************************************/




/*****************************************************************************************************************/


//void  s_printf (LogId_t log_id, char *format, ...)
int	s_printf (const char *fmt, ...)
{
	va_list		vArgs;
	char		log_file_name[20] ;
	char		file_ext[10]  = ".log";
	FILE		*ptr_log_file;
	char 		*ch = format ;
	int 		count_arg = 0 ;
	int 		idx=0, format_idx=0;
	char		format_arr[50] = {'\0'} ;
	uint8		ini_proc_st;

	idx = 0;
	Global_itr_cnt++ ;

	strcpy(log_file_name, st_LogTable[log_id].logName) ;
	strcat(log_file_name, file_ext) ;
	//printf("\nlog_file_name = %s\t", log_file_name)  ;



	format_idx=0;

	do {
		if(*ch == '%')
		{
			count_arg++;
		}
		else
			if(isalpha(*ch))
			{
				format_arr[format_idx++] = *ch ;
			}
		ch++;

	} while (*ch != '\n');

	//    printf("count = %d\n", count_arg);

	va_start(vArgs, format);

	for( idx=0; idx<count_arg; idx++ )
	{
		union Printable_t
		{
			int     i;
			float   f;
			char    c;
			char   *s;
		} Printable;


		switch( format_arr[idx] )
		{
		case 'd':
		Printable.i = va_arg( vArgs, int );
		fprintf(ptr_log_file,"%d", Printable.i)	;
		break;

		case 'f':
			Printable.f = va_arg( vArgs, double );
			fprintf(ptr_log_file,"%f", Printable.f)	;
			break;

		case 'c':
			Printable.c = va_arg( vArgs, char );
			fprintf(ptr_log_file,"%c", Printable.c)	;
			break;

		case 's':
			Printable.s = va_arg( vArgs, char * );
			fprintf(ptr_log_file,"%s", Printable.s)	;
			break;

		default:
			break;
		}

	}

	va_end(vArgs);

}


/*****************************************************************************************************************/
