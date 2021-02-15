
#include "main_inf.h"
#include "global_variables.h"
//#include "logging.h"
/*
 ============================================================================
 Name        : WT_Run_feature.c
 Author      : skn2kor
 Version     :
 Copyright   :
 Description :
 ============================================================================
*/
//char logBuffer[5000];
//
//char *Train_getLogBuffer()
//{
//	return logBuffer;
//}

  static uint8  Dev_Search_flag_P = 0 ;
  static uint8  Dev_Search_flag_Q = 0 ;

#if (   (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)  ||   (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)    )

void WT_Run_Feature_P_Proc(void)
{

  uint32        Ring_Buff_Min_P=0x7FFFFFFF ;
  
  uint8 wt_ret_p = 0              ;
  uint8 RunSess_WT_P_st           ;
  uint8 sys_session_P             ;
  uint8 feature_extact_retp = 0   ;
  uint8 match_ret           = 0   ;
  uint8 Device_Detected		  ;
  uint8 pop_p_ret                 ;
  uint16 i;    
//   char *buf;
//       // Get the pointer to log buffer
//  buf = Train_getLogBuffer();  

    #if (WAVELET == ENABLED)
      #if (TRSFRM_TYP == HAAR)
        uint32  Haar_output_buff_P[COEFF_ARR_SIZE]    ;       //  float
      #endif
    #endif
  
  
  sys_session_P     = Get_StM_Sess()    ;  
  RunSess_WT_P_st   = Get_RunSess_WT()  ;


  if ( (sys_session_P == RUN_SESSION) && (RunSess_WT_P_st == RUNSESS_WT_START) )
  {
/*    
    if ( event_trans_dir == OFF_TO_ON )
      pre_smpl_cnt_P = Event_Pre_Samp_Cnt_OFF_TO_ON_C ;
    else if ( event_trans_dir == ON_TO_OFF )
      pre_smpl_cnt_P = Event_Pre_Samp_Cnt_ON_TO_OFF_C ;
*/    
                                   
    // Re align  
    pop_p_ret = PopFromRing_P(P_Ring_Buff[WT_search_buff], Run_time_wt_P, BYTES_TO_READ, u16_StartDataPtr_P)  ;	// to align P&Q from arr[0] for wt calculation

    Ring_Buff_Min_P=0x7FFFFFFF ;

    for (i=0; i< RINGBUFF_SIZE; i++)
      Ring_Buff_Min_P = Srvc_Min_U32(Run_time_wt_P[i], Ring_Buff_Min_P) ;
                                   
    for (i=0; i< RINGBUFF_SIZE; i++)
      Run_time_wt_P[i]-=Ring_Buff_Min_P ;

    if (pop_p_ret == 1)
    {
      Set_PopRing_WT (RING_P_REALIGN_DONE)  ;

#if (LOGGING==ENABLED)
      for(i =0; i<RINGBUFF_SIZE; i++)
      {
        LOG(Run_Buff_P_Log,"%d %d %d\n",u32iterationCnt,Run_time_wt_P[i], Ring_Buff_Min_P);
      }
#endif
    }
    
    #if (WAVELET == ENABLED)
      #if (TRSFRM_TYP == HAAR)
        haar(Run_time_wt_P, Haar_output_buff_P)  ;
        Mem_Copy(Run_time_wt_P, Haar_output_buff_P, (COEFF_ARR_SIZE*sizeof(uint32))  );
      #elif (TRSFRM_TYP == DB)
        wt_ret_p = wt( Run_time_wt_P );
        if (wt_ret_p == 1)
        {
          Dev_Search_flag_P  =  1   ;
          Set_RunSess_WT(RUNSESS_WT_P_END);
        }
      #endif
    #else
          Dev_Search_flag_P  =  1   ;
          Set_RunSess_WT(RUNSESS_WT_P_END);
    #endif
    
    
    P_Ref = P_Ins	;
    Q_Ref = Q_Ins	;
   
    
    //feature_extraction
    if ( Dev_Search_flag_P == 1 )
    {
      feature_extact_retp = feature_extract(Run_time_wt_P, RUN_SESSION, ZERO, trans_dir, PWR) ;
      
#if (FEATURE_EXTRACTION_FLD != PQ_FEATURE_EXTRACTION)      
      //search feature best match
      Device_Detected = 0 ;
      match_ret = feature_match(Dev_Cnt_Idx, trans_dir, &Device_Detected);
      
      if(match_ret == 1)
      {
        Dev_Search_flag_P = 0	;
      }
#endif      
        Set_WT_DevDet_P_St (WT_DEVDET_P_END);
      
    }   //  if ( Dev_Search_flag_P == 1 )
    
  }     //  if ( (sys_session_P == RUN_SESSION) && (RunSess_WT_P_st == RUNSESS_WT_START) )

}       // proc


#endif                //    #if (   (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)  ||   (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)    )

/***************************************************************************************************/
/***************************************************************************************************/

#if (   (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION)  ||   (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)    )

void WT_Run_Feature_Q_Proc(void)
{
uint32        Ring_Buff_Min_Q=0x7FFFFFFF ;

  uint8 wt_ret_q = 0    ;
  uint8 RunSess_WT_Q_st ;
  uint8 sys_session_Q   ;
  uint8 feature_extact_retq = 0   ;
  uint8 match_ret           = 0   ;
  uint8 Device_Detected		  ;
  uint8 pop_q_ret                 ;
//  uint8 pop_ring_st               ;
    uint16 i;

    #if (WAVELET == ENABLED)
      #if (TRSFRM_TYP == HAAR)
        uint32  Haar_output_buff_Q[COEFF_ARR_SIZE]    ;       //  float
      #endif
    #endif


  sys_session_Q   = Get_StM_Sess()    ;  
  RunSess_WT_Q_st = Get_RunSess_WT()  ;

#if (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)
  if ( (sys_session_Q == RUN_SESSION) && (RunSess_WT_Q_st == RUNSESS_WT_P_END) )
#elif ( (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION) )
  if ( (sys_session_Q == RUN_SESSION) && (RunSess_WT_Q_st == RUNSESS_WT_START) )
#endif          
  {
    // Re align  
    pop_q_ret = PopFromRing_Q(Q_Ring_Buff[WT_search_buff], Run_time_wt_Q, BYTES_TO_READ, u16_StartDataPtr_Q)  ;	// to align P&Q from arr[0] for wt calculation

    Ring_Buff_Min_Q=0x7FFFFFFF ;
    
    for (i=0; i< RINGBUFF_SIZE; i++)
      Ring_Buff_Min_Q = Srvc_Min_U32(Run_time_wt_Q[i], Ring_Buff_Min_Q) ;
                                   
    for (i=0; i< RINGBUFF_SIZE; i++)
      Run_time_wt_Q[i]-=Ring_Buff_Min_Q ;

    if (pop_q_ret==1)    
    {
      Set_PopRing_WT (RING_Q_REALIGN_DONE)  ;
      
#if (LOGGING==ENABLED)
      for(i =0; i<RINGBUFF_SIZE;i++)
      {
    	  LOG(Run_Buff_Q_Log,"%d %d %d\n",u32iterationCnt,Run_time_wt_Q[i], Ring_Buff_Min_Q);
      }
#endif

	  }    
    #if (WAVELET == ENABLED)
      #if (TRSFRM_TYP == HAAR)
        haar(Run_time_wt_Q, Haar_output_buff_Q)  ;
        Mem_Copy(Run_time_wt_Q, Haar_output_buff_Q, (COEFF_ARR_SIZE*sizeof(uint32)));        
      #elif (TRSFRM_TYP == DB)
        wt_ret_q = wt( Run_time_wt_Q );
        if ( wt_ret_q == 1 )
        {
          Dev_Search_flag_Q  =  1   ;
          Set_RunSess_WT(RUNSESS_WT_Q_END);
        }        
      #endif
    #else
          Dev_Search_flag_Q  =  1   ;
          Set_RunSess_WT(RUNSESS_WT_Q_END);
    #endif
    
    
    P_Ref = P_Ins	;
    Q_Ref = Q_Ins	;

    
    //feature_extraction
    if ( Dev_Search_flag_Q == 1 )
    {
      feature_extact_retq = feature_extract(Run_time_wt_Q, RUN_SESSION, ZERO, trans_dir, VAR) ;      //  SKN - CHECK:  create struct element for Q

      //search feature best match
      Device_Detected = 0 ;
      match_ret = feature_match(Dev_Cnt_Idx, trans_dir, &Device_Detected);                            //  SKN - CHECK:  create struct element for Q
      
//      if(match_ret == 1)
//      {
        Dev_Search_flag_P = 0   ;
        Dev_Search_flag_Q = 0	;
//      }

      Set_WT_DevDet_Q_St (WT_DEVDET_Q_END);
      
#if( (SIMULATOR == ENABLED) && (SIMU_READ_PATH == FILE_READ) )
  Set_New_DvTrn_Start(DEV_RUN_END) ;
  Set_GUI_IP_Req_St (GUI_IP_REQUIRED) ;
#endif


    }   //  if ( Dev_Search_flag_P == 1 )
    
  }     //  if ( (sys_session_Q == RUN_SESSION) && (RunSess_WT_Q_st == RUNSESS_WT_P_END) )

}       // proc

#endif                  // #if (   (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION)  ||   (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)    )

/***************************************************************************************************/
/***************************************************************************************************/


