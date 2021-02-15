
/*
#define LED1        13UL
#define LED2        14UL
#define LED_OFF_ST  1UL
*/

/****************************************** Inclusions ****************************************/

#include "main_inf.h"

//#include <includes.h>
//#include <port.h>
//#include <dio.h>
//#include <datcol.h>
//#include <dsm.h>

//#if (SHELL_KEYPAD_EN  > 0)
//  #include <shell.h>
//#else
//  #define Shell_Printf(...)
//#endif
//#include "keypad.h"

/* [QAC-Supress] DSM Errhandler return type */
//PRQA S 3200 EOF
/****************************************** Static Variables ****************************************/

static Srvc_DebounceParam_t      st_KeyPad_tmDebBtn1_ms; ///< Timing parameter for Button 1 debouncing
static Srvc_DebounceParam_t      st_KeyPad_tmDebBtn2_ms; ///< Timing parameter for Button 2 debouncing
//static Srvc_DebounceParam_t      st_KeyPad_tmDebBtn3_ms; ///< Timing parameter for Button 3 debouncing
//static Srvc_DebounceParam_t      st_KeyPad_tmDebBtn4_ms; ///< Timing parameter for Button 4 debouncing
//static Srvc_DebounceParam_t      st_KeyPad_tmDebBtn5_ms; ///< Timing parameter for Button 5 debouncing
//static Srvc_DebounceParam_t      st_KeyPad_tmDebBtn6_ms; ///< Timing parameter for Button 6 debouncing

static Srvc_SWTmrU32_t           st_KeyPad_TmSinceBtnPrsd;  ///< Timer to track time since last button pressed
static Srvc_SWTmrU32_t           st_KeyPad_TmSinceBtnRlsd;  ///< Timer to track time since last button released

static bool                      u8_KeyPad_Acknowledge;     ///< Status set by HMI when key press is acknowledged


/**
*********************************************************************************************************
* Keypad Initialization process
*********************************************************************************************************
*/


/*----------------------------------------------------*/

void Blink_LED(uint8 LED_numa)
{
static bool Blnk_LED_st;  
static uint8 LED_Blnk_Del = LED_BLNK_RAS_CNT;

  if( !(LED_Blnk_Del--) )
    {
      Blnk_LED_st^= 1;

      DIO_Set (PORT_A, LED_numa, !Blnk_LED_st); // on
      LED_Blnk_Del = LED_BLNK_RAS_CNT ;
    } 
}

/*--------------------------*/

void Toggle_LED(uint8 LED_numb)
{
  static bool Tog_LED_st=0 ;
      Tog_LED_st^= 1;
      DIO_Set (PORT_A, LED_numb, !Tog_LED_st); // on
}

/*----------------------------------------------------*/

void Set_LED(uint8 LED_numc)
{
	DIO_Set (PORT_A, LED_numc, LED_ON_ST); // on
}

/*----------------------------------------------------*/

void Clear_LED(uint8 LED_numd)
{
	DIO_Set (PORT_A, LED_numd, LED_OFF_ST); // on
}

/*----------------------------------------------------*/


void  KeyPad_ProcIni (void)
{
  // Code_TAG: SRC_KEYPAD_001: Update the button press and release debouncing time
  st_KeyPad_tmDebBtn1_ms.TimeLowHigh =  KEYPAD_DEBTIME_BTNPRESS;
  st_KeyPad_tmDebBtn1_ms.TimeHighLow =  KEYPAD_DEBTIME_BTNRLS;
  
  st_KeyPad_tmDebBtn2_ms.TimeLowHigh =  KEYPAD_DEBTIME_BTNPRESS;
  st_KeyPad_tmDebBtn2_ms.TimeHighLow =  KEYPAD_DEBTIME_BTNRLS;
  
//  st_KeyPad_tmDebBtn3_ms.TimeLowHigh =  KEYPAD_DEBTIME_BTNPRESS;
//  st_KeyPad_tmDebBtn3_ms.TimeHighLow =  KEYPAD_DEBTIME_BTNRLS;
//
//  st_KeyPad_tmDebBtn4_ms.TimeLowHigh =  KEYPAD_DEBTIME_BTNPRESS;
//  st_KeyPad_tmDebBtn4_ms.TimeHighLow =  KEYPAD_DEBTIME_BTNRLS;
//
//  st_KeyPad_tmDebBtn5_ms.TimeLowHigh =  KEYPAD_DEBTIME_BTNPRESS;
//  st_KeyPad_tmDebBtn5_ms.TimeHighLow =  KEYPAD_DEBTIME_BTNRLS;
//
//  st_KeyPad_tmDebBtn6_ms.TimeLowHigh =  KEYPAD_DEBTIME_BTNPRESS;
//  st_KeyPad_tmDebBtn6_ms.TimeHighLow =  KEYPAD_DEBTIME_BTNRLS;

  DIO_Set (PORT_A, LED1, LED_OFF_ST); // on  
  DIO_Set (PORT_A, LED2, LED_OFF_ST); // on
}


/**
*********************************************************************************************************
* Keypad process to read the keypad button status.
*
* This process scans the keypad buttons and read the status. The digital input value is read and 
* debounced to filter out any noise if present. The read status (debounced) is updated into the 
* shared memory using data collector interface.
*********************************************************************************************************
*/
void  KeyPad_Proc (void)
{
  static uint8 u8_BtnPresd;       // Remember which button was pressed
  static Srvc_DebounceState_t      st_KeyPad_stateDebBtn1;      ///< State variable for Button 1 debouncing
  static Srvc_DebounceState_t      st_KeyPad_stateDebBtn2;      ///< State variable for Button 2 debouncing 
//  static Srvc_DebounceState_t      st_KeyPad_stateDebBtn3;      ///< State variable for Button 3 debouncing
//  static Srvc_DebounceState_t      st_KeyPad_stateDebBtn4;      ///< State variable for Button 4 debouncing
//  static Srvc_DebounceState_t      st_KeyPad_stateDebBtn5;      ///< State variable for Button 5 debouncing
//  static Srvc_DebounceState_t      st_KeyPad_stateDebBtn6;      ///< State variable for Button 6 debouncing
  
  static bool tog_led_flag ; 
  
  DC_KeyPad_Intfce_Struct  st_KeyPad_rawVal;           // Local structure to store the button status (raw value)
  DC_KeyPad_Intfce_Struct  st_KeyPad_debVal;           // Local structure to store the button status (debounced)

  // Local structure to store the final button status (after button release)
  DC_KeyPad_Intfce_Struct  st_KeyPad_finalVal = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, // Buttons
                                                 FALSE,  // Multiple key-press
                                                 0};     // Timer

  // Code_TAG: SRC_KEYPAD_002: Scan the keys to get the raw value (pressed key retuns 'LOW')
  st_KeyPad_rawVal.u8_statBtn1 = (uint8)((DIO_Get (KEYPAD1_I_D) == 0)? TRUE : FALSE);  
  st_KeyPad_rawVal.u8_statBtn2 = (uint8)((DIO_Get (KEYPAD2_I_D) == 0)? TRUE : FALSE);  
//  st_KeyPad_rawVal.u8_statBtn3 = (uint8)((DIO_Get (KEYPAD3_I_D) == 0)? TRUE : FALSE);
//  st_KeyPad_rawVal.u8_statBtn4 = (uint8)((DIO_Get (KEYPAD4_I_D) == 0)? TRUE : FALSE);
//  st_KeyPad_rawVal.u8_statBtn5 = (uint8)((DIO_Get (KEYPAD5_I_D) == 0)? TRUE : FALSE);
//  st_KeyPad_rawVal.u8_statBtn6 = (uint8)((DIO_Get (KEYPAD6_I_D) == 0)? TRUE : FALSE);

  // Code_TAG: SRC_KEYPAD_003: Debounce the raw values and update into the local memory
  // Debouncing for Button 1
  st_KeyPad_debVal.u8_statBtn1 = Srvc_Debounce (st_KeyPad_rawVal.u8_statBtn1,
                                                &st_KeyPad_stateDebBtn1,
                                                &st_KeyPad_tmDebBtn1_ms,
                                                DT);

  // Debouncing for Button 2
  st_KeyPad_debVal.u8_statBtn2 = Srvc_Debounce (st_KeyPad_rawVal.u8_statBtn2,
                                                &st_KeyPad_stateDebBtn2,
                                                &st_KeyPad_tmDebBtn2_ms,
                                                DT);
  
//  // Debouncing for Button 3
//  st_KeyPad_debVal.u8_statBtn3 = Srvc_Debounce (st_KeyPad_rawVal.u8_statBtn3,
//                                                &st_KeyPad_stateDebBtn3,
//                                                &st_KeyPad_tmDebBtn3_ms,
//                                                DT);
//
//  // Debouncing for Button 4
//  st_KeyPad_debVal.u8_statBtn4 = Srvc_Debounce (st_KeyPad_rawVal.u8_statBtn4,
//                                                &st_KeyPad_stateDebBtn4,
//                                                &st_KeyPad_tmDebBtn4_ms,
//                                                DT);
//
//  // Debouncing for Button 5
//  st_KeyPad_debVal.u8_statBtn5 = Srvc_Debounce (st_KeyPad_rawVal.u8_statBtn5,
//                                                &st_KeyPad_stateDebBtn5,
//                                                &st_KeyPad_tmDebBtn5_ms,
//                                                DT);
//
//  // Debouncing for Button 6
//  st_KeyPad_debVal.u8_statBtn6 = Srvc_Debounce (st_KeyPad_rawVal.u8_statBtn6,
//                                                &st_KeyPad_stateDebBtn6,
//                                                &st_KeyPad_tmDebBtn6_ms,
//                                                DT);
                                                
  // Check if any button is pressed and previous button is cleared (release time is stopped)
//  if ((Srvc_TestSWTmrU32(&st_KeyPad_TmSinceBtnRlsd) != FALSE) &&
//      ((st_KeyPad_debVal.u8_statBtn1 != FALSE) || (st_KeyPad_debVal.u8_statBtn2 != FALSE) || (st_KeyPad_debVal.u8_statBtn3 != FALSE) ||
//       (st_KeyPad_debVal.u8_statBtn4 != FALSE) || (st_KeyPad_debVal.u8_statBtn5 != FALSE) || (st_KeyPad_debVal.u8_statBtn6 != FALSE))
//     )
  if (
		  (Srvc_TestSWTmrU32(&st_KeyPad_TmSinceBtnRlsd) != FALSE) &&
		  ((st_KeyPad_debVal.u8_statBtn1 != FALSE) || (st_KeyPad_debVal.u8_statBtn2 != FALSE) )
     )
  {
    Srvc_StartSWTmrU32(&st_KeyPad_TmSinceBtnPrsd); // Restart the timer 

    // Remember which button is pressed
    if(st_KeyPad_debVal.u8_statBtn1 != FALSE)
    {
      u8_BtnPresd = 1;
    }
    else if (st_KeyPad_debVal.u8_statBtn2 != FALSE)
    {
      u8_BtnPresd = 2;
    }
//    else if (st_KeyPad_debVal.u8_statBtn3 != FALSE)
//    {
//      u8_BtnPresd = 3;
//    }
//    else if (st_KeyPad_debVal.u8_statBtn4 != FALSE)
//    {
//      u8_BtnPresd = 4;
//    }
//    else if (st_KeyPad_debVal.u8_statBtn5 != FALSE)
//    {
//      u8_BtnPresd = 5;
//    }
//    else if (st_KeyPad_debVal.u8_statBtn6 != FALSE)
//    {
//      u8_BtnPresd = 6;
//    }
  }
//  else if((u8_BtnPresd != 0) && // If button pressed
//          (Srvc_TestSWTmrU32(&st_KeyPad_TmSinceBtnRlsd) != FALSE) && // release timer is not started yet
//          (st_KeyPad_debVal.u8_statBtn1 == FALSE) && (st_KeyPad_debVal.u8_statBtn2 == FALSE) && (st_KeyPad_debVal.u8_statBtn3 == FALSE) &&
//          (st_KeyPad_debVal.u8_statBtn4 == FALSE) && (st_KeyPad_debVal.u8_statBtn5 == FALSE) && (st_KeyPad_debVal.u8_statBtn6 == FALSE)
//         )
  else if(
		  (u8_BtnPresd != 0) && // If button pressed
          (Srvc_TestSWTmrU32(&st_KeyPad_TmSinceBtnRlsd) != FALSE) && // release timer is not started yet
          (st_KeyPad_debVal.u8_statBtn1 == FALSE) && (st_KeyPad_debVal.u8_statBtn2 == FALSE)
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

      case 2:
      {
        st_KeyPad_finalVal.u8_statBtn2 = TRUE;
      }
      break;

//      case 3:
//      {
//        st_KeyPad_finalVal.u8_statBtn3 = TRUE;
//      }
//      break;
//
//      case 4:
//      {
//        st_KeyPad_finalVal.u8_statBtn4 = TRUE;
//      }
//      break;
//
//      case 5:
//      {
//        st_KeyPad_finalVal.u8_statBtn5 = TRUE;
//      }
//      break;
//
//      case 6:
//      {
//        st_KeyPad_finalVal.u8_statBtn6 = TRUE;
//      }
//      break;

      default:
      break;
    }

  }

  // Check for simultaneous key-press of specific keys (button-1 to button-4)
  // Used for special cases (e.g. reset of PIN)
//  if((st_KeyPad_debVal.u8_statBtn1 != FALSE) && (st_KeyPad_debVal.u8_statBtn2!= FALSE) &&
//     (st_KeyPad_debVal.u8_statBtn3 != FALSE) && (st_KeyPad_debVal.u8_statBtn4!= FALSE))
  if((st_KeyPad_debVal.u8_statBtn1 != FALSE) && (st_KeyPad_debVal.u8_statBtn2!= FALSE))

  {
    st_KeyPad_finalVal.u8_multipleKeyPrsd = TRUE;
  }
  
  // Simulatneous key-press of buttons 1 2 3 starts ASD Logging
//  if((TRUE == st_KeyPad_debVal.u8_statBtn1) &&
//     (TRUE == st_KeyPad_debVal.u8_statBtn2) &&
//     (TRUE == st_KeyPad_debVal.u8_statBtn3) &&
//     (FALSE == st_KeyPad_debVal.u8_statBtn4) &&
//     (FALSE == st_KeyPad_debVal.u8_statBtn5) &&
//     (FALSE == st_KeyPad_debVal.u8_statBtn6))
  if(
	(TRUE == st_KeyPad_debVal.u8_statBtn1) &&
    (TRUE == st_KeyPad_debVal.u8_statBtn2)
    )
  {
    st_KeyPad_finalVal.u8_ASD_Detect = TRUE;
  }

  // Update the time
  st_KeyPad_finalVal.u32_tmSinceBtnPrsd_ms = Srvc_DiffSWTmrU32(&st_KeyPad_TmSinceBtnPrsd);

  // Code_TAG: SRC_KEYPAD_004: Update the keypad status to data collector
//  DatCol_Write (DC_INTFCE_KEYPAD_1, &st_KeyPad_finalVal);

  
  // updating the status of LED 1: port_A 13 in respect to switch 1: port_A 24

/*-------------------------
  
 Blink_LED(LED2);
  
    if(st_KeyPad_debVal.u8_statBtn1 == FALSE)
          tog_led_flag = 0;

  if ( (st_KeyPad_debVal.u8_statBtn1 != FALSE)  && (tog_led_flag != st_KeyPad_debVal.u8_statBtn1) )
  {
    tog_led_flag = 1 ;
    Toggle_LED(LED1);  
//    Toggle_LED(LED2);      
  }
--------------------------*/

   
   /*
    // updating the status of LED 1: port_A 13 in respect to switch 1: port_A 24
  if(st_KeyPad_finalVal.u8_statBtn1)
    LED_status[0] = 1;
   else
     LED_status[0] = 0;
  // updating the status of LED 2: port_A 14 in respect to switch 2: port_A 25
   if(st_KeyPad_finalVal.u8_statBtn2)
     LED_status[1] = 1;
   else
     LED_status[1] = 0;
   */
   
//
//  // Detect if any button is stuck for long time and update the error vector if so
//  DSM_ErrHdlr (KEYPAD_BTN1STCK_ERR, st_KeyPad_debVal.u8_statBtn1 != FALSE);  // Update the error vector
//  DSM_ErrHdlr (KEYPAD_BTN2STCK_ERR, st_KeyPad_debVal.u8_statBtn2 != FALSE);  // Update the error vector
//  DSM_ErrHdlr (KEYPAD_BTN3STCK_ERR, st_KeyPad_debVal.u8_statBtn3 != FALSE);  // Update the error vector
//  DSM_ErrHdlr (KEYPAD_BTN4STCK_ERR, st_KeyPad_debVal.u8_statBtn4 != FALSE);  // Update the error vector
//  DSM_ErrHdlr (KEYPAD_BTN5STCK_ERR, st_KeyPad_debVal.u8_statBtn5 != FALSE);  // Update the error vector
//  DSM_ErrHdlr (KEYPAD_BTN6STCK_ERR, st_KeyPad_debVal.u8_statBtn6 != FALSE);  // Update the error vector
  
//  Shell_Printf ("1: %d   %d  %d\n", st_KeyPad_rawVal.u8_statBtn1,  st_KeyPad_debVal.u8_statBtn1, st_KeyPad_finalVal.u8_statBtn1);
//  Shell_Printf ("2: %d   %d  %d\n", st_KeyPad_rawVal.u8_statBtn2,  st_KeyPad_debVal.u8_statBtn2, st_KeyPad_finalVal.u8_statBtn2);
//  Shell_Printf ("3: %d   %d  %d\n", st_KeyPad_rawVal.u8_statBtn3,  st_KeyPad_debVal.u8_statBtn3, st_KeyPad_finalVal.u8_statBtn3);
//  Shell_Printf ("4: %d   %d  %d\n", st_KeyPad_rawVal.u8_statBtn4,  st_KeyPad_debVal.u8_statBtn4, st_KeyPad_finalVal.u8_statBtn4);
//  Shell_Printf ("5: %d   %d  %d\n", st_KeyPad_rawVal.u8_statBtn5,  st_KeyPad_debVal.u8_statBtn5, st_KeyPad_finalVal.u8_statBtn5);
//  Shell_Printf ("6: %d   %d  %d\n", st_KeyPad_rawVal.u8_statBtn6,  st_KeyPad_debVal.u8_statBtn6, st_KeyPad_finalVal.u8_statBtn6);
//  Shell_Printf ("\n\n");

}

/**
***************************************************************************************************
* Function to restart KeyPad timer
***************************************************************************************************
*/
void KeyPad_ResetTmr(void)
{
  Srvc_StartSWTmrU32(&st_KeyPad_TmSinceBtnPrsd); // Restart the timer
}  

/**
***************************************************************************************************
* Function to acknowledge and reset the key pressed status
***************************************************************************************************
*/
void KeyPad_AckAndReset(void)
{
  u8_KeyPad_Acknowledge = TRUE;
}

/**
***************************************************************************************************
* Function to write keypad status values to shared memory
*
* \param  *Dest   Pointer to destination data (shared memory location)
* \param  *Src    Pointer to source data (local memory location)
*
* \return None.
***************************************************************************************************
*/
void DC_KeyPad_1_Wr (void *Dest, void *Src)
{
  DC_KeyPad_Intfce_Struct   *st_SharedMemPtr;  // Pointer to shared memory
  DC_KeyPad_Intfce_Struct   *st_LocaldMemPtr;  // Pointer to local memory
  
  st_SharedMemPtr = (DC_KeyPad_Intfce_Struct*)Dest;
  st_LocaldMemPtr = (DC_KeyPad_Intfce_Struct*)Src;
  
  // Copy operation from local memory to shared memory
  Mem_Copy (st_SharedMemPtr, st_LocaldMemPtr, sizeof(DC_KeyPad_Intfce_Struct));
}


