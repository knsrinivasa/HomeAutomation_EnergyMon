
/***************************************************************************************************
 * Includes
 ***************************************************************************************************
*/

#include "main_inf.h"
#include "global_variables.h"
//#include "logging.h"

/***************************************************************************************************
 * Prototypes
 ***************************************************************************************************
*/


/***************************************************************************************************
 * Defines
 ***************************************************************************************************
*/

/***************************************************************************************************
 * Variables
 ***************************************************************************************************
*/

/*
 ============================================================================
 Name        : Steady_WT_mgr.c
 Author      : skn2kor
 Version     :
 Copyright   :
 Description : To decide Device status from both Steady_st and WT
 ============================================================================
 */


void DevTrn_ReIni_Proc(void)
{
uint8 re_ini_session  ;
uint8 i;
uint8 j;
uint16 Combi_BP   ;
uint8 trained_dv_cnt  ;

    re_ini_session = Get_StM_Sess()  ;
    trained_dv_cnt = Get_DvTrn_Cnt () ;

    if ( (re_ini_session == RE_INIT_SESSION ) && (trained_dv_cnt == DV_MAX_CT) )
//    if (init_flag == 0)                                 // steady_st_flag == INIT_PATH
    {
        /*----- Train the devices -----*/
//////	train_device(DV_MAX_CT, device_list, device_st);				// To train the devices
//////	set_device_status(device_list, device_st);					// To ON/OFF the devices
//////	set_dv_trigger(DV_MAX_CT, device_list, device_st);				// To generate trigger for any device

    #if( DV_DETECT_ALGO == DV_DETECT_STEADY_ST)      
          device_db(DV_MAX_CT, device_list, device_st);				        // Database of the devices
    #endif
      
          comb_size_max = pow(2,DV_MAX_CT) ;              // SKN: CHECK:  Dv_Cnt_Idx
//	comb_size_max = 8 ;                             // Srini:  Device dependent
        
      
      /*----- call for Compare sort -----*/
        comp_sort(DV_MAX_CT, device_list, device_st, DP);		// Based on PWR, Sort the Device List in ascending Order using Compare sort
  //	comp_sort(DV_MAX_CT, device_list, device_st, DQ);		// Based on VAR, Sort the Device List in ascending Order using Compare sort
          
/*-----  Create all combination array	-----*/

	for(i=0;i<comb_size_max;i++)
	{
		cmb_st[i].cmb_idx = i;
		j = i;
		Combi_BP=0;

		do
		{
			if(j&0x1)
			{
				cmb_st[i].cmb_p+=device_list[Combi_BP].pn ;
				cmb_st[i].cmb_q+=device_list[Combi_BP].qn ;
			}
			j>>=1 ;
			Combi_BP++;
		}while(j) ;
	}

/*-----	Sort Combination structure: cmb_st[]	-----*/

	comp_sort_combi(comb_size_max, cmb_st, DP);		// Based on PWR, Sort the Combination List in ascending Order using Compare sort
//	comp_sort_combi(comb_size_max, cmb_st, DQ);		// Based on VAR, Sort the Combination List in ascending Order using Compare sort
        
	MAX_PWR_CALC(&TP_MAX);
	MAX_VAR_CALC(&TQ_MAX);

#if (SVM == ENABLED)
  if(Dev_Cnt_Idx == SVM_SAMPL_CNT)
  {
	  main_learn() ;
	  SVM_learn_st = SVM_LEARNT ;
  }
#endif

        Set_StM_Sess(RUN_SESSION)  ;                            // SYSTEM Mode set to -> RE_INIT_SESSION  //  to indicate re-init session after adding new device
        
    }           //if(init_flag == 0)
     
  
}

/*----------------------------------------------------------------------------*/


void DevDet_merge_Proc(void)
{
  uint8 idx ;
  uint8 u8deviceStatus[DV_MAX_CT]={2};
  uint8   WT_DvDetect_st        ;
  uint8   Std_St_DvDetect_st    ;

  uint8 u8tempCnt ;
  
//  uint8 j ;
//  uint16  l=0             ;
//  uint8 wt_dvdet_p_st     ;
//  uint8 wt_dvdet_q_st     ;
//  uint8 steady_dvdet_st   ;
//  
//  wt_dvdet_p_st   =   Get_WT_DevDet_P_St()    ;
//  wt_dvdet_q_st   =   Get_WT_DevDet_Q_St()    ;
//  steady_dvdet_st =   Get_Steady_DevDet_St()  ;

/*  
  if ( (wt_dvdet_p_st==WT_DEVDET_P_END) && (wt_dvdet_q_st==WT_DEVDET_Q_END) && (steady_dvdet_st==STEADY_DEVDET_END) )     //  both WT & Steady_st are completed
  {
    
    //----- Check if the known devices status are in relation to the probability states
    
    for(i=0;i<DV_MAX_CT;i++)
    {
      // delete probability states of OFF devices if in probability array considered as ON
      if ( (device_st[i].trg & 0x04) && (device_st[i].trg & 0x02)  )
      {
        for(j=0; j<cmb_arr_size; j++)
        {
          if( cmb_st[cmb_arr[j]].cmb_idx & (1 << i) )
          {
            for (l=j; l<cmb_arr_size-1; l++)
            {
              if(cmb_arr_size == 1)
                cmb_arr[l] = 0 ;
              else
                cmb_arr[l] = cmb_arr[l+1];
            }
            if(l==cmb_arr_size-1)
            {
              cmb_arr[l] = 0;
              cmb_arr_size--;
            }
          }
        }
      }
      
      // delete probability states of ON devices if in probability array considered as OFF
      else if( (device_st[i].trg & 0x08 ) && (device_st[i].trg & 0x02 ) )
      {
        for(j=0; j<cmb_arr_size; j++)
        {
          if( !( cmb_st[cmb_arr[j]].cmb_idx & (1 << i) ) )
          {
            for (l=j; l<cmb_arr_size-1; l++)
            {
              if(cmb_arr_size == 1)
                cmb_arr[l] = 0 ;
              else
                cmb_arr[l] = cmb_arr[l+1];
            }
            if(l==cmb_arr_size-1)
            {
              cmb_arr[l] = 0;
              cmb_arr_size--;
            }
          }
        }
      }
    }
  }
*/

  
  WT_DvDetect_st = Get_RunSess_WT() ;    
  Std_St_DvDetect_st = Get_Steady_DevDet_St() ;
  
    if ( (WT_DvDetect_st == RUNSESS_WT_END) && (Std_St_DvDetect_st == STEADY_DEVDET_END) )     //  both WT & Steady_st are completed
    {
      for(idx=0; idx<Dev_Cnt_Idx; idx++)
      {
        if ( device_st[idx].trg == TRIGGERED  )
        {
          
          device_st[idx].final_status = device_st[idx].wt_status  ;
          
        }       //  if ( device_st[idx].trg == TRIGGERED )
        
        else  if ( device_st[idx].trg == UNTRIGGERED  )
        {
          device_st[idx].final_status = device_st[idx].status  ;
          
        }       //      else  if ( device_st[idx].trg = UNTRIGGERED  )
        
      }         //  for(idx=0; idx<dev_cnt; idx++)
  
      // For logging purpose    
      for(u8tempCnt=0; u8tempCnt<Dev_Cnt_Idx; u8tempCnt++)
      { 
        if (device_st[u8tempCnt].final_status == 0x06)
          u8deviceStatus[u8tempCnt]=0;  // Device ON
        else if (device_st[u8tempCnt].final_status == 0x0A)
          u8deviceStatus[u8tempCnt]=1;  // Device OFF
        else if (device_st[u8tempCnt].final_status == 0x01)
          u8deviceStatus[u8tempCnt]=2;  // Unresolved           
      }

#if (LOGGING==ENABLED)
    LOG(finalState,"%d %d %d %d %d \n",u32iterationCnt,u8deviceStatus[0],u8deviceStatus[1],u8deviceStatus[2],u8deviceStatus[3]);
#endif
  
    }  //   if ( (wt_dvdet_p_st==WT_DEVDET_P_END) && (wt_dvdet_q_st==WT_DEVDET_Q_END) && (steady_dvdet_st==STEADY_DEVDET_END) )     //  both WT & Steady_st are completed
  

}


/*=================================================================================================*/


