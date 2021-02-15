

/*
 ***************************************************************************************************
 * Includes
 ***************************************************************************************************
*/

#if ( TRN_INTRFC == KEY_SENSE)

#include <includes.h>
#include <port.h>
#include <dio.h>
#include <datcol.h>
#include <dsm.h>
#if (SHELL_KEYPAD_EN  > 0)
  #include <shell.h>
#else
  #define Shell_Printf(...)
#endif
#include "keypad.h"


#include "main_inf.h"
#include "global_variables.h"

//static uint32	WattValue	;
//static uint32	VarValue	;
static uint32	P		;
static uint32	Q		;
//static uint32	P_Calib		;
//static uint32	Q_Calib		;

static uint32	P_Trn_Norm	;
static uint32	Q_Trn_Norm	;        

/***************************************************************************************************
 * Prototypes
 ***************************************************************************************************
*/

void trn_key_sens_proc(void);
uint8 train_device(uint16 dv_ct, device_struct *device_list, device_status *device_st, uint32 P_Ins, uint32 Q_Ins) ;


/*
 ***************************************************************************************************
 * Defines
 ***************************************************************************************************
*/

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

//static Srvc_DebounceState_t		st_KeyPad_stateDebBtn1				;      ///< State variable for Button 1 debouncing		//	Key for Zero Calib
//static Srvc_DebounceState_t		st_KeyPad_stateDebBtn2				;      ///< State variable for Button 2 debouncing		//	Key for Dev training
static Srvc_DebounceParam_t             st_KeyPad_tmDebBtn1_ms; ///< Timing parameter for Button 1 debouncing
static Srvc_DebounceParam_t             st_KeyPad_tmDebBtn2_ms; ///< Timing parameter for Button 2 debouncing

//static Srvc_SWTmrU32_t			TrnSess_OnTmr_ms				;
static Srvc_SWTmrU32_t			st_KeyPad_tiSinceBtnPrsd			;  ///< Timer to track time since last button pressed
static Srvc_SWTmrU32_t			st_KeyPad_TmSinceBtnRlsd			;  ///< Timer to track time since last button pressed
static Srvc_SWTmrU32_t                  st_KeyPad_TmSinceBtnPrsd;  ///< Timer to track time since last button pressed
static Srvc_SWTmrU32_t                  st_KeyPad_TmSinceBtnRlsd;  ///< Timer to track time since last button released
static bool                             u8_KeyPad_Acknowledge;     ///< Status set by HMI when key press is acknowledged

static uint8					Dev_trn_Discard		= FALSE			;
static uint8					Trn_Next_Device		= FALSE			;
static uint8					Trn_Sess_Key_Press	= DEPRESSED		;
static uint8					Trn_Dev_Key_Press	= DEPRESSED		;
static uint8					Dv_trn_st		= INIT		        ;
static uint8					ring_buff_ret		= INIT			;

static uint8 					PQ_zero_calib_flag				;
static uint8					trn_pq_ini_flag 				;
static uint8					dv_init						;


/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/


void  trn_key_sens_ProcIni (void)
{
  // Code_TAG: SRC_KEYPAD_001: Update the button press and release debouncing time
  st_KeyPad_tmDebBtn1_ms.TimeLowHigh =  KEYPAD_DEBTIME_BTNPRESS;
  st_KeyPad_tmDebBtn1_ms.TimeHighLow =  KEYPAD_DEBTIME_BTNRLS;

  st_KeyPad_tmDebBtn2_ms.TimeLowHigh =  KEYPAD_DEBTIME_BTNPRESS;
  st_KeyPad_tmDebBtn2_ms.TimeHighLow =  KEYPAD_DEBTIME_BTNRLS;

  DIO_Set (PORT_A, LED1, LED_OFF_ST); // on
  DIO_Set (PORT_A, LED2, LED_OFF_ST); // on
}

void trn_key_sens_proc(void)		// 1ms Proc
{
	static bool tog_led_flag ;
	static uint8 u8_BtnPresd;       // Remember which button was pressed
	static Srvc_DebounceState_t      st_KeyPad_stateDebBtn1;      ///< State variable for Button 1 debouncing
	static Srvc_DebounceState_t      st_KeyPad_stateDebBtn2;      ///< State variable for Button 2 debouncing
	uint8 u8caliReturn;
	uint8 trn_dv_ret;
        
        DC_KeyPad_Intfce_Struct  st_KeyPad_rawVal;           // Local structure to store the button status (raw value)
        DC_KeyPad_Intfce_Struct  st_KeyPad_debVal;           // Local structure to store the button status (debounced)

  DC_KeyPad_Intfce_Struct  st_KeyPad_finalVal = {FALSE, FALSE, // Buttons
                                                 FALSE,  // Multiple key-press
                                                 0};     // Timer

	st_KeyPad_rawVal.u8_statBtn1 = (uint8)((DIO_Get (KEYPAD1_I_D) == 0)? TRUE : FALSE);

	// Debouncing for Button 1

	st_KeyPad_debVal.u8_statBtn1 = Srvc_Debounce (st_KeyPad_rawVal.u8_statBtn1,
						      &st_KeyPad_stateDebBtn1,
						      &st_KeyPad_tmDebBtn1_ms,
						      DT);

	//if ( ( (st_KeyPad_debVal.u8_statBtn1) != FALSE ) && (Srvc_TestSWTmrU32 (&st_KeyPad1_tiSinceBtnPrsd) == TRUE) )
//	if( ((st_KeyPad_debVal.u8_statBtn1) != FALSE) && (Trn_Sess_Key_Press == DEPRESSED) )
//	if( ((st_KeyPad_rawVal.u8_statBtn1) != FALSE) && (Trn_Sess_Key_Press == DEPRESSED) )
	if (
			(Srvc_TestSWTmrU32(&st_KeyPad_TmSinceBtnRlsd) != FALSE) &&
			(st_KeyPad_debVal.u8_statBtn1 != FALSE) && (Trn_Sess_Key_Press == DEPRESSED)
		)
	{
	  Srvc_StartSWTmrU32(&st_KeyPad_TmSinceBtnPrsd); // Start the timer
	  Trn_Sess_Key_Press = PRESSED ;
	    if(st_KeyPad_debVal.u8_statBtn1 != FALSE)
	    {
	      u8_BtnPresd = 1;
	    }

	}
	  else if(
			  (u8_BtnPresd != 0) && // If button pressed
	          (Srvc_TestSWTmrU32(&st_KeyPad_TmSinceBtnRlsd) != FALSE) && // release timer is not started yet
	          (st_KeyPad_debVal.u8_statBtn1 == FALSE)
	         )
	  {
	    Srvc_StartSWTmrU32(&st_KeyPad_TmSinceBtnRlsd); // Start the release timer
	  }
	  else if ((Srvc_TestSWTmrU32(&st_KeyPad_TmSinceBtnRlsd) == FALSE) && // If button released timer is running and
	           ((u8_KeyPad_Acknowledge != FALSE) ||  // Acknowledged from HMI or Max time is elapsed
	            (Srvc_DiffSWTmrU32(&st_KeyPad_TmSinceBtnRlsd) >  KEYPAD_MAX_SETTIME) )
	           )    // timer is elapsed
	  {
	    Srvc_StopSWTmrU32(&st_KeyPad_TmSinceBtnRlsd);   // Stop the release timer
	    u8_KeyPad_Acknowledge = FALSE;                   // Reset the ack flag
	    u8_BtnPresd = 0; // Reset button pressed status
	  }
	  else
	  {
	    // Do nothing
	  }

	  // Release timer is running --> Update the final button status
	  if((Srvc_TestSWTmrU32(&st_KeyPad_TmSinceBtnRlsd) == FALSE))
	  {
	    // Activate which button is pressed
	    switch(u8_BtnPresd)
	    {
	      case 1:
	      {
	        st_KeyPad_finalVal.u8_statBtn1 = TRUE;
	      }
	      break;

	      default:
	      break;
	    }

	  }


	  // Update the time
	  st_KeyPad_finalVal.u32_tmSinceBtnPrsd_ms = Srvc_DiffSWTmrU32(&st_KeyPad_TmSinceBtnPrsd);

/*---------------------------------------------------------------------------------*/

// Time to enter or exit Training Session
	if ( ((st_KeyPad_debVal.u8_statBtn1) == FALSE) && (Trn_Sess_Key_Press==PRESSED) && ( (st_KeyPad_debVal.u32_tmSinceBtnPrsd_ms >= TrnSess_tiThres_C) && (st_KeyPad_debVal.u32_tmSinceBtnPrsd_ms < KeyPress_tiError_C) ))
	{
		Trn_Sess_Key_Press = DEPRESSED ;

		if( Sys_Mode != TRAIN_SESSION )
			Sys_Mode = TRAIN_SESSION  ;

		else if ( Sys_Mode == TRAIN_SESSION )
			Sys_Mode = RUN_SESSION  ;
	}


	if ( Sys_Mode == TRAIN_SESSION )
		Set_LED(LED1)	;
	else if ( Sys_Mode == RUN_SESSION )
	{
		Clear_LED(LED1)	;
		Blink_LED(LED2) ;
	}

/*---------------------------------------------------------------------------------*/

	if ( Sys_Mode == TRAIN_SESSION )
	{
	  st_KeyPad_rawVal.u8_statBtn2 = (uint8)((DIO_Get (KEYPAD2_I_D) == 0)? TRUE : FALSE);


	  // Debouncing for Button 2
	  st_KeyPad_debVal.u8_statBtn2 = Srvc_Debounce (st_KeyPad_rawVal.u8_statBtn2,
	                                                &st_KeyPad_stateDebBtn2,
	                                                &st_KeyPad_tmDebBtn2_ms,
	                                                DT);


	  if ( (Srvc_TestSWTmrU32(&st_KeyPad_TmSinceBtnRlsd) != FALSE) && ((st_KeyPad_debVal.u8_statBtn2) != FALSE) && (Trn_Dev_Key_Press==DEPRESSED) )
	  {
		Srvc_StartSWTmrU32(&st_KeyPad_tiSinceBtnPrsd); // Restart the timer
	    Trn_Dev_Key_Press = PRESSED ;

		  if (st_KeyPad_debVal.u8_statBtn2 != FALSE)
	      {
	        u8_BtnPresd = 2;
	      }

	  }

	  else if(
			  (u8_BtnPresd != 0) && // If button pressed
	          (Srvc_TestSWTmrU32(&st_KeyPad_TmSinceBtnRlsd) != FALSE) && // release timer is not started yet
	          (st_KeyPad_debVal.u8_statBtn2 == FALSE)
	         )
	  {
	    Srvc_StartSWTmrU32(&st_KeyPad_TmSinceBtnRlsd); // Start the release timer
	  }
	  else if ((Srvc_TestSWTmrU32(&st_KeyPad_TmSinceBtnRlsd) == FALSE) && // If button released timer is running and
	           ((u8_KeyPad_Acknowledge != FALSE) ||  // Acknowledged from HMI or Max time is elapsed
	            (Srvc_DiffSWTmrU32(&st_KeyPad_TmSinceBtnRlsd) >  KEYPAD_MAX_SETTIME) )
	           )    // timer is elapsed
	  {
	    Srvc_StopSWTmrU32(&st_KeyPad_TmSinceBtnRlsd);   // Stop the release timer
	    u8_KeyPad_Acknowledge = FALSE;                   // Reset the ack flag
	    u8_BtnPresd = 0; // Reset button pressed status
	  }
	  else
	  {
	    // Do nothing
	  }

	  // Release timer is running --> Update the final button status
	  if((Srvc_TestSWTmrU32(&st_KeyPad_TmSinceBtnRlsd) == FALSE))
	  {
	    // Activate which button is pressed
	    switch(u8_BtnPresd)
	    {
	      case 2:
	      {
	        st_KeyPad_finalVal.u8_statBtn2 = TRUE;
	      }
	      break;

	      default:
	      break;
	    }
	  }

/*---------------------------------------------------------------------------------*/
	  // Check for simultaneous key-press of specific keys (button-1 to button-4)
	  // Used for special cases (e.g. reset of PIN)
	  if  (
			  ( Sys_Mode == TRAIN_SESSION ) && (Dv_trn_st == DV_TRN_START) &&
			  (st_KeyPad_debVal.u8_statBtn1 != FALSE) && (st_KeyPad_debVal.u8_statBtn2!= FALSE)
	  	  )
	  {
	    st_KeyPad_finalVal.u8_multipleKeyPrsd	= TRUE	;
	    Dev_trn_Discard				= TRUE	;
	    Trn_Next_Device				= FALSE	;
	  }

	  // Update the time
	  st_KeyPad_finalVal.u32_tmSinceBtnPrsd_ms = Srvc_DiffSWTmrU32(&st_KeyPad_TmSinceBtnPrsd);

	  if ( ((st_KeyPad_debVal.u8_statBtn2) == FALSE) && (Trn_Dev_Key_Press==PRESSED) && ( st_KeyPad_finalVal.u32_tmSinceBtnPrsd_ms > TrnDev_tiThres_C ))
	  {
		Trn_Dev_Key_Press = DEPRESSED ;

		if(Dv_trn_st != DV_TRN_START)
		{
			Dv_trn_st = DV_TRN_START ;
			Set_LED(LED2)	;
		}
		else if ( (Dv_trn_st == DV_TRN_START) && (Dev_trn_Discard == FALSE) )
		{
			Dv_trn_st = DV_TRN_END ;
			Trn_Next_Device = TRUE ;
			Clear_LED(LED2)	;
		}

	  }
		if(PQ_zero_calib_flag == 0)
		{
			PQ_zero_calib_flag = 1;

			#if (PQ_ZERO_CAL == ENABLED)
				Blink_LED(LED2) ;
				u8caliReturn = PQ_Zero_Calib( WattValue, VarValue, &P_Calib, &Q_Calib ) ;
				Clear_LED(LED2)	;
			#endif
		}

		if ( Dv_trn_st == DV_TRN_START )
			Set_LED(LED2)	;

		else if ( Dv_trn_st == DV_TRN_END )
			Clear_LED(LED2)	;

		else if ( (Dev_trn_Discard == TRUE) )
			Clear_LED(LED2)	;

		P = (WattValue-P_Calib)/10		;
		Q = (VarValue-Q_Calib)/10		;

		if (Dv_trn_st == DV_TRN_START)
			trn_dv_ret = train_device(Dev_Cnt_Idx, device_list, device_st, P, Q);				        // Train of the devices

		if ( (Dv_trn_st == DV_TRN_END) && (Dev_trn_Discard == FALSE) && (Trn_Next_Device == TRUE) )
		{
			Trn_Next_Device = FALSE ;
			ring_buff_ret = INIT	;
			if( Train_ON_OFF_Flag == 2)
			{
				Train_ON_OFF_Flag = 0	;
				Dev_Cnt_Idx++			;
			}
			trn_pq_ini_flag	=	0		;
			dv_init = 0				;
			//	page_write(pop_string)	;
		}

		else if (Dev_trn_Discard == TRUE)
		{
			Dev_trn_Discard	= FALSE ;
			Dv_trn_st = DV_TRN_END	;

			ring_buff_ret = INIT	;
	//		Dev_Cnt_Idx++			;
			trn_pq_ini_flag	=	0		;
			dv_init = 0				;
		}

	}		//if ( Sys_Mode == TRAIN_SESSION )

/*---------------------------------------------------------------------------------*/

//	 // Detect if any button is stuck for long time and update the error vector if so
//	if( (Srvc_DiffSWTmrU32(&st_KeyPad1_tiSinceBtnPrsd) >= KeyPress_tiError_C) || (Srvc_DiffSWTmrU32(&st_KeyPad2_tiSinceBtnPrsd) >= KeyPress_tiError_C) )	// Key Jammed
//	{
//		Trn_Sess_Key_Press	= KEY_JAM	;
//		Trn_Dev_Key_Press	= KEY_JAM	;
////		printf("\nKeypad Error: Key Jammed\n") ;			// SET a LED or alarm
//	}


}


/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/


/* On DIO interrupt, start training session	*/
uint8 train_device(uint16 dv_ct_i, device_struct *device_list, device_status *device_st, uint32 P_Ins, uint32 Q_Ins)
{

	static uint32	P_Trn_Ref			;
	static uint32	Q_Trn_Ref			;
	static uint32	P_Ins_Old			;
	static uint32	Q_Ins_Old			;
	static uint8	Off_On_trans_dir_Flag		;
	static uint8	Trn_Event_Occur = 0		;
	static sint8	Deb_ras_cnt = DEB_RAS_CNT	;
	static uint8	trans_dir                       ;

	uint32		*Dest_Dev_Buff_ptr		;
	uint32		Volt_Ins			;
	uint8		j				;
	uint8		Delay_Flag			;
	uint8           feature_extact_ret              ;
        
	uint8 Dev_Name_list[20][DV_NAME_LEN]={
									{'D','E','V','I','C','E',' ','#','1','\0'},
									{'D','E','V','I','C','E',' ','#','2','\0'},
									{'D','E','V','I','C','E',' ','#','3','\0'},
									{'D','E','V','I','C','E',' ','#','4','\0'},
									{'D','E','V','I','C','E',' ','#','5','\0'},
									{'D','E','V','I','C','E',' ','#','6','\0'},
									{'D','E','V','I','C','E',' ','#','7','\0'},
									{'D','E','V','I','C','E',' ','#','8','\0'},
									{'D','E','V','I','C','E',' ','#','9','\0'},
									{'D','E','V','I','C','E',' ','#','1','0','\0'}
								};

    #if (WAVELET == ENABLED)
      #if (TRSFRM_TYP == HAAR)
        
            #if (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)
              uint32  Haar_Trn_output_buff_P[COEFF_ARR_SIZE]    ;       //  float
            #elif (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION)
              uint32  Haar_Trn_output_buff_Q[COEFF_ARR_SIZE]    ;       //  float        
            #elif (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)
              uint32  Haar_Trn_output_buff_P[COEFF_ARR_SIZE]    ;       //  float
              uint32  Haar_Trn_output_buff_Q[COEFF_ARR_SIZE]    ;       //  float        
            #endif

      #endif
    #endif
  
        
        
// create device structure: grazr alloc()

//-----------------	Dv_trn_st == DV_TRN_START &&  trn_dv_finish == 0 ; ;
	P_Ins = (WattValue-P_Calib)/10	;
	Q_Ins = (VarValue-Q_Calib)/10	;
//********************************************

	if(trn_pq_ini_flag == 0)
	{
		trn_pq_ini_flag		=	1		;
		P_Trn_Ref		=	P_Ins		;
		Q_Trn_Ref		=	Q_Ins		;
		Volt_Trn		=	Volt_Ins	;
	}


/*-------- device init -------*/
	if (dv_init ==  0)
	{
		dv_init = 1 ;

		device_list[dv_ct_i].idx	= dv_ct_i	;
		device_list[dv_ct_i].ld_group	= 1		;
		device_list[dv_ct_i].ld_type	= 1		;
	}
//	device_list[dv_ct_i].name		= dv_ct_i	;
	for(j=0;j<DV_NAME_LEN ;j++ )
		device_list[dv_ct_i].name[j]	= Dev_Name_list[dv_ct_i][j]	;

/*
	if( (Off_On_trans_dir_Flag == 0) || (Off_On_trans_dir_Flag == ON_TO_OFF) )
		device_list[dv_ct_i].trans_dir = OFF_TO_ON	;
	else if(Off_On_trans_dir_Flag == OFF_TO_ON)
		device_list[dv_ct_i].trans_dir = ON_TO_OFF	;
*/


    device_st[dv_ct_i].device_idx	= dv_ct_i	;
    device_st[dv_ct_i].status     	= UNKNOWN_ST	;
    device_st[dv_ct_i].trg        	= UNTRIGGERED	;
    device_st[dv_ct_i].wt_status	= UNKNOWN_ST	;

/*-------- device init -------*/


	Delay_Flag = (!(--Deb_ras_cnt)) ;
	if(Deb_ras_cnt == 0)
		Deb_ras_cnt = DEB_RAS_CNT;


	if ( (Trn_Event_Occur == 0) && ( abs(P_Ins_Old - P_Ins) >= P_DIFF_RES )  &&  ( abs(Q_Ins_Old - Q_Ins) >= Q_DIFF_RES ) && Delay_Flag )
	{
		Trn_Event_Occur = EVENT_OCCURED ;

		if( P_Ins_Old < P_Ins )										// Rising Edge
			device_list[dv_ct_i].trans_dir = OFF_TO_ON	;
		else if( P_Ins_Old > P_Ins )								// Falling Edge
			device_list[dv_ct_i].trans_dir = ON_TO_OFF	;
	}

	if ( (Trn_Event_Occur == 1) && ( abs(P_Ins_Old - P_Ins) < P_DIFF_RES )  &&  ( abs(Q_Ins_Old - Q_Ins) < Q_DIFF_RES )  && Delay_Flag )
		Trn_Event_Occur = EVENT_CLEARED;


	P_Trn_Norm = P_Ins - P_Trn_Ref	;
	Q_Trn_Norm = Q_Ins - Q_Trn_Ref	;


	switch(ring_buff_ret)
	{
		case 0:	//success

//				Dest_Dev_Buff_ptr = device_list[dv_ct_i].device_wt_buff.P_Coeff ;
//                                        uint8 PushToRing(uint32 * u8_RingBuff, uint32 * NewData, uint16 u16_Len, uint8 event_flag, uint8 session, uint16 *StartPtr_offset)                  
//				ring_buff_ret = PushToRing_P(device_list[dv_ct_i].device_wt_buff.P_Coeff, &P_Trn_Norm, BYTES_TO_STORE, Trn_Event_Occur, TRAIN_SESSION, &u16_StartDataPtr);
				ring_buff_ret = PushToRing_P(device_wt_buff[dv_ct_i].P_Coeff, &P_Trn_Norm, BYTES_TO_STORE, Trn_Event_Occur, TRAIN_SESSION, &u16_StartDataPtr);        
//				Dest_Dev_Buff_ptr = device_list[dv_ct_i].device_wt_buff.Q_Coeff ;
//				ring_buff_ret = PushToRing_Q(device_list[dv_ct_i].device_wt_buff.Q_Coeff, &Q_Trn_Norm, BYTES_TO_STORE, Trn_Event_Occur, TRAIN_SESSION, &u16_StartDataPtr);
				ring_buff_ret = PushToRing_Q(device_wt_buff[dv_ct_i].Q_Coeff, &Q_Trn_Norm, BYTES_TO_STORE, Trn_Event_Occur, TRAIN_SESSION, &u16_StartDataPtr);
				break;

		case 1: //end reached
				// Re align

                                  #if (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)
//                                                      PopFromRing_P(device_list[dv_ct_i].device_wt_buff.P_Coeff, Dev_Trn_wt_P, BYTES_TO_READ, u16_StartDataPtr);		// to align P&Q from arr[0] for wt calculation
                                                      PopFromRing_P(device_wt_buff[dv_ct_i].P_Coeff, Dev_Trn_wt_P, BYTES_TO_READ, u16_StartDataPtr);		// to align P&Q from arr[0] for wt calculation                                                      
                                  #elif (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION)
//                                                      PopFromRing_Q(device_list[dv_ct_i].device_wt_buff.Q_Coeff, Dev_Trn_wt_Q, BYTES_TO_READ, u16_StartDataPtr);		// to align P&Q from arr[0] for wt calculation
                                                      PopFromRing_Q(device_wt_buff[dv_ct_i].Q_Coeff, Dev_Trn_wt_Q, BYTES_TO_READ, u16_StartDataPtr);		// to align P&Q from arr[0] for wt calculation
                                  #elif (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)
//                                                      PopFromRing_P(device_list[dv_ct_i].device_wt_buff.P_Coeff, Dev_Trn_wt_P, BYTES_TO_READ, u16_StartDataPtr);		// to align P&Q from arr[0] for wt calculation
//                                                      PopFromRing_Q(device_list[dv_ct_i].device_wt_buff.Q_Coeff, Dev_Trn_wt_Q, BYTES_TO_READ, u16_StartDataPtr);		// to align P&Q from arr[0] for wt calculation
                                                      PopFromRing_P(device_wt_buff[dv_ct_i].P_Coeff, Dev_Trn_wt_P, BYTES_TO_READ, u16_StartDataPtr);		// to align P&Q from arr[0] for wt calculation
                                                      PopFromRing_Q(device_wt_buff[dv_ct_i].Q_Coeff, Dev_Trn_wt_Q, BYTES_TO_READ, u16_StartDataPtr);		// to align P&Q from arr[0] for wt calculation                                                      
                                  #endif

				#if (WAVELET == ENABLED)
					#if (TRSFRM_TYP == HAAR)
                                            #if (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)
                                                          haar(Dev_Trn_wt_P, Haar_Trn_output_buff_P)  ;
                                                          Mem_Copy(Dev_Trn_wt_P, Haar_Trn_output_buff_P, COEFF_ARR_SIZE);
          
                                            #elif (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION)
                                                          haar(Dev_Trn_wt_Q, Haar_Trn_output_buff_Q)  ;
                                                          Mem_Copy(Dev_Trn_wt_Q, Haar_Trn_output_buff_Q, COEFF_ARR_SIZE);
          
                                            #elif (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)
                                                          haar(Dev_Trn_wt_P, Haar_Trn_output_buff_P)  ;
                                                          Mem_Copy(Dev_Trn_wt_P, Haar_Trn_output_buff_P, COEFF_ARR_SIZE);
          
                                                          haar(Dev_Trn_wt_Q, Haar_Trn_output_buff_Q)  ;
                                                          Mem_Copy(Dev_Trn_wt_Q, Haar_Trn_output_buff_Q, COEFF_ARR_SIZE);
                                            #endif
					#elif (TRSFRM_TYP == DB)
	//					wt( TRAIN_SESSION,device_list, device_wt_buff, PRM_P );
	//					wt( TRAIN_SESSION,device_list, device_wt_buff, PRM_Q );
                                                
                                            #if (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)
						wt( Dev_Trn_wt_P );          
                                            #elif (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION)
						wt( Dev_Trn_wt_Q );          
                                            #elif (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)
						wt( Dev_Trn_wt_P );                                            
						wt( Dev_Trn_wt_Q );                                            
                                            #endif
					#endif
				#endif


                                            #if (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)
                      				feature_extact_ret = feature_extact(Dev_Trn_wt_P, TRAIN_SESSION, dv_ct_i, trans_dir ) ;
                                            #elif (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION)
			                  	feature_extact_ret = feature_extact(Dev_Trn_wt_Q, TRAIN_SESSION, dv_ct_i, trans_dir ) ;
                                            #elif (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)
                				feature_extact_ret = feature_extact(Dev_Trn_wt_P, TRAIN_SESSION, dv_ct_i, trans_dir ) ;
		                		feature_extact_ret = feature_extact(Dev_Trn_wt_Q, TRAIN_SESSION, dv_ct_i, trans_dir ) ;
                                            #endif
                                
				device_list[dv_ct_i].pn = P_Ins	+ DvTrn_P_PosOffset_C - DvTrn_P_NegOffset_C ;
				device_list[dv_ct_i].qn = Q_Ins	+ DvTrn_Q_PosOffset_C - DvTrn_Q_NegOffset_C ;
//				device_list[dv_ct_i].pn = ( device_list[dv_ct_i].pn/2 + P_Ins/2 )	;
//				device_list[dv_ct_i].qn = ( device_list[dv_ct_i].qn/2 + Q_Ins/2 ) 	;

				P_Trn_Ref = P_Ins	;
				Q_Trn_Ref = Q_Ins	;

//				Volt_Trn = (Volt_Trn/2 + Volt_Ins/2)	;				// Training voltage during device training	at the end of WT same as P and Q

				break;

		case 2: //error
				break;
	}

	Volt_Trn = ( Volt_Trn/2 + Volt_Ins/2 ) + VolTrn_PosOffset_C - VolTrn_NegOffset_C;				// Training voltage during device training	at the end of WT same as P and Q


	if ( (Trn_Event_Occur == 0) && Delay_Flag )
	{
		P_Ins_Old = ( P_Ins/2 + P_Ins_Old/2 ) 	;
		Q_Ins_Old = ( Q_Ins/2 + Q_Ins_Old/2 )	;

		P_Trn_Ref =  ( P_Ins/2 + P_Trn_Ref/2 ) ;
		Q_Trn_Ref =  ( Q_Ins/2 + Q_Trn_Ref/2 ) ;

//		Volt_Trn = ( Volt_Trn/2 + Volt_Ins/2 )	;				// Training voltage during device training	at the end of WT same as P and Q
	}

	trn_dv_finish = 1 ;

	return 1;

}

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/


#endif      //#if ( TRN_INTRFC == KEY_SENSE)

