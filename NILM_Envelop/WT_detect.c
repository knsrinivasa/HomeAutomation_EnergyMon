
/***************************************************************************************************
 * Includes
 ***************************************************************************************************
*/

#include "main_inf.h"
#include "global_variables.h"

/***************************************************************************************************
 * Prototypes
 ***************************************************************************************************
*/


/***************************************************************************************************
 * Defines
 ***************************************************************************************************
*/

#define   EVENT_DEB_TMR   200

/***************************************************************************************************
 * Variables
 ***************************************************************************************************
*/
/*
device_wt_struct	pp_co_eff_buff[PING_PONG]	;
device_struct		device_list[DV_MAX_CT]		;
device_status		device_st[DV_MAX_CT]		;
*/

/*
 ============================================================================
 Name        : WT_Std_st.c
 Author      : skn2kor
 Version     :
 Copyright   :
 Description :
 ============================================================================
 */

//int main(void)
void WT_Run_Proc(void)
{
  
  static uint32	P_Ins_Old	;
  static uint32	Q_Ins_Old	;
  static uint32	P_Norm	        ;
  static uint32	Q_Norm	        ;
  static uint8	pq_ini_flag     ;
  static uint8	Event_Occur= 0	;
  static uint8 	ring_ret_p      ;
  static uint8 	ring_ret_q      ;
//  static uint8 	Dev_Search_flag			;
//  static uint8    PQ_Old_update_delay = PQ_OLD_UPDATE_DELAY ;          
  static uint8  Delay_Flag;
//  static uint8  ring_start = 0  ;

//  static uint16 Event_Deb_Tmr = 0      ;
  static uint8  WT_EventDetect_Delay = WT_EVENTDETECT_DELAY_RAS_CNT ;
  
  static uint32  P_Ref_Old  ;
  static uint32  Q_Ref_Old  ;

  uint32        abs_p_diff = 0  ;
  uint32        abs_q_diff = 0  ;
  uint8 	sys_session     ;
  uint8         RingRead_st     ;
  uint16        i               ;
  uint8         idx             ;
  
  abs_p_diff = 0  ;
  abs_q_diff = 0  ;

        sys_session = Get_StM_Sess()  ;
        
	if (sys_session == RUN_SESSION )
	{
//		#if( (SIMULATOR == ENABLED) && (SIMU_READ_PATH == FILE_READ) )
//			Set_Simu_Run_PQ_Read_St (START_READ_RUN_PQ) ;
//		#endif

          #if (PQ_ZERO_CAL == ENABLED)
          P_Ins = Srv_PosDiff32( WattValue, P_Calib) ;
          Q_Ins = Srv_PosDiff32( VarValue , Q_Calib) ;
            
          #else
            P_Ins = WattValue	;
            Q_Ins = VarValue	;
          #endif

           
          if(pq_ini_flag == 0)
          {
            pq_ini_flag   =   1   ;
            ring_ret_p    =   0   ;
            ring_ret_q    =   0   ;            
            
//            P_Ref       =   (P_Ins>30)? (P_Ins-10) : P_Ins	;     //  SKN TEST
//            Q_Ref       =   (Q_Ins>10)? (Q_Ins-5) : Q_Ins       ;     //  SKN TEST
            
            P_Ref       =   P_Ins	;     //  SKN TEST
            Q_Ref       =   Q_Ins       ;     //  SKN TEST            
            P_Ins_Old   =   P_Ins ;
            Q_Ins_Old   =   Q_Ins ;
           
          }

      if( !(WT_EventDetect_Delay--) )
      {
        WT_EventDetect_Delay = WT_EVENTDETECT_DELAY_RAS_CNT ;
        
        abs_p_diff = Srv_AbsDiff32(P_Ins_Old , P_Ins) ;
        abs_q_diff = Srv_AbsDiff32(Q_Ins_Old , Q_Ins) ;
      }          
      
//          if ( (Event_Occur == EVENT_CLEARED) && ( abs_p_diff >= P_DIFF_RES )  &&  ( abs_q_diff >= Q_DIFF_RES )  && (!Delay_Flag) )   //  SKN TEST
          if ( (Event_Occur == EVENT_CLEARED) && ( abs_p_diff >= P_DIFF_RES )  )   //  SKN TEST            
          {
            Event_Occur = EVENT_OCCURED ;

            printf("\n==================================================================== Run_Event_Occur = EVENT_OCCURED\n") ;

            for(idx=0; idx<Dev_Cnt_Idx; idx++)
              device_st[idx].trg = UNTRIGGERED ;

            if( P_Ins_Old < P_Ins )									// Rising Edge
              trans_dir = OFF_TO_ON	;
            else if( P_Ins_Old > P_Ins )								// Falling Edge
              trans_dir = ON_TO_OFF	;
          }

          if ( Event_Occur == EVENT_CLEARED)
          {
            if( P_Ref >= P_Ins )
            {
              P_Ref = Srvc_Min_U32(P_Ref, P_Ins)  ;
              Q_Ref = Srvc_Min_U32(Q_Ref, Q_Ins)  ;
            }
//            else 
//            {
//              P_Ref = P_Ref +  (P_Ins-P_Ref)/  ;
//              Q_Ref = Q_Ref +  (Q_Ins-Q_Ref)/  ;
//              
//              Q_Ref = Srvc_Min_U32(Q_Ref, Q_Ins)  ;
//            }
            P_Ref_Min = P_Ref ;
            Q_Ref_Min = Q_Ref ;
          }

//          Envelop_create(P_Ref, &P_Ref, &P_Ref_Old, NORM_PWR_VAR_ENVELOP );
//          Envelop_create(Q_Ref, &Q_Ref, &Q_Ref_Old, NORM_PWR_VAR_ENVELOP );

//          P_Norm = ( (P_Ins >= P_Ref) )? (P_Ins - P_Ref) : ( (P_Ins>5)? P_Ins: 0 );                      // check: with variations, it can go negative ???
//          Q_Norm = ( (Q_Ins >= Q_Ref) )? (Q_Ins - Q_Ref) : ( (Q_Ins>1)? Q_Ins: 0 ); 

//          P_Norm = Srv_AbsDiff32(P_Ins , P_Ref) ;
//          Q_Norm = Srv_AbsDiff32(Q_Ins , Q_Ref) ;

//            if( P_Ref >= P_Ins )
//            {
//              P_Ref = Srvc_Min_U32(P_Norm, P_Ins)  ;
//              Q_Ref = Srvc_Min_U32(Q_Norm, Q_Ins)  ;
//            }
          
        if(pq_ini_flag == 1)
        {
          pq_ini_flag = 2 ;
            for (i=0; i<RINGBUFF_SIZE ; i++)              
            {
              P_Ring_Buff[buff_toggle_flag][i]  = P_Ins ;
              Q_Ring_Buff[buff_toggle_flag][i]  = Q_Ins ;            
            }

        }            
          if ( Delay_Flag == 0 )
          {
            ring_ret_p = PushToRing_P(P_Ring_Buff[buff_toggle_flag], &P_Ins, BYTES_TO_STORE, Event_Occur, RUN_SESSION, &u16_StartDataPtr_P, trans_dir );
            ring_ret_q = PushToRing_Q(Q_Ring_Buff[buff_toggle_flag], &Q_Ins, BYTES_TO_STORE, Event_Occur, RUN_SESSION, &u16_StartDataPtr_Q, trans_dir );
          }
          
          if ((ring_ret_p == 1) && (ring_ret_q == 1))
          { 
			#if( (SIMULATOR == ENABLED) && (SIMU_READ_PATH == FILE_READ) )
			  Set_Simu_Run_PQ_Read_St (STOP_READ_RUN_PQ) ;
			  printf("\n==================================================================== Run File read Stopped\n") ;
			#endif

            Delay_Flag = 1;
                           
            WT_search_buff   =  buff_toggle_flag	;
            buff_toggle_flag^=  1		        ;                  
            ring_ret_p       =  0	                ;
            ring_ret_q       =  0	                ;            
            pq_ini_flag      =  0	                ;

            Runtime_Event_Pn = P_Norm ;
            Runtime_Event_Qn = Q_Norm ;
            
            Runtime_Event_dPn = Srv_AbsDiff32 ( P_Ins , P_Ins_Old )   ;
            Runtime_Event_dQn = Srv_AbsDiff32 ( Q_Ins , Q_Ins_Old )   ;
            
            P_Ins_Old = P_Ins ;           //    SKN Test
            Q_Ins_Old = Q_Ins ;           //    SKN Test

            P_Ref       =   P_Ins   ;     //  SKN TEST
            Q_Ref       =   Q_Ins   ;     //  SKN TEST
            
            
            Set_RunSess_WT(RUNSESS_WT_START)      ;
            Set_PopRing_WT(RING_REALIGN_START)    ;

            #if (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)
              Set_WT_DevDet_P_St (WT_DEVDET_P_START);            
            #elif (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION)
              Set_WT_DevDet_Q_St (WT_DEVDET_Q_START);                    
            #elif (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)
              Set_WT_DevDet_P_St (WT_DEVDET_P_START);
              Set_WT_DevDet_Q_St (WT_DEVDET_Q_START);        
            #endif
            
          }


//        if( (Event_Occur == 0) && ( !(PQ_Old_update_delay--) ) )
//        {
//          P_Ins_Old = P_Ins ;           //    SKN Test
//          Q_Ins_Old = Q_Ins ;           //    SKN Test
//
//          P_Ref = P_Ins	;
//          Q_Ref = Q_Ins	;
//          
//          PQ_Old_update_delay = PQ_OLD_UPDATE_DELAY ;          
//        } 
        
          
          RingRead_st = Get_PopRing_WT()  ;

#if   (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)        
            if (RingRead_st == RING_P_REALIGN_DONE )
#elif ( (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION) || (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION) )
            if (RingRead_st==RING_Q_REALIGN_DONE )
#endif          
            {
              RingRead_st = RING_REALIGN_DONE     ;
              Set_PopRing_WT (RING_REALIGN_DONE)  ;
//              Event_Occur = EVENT_CLEARED;              
              
              if(Delay_Flag == 1)
              {
                // if ( (Event_Occur == EVENT_OCCURED) && ( abs_p_diff < P_DIFF_RES )  &&  ( abs_q_diff < Q_DIFF_RES )  )              //  SKN TEST
                if ( (Event_Occur == EVENT_OCCURED) && ( abs_p_diff < P_DIFF_RES )  )              //  SKN TEST
                {
                  Event_Occur = EVENT_CLEARED;
                  Delay_Flag = 0 ;          
                }
              }
              
            }
          
          RunSess_WT_st = Get_RunSess_WT() ;
          if (RunSess_WT_st == RUNSESS_WT_Q_END)
          {
            Set_RunSess_WT(RUNSESS_WT_END) ;
//			#if( (SIMULATOR == ENABLED) && (SIMU_READ_PATH == FILE_READ) )
//			  Set_New_DvTrn_Start(DEV_RUN_END) ;
//			  Set_GUI_IP_Req_St (GUI_IP_REQUIRED) ;
//			#endif
          }

	}   //  if (sys_session == RUN_SESSION )

} //  proc


