
#include "main_inf.h"
//#include "logging.h"
/*
============================================================================
Name        : WT_Train_feature.c
Author      : skn2kor
Version     :
Copyright   :
Description :
============================================================================
*/



/***************************************************************************************************/
/***************************************************************************************************/
#if (   (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)  ||   (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)    )

void WT_Train_Feature_P_Proc(void)
{
uint32        Trn_Ring_Buff_Min_P=0x7FFFFFFF ;  
  
  uint8 Trn_WT_P_st           ;
  uint8 feature_extract_ret_p ;
  uint8 session_p             ;
  uint16 i;

    #if (WAVELET == ENABLED)
      #if (TRSFRM_TYP == HAAR)
        uint32  Haar_Trn_output_buff_P[COEFF_ARR_SIZE]    ;       //  float
      #endif
    #endif

  session_p     = Get_StM_Sess()  ;
  Trn_WT_P_st   = Get_DvTrn_WT()  ;
  
  if ( (session_p == TRAIN_SESSION) && (Trn_WT_P_st == DV_TRN_WT_START) )
  {
    // Re align
    //    PopFromRing_P(device_list[Dev_Cnt_Idx].device_wt_buff.P_Coeff, Dev_Trn_wt_P, BYTES_TO_READ, u16_StartDataPtr);		// to align P&Q from arr[0] for wt calculation
    PopFromRing_P(device_wt_buff[Dev_Cnt_Idx].P_Coeff, Dev_Trn_wt_P, BYTES_TO_READ, u16_StartDataPtr_P);		// to align P&Q from arr[0] for wt calculation    

    Trn_Ring_Buff_Min_P=0x7FFFFFFF ;      
    
    for (i=0; i< RINGBUFF_SIZE; i++)
      Trn_Ring_Buff_Min_P = Srvc_Min_U32(Dev_Trn_wt_P[i], Trn_Ring_Buff_Min_P) ;
                                   
    for (i=0; i< RINGBUFF_SIZE; i++)
      Dev_Trn_wt_P[i]-=Trn_Ring_Buff_Min_P ;

#if (LOGGING==ENABLED)
        for(i =0; i<RINGBUFF_SIZE;i++)
        {      
//          LOG(Train_Buff_P_Log,"%d %d %d\n",u32iterationCnt,Dev_Trn_wt_P[i], Trn_Ring_Buff_Min_P);
            LOG(Train_Buff_P_Log,"%d %d\n",Dev_Trn_wt_P[i], Trn_Ring_Buff_Min_P);
        }    
#endif

#if (WAVELET == ENABLED)
#if (TRSFRM_TYP == HAAR)
    haar(Dev_Trn_wt_P, Haar_Trn_output_buff_P)  ;
        Mem_Copy(Dev_Trn_wt_P, Haar_Trn_output_buff_P, (COEFF_ARR_SIZE*sizeof(uint32)));
#elif (TRSFRM_TYP == DB)
    wt( Dev_Trn_wt_P );
#endif
#endif
    
    feature_extract_ret_p = feature_extract(Dev_Trn_wt_P, TRAIN_SESSION, Dev_Cnt_Idx, DvTrn_trans_dir, PWR ) ;
    
    //    device_list[Dev_Cnt_Idx].pn = ( device_list[Dev_Cnt_Idx].pn/2 + P_Ins_Trn/2 )	  ;    
    //    device_list[Dev_Cnt_Idx].pn = P_Ins_Trn + DvTrn_P_PosOffset_C - DvTrn_P_NegOffset_C ;
    
    if ( DvTrn_trans_dir == OFF_TO_ON )
    {
#if( (SIMULATOR == ENABLED) && (SIMU_READ_PATH == FILE_READ) )
      device_list[Dev_Cnt_Idx].pn = P_Ins_Trn + device_list[Dev_Cnt_Idx].pn_PosOffset_C - device_list[Dev_Cnt_Idx].pn_NegOffset_C + 3 ;
      device_list[Dev_Cnt_Idx].qn = Q_Ins_Trn + device_list[Dev_Cnt_Idx].qn_PosOffset_C - device_list[Dev_Cnt_Idx].qn_NegOffset_C ;
#else
      device_list[Dev_Cnt_Idx].pn = Steady_P + device_list[Dev_Cnt_Idx].pn_PosOffset_C - device_list[Dev_Cnt_Idx].pn_NegOffset_C + 3 ;
      device_list[Dev_Cnt_Idx].qn = Steady_Q + device_list[Dev_Cnt_Idx].qn_PosOffset_C - device_list[Dev_Cnt_Idx].qn_NegOffset_C ;
#endif

    }  
    //  Volt_Trn = (Volt_Trn/2 + Volt_Ins/2)	;				// Training voltage during device training	at the end of WT same as P and Q
    
    Set_DvTrn_WT(DV_TRN_WT_P_DONE)  ;    
  }
  
}

#endif      //  #if (   (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)  ||   (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)    )

/***************************************************************************************************/
/***************************************************************************************************/

#if (   (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION)  ||   (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)    )

void WT_Train_Feature_Q_Proc(void)
{
uint32        Trn_Ring_Buff_Min_Q=0x7FFFFFFF ;  
  uint16 i;
  uint8 Trn_WT_Q_st           ;
  uint8 feature_extract_ret_q ;
  uint8 session_q             ;
#if (WAVELET == ENABLED)
#if (TRSFRM_TYP == HAAR)
        uint32  Haar_Trn_output_buff_Q[COEFF_ARR_SIZE]    ;       //  float        
#endif
#endif
  
  session_q   = Get_StM_Sess()  ;
  Trn_WT_Q_st = Get_DvTrn_WT()  ;
  
#if (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)
  if ( (session_q == TRAIN_SESSION) && (Trn_WT_Q_st == DV_TRN_WT_P_DONE) )
#elif ( (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION) )
    if ( (session_q == TRAIN_SESSION) && (Trn_WT_Q_st == DV_TRN_WT_START) )
#endif          
    {      
      // Re align
      //    PopFromRing_Q(device_list[Dev_Cnt_Idx].device_wt_buff.Q_Coeff, Dev_Trn_wt_Q, BYTES_TO_READ, u16_StartDataPtr);		// to align P&Q from arr[0] for wt calculation
      PopFromRing_Q(device_wt_buff[Dev_Cnt_Idx].Q_Coeff, Dev_Trn_wt_Q, BYTES_TO_READ, u16_StartDataPtr_Q);		// to align P&Q from arr[0] for wt calculation    

      Trn_Ring_Buff_Min_Q=0x7FFFFFFF ;  
      for (i=0; i< RINGBUFF_SIZE; i++)
        Trn_Ring_Buff_Min_Q = Srvc_Min_U32(Dev_Trn_wt_Q[i], Trn_Ring_Buff_Min_Q) ;
      
      for (i=0; i< RINGBUFF_SIZE; i++)
        Dev_Trn_wt_Q[i]-=Trn_Ring_Buff_Min_Q ;

#if (LOGGING==ENABLED)
      for(i =0; i<RINGBUFF_SIZE;i++)
      {      
        LOG(Train_Buff_Q_Log,"%d %d %d\n",u32iterationCnt,Dev_Trn_wt_Q[i], Trn_Ring_Buff_Min_Q);
      }
#endif

#if (WAVELET == ENABLED)
#if (TRSFRM_TYP == HAAR)
      haar(Dev_Trn_wt_Q, Haar_Trn_output_buff_Q)  ;
        Mem_Copy(Dev_Trn_wt_Q, Haar_Trn_output_buff_Q, (COEFF_ARR_SIZE*sizeof(uint32)) );
#elif (TRSFRM_TYP == DB)
      wt( Dev_Trn_wt_Q );
#endif
#endif
      
      
      feature_extract_ret_q = feature_extract(Dev_Trn_wt_Q, TRAIN_SESSION, Dev_Cnt_Idx, DvTrn_trans_dir, VAR ) ;

      
      //    device_list[Dev_Cnt_Idx].qn = ( device_list[Dev_Cnt_Idx].qn/2 + Q_Ins_Trn/2 ) 	  ;    
      //    device_list[Dev_Cnt_Idx].qn = Q_Ins_Trn + DvTrn_Q_PosOffset_C - DvTrn_Q_NegOffset_C ;
      
      //    if ( DvTrn_trans_dir == OFF_TO_ON )
      //    {
      //      device_list[Dev_Cnt_Idx].pn = P_Ins_Trn + device_list[Dev_Cnt_Idx].pn_PosOffset_C - device_list[Dev_Cnt_Idx].pn_NegOffset_C + 3;
      //      device_list[Dev_Cnt_Idx].qn = Q_Ins_Trn + device_list[Dev_Cnt_Idx].qn_PosOffset_C - device_list[Dev_Cnt_Idx].qn_NegOffset_C ;    
      //    }   
      //  Volt_Trn = (Volt_Trn/2 + Volt_Ins/2)	;				// Training voltage during device training	at the end of WT same as P and Q
      
      Set_DvTrn_WT(DV_TRN_WT_Q_DONE)  ;     
    }
  
}
#endif      //  #if (   (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION)  ||   (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)    )
