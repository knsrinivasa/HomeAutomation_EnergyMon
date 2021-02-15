/*
 * simu_log_hdlr.c
 *
 *  Created on: Nov 15, 2013
 *      Author: skn2kor
 */

#include "main_inf.h"


/*****************************************************************************************************************/

void print_data(void)
{
static int i;

//	for ( i = 0; i < Simu_read_data_Num; i++)
		if(i<Simu_read_data_Num)
		{
//			printf("idx = %d\tcol1 = %lf\tcol2 = %lf\tcol3 = %lf\n", i+1, index[i], signal_P[i], signal_Q[i]);
			i++;
		}

}


/*****************************************************************************************************************/

void LOG_simu (char *file_header, uint32 Watt, uint32 Var, uint32 i)
{

	FILE *ptr_log_file;
	char log_file_name[20] ;// = file_header ;
	char file_ext[10]  = ".log";

	strcpy(log_file_name, file_header) ;
 	strcat(log_file_name, file_ext) ;

 	ptr_log_file = fopen(log_file_name, "a");

		if (!ptr_log_file)
		{
			printf("\nLog File not opened\n");
			exit (0) ;
		}
		else
		{
//			printf("\nLog File opened\n");
			if(i == 0)
				fprintf(ptr_log_file,"%% %s\n", file_header)	;
		}

//	printf("\ni = %d\tWattValue = %d\tVarValue = %d\n", i+1, Watt, Var);
	fprintf(ptr_log_file,"Idx = %d \tWattValue = %d\tVarValue = %d\n", i+1, Watt, Var)	;

	fclose(ptr_log_file) ;
}


/*****************************************************************************************************************/

void Log_head_writer(void)
{
	LOG_HEADER (powerlog, "%s\n", "%% 0:powerlog.log\n");
	LOG_HEADER (powerlog, "%s\n", "%% 0:u32iterationCnt 1:Gui_Voltage 2:Gui_Current 3:Gui_Power 4:Gui_React 5:Volt_Trn 6:P_Calib 7:Q_Calib 8:P_Calib_Ratio 9:Q_Calib_Ratio 10:P_RatVtg_Corr 11:Q_RatVtg_Corr 12:No.Of_Dv_trained 13:Learningmode\n");

	//  // Log header for Device Status
	// LOG_HEADER (StatusLog, %s, "%% 0:StatusLog.log\n");
	// LOG_HEADER (StatusLog, %s, "%% 0:Iterationcnt 1:name 2:status\n");

	LOG_HEADER (steadyState, "%s\n", "%% 0:steadyState.log\n");
	LOG_HEADER (steadyState, "%s\n", "%% 0:Iterationcnt 1:Dv_status\n");

	LOG_HEADER (EnvelopState, "%s\n", "%% 0:EnvelopState.log\n");
	LOG_HEADER (EnvelopState, "%s\n", "%% 0:Iterationcnt 1:Eventnumber 2:Orderofstate 3:Dv_status\n");

	LOG_HEADER (finalState, "%s\n", "%% 0:finalState.log\n");
	LOG_HEADER (finalState, "%s\n", "%% 0:Iterationcnt 1:Dv_status\n");

	LOG_HEADER (TrainLog_P, "%s\n", "%% 0:TrainLog_P.log\n");
	LOG_HEADER (TrainLog_P, "%s\n", "%% 0:Iteationcnt 1:Direction 2:Mean 3:Min 4:Max 5:Range 6:Median 7:Std_Dev 8:Variance 9:Skewness 10:Kurtosis\n");

	LOG_HEADER (TrainLog_Q, "%s\n", "%% 0:TrainLog_Q.log\n");
	LOG_HEADER (TrainLog_Q, "%s\n", "%% 0:Iteationcnt 1:Direction 2:Mean 3:Min 4:Max 5:Range 6:Median 7:Std_Dev 8:Variance 9:Skewness 10:Kurtosis\n");

	LOG_HEADER (EventLog_P, "%s\n", "%% 0:EventLog_P.log\n");
	LOG_HEADER (EventLog_P, "%s\n", "%% 0:Iteationcnt 1:Direction 2:Mean 3:Min 4:Max 5:Range 6:Median 7:Std_Dev 8:Variance 9:Skewness 10:Kurtosis\n");

	LOG_HEADER (EventLog_Q, "%s\n", "%% 0:EventLog_Q.log\n");
	LOG_HEADER (EventLog_Q, "%s\n", "%% 0:Iteationcnt 1:Direction 2:Mean 3:Min 4:Max 5:Range 6:Median 7:Std_Dev 8:Variance 9:Skewness 10:Kurtosis\n");

    LOG_HEADER (Train_Buff_P_Log, "%s\n", "%% 0:Train_Buff_P_Log.log\n");
	LOG_HEADER (Train_Buff_P_Log, "%s\n", "%% 0:Iteationcnt 1:Buff_Value 2:Trn_Ring_Buff_Min_P\n");

	LOG_HEADER (Train_Buff_Q_Log, "%s\n", "%% 0:Train_Buff_Q_Log.log\n");
	LOG_HEADER (Train_Buff_Q_Log, "%s\n", "%% 0:Iteationcnt 1:Buff_Value 2:Trn_Ring_Buff_Min_Q\n");

	LOG_HEADER (Run_Buff_P_Log, "%s\n", "%% 0:Run_Buff_P_Log.log\n");
	LOG_HEADER (Run_Buff_P_Log, "%s\n", "%% 0:Iteationcnt 1:Buff_Value 2:Ring_Buff_Min_P 3:\n");

	LOG_HEADER (Run_Buff_Q_Log, "%s\n", "%% 0:Run_Buff_Q_Log.log\n");
	LOG_HEADER (Run_Buff_Q_Log, "%s\n", "%% 0:Iteationcnt 1:Buff_Value 2:Ring_Buff_Min_Q 3:\n");

	LOG_HEADER (EventScore_P_Log, "%s\n", "%% 0:EventScore_P_Log.log\n");
	LOG_HEADER (EventScore_P_Log, "%s\n", "%% 0:Iteationcnt 1:No._Of_Para 2:Mean 3:Min 4:Max 5:Range 6:Median 7:Std_Dev 8:Variance 9:Skewness 10:Kurtosis 11:NetScore \n");

	LOG_HEADER (EventScore_Q_Log, "%s\n", "%% 0:EventScore_Q_Log.log\n");
	LOG_HEADER (EventScore_Q_Log, "%s\n", "%% 0:Iteationcnt 1:No._Of_Para 2:Mean 3:Min 4:Max 5:Range 6:Median 7:Std_Dev 8:Variance 9:Skewness 10:Kurtosis 11:NetScore \n");

}


/*****************************************************************************************************************/

#if (LOG_FILE_REMOVER  == ENABLED)
void Log_File_Remover(void)
{

	remove("powerlog.log") ;
	remove("steadyState.log") ;
	remove("EnvelopState.log") ;
	remove("finalState.log") ;
	remove("TrainLog_P.log") ;
	remove("TrainLog_Q.log") ;
	remove("EventLog_P.log") ;
	remove("EventLog_Q.log") ;
	remove("Train_Buff_P_Log.log") ;
	remove("Train_Buff_Q_Log.log") ;
	remove("Run_Buff_P_Log.log") ;
	remove("Run_Buff_Q_Log.log") ;
	remove("EventScore_P_Log.log") ;
	remove("EventScore_Q_Log.log") ;

}
#endif


/*****************************************************************************************************************/


void  Log_Data (LogId_t log_id, char *format, ...)
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

  if( st_LogTable[log_id].logEnable )
  {

	strcpy(log_file_name, st_LogTable[log_id].logName) ;
 	strcat(log_file_name, file_ext) ;
 	//printf("\nlog_file_name = %s\t", log_file_name)  ;

 	ptr_log_file = fopen(log_file_name, "a");

 	if (!ptr_log_file)
 	{
 		printf("\nLog File not opened\n");
 		exit (0) ;
 	}
 	else
 	{
 		//printf("\nLog File opened\n");
 	}

 	if(ini_proc_st != PROC_INIT)
 		fprintf(ptr_log_file,"%d ", Global_itr_cnt) ;

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

	      ini_proc_st = Get_Ini_Proc_Sched_st() ;

#if(LOG_FORMAT == MACHINE_LEARN_LOG)
	      if(ini_proc_st == PROC_INIT)
	      {
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
	      else
	      {
		      switch( format_arr[idx] )
		      {
		         case 'd':
		             Printable.i = va_arg( vArgs, int );
		             fprintf(ptr_log_file,"%d:%d ", idx+1, Printable.i)	;
		          break;

		         case 'f':
		             Printable.f = va_arg( vArgs, double );
		             fprintf(ptr_log_file,"%d:%f ", idx+1, Printable.f)	;
		         break;

		          case 'c':
		              Printable.c = va_arg( vArgs, char );
					  fprintf(ptr_log_file,"%d:%c ", idx+1, Printable.c)	;
		          break;

		          case 's':
		              Printable.s = va_arg( vArgs, char * );
					  fprintf(ptr_log_file,"%d:%s ", idx+1, Printable.s)	;
		          break;

		          default:
		          break;
		      }
	      }
#elif(LOG_FORMAT == MATLAB_LOG)
	      if(ini_proc_st == PROC_INIT)
	      {
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
	      else
	      {
		      switch( format_arr[idx] )
		      {
		         case 'd':
		             Printable.i = va_arg( vArgs, int );
		             fprintf(ptr_log_file,"%d ", Printable.i)	;
		          break;

		         case 'f':
		             Printable.f = va_arg( vArgs, double );
		             fprintf(ptr_log_file,"%f ", Printable.f)	;
		         break;

		          case 'c':
		              Printable.c = va_arg( vArgs, char );
					  fprintf(ptr_log_file,"%c ", Printable.c)	;
		          break;

		          case 's':
		              Printable.s = va_arg( vArgs, char * );
					  fprintf(ptr_log_file,"%s ", Printable.s)	;
		          break;

		          default:
		          break;
		      }
	      }
#endif

	   }

	   if(ini_proc_st != PROC_INIT)
		   fprintf(ptr_log_file,"\n")	;

	  fclose(ptr_log_file) ;

	  va_end(vArgs);
  }

}


/*****************************************************************************************************************/
