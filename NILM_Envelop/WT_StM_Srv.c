/*
 ===========================================================================================
 Name        : NILM_Srv_misc.c
 Author      : skn2kor
 Version     :
 Copyright   :
 Description : APIs for Setting and Clearing State Machine states from multiple files
 ===========================================================================================
 */

#include "main_inf.h"
#include "global_variables.h"


/*----------------------------------------------------*/


uint8 Get_DvTrn_Cnt(void)
{
  return Dev_Cnt_Idx ;
}


/*----------------------------------------------------*/


uint8 Get_Dv_OFF_to_ON_Flag(void)
{
  return DvTrn_trans_dir_OFF_TO_ON_flag ;
}

/*----------------------------------------------------*/


uint8 Get_Dv_ON_to_OFF_Flag(void)
{
  return DvTrn_trans_dir_ON_TO_OFF_flag ;
}

/*----------------------------------------------------*/

void Set_StM_Sess(uint8 Session)
{
    Sys_Mode = Session  ;                       // SYSTEM Mode set to -> TRAIN_SESSION / RE_INIT_SESSION  / RUN_SESSION
      
}

/*----------------------------------------------------*/

uint8 Get_StM_Sess(void)
{
  return Sys_Mode ;
}

/*----------------------------------------------------*/

void Set_StM_DvTrn(uint8 Trn_State, uint8 cond)
{
    uint8 Trn_2Sig_ct ;
    
    Dv_trn_st = Trn_State ;                  // Dv_trn_st set to -> DV_TRN_START

    if (Trn_State == DV_TRN_START)
    {
//      Set_DvTrn_WT(DV_TRN_WT_START) ;
      
      trn_dv_finish     = 0   ;
      trn_pq_ini_flag   = 0   ;
      dv_init           = 0   ;
      ring_buff_ret_p   = 0   ;
      ring_buff_ret_q   = 0   ;
      
//      DvTrn_trans_dir = 0   ;
    }
    else if (Trn_State == DV_TRN_END)
    {
      trn_dv_finish   =   1           ;
      Dv_trn_st       =   DV_TRN_END  ;
      
      if (cond == ACCEPT)
      {
        Set_DvTrn_AccpRej_Confirm(DV_TRN_ACCEPTED) ;

        Store_Device_features(Dev_Cnt_Idx);
        
        Trn_2Sig_ct = Get_StM_DvTrn_2Sig();                   // Get the status of Device ON/OFF signature training        
        //if( Train_ON_OFF_Flag == 2)
        if( Trn_2Sig_ct == 2)
        {
          //Train_ON_OFF_Flag = 0;
          Set_StM_DvTrn_2Sig (0)  ;
          device_list[Dev_Cnt_Idx].orderOfLearn=Dev_Cnt_Idx;
          ++Dev_Cnt_Idx		  ;
          
          // page_write(pop_string)	;
          /*-------------------------------------*/
          //      if(boolwriteFlag==TRUE)
          //      {
          //        FlashHdlr_ReadWrite (FLASH_ID_SANDY, WRITE_MODE, DUMMYData_FlashStore, &u8_DUMMY_FlashWriteStat);      
          //        boolreadFlag = TRUE;
          //        boolwriteFlag = FALSE;
          //      }        
        }
        
      }
      else if (cond == REJECT)
      {
        Set_DvTrn_AccpRej_Confirm(DV_TRN_REJECTED) ;
        
        if (DvTrn_trans_dir == OFF_TO_ON)
          DvTrn_trans_dir_OFF_TO_ON_flag = 0 ;
        else if(DvTrn_trans_dir == ON_TO_OFF)
          DvTrn_trans_dir_ON_TO_OFF_flag = 0 ;

        DvTrn_trans_dir = 0 ;
        // trn_dv_ret = 0  ;
        
        // No page_write(pop_string)	;      
      }
      
    }
    
}
/*----------------------------------------------------*/

uint8 Get_StM_DvTrn(void)
{

  return Dv_trn_st ;  
}

/*----------------------------------------------------*/

void Set_DvTrn_AccpRej_Confirm(uint8 accep_rej_st)
{
  DvTrn_AccpRej_st = accep_rej_st ;
}

/*----------------------------------------------------*/

uint8 Get_DvTrn_AccpRej_Confirm(void)
{
  return DvTrn_AccpRej_st ;  
}

/*----------------------------------------------------*/

uint8 Get_DvTrn_st(void)
{
  return trn_dv_finish ;
}

/*----------------------------------------------------*/

void Set_StM_DvTrn_2Sig(uint8 Trn_2Sig_ct)
{
    Train_ON_OFF_Flag = Trn_2Sig_ct ;

}

/*----------------------------------------------------*/

void Set_StM_DvTrn_2Sig_Incr(uint8 incr_value)
{
  Train_ON_OFF_Flag+= incr_value ;
  
}

/*----------------------------------------------------*/

uint8 Get_StM_DvTrn_2Sig(void)
{
  
  return Train_ON_OFF_Flag ;  
}

/*----------------------------------------------------*/


void Set_DvTrn_WT(uint8 wt_store_st)
{
    Dv_Trn_WT_st = wt_store_st ;

}

/*----------------------------------------------------*/

uint8 Get_DvTrn_WT(void)
{
  return Dv_Trn_WT_st ;
}

/*----------------------------------------------------*/


void Set_RunSess_WT(uint8 RunSess_wt_st)
{
    RunSess_WT_st = RunSess_wt_st ;

}

/*----------------------------------------------------*/

uint8 Get_RunSess_WT(void)
{
  return RunSess_WT_st ;
}

/*----------------------------------------------------*/

void Set_PopRing_WT(uint8 pop_ring_st)
{
    PopRing_Realign_st = pop_ring_st ;

}

/*----------------------------------------------------*/

uint8 Get_PopRing_WT(void)
{
  return PopRing_Realign_st ;
}

/*----------------------------------------------------*/


void Set_WT_DevDet_P_St(uint8 wt_det_p_st)
{
  WT_DevDet_P_st = wt_det_p_st ;
}


/*----------------------------------------------------*/

uint8 Get_WT_DevDet_P_St(void)
{
  return WT_DevDet_P_st ;
}

/*----------------------------------------------------*/


void Set_WT_DevDet_Q_St(uint8 wt_det_q_st)
{
  WT_DevDet_Q_st = wt_det_q_st ;
}

/*----------------------------------------------------*/

uint8 Get_WT_DevDet_Q_St(void)
{
  return WT_DevDet_Q_st ;
}

/*----------------------------------------------------*/

void Set_Steady_DevDet_St(uint8 steady_det_st)
{
    Steady_DevDet_st = steady_det_st ;

}

/*----------------------------------------------------*/

uint8 Get_Steady_DevDet_St(void)
{
  return Steady_DevDet_st ;
}

/*----------------------------------------------------*/


/*---------------------------------------------------------------------*/
/*--------------------- FOR SIMULATOR ---------------------------------*/
/*---------------------------------------------------------------------*/


/*----------------------------------------------------*/
static uint8 Simu_Train_PQ_Read_st ;
static uint8 Simu_Run_PQ_Read_st ;
static uint8 New_DvTrn_Start_End_st ;
static uint8 Init_sched_flag = PROC_INIT ;

/*----------------------------------------------------*/

void Set_Simu_Train_PQ_Read_St(uint8 simu_train_pq_read_st)
{
	Simu_Train_PQ_Read_st = simu_train_pq_read_st ;

}

/*----------------------------------------------------*/

uint8 Get_Simu_Train_PQ_Read_St(void)
{
  return Simu_Train_PQ_Read_st ;
}

/*----------------------------------------------------*/

void Set_Simu_Run_PQ_Read_St(uint8 simu_run_pq_read_st)
{
	Simu_Run_PQ_Read_st = simu_run_pq_read_st ;
}

/*----------------------------------------------------*/

uint8 Get_Simu_Run_PQ_Read_St(void)
{
  return Simu_Run_PQ_Read_st ;
}

/*----------------------------------------------------*/

void Set_GUI_IP_Req_St(uint8 GUI_ip_st)
{
	GUI_ip_Required = GUI_ip_st ;
}

/*----------------------------------------------------*/

uint8 Get_GUI_IP_Req_St(void)
{
  return GUI_ip_Required ;
}

/*----------------------------------------------------*/

void Set_New_DvTrn_Start(uint8 dv_trn_start_end_st)
{
	New_DvTrn_Start_End_st = dv_trn_start_end_st;
}

/*----------------------------------------------------*/

uint8 Get_New_DvTrn_Start(void)
{
	return New_DvTrn_Start_End_st ;
}


/*----------------------------------------------------*/

void Set_Ini_Proc_Sched_st(uint8 ini_proc_sched_st)
{
	Init_sched_flag = ini_proc_sched_st;
}

/*----------------------------------------------------*/

uint8 Get_Ini_Proc_Sched_st(void)
{
	return Init_sched_flag ;
}

/*----------------------------------------------------*/


/*----------------------------------------------------*/



/*----------------------------------------------------*/





