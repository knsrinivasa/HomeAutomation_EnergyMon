
/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
*/


#include "main_inf.h"
#include "global_variables.h"
//#include <shell.h>	       // include shell

/***************************************************************************************************
 * Prototypes
 ***************************************************************************************************
*/


/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
*/

#define   EVENT_CLEAR_DEB   20
/*
 ***************************************************************************************************
 * Variables
 ***************************************************************************************************
*/


/*
 ============================================================================
 Name        : Trn_Key_Sns.c
 Author      : skn2kor
 Version     :
 Copyright   :
 Description :
 ============================================================================
 */

static uint32	P_Trn_Norm	;
static uint32	Q_Trn_Norm	;        



void Train_Device_Proc(void)
{
//        static uint8    Trn_PQ_Old_update_delay = TRN_PQ_OLD_UPDATE_DELAY ;
  
	static uint32	P_Trn_Ref			;
	static uint32	Q_Trn_Ref			;
	static uint32	P_Trn_Ins_Old			;
	static uint32	Q_Trn_Ins_Old			;
	static uint8	Trn_Event_Occur = 0		;
        
	uint32		Volt_Ins			;
        
	uint8		i				;
	uint8           Dv_2Sig_ct                      ;
	uint8           session                         ;
	uint8           trn_st                          ;
	uint8           dv_trn_state                    ;
        
/*
	uint8 Dev_Name_list[20][DV_NAME_LEN]={
                                              {'F','R','I','D','G','E',' ',' ',' ','\0'},
                                              {'M','I','X','E','R',' ',' ',' ',' ','\0'},
                                              {'C','O','O','L','E','R',' ',' ',' ','\0'},
                                              {'M','I','C','R','O',' ',' ',' ',' ','\0'},
                                              {'D','E','V','I','C','E',' ','#','5','\0'},
                                              {'D','E','V','I','C','E',' ','#','6','\0'},
                                              {'D','E','V','I','C','E',' ','#','7','\0'},
                                              {'D','E','V','I','C','E',' ','#','8','\0'},
                                              {'D','E','V','I','C','E',' ','#','9','\0'},
                                              {'D','E','V','I','C','E',' ','#','1','0','\0'}
                                            };
*/
  
	uint8 Dev_Name_list[20][DV_NAME_LEN]={
                                          "Device 1",
                                          "Device 2",
                                          "Device 3",
                                          "Device 4",
                                          "Device 5",
                                          "Device 6",
                                          "Device 7",
                                          "Device 8",
                                          "Device 9",
                                          "Device 10"
  } ;

    static uint8  Delay_Clear_Flag;
    static uint8  ring_buff_start = 0 ;
    static uint8  Trn_EventDetect_Delay = TRN_EVENTDETECT_DELAY_RAS_CNT ;

    static uint32  P_Trn_Ref_Old  ;
    static uint32  Q_Trn_Ref_Old  ;
    
    uint32 abs_p_trn_diff = 0 ;
    uint32 abs_q_trn_diff = 0 ;
//    static uint16 Event_Clear_Deb_Tmr = 0 ;
    uint8 DvTrn_WT_st=0 ;

#if( DV_DETECT_ALGO == DV_DETECT_COMBINED)
        
    session       = Get_StM_Sess()  ;
    trn_st        = Get_StM_DvTrn() ;
    dv_trn_state  = Get_DvTrn_st()  ;
    
  if( (session == TRAIN_SESSION) && (trn_st == DV_TRN_START) && (dv_trn_state == 0) )
  {          
//	#if( (SIMULATOR == ENABLED) && (SIMU_READ_PATH == FILE_READ) )
//      Set_Simu_Train_PQ_Read_St (START_READ_TRAIN_PQ) ;
//	#endif

        Volt_Ins  =  Gui_Voltage                 ;        // Instant voltage		//train_device.c        

	#if (PQ_ZERO_CAL == ENABLED)
          P_Ins_Trn = Srv_PosDiff32( WattValue, P_Calib) ;
          Q_Ins_Trn = Srv_PosDiff32( VarValue , Q_Calib) ;
	#else
          P_Ins_Trn = (WattValue)  ;
          Q_Ins_Trn = (VarValue)   ;
	#endif
                    
//********************************************

	if(trn_pq_ini_flag == 0)
	{
          trn_pq_ini_flag	=	1	    ;
          ring_buff_ret_p       =       0           ;
          ring_buff_ret_q       =       0           ;          
          ring_buff_start       =       0           ;
          
          Trn_Event_Occur = EVENT_CLEARED ;
          DvTrn_trans_dir = 0   ;
          Delay_Clear_Flag = 0  ;

          P_Trn_Ref		= 0;  //	P_Ins_Trn   ;     //  SKN TEST
          Q_Trn_Ref		= 0;  //	Q_Ins_Trn   ;     //  SKN TEST
          Volt_Trn		=	Volt_Ins    ;
          P_Trn_Ins_Old         =       P_Ins_Trn   ;
          Q_Trn_Ins_Old         =       Q_Ins_Trn   ;
                    
	}

/*-------- device init -------*/
        Dv_2Sig_ct = Get_StM_DvTrn_2Sig();                   // Get the status of Device ON/OFF signature training
   
	if( (dv_init ==  0) && (Dv_2Sig_ct == 0) )
	{
          dv_init = 1 ;
          
          device_list[Dev_Cnt_Idx].idx	        = Dev_Cnt_Idx	;
          device_list[Dev_Cnt_Idx].ld_group	= 1		;
          device_list[Dev_Cnt_Idx].ld_type	= 1		;


          for(i=0;i<DV_NAME_LEN ;i++ )
            device_list[Dev_Cnt_Idx].name[i]	= Dev_Name_list[Dev_Cnt_Idx][i]	;
          
          
          device_st[Dev_Cnt_Idx].device_idx	= Dev_Cnt_Idx	;
          device_st[Dev_Cnt_Idx].status     	= UNKNOWN_ST	;
          device_st[Dev_Cnt_Idx].trg        	= UNTRIGGERED	;
          device_st[Dev_Cnt_Idx].wt_status	= UNKNOWN_ST	;
          device_st[Dev_Cnt_Idx].final_status   = UNKNOWN_ST	;
        }
/*-------- device init -------*/

      if( !(Trn_EventDetect_Delay--) )
      {
        Trn_EventDetect_Delay = TRN_EVENTDETECT_DELAY_RAS_CNT ;
        
        abs_p_trn_diff = Srv_AbsDiff32( P_Ins_Trn, P_Trn_Ins_Old ) ;
        abs_q_trn_diff = Srv_AbsDiff32( Q_Ins_Trn, Q_Trn_Ins_Old ) ;
      }


//	if ( (Trn_Event_Occur == 0) && ( abs_p_trn_diff >= P_DIFF_RES )  &&  ( abs_q_trn_diff >= Q_DIFF_RES ) && (!Delay_Clear_Flag)  )                         //  SKN TEST
	if ( (Trn_Event_Occur == 0) && ( abs_p_trn_diff >= P_DIFF_RES ) && (!Delay_Clear_Flag)  )                         //  SKN TEST          
	{
          Trn_Event_Occur = EVENT_OCCURED ;
          printf("\n==================================================================== Trn_Event_Occur = EVENT_OCCURED\n") ;
//          getch() ;
          if( ( P_Trn_Ins_Old < P_Ins_Trn ) && (DvTrn_trans_dir_OFF_TO_ON_flag==0) )			// Rising Edge
          {
            DvTrn_trans_dir_OFF_TO_ON_flag = 1 ;
            DvTrn_trans_dir = OFF_TO_ON	;
          }
          else if( ( P_Trn_Ins_Old > P_Ins_Trn ) && (DvTrn_trans_dir_ON_TO_OFF_flag==0) )		// Falling Edge
          {
            DvTrn_trans_dir_ON_TO_OFF_flag = 1 ;
            DvTrn_trans_dir = ON_TO_OFF	;
          }
          else
            Trn_Event_Occur = EVENT_CLEARED ;
	}

          if ( Trn_Event_Occur == EVENT_CLEARED)
          {
            P_Trn_Ref = Srvc_Min_U32(P_Trn_Ref, P_Ins_Trn)  ;
            Q_Trn_Ref = Srvc_Min_U32(Q_Trn_Ref, Q_Ins_Trn)  ;

            P_Trn_Ref_Min = P_Trn_Ref ;
            Q_Trn_Ref_Min = Q_Trn_Ref ;
          }
        
//          Envelop_create(P_Trn_Ref, &P_Trn_Ref, &P_Trn_Ref_Old, NORM_PWR_VAR_ENVELOP );
//          Envelop_create(Q_Trn_Ref, &P_Trn_Ref, &P_Trn_Ref_Old, NORM_PWR_VAR_ENVELOP );

//	P_Trn_Norm = ( (P_Ins_Trn >= P_Trn_Ref) )? (P_Ins_Trn - P_Trn_Ref) : ( (P_Ins_Trn>5)? P_Ins_Trn: 0 );                      // check: with variations, it can go negative ???
//	Q_Trn_Norm = ( (Q_Ins_Trn >= Q_Trn_Ref) )? (Q_Ins_Trn - Q_Trn_Ref) : ( (Q_Ins_Trn>1)? Q_Ins_Trn: 0 ); 

        P_Trn_Norm = Srv_AbsDiff32(P_Ins_Trn , P_Trn_Ref) ;
        Q_Trn_Norm = Srv_AbsDiff32(Q_Ins_Trn , Q_Trn_Ref) ;
        
        if (ring_buff_start == 0)
        {
//          ring_buff_ret_p = PushToRing_P(device_list[Dev_Cnt_Idx].device_wt_buff.P_Coeff, &P_Trn_Norm, BYTES_TO_STORE, Trn_Event_Occur, TRAIN_SESSION, &u16_StartDataPtr);
//          ring_buff_ret_q = PushToRing_Q(device_list[Dev_Cnt_Idx].device_wt_buff.Q_Coeff, &Q_Trn_Norm, BYTES_TO_STORE, Trn_Event_Occur, TRAIN_SESSION, &u16_StartDataPtr);
          ring_buff_ret_p = PushToRing_P(device_wt_buff[Dev_Cnt_Idx].P_Coeff, &P_Ins_Trn, BYTES_TO_STORE, Trn_Event_Occur, TRAIN_SESSION, &u16_StartDataPtr_P, DvTrn_trans_dir);
          ring_buff_ret_q = PushToRing_Q(device_wt_buff[Dev_Cnt_Idx].Q_Coeff, &Q_Ins_Trn, BYTES_TO_STORE, Trn_Event_Occur, TRAIN_SESSION, &u16_StartDataPtr_Q, DvTrn_trans_dir);          //  SKN Test
//          printf("\nRingbuffer filling\t P = %d\tQ = %d\n", P_Ins_Trn, Q_Ins_Trn) ;
        }

        trn_st = Get_StM_DvTrn() ;
        if ( ((ring_buff_ret_p == 1)||(ring_buff_ret_q == 1)) && (ring_buff_start == 0) && (trn_st!=DV_TRN_END) )
	{
          ring_buff_start = 1 ;
          Delay_Clear_Flag = 1 ;

          #if( (SIMULATOR == ENABLED) && (SIMU_READ_PATH == FILE_READ) )
		  Set_Simu_Train_PQ_Read_St (STOP_READ_TRAIN_PQ) ;
		  printf("\n==================================================================== Train File read Stopped\n") ;
		#endif

          Set_DvTrn_WT(DV_TRN_WT_START);
          
          Train_Event_Pn = P_Trn_Norm ;
          Train_Event_Qn = Q_Trn_Norm ;
          
          Train_Event_dPn = Srv_AbsDiff32 ( P_Ins_Trn , P_Trn_Ins_Old )   ;
          Train_Event_dQn = Srv_AbsDiff32 ( Q_Ins_Trn , Q_Trn_Ins_Old )   ;

          P_Trn_Ins_Old   = P_Ins_Trn ;
          Q_Trn_Ins_Old   = Q_Ins_Trn ;

          P_Trn_Ref	  =	P_Ins_Trn   ;     //  SKN TEST
          Q_Trn_Ref	  =	Q_Ins_Trn   ;     //  SKN TEST
          
	}

//        if(Delay_Clear_Flag)
//        {
//          Event_Clear_Deb_Tmr++ ;
//          
//          if (Event_Clear_Deb_Tmr > EVENT_CLEAR_DEB)
//          {
//            Event_Clear_Deb_Tmr = 0;
//
////            if ( (Trn_Event_Occur == 1) && ( abs_p_trn_diff < P_DIFF_RES )  &&  ( abs_q_trn_diff < Q_DIFF_RES ) )                      //  SKN TEST                       
//            if ( (Trn_Event_Occur == EVENT_OCCURED) && ( abs_p_trn_diff < P_DIFF_RES )  )                      //  SKN TEST                                     
//            {
//              Trn_Event_Occur = EVENT_CLEARED;
//              Delay_Clear_Flag = 0 ;          
//            }
//            
//          }
//        }
        
        if(Delay_Clear_Flag == 1)
        {
          //  if ( (Trn_Event_Occur == 1) && ( abs_p_trn_diff < P_DIFF_RES )  &&  ( abs_q_trn_diff < Q_DIFF_RES ) )                      //  SKN TEST                       
          if ( (Trn_Event_Occur == EVENT_OCCURED) && ( abs_p_trn_diff < P_DIFF_RES )  )                      //  SKN TEST                                     
          {
            Trn_Event_Occur = EVENT_CLEARED;
            Delay_Clear_Flag = 0 ;          
          }
        }
        
	Volt_Trn = ( Volt_Trn/2 + Volt_Ins/2 ) + VolTrn_PosOffset_C - VolTrn_NegOffset_C;				// Training voltage during device training	at the end of WT same as P and Q


//          if( (Trn_Event_Occur == 0) && ( !(Trn_PQ_Old_update_delay--) ) )                  // To avoid second event detection before first is processed
//          {
//            P_Trn_Ins_Old = P_Ins_Trn ;
//            Q_Trn_Ins_Old = Q_Ins_Trn ;
//            
////            P_Trn_Ref = ( P_Ins_Trn/2 + P_Trn_Ins_Old/2 ) ;
////            Q_Trn_Ref = ( Q_Ins_Trn/2 + Q_Trn_Ins_Old/2 ) ;
//            
//            //  Volt_Trn = ( Volt_Trn/2 + Volt_Ins/2 )	;				// Training voltage during device training	at the end of WT same as P and Q
//            Trn_PQ_Old_update_delay = TRN_PQ_OLD_UPDATE_DELAY ;
//          }
        
        
        DvTrn_WT_st   = Get_DvTrn_WT()  ;  
#if (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)
        if (DvTrn_WT_st == DV_TRN_WT_P_DONE )
#elif ( (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION) || (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION) )
        if (DvTrn_WT_st == DV_TRN_WT_Q_DONE )          
#endif          
        {
          Trn_Event_Occur = EVENT_CLEARED ;
//          DvTrn_trans_dir = 0 ;
          ring_buff_start = 0 ;
          ring_buff_ret_p = 0 ;
          ring_buff_ret_q = 0 ;          
          Delay_Clear_Flag = 0 ;
          
          if ( ( (DvTrn_trans_dir_OFF_TO_ON_flag + DvTrn_trans_dir_ON_TO_OFF_flag ) % 2 ) == 0 )
          {
            DvTrn_trans_dir_OFF_TO_ON_flag = 0  ;
            DvTrn_trans_dir_ON_TO_OFF_flag = 0  ;
          }            
          
          Set_DvTrn_WT(DV_TRN_WT_END)   ;
          Set_StM_DvTrn(DV_TRN_END, 0)  ;                            // Set the status of Device training to => START & Cond - NA

		#if( (SIMULATOR == ENABLED) && (SIMU_READ_PATH == FILE_READ) )
          Set_New_DvTrn_Start(DEV_TRAIN_END) ;
          Set_GUI_IP_Req_St (GUI_IP_REQUIRED) ;
		#endif
        }

    }


#endif      //  if( DV_DETECT_ALGO == DV_DETECT_COMBINED)
}     //  Train_Device_Proc



/*--------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/

uint8 PQ_Zero_Calib(uint32 Watt, uint32 Var, uint32 *P, uint32 *Q )
{

	#if (PQ_ZERO_CAL == ENABLED)
		*P = Watt;
		*Q = Var;
	#else
		*P = 0;
		*Q = 0;
	#endif

return 1;
}

/*--------------------------------------------------------------------------------------------------------
                                RATIO-METRIC VOLTAGE CORRECTION
- select == 1 for correcting power & reactive power 
  m = trained voltage/present Voltage;

- select == 2 for correcting Zero calibrated P_calib & Q_calib
  m = present Voltage/trained Voltage;
--------------------------------------------------------------------------------------------------------*/

void Ratio_Vol_Corr(uint32 tV, uint32 pV, uint32 present_P, uint32 present_Q, uint32 *P_new, uint32 *Q_new, uint8 select)
{  
// y = mx+c
float m; 
uint32 c = 0;
if(select == 1)
{
  if(pV != 0)
    m = (float) tV/pV;
  else m = 1;
}
else
{
  if(tV != 0)
    m = (float) pV/tV;
  else m = 1;
}
  *P_new = ((uint32) (m * present_P)) + c;
  *Q_new = ((uint32) (m * present_Q)) + c;

}
 /*--------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/
