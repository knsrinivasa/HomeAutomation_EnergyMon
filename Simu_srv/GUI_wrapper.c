/*
 * GUI_wrapper.c
 *
 *  Created on: Nov 29, 2013
 *      Author: skn2kor
 */

#include "main_macros.h"

#define	FLASH_STORE	DISABLED

#define CALIB_DISP_DEL  0
#define SUCESS_RET_DEL 0
#define ABOUT_DELAY 0

#define DEVICE_STORE_FLASH_SIZE 5000
#define TRAINDATA_FLASHSIZE     50
#define	MAX_DEV_DELETE	DV_MAX_CT               // delete device array size (for static & dynamic mem allocation)

#define FIRST           1
#define SECOND          2
#define THIRD           3

#define IDLE            0
#define INPROCESS       1
#define DONE            2

/*------------------------------------------------------------------------------*/
void Runtime_Info(device_struct *List_st) ;
void SystemInfo(device_struct *List_st) ;
void DeviceList( device_struct *List_st) ;
void DeviceStatus( device_struct *List_st, device_status *Status_st, uint8 class_fld);
void print_about(void) ;
void System_para(System_parameter passed_st []) ;
void DeleteDevice_List( device_struct *List_st) ;

/*------------------------------------------------------------------------------*/


static uint8 u8initDelay = CALIB_DISP_DEL ;
uint8 u8select;
static uint8 deldev_cnt = 0;


/************************************ Static Variables **************************************************/

static uint8 DeviceData_FlashReadStatus;                 ///< Flash read order status
static uint8 DeviceData_FlashWriteStatus;                ///< Flash write order status
static uint8 DeviceData_FlashEraseStatus;                ///< Flash write order status


static uint8 TrainData_FlashReadStatus;                 ///< Flash read order status
static uint8 TrainData_FlashWriteStatus;                ///< Flash write order status
static uint8 TrainData_FlashEraseStatus;                ///< Flash write order status

static  uint8 u8caliReturn;         ///< verifies calibration done or not
static uint8 Zero_ini_flag = 0;
static uint8 u8initDelay_Sucess_ret= SUCESS_RET_DEL;
static uint8 u8initDelay_calib = CALIB_DISP_DEL;
static uint8 u8initFlashDelay = CALIB_DISP_DEL;
static uint8 u8initAboutDelay = ABOUT_DELAY;

/*************************************Global Variables**********************************************/
uint8 u8userButton;         ///< Stores the particular state of the button press
static uint8 u8currentState;       ///< Stores the previous state
uint8 u8delay;
uint8 u8runTimeWindow;      ///< to differentiate the run time info screen
uint8 u8calibrationStatus;  ///< stores the current calibration status
uint8 u8roll;               ///< for sliding the window
uint8 DeviceData_FlashStore  [DEVICE_STORE_FLASH_SIZE];     ///< Store device parameter data
uint8 TrainDataFlashStore [TRAINDATA_FLASHSIZE] ;           ///< Store train variables data

static uint8 DeleteDevice_arr[MAX_DEV_DELETE];                                  // Array to store list of devices need to delete (store device ID)

device_struct* device_list_flash_copyptr;
Train_Init_struct* Train_data_flash_copyptr;

#if ( (PQ_ZERO_CAL == ENABLED) && (RATIOMETRIC_CORR == ENABLED) )
System_parameter SystemInfo_st[11]={0};
#elif ( (PQ_ZERO_CAL == ENABLED) || (RATIOMETRIC_CORR == ENABLED) )
System_parameter SystemInfo_st[9]={0};
#else
System_parameter SystemInfo_st[6]={0};
#endif

bool booltrainingflag;
bool boolwriteFlag = TRUE;
bool boolreadFlag = TRUE;
bool booleraseFlag = TRUE;
bool boolsingleeraseFlag = TRUE;
bool boolstateBusy;
bool boolwritedone = FALSE;
bool booltrainwrite = FALSE;
bool boolerasedone = FALSE;
static bool boolsessionComplete = FALSE;
bool boolmaxcheck               = FALSE;                                        // flag to check max devices are trained for static memory allocation
static bool booldeldevselect = FALSE;                                           // flag used for Select & delete device

/*--------------------------------------------------------------------------------------------------------*/
int gui_hdlr(int option)
{

	  static uint8	trn_session_flag = 0 ;
	  static uint8	trn_dv_flag	 = 0 ;
	  static uint8	trn_accept_flag	 = 0 ;
	  static uint8	trn_reject_flag	 = 0 ;
	  static uint8	trn_calib_flag	 = 0 ;
	  uint8 device_trn_st            = 0 ;
	  uint8 dvtrn_accep_rej_st       = 0 ;
	  uint16 DeviceData_FlashCheckSum = 0;
	  uint16 TrainData_FlashCheckSum = 0;
	  uint8 trained_dv_cnt            = 0 ;
	  uint8 u8Mode = 0;
	  uint8 u8devCnt = 0;
	  uint8 u8eventONtoOFF = 0;
	  uint8 u8eventOFFtoON= 0;
	  uint8 i ;
	  u32iterationCnt++;

	switch(option)
	{
/*
	  case FLASH_READ_INIT:
	    {
	      FlashRead();
	      break;
	    }
*/

	case OFFLINE_TRAIN_LOAD:
		// Load offline Device training Data: from file
		break ;

	  case DEVICE_STATUS:
	    {
	      // remembers the previous state
	      u8currentState = DEVICE_STATUS ;
	      break;
	    }

	  case WT_STATUS:
	    {
	      DeviceStatus( device_list, device_st, WT_ST )  ;
	      // remembers the previous state
	      u8currentState = WT_STATUS ;

	      break;
	    }

	  case DEVICE_LIST:
	    {
	      DeviceList(device_list);
	      // remembers the previous state
	      u8currentState = DEVICE_LIST ;
	      break;
	    }

	  case ST_STATUS:
	    {
	      DeviceStatus( device_list, device_st, STD_ST )  ;

	      // remembers the previous state
	      u8currentState = ST_STATUS ;
	      break;
	    }

	  case FN_STATUS:
	    {
	      DeviceStatus( device_list, device_st, FNL_ST)  ;

	      // remembers the previous state
	      u8currentState = FN_STATUS ;

	      break;
	    }

	  case SYSTEM_INFO:
	    {
	    	SystemInfo(device_list);
	        u8currentState = SYSTEM_INFO ;
	    	break;
	    }

	  case TRAINING_SESSION:
	    {
	#if (PQ_ZERO_CAL == ENABLED)
	      if (Zero_ini_flag==0)
	      {
	        boolstateBusy = TRUE;
	        gputs_vp("Zero Calibration is going on...");

	        u8caliReturn = PQ_Zero_Calib( WattValue, VarValue, &P_Calib, &Q_Calib ) ;
	        if(u8caliReturn==1)
	        {
	          Zero_ini_flag = 1;
	          u8caliReturn = 0;
	          break;
	        }
	      }
	      if( (Zero_ini_flag==1) && (u8initDelay--) )
	      {
	        u8calibrationStatus = INPROCESS ;
	        u8caliReturn  = 0 ;
	        gputs_vp("Zero Calibration is Done.\n");
	        break;
	      }
	      else if(Zero_ini_flag==1)
	      {
	        Zero_ini_flag = 2 ;
	      }
	#else
	      Zero_ini_flag=2 ;
	#endif

	      //      Sys_Mode = TRAIN_SESSION  ;                           // SYSTEM Mode set to -> TRAIN_SESSION
	      if(trn_session_flag == 0)
	      {
	        trn_session_flag = 1 ;
	        Set_StM_Sess(TRAIN_SESSION) ;                           // SYSTEM Mode set to -> TRAIN_SESSION
	      }
	      if( Zero_ini_flag==2 )
	      {
	//        u8initDelay = 5;
	        boolstateBusy = FALSE;
	        boolmaxcheck = FALSE;

	        // remembers the previous state
	        u8currentState = TRAINING_SESSION ;

	        break;
	      }
	      break;
	    }

	  case TRAIN_DEVICE :
	    {
	      trained_dv_cnt = Get_DvTrn_Cnt () ;

	     #if(MEM_ALLOCATION == STATIC)
	        if (trained_dv_cnt == DV_MAX_CT )
	        {
	           boolmaxcheck = TRUE;
	        }
	     #endif

	        if(boolmaxcheck != TRUE)
	        {
	        	if(trn_dv_flag == 0)
				{
				  trn_dv_flag = 1 ;

				  Set_StM_DvTrn(DV_TRN_START, 0);                            // Set the status of Device training to => START & Cond - NA for start
				  Set_DvTrn_AccpRej_Confirm(INIT) ;
				}
	        //      if(trn_dv_ret!=1)
	        device_trn_st = Get_StM_DvTrn();                            // Get the status of Device training

	        //      if(trn_dv_finish!=1)
	        if(device_trn_st != DV_TRN_END)
	        {
	          //        Set_StM_DvTrn(DV_TRN_START, 0);                            // Set the status of Device training to => START & Cond - NA for start
	          gputs_vp("Training is going on...\n");

				#if( (SIMULATOR == ENABLED) && (SIMU_READ_PATH == FILE_READ) )
				  Set_Simu_Train_PQ_Read_St (START_READ_TRAIN_PQ) ;
				  Set_GUI_IP_Req_St (GUI_IP_ENTERED) ;
				#endif
	        }

	        dvtrn_accep_rej_st =  Get_DvTrn_AccpRej_Confirm() ;
	        //      if(trn_dv_ret==1)
	        //      if(trn_dv_finish==1)
	        if ( (device_trn_st == DV_TRN_END) && (dvtrn_accep_rej_st==0) )
	        {
	          gputs_vp("Device Training is Done\n> Accept or Reject< \n");

	        }
	      }     //       if (trained_dv_cnt < DV_MAX_CT )
	       else
	        {
	           gputs_vp("\t!!! Max Devices Trained !!!\n\n");
	        }

	      u8currentState= TRAIN_DEVICE;

	//      Zero_ini_flag = 0;
	      break;
	    }

	  case CALIBRATION :
	    {
	      u8caliReturn = 0;

	#if (PQ_ZERO_CAL == ENABLED)
	      if(trn_calib_flag == 0)
	      {
	        trn_calib_flag = 1 ;
			puts("\n---- System Zero Calibration is going on: ----\n");
	        u8caliReturn = PQ_Zero_Calib( WattValue, VarValue, &P_Calib, &Q_Calib ) ;
			printf("Zero Calibrated Values:\tP_Calib = %d\tQ_Calib = %d\n", P_Calib, Q_Calib );
	        u8calibrationStatus = INPROCESS ;
//	        gputs_vp("Zero Calibration is going on...");

	      }

	      if(u8caliReturn==1)
	      {
	        u8calibrationStatus = DONE;
	        gputs_vp("Zero Calibration is Done.\n");
	      }
	#else
	      gputs_vp("Zero Calibration is Disabled\n");
	#endif
	      if((u8initDelay_calib--)==1)
	      {
	        u8initDelay_calib   = CALIB_DISP_DEL  ;
	        trn_calib_flag = 0 ;
	         u8userButton = TRAINING_SESSION;
	        break;
	      }

	      u8currentState= CALIBRATION;
	      break;
	    }

	  case TRAINING_ACCEPTED:
	    {
	      //#if(MEM_ALLOCATION == STATIC)
	        //if(trained_dv_cnt == DV_MAX_CT)
	        //{
	          //boolsessionComplete= TRUE;
	        //}
	      //#endif

	      if( trn_accept_flag == 0)
	      {
	        trn_accept_flag = 1  ;
	        Set_StM_DvTrn(DV_TRN_END, ACCEPT);                    // Set the status of Device training to => END and training => ACCEPT
	      }

#if (FLASH_STORE == ENABLED)
	      if(boolwriteFlag==FALSE)
	      {
	        // For device parameters
	        Mem_Copy( (void*)DeviceData_FlashStore, (void*)device_list, sizeof(device_struct)* Dev_Cnt_Idx);

	        if ((DeviceData_FlashWriteStatus == FLASHHDLR_ORDER_NONE))   // Write has not happened
	        {
	          // For device parameters
	          DeviceData_FlashCheckSum = FlashHdlr_CalcChecksum (DeviceData_FlashStore,(DEVICE_STORE_FLASH_SIZE-2));

	          DeviceData_FlashStore[DEVICE_STORE_FLASH_SIZE-2] = (uint8)(DeviceData_FlashCheckSum & 0xFF);
	          DeviceData_FlashStore[DEVICE_STORE_FLASH_SIZE-1] = (uint8)(DeviceData_FlashCheckSum >> 8) ;
	          FlashHdlr_ReadWrite (FLASH_ID_NILM, WRITE_MODE, DeviceData_FlashStore, &DeviceData_FlashWriteStatus);
	        }
	        else if (DeviceData_FlashWriteStatus == FLASHHDLR_ORDER_ERROR)  // Write error occurred --> Try again
	        {
	          //    FlashHdlr_ReadWrite (FLASH_ID_RUDIMAP, WRITE_MODE, u8_SysSM_RudiMapFlashStore, &u8_RudiMapWriteStat);
	        }
	        else if (DeviceData_FlashWriteStatus == FLASHHDLR_ORDER_SUCCESS)
	        {
	          boolwritedone = TRUE;
	          DeviceData_FlashWriteStatus = FLASHHDLR_ORDER_NONE;
	        }
	      }
	      if(boolwritedone)
	      {
	        // For train re init
	        TrainComplete_st[0].u8Dv_Cnt_Idx = Dev_Cnt_Idx;
	        TrainComplete_st[0].u32P_Calib   = P_Calib    ;
	        TrainComplete_st[0].u32Q_Calib   = Q_Calib    ;
	        TrainComplete_st[0].u16Volt_trn  = Volt_Trn   ;
	        TrainComplete_st[0].boolsessionFlashComplete  = boolsessionComplete   ;

	        Mem_Copy( (void*)TrainDataFlashStore, (void*)TrainComplete_st, sizeof(Train_Init_struct));
	        // For train re init
	        TrainData_FlashCheckSum = FlashHdlr_CalcChecksum (TrainDataFlashStore,(TRAINDATA_FLASHSIZE-2));

	        TrainDataFlashStore[TRAINDATA_FLASHSIZE-2] = (uint8)(TrainData_FlashCheckSum & 0xFF);
	        TrainDataFlashStore[TRAINDATA_FLASHSIZE-1] = (uint8)(TrainData_FlashCheckSum >> 8) ;
	        FlashHdlr_ReadWrite (FLASH_ID_NILM_INIT, WRITE_MODE, TrainDataFlashStore, &TrainData_FlashWriteStatus);
	        boolwriteFlag = TRUE;
	        boolreadFlag = TRUE;
	        boolwritedone = FALSE;
	      }
	      else if (TrainData_FlashWriteStatus == FLASHHDLR_ORDER_ERROR)  // Write error occurred --> Try again
	      {
	        //    FlashHdlr_ReadWrite (FLASH_ID_RUDIMAP, WRITE_MODE, u8_SysSM_RudiMapFlashStore, &u8_RudiMapWriteStat);
	      }
	      else if (TrainData_FlashWriteStatus == FLASHHDLR_ORDER_SUCCESS)
	      {
	        Shell_Printf ("Write-1-OK\n");
	        gputs_vp("Write OK");
	        TrainData_FlashWriteStatus = FLASHHDLR_ORDER_NONE;
	      }

	      Shell_Printf("bytes\n");
	      Shell_Printf("%d", sizeof(device_struct)* Dev_Cnt_Idx);
#endif
	      gputs_vp("\n\t******* Training is Successful ******* \n\n");

	      // Training status flag
	      booltrainingflag = TRUE;
	      u8currentState= TRAINING_ACCEPTED;
	      if(!(u8initDelay_Sucess_ret--))
	      {
	        u8userButton = TRAINING_SESSION;
	        u8initDelay_Sucess_ret = SUCESS_RET_DEL;
	        trn_accept_flag = 0  ;
	        trn_dv_flag     = 0  ;
	        Set_DvTrn_AccpRej_Confirm(INIT) ;
	      }
	      break;
	    }

	    case TRAINING_REJECTED:
	    {
	      if( trn_reject_flag == 0)
	      {
	        trn_reject_flag = 1  ;
	        Set_StM_DvTrn(DV_TRN_END, REJECT);                    // Set the status of Device training to => END and training => REJECT
	      }

	      gputs_vp("\n\t??????? Training is Rejected ???????\n\n");

	      // Training status flag
	      booltrainingflag = FALSE;
	      u8currentState= TRAINING_REJECTED;
	      break;
	    }

	  // Selection page for delete single or all
	  case DELETE:
	    {
	      gputs_vp("Delete the selected devices");

//	      if(Dev_Cnt_Idx != 0)
//	      SelectiveDelete();
//
//	      DeleteALL();

	      u8currentState= DELETE;

	      break;
	    }

	    case DEVICE_INFO:
	    {
	      DeviceList(device_list);
	      // remembers the previous state
	      u8currentState = DEVICE_INFO ;
	      break;
	    }

	    case RUN_TIME_INFO:
	    {
	    	Runtime_Info(device_list);

	      // remembers the previous state
	      u8currentState = RUN_TIME_INFO ;

	      break;
	    }

	    case SELECT_DELETE:
	    {
	      DeleteDevice(device_list);
	      // remembers the previous state
	      u8currentState = SELECT_DELETE ;

	      break;
	    }

	    case DELETE_CONFIRM:
	    {
	      boolsingleeraseFlag = TRUE;                             // enabling the erase flag
	      gputs_vp("Confirm Delete");

	      DeleteDevice_List(device_list);                // List-out selected devices for deletion
//	      ConfirmWindow();
	      u8currentState= DELETE_CONFIRM;
	      break;
	    }

/*#if (FLASH_STORE == DISABLED)	                                                              // Deleting selected device
	  case SELECT_ACCEPT:                                         //YES Button
	    {
	      boolstateBusy = TRUE;

	      if(boolsingleeraseFlag == TRUE)
	      {
	        if ((DeviceData_FlashWriteStatus == FLASHHDLR_ORDER_NONE))   // Write has not happened
	        {
	          if(booldeldevselect == TRUE)                                // condition to decrement deldev_cnt by one
	          {
	            deldev_cnt--;
	            for(i = 0; i < deldev_cnt; i++)
	            {
	              device_list[DeleteDevice_arr[i]] = null_device_list[0];         // loop to delete selected devices
	            }
	          }
	          else
	          {
	            for(i = 0; i < deldev_cnt; i++)
	            {
	              device_list[DeleteDevice_arr[i]] = null_device_list[0];        // loop to delete selected devices
	            }
	          }

	          if(Dev_Cnt_Idx > 0)
	              Sort_Delete(device_list, Dev_Cnt_Idx);                  // send all 0 to last

	            booldeldevselect = FALSE;
	            for(i=0; i < MAX_DEV_DELETE; i++)                           // release all the device id after deletion
	                DeleteDevice_arr[i] = 0;

	            Dev_Cnt_Idx = Dev_Cnt_Idx - deldev_cnt;                         // decrement Dev_Cnt_Idx after deletion

	            deldev_cnt = 0;                                               // back to 0 deldev_cnt


	          // For Each device parameters
	          Shell_Printf ("Flash Erase\n");
	          // For device parameters
	          Mem_Copy( (void*)DeviceData_FlashStore, (void*)device_list, sizeof(device_struct)* Dev_Cnt_Idx);

	          DeviceData_FlashCheckSum = FlashHdlr_CalcChecksum (DeviceData_FlashStore,(DEVICE_STORE_FLASH_SIZE-2));

	          DeviceData_FlashStore[DEVICE_STORE_FLASH_SIZE-2] = (uint8)(DeviceData_FlashCheckSum & 0xFF);
	          DeviceData_FlashStore[DEVICE_STORE_FLASH_SIZE-1] = (uint8)(DeviceData_FlashCheckSum >> 8) ;
	          FlashHdlr_ReadWrite (FLASH_ID_NILM, WRITE_MODE, DeviceData_FlashStore, &DeviceData_FlashWriteStatus);
	        }

	        else if(DeviceData_FlashWriteStatus == FLASHHDLR_ORDER_ERROR)  // Write error occurred --> Try again
	        {
	          Shell_Printf ("Erase-Failed\n");
	        }

	        else if(DeviceData_FlashWriteStatus == FLASHHDLR_ORDER_SUCCESS)
	        {
	          Shell_Printf ("Erase-OK\n");
	          boolsingleeraseFlag=FALSE;
	          boolwritedone = TRUE;
	          //boolreadFlag = TRUE;
	          gputs_vp("Erase-OK");
	          DeviceData_FlashEraseStatus = FLASHHDLR_ORDER_NONE;
	          DeviceData_FlashWriteStatus = FLASHHDLR_ORDER_NONE;
	        }
	      }

	      if(boolwritedone)
	      {
	        // For train re init
	        TrainComplete_st[0].u8Dv_Cnt_Idx = Dev_Cnt_Idx;
	        TrainComplete_st[0].u32P_Calib   = P_Calib    ;
	        TrainComplete_st[0].u32Q_Calib   = Q_Calib    ;
	        TrainComplete_st[0].u16Volt_trn  = Volt_Trn   ;
	        TrainComplete_st[0].boolsessionFlashComplete  = boolsessionComplete   ;

	        Mem_Copy( (void*)TrainDataFlashStore, (void*)TrainComplete_st, sizeof(Train_Init_struct));
	        // For train re init
	        TrainData_FlashCheckSum = FlashHdlr_CalcChecksum (TrainDataFlashStore,(TRAINDATA_FLASHSIZE-2));

	        TrainDataFlashStore[TRAINDATA_FLASHSIZE-2] = (uint8)(TrainData_FlashCheckSum & 0xFF);
	        TrainDataFlashStore[TRAINDATA_FLASHSIZE-1] = (uint8)(TrainData_FlashCheckSum >> 8) ;
	        FlashHdlr_ReadWrite (FLASH_ID_NILM_INIT, WRITE_MODE, TrainDataFlashStore, &TrainData_FlashWriteStatus);
	        boolwriteFlag = TRUE;
	        boolreadFlag = TRUE;
	        boolwritedone = FALSE;
	      }
	      else if (TrainData_FlashWriteStatus == FLASHHDLR_ORDER_ERROR)  // Write error occurred --> Try again
	      {
	        //    FlashHdlr_ReadWrite (FLASH_ID_RUDIMAP, WRITE_MODE, u8_SysSM_RudiMapFlashStore, &u8_RudiMapWriteStat);
	      }
	      else if (TrainData_FlashWriteStatus == FLASHHDLR_ORDER_SUCCESS)
	      {
	        Shell_Printf ("Write-1-OK\n");
	        gputs_vp("Write OK");
	        TrainData_FlashWriteStatus = FLASHHDLR_ORDER_NONE;
	      }

	      Shell_Printf("bytes\n");
	      Shell_Printf("%d", sizeof(device_struct)* Dev_Cnt_Idx);

	      gputs_vp("Deleting selected trained value");

	      if(!(u8initFlashDelay--))
	      {
	        boolstateBusy = FALSE;
	        u8userButton = FLASH_READ_INIT;
	        boolsingleeraseFlag = TRUE;
	        u8initFlashDelay = CALIB_DISP_DEL;
	      }
	      break;
	    }
*/
	  case CONFIRM_WINDOW:
	    {
	      booleraseFlag=TRUE;                                                                   // enabling the erase flag

	      gputs_vp("Confirm Delete");

//	      ConfirmWindow();
	      u8currentState= CONFIRM_WINDOW;
	      break;
	    }

/*	  // Deleting all devices
	  case DELETE_ACCEPT:
	    {
	      boolstateBusy = TRUE;
	      // this is for device structure parameters
	      if(booleraseFlag==TRUE)
	      {
	        if ((DeviceData_FlashWriteStatus == FLASHHDLR_ORDER_NONE))   // Write has not happened
	        {
	          // For Each device parameters
	         Shell_Printf ("Flash Erase\n");
	       // For device parameters
	        Mem_Copy( (void*)DeviceData_FlashStore, (void*)null_device_list, sizeof(device_struct)* Dev_Cnt_Idx);
	        Mem_Copy( (void*)device_list, (void*)null_device_list, sizeof(device_struct)* Dev_Cnt_Idx);

	//          FlashHdlr_ReadWrite (FLASH_ID_NILM, ERASE_MODE, DeviceData_FlashStore, &DeviceData_FlashEraseStatus);
	          DeviceData_FlashCheckSum = FlashHdlr_CalcChecksum (DeviceData_FlashStore,(DEVICE_STORE_FLASH_SIZE-2));

	          DeviceData_FlashStore[DEVICE_STORE_FLASH_SIZE-2] = (uint8)(DeviceData_FlashCheckSum & 0xFF);
	          DeviceData_FlashStore[DEVICE_STORE_FLASH_SIZE-1] = (uint8)(DeviceData_FlashCheckSum >> 8) ;
	          FlashHdlr_ReadWrite (FLASH_ID_NILM, WRITE_MODE, DeviceData_FlashStore, &DeviceData_FlashWriteStatus);
	//
	//          //          TrainData_FlashCheckSum = FlashHdlr_CalcChecksum (TrainDataFlashStore,(TRAINDATA_FLASHSIZE-2));
	//          //
	//          //          TrainDataFlashStore[TRAINDATA_FLASHSIZE-2] = (uint8)(TrainData_FlashCheckSum & 0xFF);
	//          //          TrainDataFlashStore[TRAINDATA_FLASHSIZE-1] = (uint8)(TrainData_FlashCheckSum >> 8) ;
	//          //          FlashHdlr_ReadWrite (FLASH_ID_NILM_INIT, WRITE_MODE, TrainDataFlashStore, &TrainData_FlashEraseStatus);
	//          DeviceData_FlashEraseStatus = FLASHHDLR_ORDER_SUCCESS;
	        }
	        else if (DeviceData_FlashWriteStatus == FLASHHDLR_ORDER_ERROR)  // Write error occurred --> Try again
	        {
	          Shell_Printf ("Erase-Failed\n");

	        }
	        else if (DeviceData_FlashWriteStatus == FLASHHDLR_ORDER_SUCCESS)
	        {
	          Shell_Printf ("Erase-OK\n");
	          booleraseFlag=FALSE;
	          boolreadFlag = TRUE;
	          DeviceData_FlashEraseStatus = FLASHHDLR_ORDER_NONE;
	          DeviceData_FlashWriteStatus = FLASHHDLR_ORDER_NONE;
	          boolerasedone = TRUE;
	        }
	      }
	      // this is for other parameters
	      if(boolerasedone)
	      {
	          // For Train init parameters
	        Mem_Copy( (void*)TrainDataFlashStore, (void*)null_TrainComplete_st, sizeof(Train_Init_struct));
	        TrainData_FlashCheckSum = FlashHdlr_CalcChecksum (TrainDataFlashStore,(TRAINDATA_FLASHSIZE-2));

	        TrainDataFlashStore[TRAINDATA_FLASHSIZE-2] = (uint8)(TrainData_FlashCheckSum & 0xFF);
	        TrainDataFlashStore[TRAINDATA_FLASHSIZE-1] = (uint8)(TrainData_FlashCheckSum >> 8) ;
	        FlashHdlr_ReadWrite (FLASH_ID_NILM_INIT, WRITE_MODE, TrainDataFlashStore, &TrainData_FlashWriteStatus);
	//          FlashHdlr_ReadWrite (FLASH_ID_NILM_INIT, ERASE_MODE, TrainDataFlashStore, &TrainData_FlashEraseStatus);
	            boolerasedone = FALSE;
	      }
	      else if(TrainData_FlashWriteStatus == FLASHHDLR_ORDER_SUCCESS)
	      {
	          gputs_vp("Erase-OK");
	          TrainData_FlashWriteStatus = FLASHHDLR_ORDER_NONE;

	      }
	      gputs_vp("Deleting all trained values");

	      if(!(u8initFlashDelay--))
	      {
	        boolstateBusy = FALSE;
	        u8userButton = FLASH_READ_INIT;
	        booleraseFlag = TRUE;
	        u8initFlashDelay = CALIB_DISP_DEL;
	      }
	      break;
	    }
*/
	  case MORE_REJECT:                                    // New page created for device delete
	    {
	       if(deldev_cnt == 1)                                    // Condition for first device
	        {
	         DeleteDevice_arr[deldev_cnt-1] = u8select;           // 0th location in array

	         gputs_vp("       Device is selected ");
	        }

	       else
	        {
	          if(deldev_cnt > MAX_DEV_DELETE)                     // if length exceed
	          {
	            booldeldevselect = TRUE;
	            gputs_vp(" Max no. of Devices are selected ");
	          }
	          else
	          {
	            for(i = 0; i < deldev_cnt-1; i++ )
	            {
	              if(DeleteDevice_arr[i] == u8select )            // Condition for already selected device need to decrement deldev_cnt
	              {
	                booldeldevselect = TRUE;                         // set flag for this condition need to decrement deldev_cnt
	                gputs_vp("     Device is already selected try again");
	              }
	            }
	           }
	          if(booldeldevselect != TRUE)            // Condition for device selection
	          {
	            DeleteDevice_arr[deldev_cnt-1] = u8select;
	            gputs_vp("       Device is selected ");
	          }
	        }

	      u8currentState= MORE_REJECT;
	       break;
	    }

	  case ABOUT:
	    {

	        u8userButton=u8currentState;
	        print_about() ;
//	      u8initAboutDelay = ABOUT_DELAY;
	        u8currentState = RUN_SESSION ;
	      break;
	    }
	  case BACK:
	    {
	      u8roll =0;
	      // to go to previous screen this will provide the information
	      switch(u8currentState)
	      {
	      case HOME:
	        u8userButton = HOME;
	        break;

	      case TRAINING_SESSION:
	        u8userButton = HOME;
	        //        Sys_Mode = RE_INIT_SESSION  ;                                               //  WT
	        trn_session_flag = 0 ;
	        Set_StM_Sess(RE_INIT_SESSION) ;                           // SYSTEM Mode set to -> RE_INIT_SESSION  //  to indicate re-init session after adding new device
	        break;

	      case SYSTEM_INFO:
//	        if(u8runTimeWindow == FIRST)
//	          u8userButton = HOME;
	    	  SystemInfo(device_list);
	        break;

	      case TRAIN_DEVICE:
	        trn_dv_flag = 0 ;
	        Set_DvTrn_AccpRej_Confirm(INIT) ;
	        u8userButton = TRAINING_SESSION;
	        Set_StM_DvTrn(DV_TRN_END, 0);                            // Set the status of Device training to => END, & COND - NA
	        break;

	      case CALIBRATION:
	        trn_calib_flag  = 0 ;
	        u8caliReturn    = 0 ;
	        u8userButton = TRAINING_SESSION;
	        break;

	      case TRAINING_ACCEPTED:
	        trn_accept_flag = 0  ;
	        trn_dv_flag     = 0  ;
	        Set_DvTrn_AccpRej_Confirm(INIT) ;
	        u8userButton = TRAINING_SESSION;
	        break;

	      case TRAINING_REJECTED:
	        trn_reject_flag = 0  ;
	        trn_dv_flag     = 0  ;
	        Set_DvTrn_AccpRej_Confirm(INIT) ;
	        u8userButton = TRAINING_SESSION;
	        break;

	      case DEVICE_STATUS:
	        u8userButton = HOME;
	        break;

	      case DEVICE_LIST:
	        u8userButton = HOME;
	        break;

	      case DELETE:
	        u8userButton = TRAINING_SESSION;
	        break;

	      case WT_STATUS:
	        u8userButton = DEVICE_STATUS;
	        break;

	      case ST_STATUS:
	        u8userButton = DEVICE_STATUS;
	        break;

	      case FN_STATUS:
	        u8userButton = DEVICE_STATUS;
	        break;

	      case DEVICE_INFO:
	        u8userButton = DEVICE_STATUS;
	        break;

	      case RUN_TIME_INFO:
	        u8userButton = TRAINING_SESSION;
	        break;

	      case CONFIRM_WINDOW:
	        u8userButton = DELETE;
	        break;
	      case SELECT_DELETE:
	        u8userButton = DELETE;
	        break;
	      case DELETE_CONFIRM:
	        u8userButton = SELECT_DELETE;
	        break;
	      }
	      break;

	    }

	}

	return 0 ;

}


/*****************************************************************************************************************/

void print_about(void)
{
	puts("  \nNILM Simulator V02\n-----------------------\n"
			"Device disaggregation for energy saving\n\n"
			"System Overview\n-----------------\n"
			"Device training ==> Device transient Data Acquisition ==> Signature Library buildup\n"
			"Device Recall ==> Device transient Data Acquisition ==> Signature Library match search\n\n"
			"Algorithms Used\n----------------\n"
			"Power Parameter Evaluations:\tFFT + Hilbert Transform\n"
			"Transient Feature Extraction:\tEnvelop Detection + Wavelet Transform\n"// + Gabor Transform + Predictive search + Priority Search\n"
			"Probabilistic Evaluation:\tSteady state Decision Tree\n"
			"Machine Learning:\t\tSVM Light + Incremental Re-Learning\n"
		) ;

}



/*****************************************************************************************************************/

void DeviceStatus( device_struct *List_st, device_status *Status_st, uint8 class_fld)
{
uint8 idx		;
uint8 dev_st	;


for(idx=0; idx < DV_MAX_CT ; idx++)
  {
	printf("\t%s:\t", List_st[idx].name ) ;

	switch(class_fld)
	{
		case WT_ST:
			dev_st =  Status_st[idx].wt_status ;
			break ;

		case STD_ST:
			dev_st =  Status_st[idx].status ;
			break ;

		case FNL_ST:
			dev_st =  Status_st[idx].final_status ;
			break ;
	}

	switch(dev_st)
	{
		case 0x06:
			puts("Device OFF\n");
			break ;

		case 0x0A:
			puts("Device ON\n");
			break ;

		case 0x01:
			puts("Unresolved\n");
			break ;
		default:
			puts("NA\n");
			break ;
	}

  }
printf("\n") ;
}

/*****************************************************************************************************************/

void DeviceList( device_struct *List_st)
{
	uint8 idx		;

	printf("\n") ;
	for(idx=0; idx < DV_MAX_CT ; idx++)
	  {
		printf("\t%s:\t", List_st[idx].name ) ;
		printf("PWR:\t%d W\t", List_st[idx].pn ) ;
		printf("VAR:\t%d W\n", List_st[idx].qn ) ;
	  }
	printf("\n") ;
}

/*****************************************************************************************************************/

void Runtime_Info(device_struct *List_st)
{
	uint8 u8dvCnt ;

	printf("\t|-------------------------------\n") ;
	printf("\t|  Voltage_Value   :\t%d\n", (Gui_Voltage/10) ) ;
	printf("\t|  Current_Value   :\t%d\n", Gui_Current) ;
	printf("\t|  Power_Value     :\t%d\n", Gui_Power) ;
	printf("\t|  ReactPWR_Value  :\t%d\n", Gui_React) ;

 	printf("\t|-------------------------------\n\n") ;
}

/*****************************************************************************************************************/

void SystemInfo(device_struct *List_st)
{
	uint8 u8dvCnt ;

	printf("\t|-------------------------------\n") ;
	printf("\t|  Voltage_Value   :\t%d\n", (Gui_Voltage/10) ) ;
	printf("\t|  Current_Value   :\t%d\n", Gui_Current) ;
	printf("\t|  Power_Value     :\t%d\n", Gui_Power) ;
	printf("\t|  ReactPWR_Value  :\t%d\n", Gui_React) ;

#if ( (PQ_ZERO_CAL == ENABLED) )
     printf("\t|  P_Calib_Value   :\t%d\n", P_Calib) ;
     printf("\t|  Q_Calib_Value   :\t%d\n", Q_Calib) ;
#endif

#if ( (RATIOMETRIC_CORR == ENABLED) )
     printf("\t|  Volt_Trn\t   :\t%d\n", Volt_Trn ) ;
     printf("\t|  P_RatVtg_Corr   :\t%d\n", P_RatVtg_Corr ) ;
     printf("\t|  Q_RatVtg_Corr   :\t%d\n", Q_RatVtg_Corr ) ;
#endif

     u8dvCnt = Get_DvTrn_Cnt();
     printf("\t|  No.of Dvtrained :\t%d\n", u8dvCnt ) ;
 	printf("\t|-------------------------------\n\n") ;
}

/*****************************************************************************************************************/

void System_para(System_parameter passed_st [])
{
  uint8 u8length =4;
  uint8 u8sessionMode=0;
  uint8 u8dvCnt =0;
  uint8 u8idx =0;
  uint8 i ;

  passed_st[u8idx++].Dv_Value = (Gui_Voltage/10);
  passed_st[u8idx++].Dv_Value = Gui_Current;
  passed_st[u8idx++].Dv_Value = Gui_Power;
  passed_st[u8idx++].Dv_Value = Gui_React;
#if ( (PQ_ZERO_CAL == ENABLED) )
  passed_st[u8idx++].Dv_Value = P_Calib;
  passed_st[u8idx++].Dv_Value = Q_Calib;
#endif
#if ( (RATIOMETRIC_CORR == ENABLED) )
  passed_st[u8idx++].Dv_Value = Volt_Trn;
  passed_st[u8idx++].Dv_Value = P_RatVtg_Corr;
  passed_st[u8idx++].Dv_Value = Q_RatVtg_Corr;
#endif
  //u8sessionMode= Get_StM_Sess();
  u8dvCnt = Get_DvTrn_Cnt();
  //passed_st[u8idx++].Dv_Value = u8sessionMode;
  passed_st[u8idx++].Dv_Value = u8dvCnt;

//  for(i=u8roll; i < u8roll+u8length ; i++)
  {
		//    print_dec(id,"",(int)(List_st[i].idx+1) );
		print_dec("%d\t",(int)(i+1) );

		printf("%s\t",(const char*) passed_st[i].Dv_Name);

		print_dec("%d\n",(int)passed_st[i].Dv_Value );


  }

}


/*****************************************************************************************************************/

void DeleteDevice_List( device_struct *List_st)
{
  uint8 u8length = 0, temp_deldev_cnt = 0;
  temp_deldev_cnt = deldev_cnt;               // remember number of devices selected for delete
  uint8 i ;

  if(Dev_Cnt_Idx != 0)
  {
  if(booldeldevselect == TRUE)             // decrement select device count if condition is true
  {
   temp_deldev_cnt--;
  }

  gputs_vp("%s", (const char*)"Device Name   ");
  gputs_vp("\n");

  if(temp_deldev_cnt <= 3)
  {
		u8length = temp_deldev_cnt;
  }
  else
  {
		u8length = 3;           // to display 3 values in every window.
  }

//  for(uint8 i=u8roll; i < u8roll+u8length ; i++)        // list-out all selected devices on comfirm window
   {
       print_dec("%d",(int)(i+1) );
       gputs_vp(")\t");
       gputs_vp("%s",(const char*) List_st[DeleteDevice_arr[i]].name);
       gputs_vp("\n");
  }

  }
}


/*****************************************************************************************************************/

void DeleteDevice( uint8 id, device_struct *List_st)
{
  uint8 u8length = 0;
  uint8 i, j;

  if(Dev_Cnt_Idx != 0)
  {

  // Prints Device Names
  gputs_vp("%s",(const char*)"Device Name   ");
  gputs_vp(id, "\n");

  if(Dev_Cnt_Idx <= 3)
  {
    u8length = Dev_Cnt_Idx;
  }
  else
  {
		u8length = 3;         // to display 3 values in every window.
  }
//  for(i=u8roll; i < u8roll+u8length ; i++)
	  {
		  //    print_dec(id,"",(int)(List_st[i].idx+1) );

		  print_dec("",(int)(i+1) );
		  gputs_vp(")");
		  gputs_vp((const char*) List_st[i].name);

		  for(j = 0; j < deldev_cnt; j++ )
		  {
			  if(List_st[i].idx == DeleteDevice_arr[j])
			  {
				  gputs_vp("  *");                                      // mark selected devices with *
				  break;
			  }
		  }

		  gputs_vp(id, "\n");
	  }
  }


}

/*****************************************************************************************************************/



/*****************************************************************************************************************/



