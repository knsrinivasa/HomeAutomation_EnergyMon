
/*
 ============================================================================
 Name        : os_sched.h
 Author      : skn2kor
 Created on	 : Oct 31, 2013
 Version     :
 Copyright   :
 Description :
 ============================================================================
 */

#ifndef OS_SCHED_H_
#define OS_SCHED_H_
#include "main_inf.h"
#include "NILM_prot.h"

/*
extern void NILM_ProcIni();

extern void GUI_Proc(void);         // NILM GUI

extern void GUI_ProcIni (void);
//extern void ASD_Proc(void);
extern void Check_Samples_Proc();        //by praveen
extern void Dio_Set_PA20();
extern void Dio_Reset_PA20();
extern void Meas_calcLowFreq_Power();
*/

extern void Train_Device_Proc(void)         ;
extern void WT_Train_Feature_P_Proc(void)   ;
extern void WT_Train_Feature_Q_Proc(void)   ;
extern void DevTrn_ReIni_Proc(void)         ;
extern void WT_Run_Proc(void)               ;
extern void WT_Run_Feature_P_Proc(void)     ;
extern void WT_Run_Feature_Q_Proc(void)     ;
extern void Steady_Detect_Proc(void)        ;
extern void DevDet_merge_Proc(void)         ;
extern void Nilm_DataPumper_Proc(void)      ;

#if (LOG_FILE_REMOVER  == ENABLED)
	extern void Log_File_Remover(void)	;
#endif

extern void Log_head_writer(void)	;
extern void print_data(void)		;
extern void feed_PQ_data(void)		;
extern void Get_PQ_File(void)		;
extern void simu_session_hdlr(void) ;

/**
*********************************************************************************************************
*
*                               Task container for other Process initialization
*
*********************************************************************************************************
*/
void(* const OS_TASKSri_PROCINI[] )(void) =         //  Proc ini for new developments
{
		Get_PQ_File,

#if (LOGGING == ENABLED)
	#if (LOG_FILE_REMOVER  == ENABLED)
		Log_File_Remover,
	#endif
//		Log_head_writer,
#endif

};

/**
*********************************************************************************************************
*
*                               Task container for 2 MS task
*
*********************************************************************************************************
*/
void (* const OS_TASKSri2MS[])(void) =
{                                               // 2ms  task table

};

/**
*********************************************************************************************************
*
*                               Task container for 10MS task
*
*********************************************************************************************************
*/
void (* const OS_TASKSri10MS[])(void) =
{                                               // 10ms  task table

		feed_PQ_data,

#if( DV_DETECT_ALGO == DV_DETECT_COMBINED)
                        Train_Device_Proc,          //  NILM
                        WT_Run_Proc,                //  NILM
#endif

};

/**
*********************************************************************************************************
*
*                               Task container for 20MS task
*
*********************************************************************************************************
*/
void (* const OS_TASKSri20MS[])(void) =
{                                               // 20ms  task table

};

/**
*********************************************************************************************************
*
*                               Task container for 100MS task
*
*********************************************************************************************************
*/
void (* const OS_TASKSri100MS[])(void) =
{                                               // 100ms  task table


};


/**
*********************************************************************************************************
*
*                               Task container for 200MS task
*
*********************************************************************************************************
*/
void (* const OS_TASKSri200MS[])(void) =
{                                               // 200ms  task table
		simu_session_hdlr,
};



/**
*********************************************************************************************************
*
*                               Task container for 1000MS task
*
*********************************************************************************************************
*/
void (* const OS_TASKSri1000MS[])(void) =
{                                               // 1000ms  task table

//		GUI_Proc,
//		simu_session_hdlr,

#if( DV_DETECT_ALGO == DV_DETECT_COMBINED)

            #if (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)
                         WT_Train_Feature_P_Proc,                               //  NILM
            #elif (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION)
                         WT_Train_Feature_Q_Proc,                               //  NILM
            #elif (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)
                         WT_Train_Feature_P_Proc,                               //  NILM
                         WT_Train_Feature_Q_Proc,                               //  NILM
            #endif

                         DevTrn_ReIni_Proc,                                     //  NILM    // Required for both WT & Steady_St

            #if (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)
                         WT_Run_Feature_P_Proc,                                 //  NILM
            #elif (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION)
                         WT_Run_Feature_Q_Proc,                                 //  NILM
            #elif (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)
                         WT_Run_Feature_P_Proc,                                 //  NILM
                         WT_Run_Feature_Q_Proc,                                 //  NILM
            #endif

#endif
                         Steady_Detect_Proc,                                    //  NILM    // Required for Steady_St

#if( DV_DETECT_ALGO == DV_DETECT_COMBINED)
                         DevDet_merge_Proc,                                     //  NILM
#endif

//                        Nilm_DataPumper_Proc,                                   // For pumping Data on UART for Mobile phone app...
};

#endif /* OS_SCHED_H_ */
