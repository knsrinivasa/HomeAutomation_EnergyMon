
/*
 ============================================================================
 Name        : feature_extract.c
 Author      : skn2kor
 Created on	 : Apr 24, 2013
 Version     :
 Copyright   :
 Description :	To calculate:
				Mean
				Min
				Max
				Range
				Median
				Std_Dev
				Variance
				Skewness
				Kurtosis
 ============================================================================
 */
#include "main_inf.h"
#include "global_variables.h"
//#include "logging.h"

#define SIMU_FEATURE_DEBUG_PRINTF 	DISABLED

WT_VAR_DATA_TYPE _abs_d(WT_VAR_DATA_TYPE _j)
{
  return (_j >= 0 ? _j : -_j);
}


/*------------------------------------------------------------------------------*/

uint8 feature_extract(uint32 *input_buff, uint8 session, uint16 dv_idx, uint8 transition_dir, uint8 Clas_fld)
{
//	WT_VAR_DATA_TYPE 		x 			= 0	;
	WT_VAR_DATA_TYPE		sum 		= 0	;
	WT_VAR_DATA_TYPE  	    Dev 		= 0	;
	WT_VAR_DATA_TYPE		factor 		= 0	;
  
	uint32  	temp = 0 ;
	uint16      num, i,j;

  // For log purpose
  uint8 u8devDirection;
    
  if(transition_dir==OFF_TO_ON)
    u8devDirection =1;
  else if(transition_dir==ON_TO_OFF)
    u8devDirection=0;
  
  
  //	WT_VAR_DATA_TYPE		Mean 		= 0	;
  //	WT_VAR_DATA_TYPE		Min 		= 0	;
  //	WT_VAR_DATA_TYPE		Max 		= 0	;
  //	WT_VAR_DATA_TYPE		Range 		= 0	;
  //	WT_VAR_DATA_TYPE		Median 		= 0	;
  //	WT_VAR_DATA_TYPE		Std_Dev 	= 0	;
  //	WT_VAR_DATA_TYPE		Variance	= 0	;
  //	WT_VAR_DATA_TYPE		Skewness	= 0	;
  //	WT_VAR_DATA_TYPE		Kurtosis	= 0	;
  
  
  Sign_feature	        calc_Sign_feature  = { 0 } ;
  
  num = COEFF_CMP_ARR_SIZE;		// RINGBUFF_SIZE
  
//  Shell_Printf ("In feature_extract\n");
//	for(i=0; i<num; i++)
//		printf("input_buff[%d] = %d\nPress any key ...\n", i, input_buff[i] ) ;
//	getch();

  //#if (SKN_FEA_EXT_TEST == 1)
  //	Shell_Printf ("In feature_extract\n");
  //#endif
  
  //  file_read_1( &input_buff, &num );
  //
  //	printf("\nthe number of input terms =\t%d\n\n", num);
  
  printf("\n_________________________________________________________\n") ;
  (Clas_fld == PWR)
	?
		  ( (session == TRAIN_SESSION) ? ( printf("Feature in Training Session for PWR\n") ) : ( printf("Feature in Run Session for PWR\n") ) )
	:
		  ( (session == TRAIN_SESSION) ? ( printf("Feature in Training Session for VAR\n") ) : ( printf("Feature in Run Session for VAR\n") ) )
	;

  /*---------*/
	sum = 0;
	for(i=0; i<num; i++)
	{
		sum+= input_buff[i];
	}
	calc_Sign_feature.Mean = sum/num;

	printf("\t\tMean =\t\t\t\t%f\n", calc_Sign_feature.Mean);

/*---------*/

    for(i=0; i<num-1; i++)
    {
        for(j=i+1; j<num; j++)
        {
            if(input_buff[j] < input_buff[i])
            {
//                // swap elements
//                temp = input_buff[i];
//                input_buff[i] = input_buff[j];
//                input_buff[j] = temp;

                input_buff[i]^=input_buff[j]^=input_buff[i]^= input_buff[j] ;
                
            }
        }
    }


/*---------*/
    
    calc_Sign_feature.Min = input_buff[0] ;
//    calc_Sign_feature.Max = input_buff[num-1];

    if ( Clas_fld == PWR )
    {
      if (session == TRAIN_SESSION)
        calc_Sign_feature.Max = Train_Event_dPn;                                  // SKN  test   
      else if (session == RUN_SESSION)    
        calc_Sign_feature.Max = Runtime_Event_dPn;                                  // SKN  test
    }
    else if ( Clas_fld == VAR )
    {
      if (session == TRAIN_SESSION)
        calc_Sign_feature.Max = Train_Event_dQn;                                  // SKN  test   
      else if (session == RUN_SESSION)    
        calc_Sign_feature.Max = Runtime_Event_dQn;                                  // SKN  test
    }
    
    printf("\t\tMin =\t\t\t\t%f\n\t\tMax =\t\t\t\t%f\n", calc_Sign_feature.Min, calc_Sign_feature.Max);
    // printf("Min =\t\t\t\t%lf\nMax =\t\t\t\t%lf\n",Min,Max);

    calc_Sign_feature.Range = calc_Sign_feature.Max - calc_Sign_feature.Min ;

    printf("\t\tRange =\t\t\t\t%f\n", calc_Sign_feature.Range);
// printf("Range =\t\t\t\t%lf\n",Range);

/*---------*/

    if(num%2==0)
    {
        // if there is an even number of elements, return mean of the two elements in the middle
       calc_Sign_feature.Median = ((input_buff[num/2] + input_buff[num/2 - 1]) / 2.0);
    }
    else
    {
        // else return the element in the middle
       calc_Sign_feature.Median =  input_buff[num/2];
    }
    printf("\t\tMedian =\t\t\t%f\n", calc_Sign_feature.Median);
//	printf("Median =\t\t\t%f\n", Median);

/*---------*/

//calculating Std_Dev
	sum = 0;
	for (i=0; i<num; i++)
	{
		Dev = input_buff[i] - calc_Sign_feature.Mean;
		sum+= Dev * Dev;
	}
  
	calc_Sign_feature.Variance = sum / num;
	calc_Sign_feature.Std_Dev = sqrt(calc_Sign_feature.Variance);     //standard deviation
  
    printf("\t\tStd_Dev =\t\t\t%f\n", calc_Sign_feature.Std_Dev);
    printf("\t\tVariance =\t\t\t%f\n", calc_Sign_feature.Variance);
  //	printf("Standard Deviation =\t\t%f\n", Std_Dev);
  //	printf("Variance =\t\t\t%f\n", Variance);
  
  /*---------*/
  
  calc_Sign_feature.Skewness = 0;
  calc_Sign_feature.Kurtosis = 0;
  
	for(i=0;i<num;i++)
	{
          if(calc_Sign_feature.Std_Dev != 0)
            factor = ( (input_buff[i]-calc_Sign_feature.Mean) / calc_Sign_feature.Std_Dev ) ;
          else
            factor = 1;
          
		calc_Sign_feature.Skewness+= pow(factor,3)/(num-1);         //  NUM_MAX-1
		calc_Sign_feature.Kurtosis+= pow(factor,4)/(num-1);         //  NUM_MAX-1
	}
    printf("\t\tSkewness =\t\t\t%f\n", calc_Sign_feature.Skewness);
    printf("\t\tKurtosis =\t\t\t%f\n", calc_Sign_feature.Kurtosis);
  //	printf("Skewness =\t\t\t%f\nkurtosis =\t\t\t%f\n\n", Skewness, Kurtosis );
  
  /*---------*/
  
	if (session == TRAIN_SESSION)
	{
    if ( Clas_fld == PWR )
      Train_temp_feature_P	=	calc_Sign_feature	;
    else if ( Clas_fld == VAR )
      Train_temp_feature_Q	=	calc_Sign_feature	;

#if (SIMU_FEATURE_DEBUG_PRINTF == ENABLED)
    printf("In training session, calc feature assigned to train temp feature structure\nPress any key...\n") ;
    getch() ;
#endif

	}
	else if (session == RUN_SESSION)
	{
#if (SIMU_FEATURE_DEBUG_PRINTF == ENABLED)
		printf("In Run session, calc feature assigned to Run feature structure for clas_fld = %d\nPress any key...\n", Clas_fld) ;
		getch() ;
#endif

    if ( Clas_fld == PWR )
    {
      Runtime_feature_P	=	calc_Sign_feature	;
#if (LOGGING==ENABLED)
      LOG(EventLog_P,"%d %d %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f\n",
          u32iterationCnt ,
          u8devDirection,
          Runtime_feature_P.Mean,
          Runtime_feature_P.Min,
          Runtime_feature_P.Max,
          Runtime_feature_P.Range,
          Runtime_feature_P.Median,
          Runtime_feature_P.Std_Dev,
          Runtime_feature_P.Variance,
          Runtime_feature_P.Skewness,
          Runtime_feature_P.Kurtosis
            ) ;
#endif
      
    }
    else if ( Clas_fld == VAR )
    {
      Runtime_feature_Q	=	calc_Sign_feature	;
#if (LOGGING==ENABLED)
      LOG(EventLog_Q,"%d %d %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f\n",  
          u32iterationCnt ,
          u8devDirection,
          Runtime_feature_Q.Mean,
          Runtime_feature_Q.Min,
          Runtime_feature_Q.Max,
          Runtime_feature_Q.Range,
          Runtime_feature_Q.Median,
          Runtime_feature_Q.Std_Dev,
          Runtime_feature_Q.Variance,
          Runtime_feature_Q.Skewness,
          Runtime_feature_Q.Kurtosis
            ) ;
#endif
    }
    
	}
  
	return 1;
}


/*
============================================================================
Name        : feature_extract.c
Author      : skn2kor
Created on  : Apr 24, 2013
Version     :
Copyright   :
Description :	To Store Features to device structure:
============================================================================
*/

void Store_Device_features(uint8 dv_idx)
{
  uint8 u8dvDirection;

#if (SIMU_FEATURE_DEBUG_PRINTF == ENABLED)
	printf("In Train session, train feature structure stored after accept\nPress any key...\n") ;
	getch() ;
#endif

  if (DvTrn_trans_dir == OFF_TO_ON)
  {
    device_list[dv_idx].P_feature_OFF_TO_ON	=	Train_temp_feature_P	;
    device_list[dv_idx].Q_feature_OFF_TO_ON	=	Train_temp_feature_Q	;
    
    // Train_ON_OFF_Flag = 1 ;
    Set_StM_DvTrn_2Sig_Incr (1) ;
    u8dvDirection =1;

#if (LOGGING==ENABLED)
    LOG(TrainLog_P,"%d %d %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f\n",
        u32iterationCnt ,
        u8dvDirection,
        device_list[dv_idx].P_feature_OFF_TO_ON.Mean,
        device_list[dv_idx].P_feature_OFF_TO_ON.Min,
        device_list[dv_idx].P_feature_OFF_TO_ON.Max,
        device_list[dv_idx].P_feature_OFF_TO_ON.Range,
        device_list[dv_idx].P_feature_OFF_TO_ON.Median,
        device_list[dv_idx].P_feature_OFF_TO_ON.Std_Dev,
        device_list[dv_idx].P_feature_OFF_TO_ON.Variance,
        device_list[dv_idx].P_feature_OFF_TO_ON.Skewness,
        device_list[dv_idx].P_feature_OFF_TO_ON.Kurtosis
          ) ;
    
    LOG(TrainLog_Q,"%d %d %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f\n",  
        u32iterationCnt,
        u8dvDirection,
        device_list[dv_idx].Q_feature_OFF_TO_ON.Mean,
        device_list[dv_idx].Q_feature_OFF_TO_ON.Min,
        device_list[dv_idx].Q_feature_OFF_TO_ON.Max,
        device_list[dv_idx].Q_feature_OFF_TO_ON.Range,
        device_list[dv_idx].Q_feature_OFF_TO_ON.Median,
        device_list[dv_idx].Q_feature_OFF_TO_ON.Std_Dev,
        device_list[dv_idx].Q_feature_OFF_TO_ON.Variance,
        device_list[dv_idx].Q_feature_OFF_TO_ON.Skewness,
        device_list[dv_idx].Q_feature_OFF_TO_ON.Kurtosis
          ) ;
#endif
    
  }
  
  else if (DvTrn_trans_dir == ON_TO_OFF)
  {
    device_list[dv_idx].P_feature_ON_TO_OFF	=	Train_temp_feature_P	;
    device_list[dv_idx].Q_feature_ON_TO_OFF	=	Train_temp_feature_Q	;
    
    u8dvDirection =0;
    // Train_ON_OFF_Flag = 2 ;
    Set_StM_DvTrn_2Sig_Incr (1) ;

#if (LOGGING==ENABLED)
    LOG(TrainLog_P,"%d %d %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f\n",
        u32iterationCnt ,
        u8dvDirection,
        device_list[dv_idx].P_feature_ON_TO_OFF.Mean,
        device_list[dv_idx].P_feature_ON_TO_OFF.Min,
        device_list[dv_idx].P_feature_ON_TO_OFF.Max,
        device_list[dv_idx].P_feature_ON_TO_OFF.Range,
        device_list[dv_idx].P_feature_ON_TO_OFF.Median,
        device_list[dv_idx].P_feature_ON_TO_OFF.Std_Dev,
        device_list[dv_idx].P_feature_ON_TO_OFF.Variance,
        device_list[dv_idx].P_feature_ON_TO_OFF.Skewness,
        device_list[dv_idx].P_feature_ON_TO_OFF.Kurtosis
          ) ;
    
    LOG(TrainLog_Q,"%d %d %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f %.5f\n",  
        u32iterationCnt ,
        u8dvDirection,
        device_list[dv_idx].Q_feature_ON_TO_OFF.Mean,
        device_list[dv_idx].Q_feature_ON_TO_OFF.Min,
        device_list[dv_idx].Q_feature_ON_TO_OFF.Max,
        device_list[dv_idx].Q_feature_ON_TO_OFF.Range,
        device_list[dv_idx].Q_feature_ON_TO_OFF.Median,
        device_list[dv_idx].Q_feature_ON_TO_OFF.Std_Dev,
        device_list[dv_idx].Q_feature_ON_TO_OFF.Variance,
        device_list[dv_idx].Q_feature_ON_TO_OFF.Skewness,
        device_list[dv_idx].Q_feature_ON_TO_OFF.Kurtosis
          ) ;
#endif
  }
  
}
/*
==================================================================================================
Name        : feature_extract.c
Author      : skn2kor
Created on  : Apr 24, 2013
Version     :
Copyright   :
Description :	To Match Features of device structure to runtime features on event detection
==================================================================================================
*/

#define   IDX_FLD     0  
#define   PWR_FLD    1
#define   VAR_FLD    2

#define   FEATURE_PERCENTAGE_MATCH_ARR_SIZE     3
#define   PERCENTAGE_MAX_MATCH_RESOLUTION       30

uint8 feature_match(uint8 dev_cnt, uint8 dir, uint8 *Device_Detected)
{
  
  float	Runtime_Mean_Tol_P	;
  float	Runtime_Min_Tol_P	;
  float	Runtime_Max_Tol_P	;
  float	Runtime_Range_Tol_P	;
  float	Runtime_Median_Tol_P	;
  float	Runtime_Std_Dev_Tol_P	;
  float	Runtime_Variance_Tol_P	;
  float	Runtime_Skewness_Tol_P	;
  float	Runtime_Kurtosis_Tol_P	;

  float	Runtime_Mean_Tol_Q	;
  float	Runtime_Min_Tol_Q	;
  float	Runtime_Max_Tol_Q	;
  float	Runtime_Range_Tol_Q	;
  float	Runtime_Median_Tol_Q	;
  float	Runtime_Std_Dev_Tol_Q	;
  float	Runtime_Variance_Tol_Q	;
  float	Runtime_Skewness_Tol_Q	;
  float	Runtime_Kurtosis_Tol_Q	;
  
  static uint8         match_wieghtage_P	=	ZERO	;
  static uint8         match_wieghtage_Q	=	ZERO	;
  static uint8         dv_wieghtage_P	    =	ZERO	;
  static uint8         dv_wieghtage_Q	    =	ZERO	;
  static uint8         dv_index	            =	ZERO	;
  static uint8         idx		        	=	ZERO	;

  static uint16         feature_arr[DV_MAX_CT][FEATURE_PERCENTAGE_MATCH_ARR_SIZE]       ;
  uint8         u8eventFlag;
  uint8         u8eventNumber;
  uint8         u8dveventCnt =0;
  uint8         u8sizeOfStruct;
  uint8         u8deviceStatus[DV_MAX_CT]={2};
  Sign_feature  DeviceFeature_P_st[DV_MAX_CT]={0};
  Sign_feature  DeviceFeature_Q_st[DV_MAX_CT]={0};  

  static float         Runtime_Pn_Tol                   ;
  static float         Runtime_Qn_Tol                   ;
  static float         Runtime_dPn_Tol                  ;
  static float         Runtime_dQn_Tol                  ;

  static uint8         Det_st_Flag = 0                  ;
  uint8                i, j                     	;  
  uint8 u8tempCnt ;
  uint8 u8tempcnt ;
  
#if (SIMU_FEATURE_DEBUG_PRINTF == ENABLED)
  printf("In match feature function with dev_cnt = %d\nPress any key ...\n", dev_cnt) ;
  getch() ;
#endif

        match_wieghtage_P	=	ZERO	;
        match_wieghtage_Q	=	ZERO	;
        
        dv_index	        =	ZERO	;
        dv_wieghtage_P	        =	ZERO	;
        dv_wieghtage_Q	        =	ZERO	;
  
        u8eventFlag	        =	ZERO	;
        u8dveventCnt =  0 ;  
        
	for(idx=0; idx<dev_cnt; idx++)
	{

          match_wieghtage_P	= ZERO	;
          match_wieghtage_Q	= ZERO	;
          
          
          Runtime_Pn_Tol = Mul_Non_Zero_Ret (Runtime_Event_Pn, EVENT_FEATURE_PN_THRES_C)	;
          if ( Comp_WT_Feature(Runtime_Event_Pn, device_list[idx].pn, Runtime_Pn_Tol)  )
            match_wieghtage_P+= INS_PWR_WIGHTAGE;
          
          Runtime_Qn_Tol = Mul_Non_Zero_Ret (Runtime_Event_Qn, EVENT_FEATURE_QN_THRES_C)	;
          if ( Comp_WT_Feature(Runtime_Event_Qn, device_list[idx].qn, Runtime_Qn_Tol)  )
            match_wieghtage_Q+= INS_VAR_WIGHTAGE;
          
          Runtime_dPn_Tol = Mul_Non_Zero_Ret (Runtime_Event_dPn, EVENT_FEATURE_dPN_THRES_C)	;
          if ( Comp_WT_Feature(Runtime_Event_dPn, device_list[idx].pn, Runtime_dPn_Tol)  )
            match_wieghtage_P+= DELTA_PWR_WIGHTAGE;
          
          Runtime_dQn_Tol = Mul_Non_Zero_Ret (Runtime_Event_dQn, EVENT_FEATURE_dQN_THRES_C)	;
          if ( Comp_WT_Feature(Runtime_Event_dQn, device_list[idx].qn, Runtime_dQn_Tol)  )
            match_wieghtage_Q+= DELTA_VAR_WIGHTAGE;
          
          
          if( dir == OFF_TO_ON )
          {
      /*--------- P Features --------- */
      
      Runtime_Mean_Tol_P = Runtime_feature_P.Mean * RUNTIME_MEAN_TOL_OFF_TO_ON_P ;
      if(Comp_WT_Feature( Runtime_feature_P.Mean, device_list[idx].P_feature_OFF_TO_ON.Mean, Runtime_Mean_Tol_P ) )
      {              
        match_wieghtage_P+= MEAN_WIGHTAGE_P;
        DeviceFeature_P_st[idx].Mean =MEAN_WIGHTAGE_P; 
      }
      Runtime_Min_Tol_P = Runtime_feature_P.Min * RUNTIME_MIN_TOL_OFF_TO_ON_P ;  
      if(Comp_WT_Feature( Runtime_feature_P.Min, device_list[idx].P_feature_OFF_TO_ON.Min, Runtime_Min_Tol_P ) )
      {
        match_wieghtage_P+= MIN_WIGHTAGE_P;
        DeviceFeature_P_st[idx].Min =MIN_WIGHTAGE_P;              
      }
      
      Runtime_Max_Tol_P = Runtime_feature_P.Max * RUNTIME_MAX_TOL_OFF_TO_ON_P ;            
      if(Comp_WT_Feature( Runtime_feature_P.Max, device_list[idx].P_feature_OFF_TO_ON.Max, Runtime_Max_Tol_P ) )
      {
        match_wieghtage_P+= MAX_WIGHTAGE_P+30;
        DeviceFeature_P_st[idx].Max =MAX_WIGHTAGE_P+30;
      }              
      
      Runtime_Range_Tol_P = Runtime_feature_P.Range * RUNTIME_RANGE_TOL_OFF_TO_ON_P ;            
      if(Comp_WT_Feature( Runtime_feature_P.Range, device_list[idx].P_feature_OFF_TO_ON.Range, Runtime_Range_Tol_P ) )
      {
        match_wieghtage_P+= RANGE_WIGHTAGE_P+30;
        DeviceFeature_P_st[idx].Range =RANGE_WIGHTAGE_P+30;
      }              
      
      Runtime_Median_Tol_P = Runtime_feature_P.Median * RUNTIME_MEDIAN_TOL_OFF_TO_ON_P ;            
      if(Comp_WT_Feature( Runtime_feature_P.Median, device_list[idx].P_feature_OFF_TO_ON.Median, Runtime_Median_Tol_P ) )
      {
        match_wieghtage_P+= MEDIAN_WIGHTAGE_P;
        DeviceFeature_P_st[idx].Median =MEDIAN_WIGHTAGE_P;
      }              
      
      Runtime_Std_Dev_Tol_P = Runtime_feature_P.Std_Dev * RUNTIME_STD_DEV_TOL_OFF_TO_ON_P ;            
      if(Comp_WT_Feature( Runtime_feature_P.Std_Dev, device_list[idx].P_feature_OFF_TO_ON.Std_Dev, Runtime_Std_Dev_Tol_P ) )
      {
        match_wieghtage_P+= STD_DEV_WIGHTAGE_P;
        DeviceFeature_P_st[idx].Std_Dev =STD_DEV_WIGHTAGE_P;
      }              
      
      Runtime_Variance_Tol_P = Runtime_feature_P.Variance * RUNTIME_VARIANCE_TOL_OFF_TO_ON_P ;            
      if(Comp_WT_Feature( Runtime_feature_P.Variance, device_list[idx].P_feature_OFF_TO_ON.Variance, Runtime_Variance_Tol_P ) )
      {
        match_wieghtage_P+= VARIANCE_WIGHTAGE_P;
        DeviceFeature_P_st[idx].Variance =VARIANCE_WIGHTAGE_P;
      }              
      
      Runtime_Skewness_Tol_P = Runtime_feature_P.Skewness * RUNTIME_SKEWNESS_TOL_OFF_TO_ON_P ;            
      if(Comp_WT_Feature( Runtime_feature_P.Skewness, device_list[idx].P_feature_OFF_TO_ON.Skewness, Runtime_Skewness_Tol_P ) )
      {
        match_wieghtage_P+= SKEWNESS_WIGHTAGE_P;
        DeviceFeature_P_st[idx].Skewness =SKEWNESS_WIGHTAGE_P;
      }              
      
      Runtime_Kurtosis_Tol_P = Runtime_feature_P.Kurtosis * RUNTIME_KURTOSIS_TOL_OFF_TO_ON_P ;            
      if(Comp_WT_Feature( Runtime_feature_P.Kurtosis, device_list[idx].P_feature_OFF_TO_ON.Kurtosis, Runtime_Kurtosis_Tol_P ) )
      {
        match_wieghtage_P+= KURTOSIS_WIGHTAGE_P;
        DeviceFeature_P_st[idx].Kurtosis =KURTOSIS_WIGHTAGE_P;
      }              
      
      /*--------- Q Features --------- */
      
      Runtime_Mean_Tol_Q = Runtime_feature_Q.Mean * RUNTIME_MEAN_TOL_OFF_TO_ON_Q ;
      if(Comp_WT_Feature( Runtime_feature_Q.Mean, device_list[idx].Q_feature_OFF_TO_ON.Mean, Runtime_Mean_Tol_Q ) )
      {
        match_wieghtage_Q+= MEAN_WIGHTAGE_Q;
        DeviceFeature_Q_st[idx].Mean =MEAN_WIGHTAGE_Q;               
      }
      
      Runtime_Min_Tol_Q = Runtime_feature_Q.Min * RUNTIME_MIN_TOL_OFF_TO_ON_Q ;  
      if(Comp_WT_Feature( Runtime_feature_Q.Min, device_list[idx].Q_feature_OFF_TO_ON.Min, Runtime_Min_Tol_Q ) )
      {
        match_wieghtage_Q+= MIN_WIGHTAGE_Q;
        DeviceFeature_Q_st[idx].Min =MIN_WIGHTAGE_Q;               
      }              
      
      Runtime_Max_Tol_Q = Runtime_feature_Q.Max * RUNTIME_MAX_TOL_OFF_TO_ON_Q ;            
      if(Comp_WT_Feature( Runtime_feature_Q.Max, device_list[idx].Q_feature_OFF_TO_ON.Max, Runtime_Max_Tol_Q ) )
      {
        match_wieghtage_Q+= MAX_WIGHTAGE_Q+30;
        DeviceFeature_Q_st[idx].Max =MAX_WIGHTAGE_Q+30;
      }              
      
      Runtime_Range_Tol_Q = Runtime_feature_Q.Range * RUNTIME_RANGE_TOL_OFF_TO_ON_Q ;            
      if(Comp_WT_Feature( Runtime_feature_Q.Range, device_list[idx].Q_feature_OFF_TO_ON.Range, Runtime_Range_Tol_Q ) )
      {
        match_wieghtage_Q+= RANGE_WIGHTAGE_Q+30;
        DeviceFeature_Q_st[idx].Range =RANGE_WIGHTAGE_Q+30;
      }              
      
      Runtime_Median_Tol_Q = Runtime_feature_Q.Median * RUNTIME_MEDIAN_TOL_OFF_TO_ON_Q ;            
      if(Comp_WT_Feature( Runtime_feature_Q.Median, device_list[idx].Q_feature_OFF_TO_ON.Median, Runtime_Median_Tol_Q ) )
      {
        match_wieghtage_Q+= MEDIAN_WIGHTAGE_Q;
        DeviceFeature_Q_st[idx].Median = MEDIAN_WIGHTAGE_Q;
      }              
      
      Runtime_Std_Dev_Tol_Q = Runtime_feature_Q.Std_Dev * RUNTIME_STD_DEV_TOL_OFF_TO_ON_Q ;            
      if(Comp_WT_Feature( Runtime_feature_Q.Std_Dev, device_list[idx].Q_feature_OFF_TO_ON.Std_Dev, Runtime_Std_Dev_Tol_Q ) )
      {
        match_wieghtage_Q+= STD_DEV_WIGHTAGE_Q;
        DeviceFeature_Q_st[idx].Std_Dev = STD_DEV_WIGHTAGE_Q;              
      }
      
      Runtime_Variance_Tol_Q = Runtime_feature_Q.Variance * RUNTIME_VARIANCE_TOL_OFF_TO_ON_Q ;            
      if(Comp_WT_Feature( Runtime_feature_Q.Variance, device_list[idx].Q_feature_OFF_TO_ON.Variance, Runtime_Variance_Tol_Q ) )
      {
        match_wieghtage_Q+= VARIANCE_WIGHTAGE_Q;
        DeviceFeature_Q_st[idx].Variance = VARIANCE_WIGHTAGE_Q;
      }              
      
      Runtime_Skewness_Tol_Q = Runtime_feature_Q.Skewness * RUNTIME_SKEWNESS_TOL_OFF_TO_ON_Q ;            
      if(Comp_WT_Feature( Runtime_feature_Q.Skewness, device_list[idx].Q_feature_OFF_TO_ON.Skewness, Runtime_Skewness_Tol_Q ) )
      {
        match_wieghtage_Q+= SKEWNESS_WIGHTAGE_Q;
        DeviceFeature_Q_st[idx].Skewness = SKEWNESS_WIGHTAGE_Q;
      }              
      
      Runtime_Kurtosis_Tol_Q = Runtime_feature_Q.Kurtosis * RUNTIME_KURTOSIS_TOL_OFF_TO_ON_Q ;            
      if(Comp_WT_Feature( Runtime_feature_Q.Kurtosis, device_list[idx].Q_feature_OFF_TO_ON.Kurtosis, Runtime_Kurtosis_Tol_Q ) )
      {
        match_wieghtage_Q+= KURTOSIS_WIGHTAGE_Q;
        DeviceFeature_Q_st[idx].Kurtosis = KURTOSIS_WIGHTAGE_Q;
      }              
      
    }
    
    else if( dir == ON_TO_OFF )
    {
      
      /*--------- P Features --------- */
      
      Runtime_Mean_Tol_P = Runtime_feature_P.Mean * RUNTIME_MEAN_TOL_ON_TO_OFF_P ;
      if(Comp_WT_Feature( Runtime_feature_P.Mean, device_list[idx].P_feature_ON_TO_OFF.Mean, Runtime_Mean_Tol_P ) )             
      {              
        match_wieghtage_P+= MEAN_WIGHTAGE_P;
        DeviceFeature_P_st[idx].Mean =MEAN_WIGHTAGE_P; 
      }            
      
      Runtime_Min_Tol_P = Runtime_feature_P.Min * RUNTIME_MIN_TOL_ON_TO_OFF_P ;  
      if(Comp_WT_Feature( Runtime_feature_P.Min, device_list[idx].P_feature_ON_TO_OFF.Min, Runtime_Min_Tol_P ) )
      {
        match_wieghtage_P+= MIN_WIGHTAGE_P;
        DeviceFeature_P_st[idx].Min =MIN_WIGHTAGE_P;              
      }            
      Runtime_Max_Tol_P = Runtime_feature_P.Max * RUNTIME_MAX_TOL_ON_TO_OFF_P ;            
      if(Comp_WT_Feature( Runtime_feature_P.Max, device_list[idx].P_feature_ON_TO_OFF.Max, Runtime_Max_Tol_P ) )
      {
        match_wieghtage_P+= MAX_WIGHTAGE_P+30;
        DeviceFeature_P_st[idx].Max =MAX_WIGHTAGE_P+30;
      } 
      
      Runtime_Range_Tol_P = Runtime_feature_P.Range * RUNTIME_RANGE_TOL_ON_TO_OFF_P ;            
      if(Comp_WT_Feature( Runtime_feature_P.Range, device_list[idx].P_feature_ON_TO_OFF.Range, Runtime_Range_Tol_P ) )
      {
        match_wieghtage_P+= RANGE_WIGHTAGE_P+30;
        DeviceFeature_P_st[idx].Range =RANGE_WIGHTAGE_P+30;
      }  
      
      Runtime_Median_Tol_P = Runtime_feature_P.Median * RUNTIME_MEDIAN_TOL_ON_TO_OFF_P ;            
      if(Comp_WT_Feature( Runtime_feature_P.Median, device_list[idx].P_feature_ON_TO_OFF.Median, Runtime_Median_Tol_P ) )
      {
        match_wieghtage_P+= MEDIAN_WIGHTAGE_P;
        DeviceFeature_P_st[idx].Median =MEDIAN_WIGHTAGE_P;
      }              
      
      Runtime_Std_Dev_Tol_P = Runtime_feature_P.Std_Dev * RUNTIME_STD_DEV_TOL_ON_TO_OFF_P ;            
      if(Comp_WT_Feature( Runtime_feature_P.Std_Dev, device_list[idx].P_feature_ON_TO_OFF.Std_Dev, Runtime_Std_Dev_Tol_P ) )
      {
        match_wieghtage_P+= STD_DEV_WIGHTAGE_P;
        DeviceFeature_P_st[idx].Std_Dev =STD_DEV_WIGHTAGE_P;
      }              
      
      Runtime_Variance_Tol_P = Runtime_feature_P.Variance * RUNTIME_VARIANCE_TOL_ON_TO_OFF_P ;            
      if(Comp_WT_Feature( Runtime_feature_P.Variance, device_list[idx].P_feature_ON_TO_OFF.Variance, Runtime_Variance_Tol_P ) )
      {
        match_wieghtage_P+= VARIANCE_WIGHTAGE_P;
        DeviceFeature_P_st[idx].Variance =VARIANCE_WIGHTAGE_P;
      }              
      
      Runtime_Skewness_Tol_P = Runtime_feature_P.Skewness * RUNTIME_SKEWNESS_TOL_ON_TO_OFF_P ;            
      if(Comp_WT_Feature( Runtime_feature_P.Skewness, device_list[idx].P_feature_ON_TO_OFF.Skewness, Runtime_Skewness_Tol_P ) )
      {
        match_wieghtage_P+= SKEWNESS_WIGHTAGE_P;
        DeviceFeature_P_st[idx].Skewness =SKEWNESS_WIGHTAGE_P;
      }              
      
      Runtime_Kurtosis_Tol_P = Runtime_feature_P.Kurtosis * RUNTIME_KURTOSIS_TOL_ON_TO_OFF_P ;            
      if(Comp_WT_Feature( Runtime_feature_P.Kurtosis, device_list[idx].P_feature_ON_TO_OFF.Kurtosis, Runtime_Kurtosis_Tol_P ) )
      {
        match_wieghtage_P+= KURTOSIS_WIGHTAGE_P;
        DeviceFeature_P_st[idx].Kurtosis =KURTOSIS_WIGHTAGE_P;
      }              
      
      /*--------- Q Features --------- */
      
      Runtime_Mean_Tol_Q = Runtime_feature_Q.Mean * RUNTIME_MEAN_TOL_ON_TO_OFF_Q ;
      if(Comp_WT_Feature( Runtime_feature_Q.Mean, device_list[idx].Q_feature_ON_TO_OFF.Mean, Runtime_Mean_Tol_Q ) )
      {
        match_wieghtage_Q+= MEAN_WIGHTAGE_Q;
        DeviceFeature_Q_st[idx].Mean =MEAN_WIGHTAGE_Q;               
      }
      Runtime_Min_Tol_Q = Runtime_feature_Q.Min * RUNTIME_MIN_TOL_ON_TO_OFF_Q ;  
      if(Comp_WT_Feature( Runtime_feature_Q.Min, device_list[idx].Q_feature_ON_TO_OFF.Min, Runtime_Min_Tol_Q ) )
      {
        match_wieghtage_Q+= MIN_WIGHTAGE_Q;
        DeviceFeature_Q_st[idx].Min =MIN_WIGHTAGE_Q;               
      }              
      
      Runtime_Max_Tol_Q = Runtime_feature_Q.Max * RUNTIME_MAX_TOL_ON_TO_OFF_Q ;            
      if(Comp_WT_Feature( Runtime_feature_Q.Max, device_list[idx].Q_feature_ON_TO_OFF.Max, Runtime_Max_Tol_Q ) )
      {
        match_wieghtage_Q+= MAX_WIGHTAGE_Q+30;
        DeviceFeature_Q_st[idx].Max =MAX_WIGHTAGE_Q+30;
      }              
      
      Runtime_Range_Tol_Q = Runtime_feature_Q.Range * RUNTIME_RANGE_TOL_ON_TO_OFF_Q ;            
      if(Comp_WT_Feature( Runtime_feature_Q.Range, device_list[idx].Q_feature_ON_TO_OFF.Range, Runtime_Range_Tol_Q ) )
      {
        match_wieghtage_Q+= RANGE_WIGHTAGE_Q+30;
        DeviceFeature_Q_st[idx].Range =RANGE_WIGHTAGE_Q+30;
      }              
      
      Runtime_Median_Tol_Q = Runtime_feature_Q.Median * RUNTIME_MEDIAN_TOL_ON_TO_OFF_Q ;            
      if(Comp_WT_Feature( Runtime_feature_Q.Median, device_list[idx].Q_feature_ON_TO_OFF.Median, Runtime_Median_Tol_Q ) )
      {
        match_wieghtage_Q+= MEDIAN_WIGHTAGE_Q;
        DeviceFeature_Q_st[idx].Median = MEDIAN_WIGHTAGE_Q;
      }              
      
      Runtime_Std_Dev_Tol_Q = Runtime_feature_Q.Std_Dev * RUNTIME_STD_DEV_TOL_ON_TO_OFF_Q ;            
      if(Comp_WT_Feature( Runtime_feature_Q.Std_Dev, device_list[idx].Q_feature_ON_TO_OFF.Std_Dev, Runtime_Std_Dev_Tol_Q ) )
      {
        match_wieghtage_Q+= STD_DEV_WIGHTAGE_Q;
        DeviceFeature_Q_st[idx].Std_Dev = STD_DEV_WIGHTAGE_Q;              
      }
      
      Runtime_Variance_Tol_Q = Runtime_feature_Q.Variance * RUNTIME_VARIANCE_TOL_ON_TO_OFF_Q ;            
      if(Comp_WT_Feature( Runtime_feature_Q.Variance, device_list[idx].Q_feature_ON_TO_OFF.Variance, Runtime_Variance_Tol_Q ) )
      {
        match_wieghtage_Q+= VARIANCE_WIGHTAGE_Q;
        DeviceFeature_Q_st[idx].Variance = VARIANCE_WIGHTAGE_Q;
      }              
      
      Runtime_Skewness_Tol_Q = Runtime_feature_Q.Skewness * RUNTIME_SKEWNESS_TOL_ON_TO_OFF_Q ;            
      if(Comp_WT_Feature( Runtime_feature_Q.Skewness, device_list[idx].Q_feature_ON_TO_OFF.Skewness, Runtime_Skewness_Tol_Q ) )
      {
        match_wieghtage_Q+= SKEWNESS_WIGHTAGE_Q;
        DeviceFeature_Q_st[idx].Skewness = SKEWNESS_WIGHTAGE_Q;
      }              
      
      Runtime_Kurtosis_Tol_Q = Runtime_feature_Q.Kurtosis * RUNTIME_KURTOSIS_TOL_ON_TO_OFF_Q ;            
      if(Comp_WT_Feature( Runtime_feature_Q.Kurtosis, device_list[idx].Q_feature_ON_TO_OFF.Kurtosis, Runtime_Kurtosis_Tol_Q ) )
      {
        match_wieghtage_Q+= KURTOSIS_WIGHTAGE_Q;
        DeviceFeature_Q_st[idx].Kurtosis = KURTOSIS_WIGHTAGE_Q;
      }          
    }
    
    
    /*------------------------- Search all devices for max match  -------------------------*/
          feature_arr[idx][IDX_FLD] = device_list[idx].idx	;
          feature_arr[idx][PWR_FLD] = match_wieghtage_P	;
          feature_arr[idx][VAR_FLD] = match_wieghtage_Q	;          
    
          // Find max match_wieghtage device and trigger it
          #if (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)
//            if ( ( feature_arr[idx][PWR_FLD] > dv_wieghtage_P ) && ( feature_arr[idx][VAR_FLD] > dv_wieghtage_Q ) )
            if ( feature_arr[idx][PWR_FLD] > dv_wieghtage_P )          
          #elif (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)
            if ( feature_arr[idx][PWR_FLD] > dv_wieghtage_P )
          #elif (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION)
            if ( feature_arr[idx][VAR_FLD] > dv_wieghtage_Q )
          #endif            
            {
              dv_index	    =	feature_arr[idx][IDX_FLD]	;
              dv_wieghtage_P  =	feature_arr[idx][PWR_FLD]	;
              dv_wieghtage_Q  =	feature_arr[idx][VAR_FLD]	;
            }

#if (SIMU_FEATURE_DEBUG_PRINTF == ENABLED)
            printf("\n");
            printf("Index = %d\nP_wght = %d\nQ_wght = %d\n", idx, dv_wieghtage_P, dv_wieghtage_Q) ;
#endif

	}//	for(idx=0; idx<dev_cnt; idx++)
	
  u8sizeOfStruct = sizeof(Sign_feature)/4;
#if (LOGGING==ENABLED)
  for(u8tempcnt=0; u8tempcnt<dev_cnt ;u8tempcnt++)
  {
    LOG(EventScore_P_Log,"%d %d %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %d \n",
        u32iterationCnt ,
        u8sizeOfStruct,
        DeviceFeature_P_st[u8tempcnt].Mean,
        DeviceFeature_P_st[u8tempcnt].Min,
        DeviceFeature_P_st[u8tempcnt].Max,
        DeviceFeature_P_st[u8tempcnt].Range,
        DeviceFeature_P_st[u8tempcnt].Median,
        DeviceFeature_P_st[u8tempcnt].Std_Dev,
        DeviceFeature_P_st[u8tempcnt].Variance,
        DeviceFeature_P_st[u8tempcnt].Skewness,
        DeviceFeature_P_st[u8tempcnt].Kurtosis,
        feature_arr[u8tempcnt][1]
          ) ;
    LOG(EventScore_Q_Log,"%d %d %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f %d \n",
        u32iterationCnt ,
        u8sizeOfStruct,
        DeviceFeature_Q_st[u8tempcnt].Mean,
        DeviceFeature_Q_st[u8tempcnt].Min,
        DeviceFeature_Q_st[u8tempcnt].Max,
        DeviceFeature_Q_st[u8tempcnt].Range,
        DeviceFeature_Q_st[u8tempcnt].Median,
        DeviceFeature_Q_st[u8tempcnt].Std_Dev,
        DeviceFeature_Q_st[u8tempcnt].Variance,
        DeviceFeature_Q_st[u8tempcnt].Skewness,
        DeviceFeature_Q_st[u8tempcnt].Kurtosis,
        feature_arr[u8tempcnt][2]
          ) ;
  }
#endif
/*--------*/

        for(i=0; i<dev_cnt-1; i++)
        {
          for(j=i+1; j<dev_cnt; j++)
          {
            if(feature_arr[j][PWR_FLD] < feature_arr[i][PWR_FLD])
            {
              // swap elements

              feature_arr[i][IDX_FLD]^=feature_arr[j][IDX_FLD]^=feature_arr[i][IDX_FLD]^=feature_arr[j][IDX_FLD];
              feature_arr[i][PWR_FLD]^=feature_arr[j][PWR_FLD]^=feature_arr[i][PWR_FLD]^=feature_arr[j][PWR_FLD];
              feature_arr[i][VAR_FLD]^=feature_arr[j][VAR_FLD]^=feature_arr[i][VAR_FLD]^=feature_arr[j][VAR_FLD];

            }
          }
        }

#if (SIMU_FEATURE_DEBUG_PRINTF == ENABLED)
        printf("Sorted Wght array:\n___________________\nIdx\tPWR\tVAR\n") ;

        for(i=0; i<dev_cnt; i++)
        {
          for(j=0; j<3; j++)
          {
        	  printf("%d\t", feature_arr[i][j]) ;
          }
          printf("\n") ;
        }

        printf("Press any key ...\n") ;
        getch();
#endif

    #if (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)
        if(dev_cnt >= 2 )
        {
			if( abs(feature_arr[dev_cnt-1][PWR_FLD]-feature_arr[dev_cnt-2][PWR_FLD]) > PERCENTAGE_MAX_MATCH_RESOLUTION)                              //  P:  Resolved
			{
			  if ( Match_Wieghtage_percent_P(feature_arr[dev_cnt-1][PWR_FLD]) >= SIGNATURE_MATCH_THRES_P )
				{
				  Det_st_Flag = 1 ;
				#if (SIMU_FEATURE_DEBUG_PRINTF == ENABLED)
				  printf("feature_arr[%d][PWR_FLD] = %d\n",(dev_cnt-1), feature_arr[dev_cnt-1][PWR_FLD]) ;
				#endif
				}
			  else                                                                                    //  P:  Un-Resolved:  Check Q for resolving
			  {
				  if ( Match_Wieghtage_percent_Q(feature_arr[dev_cnt-1][VAR_FLD]) >= SIGNATURE_MATCH_THRES_Q )
				  {
					  Det_st_Flag = 1 ;
					#if (SIMU_FEATURE_DEBUG_PRINTF == ENABLED)
					  printf("feature_arr[%d][VAR_FLD] = %d\n",(dev_cnt-1), feature_arr[dev_cnt-1][VAR_FLD]) ;
					#endif
				  }
			  }
			}
        }
        else
        {
        	if ( Match_Wieghtage_percent_P(feature_arr[dev_cnt-1][PWR_FLD]) >= SIGNATURE_MATCH_THRES_P )
        	{
        		Det_st_Flag = 1 ;
				#if (SIMU_FEATURE_DEBUG_PRINTF == ENABLED)
               		printf("feature_arr[%d][PWR_FLD] = %d\n",(dev_cnt-1), feature_arr[dev_cnt-1][PWR_FLD]) ;
				#endif
        	}
        	else                                                                                    //  P:  Un-Resolved:  Check Q for resolving
        	{
        		if ( Match_Wieghtage_percent_Q(feature_arr[dev_cnt-1][VAR_FLD]) >= SIGNATURE_MATCH_THRES_Q )
        		{
        			Det_st_Flag = 1 ;
					#if (SIMU_FEATURE_DEBUG_PRINTF == ENABLED)
        	       		printf("feature_arr[%d][VAR_FLD] = %d\n",(dev_cnt-1), feature_arr[dev_cnt-1][VAR_FLD]) ;
					#endif
        		}
        	}
        }


    #elif (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)              
        if( abs(feature_arr[dev_cnt-1][PWR_FLD]-feature_arr[dev_cnt-2][PWR_FLD]) > PERCENTAGE_MAX_MATCH_RESOLUTION)                              //  P:  Resolved
        {
          if ( Match_Wieghtage_percent_P(feature_arr[dev_cnt-1][PWR_FLD]) >= SIGNATURE_MATCH_THRES_P )
            Det_st_Flag = 1 ;
        }
        
    #elif (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION)
        if( abs(feature_arr[dev_cnt-1][VAR_FLD]-feature_arr[dev_cnt-2][VAR_FLD]) > PERCENTAGE_MAX_MATCH_RESOLUTION)                              //  P:  Resolved
        {
          if ( Match_Wieghtage_percent_Q(feature_arr[dev_cnt-1][VAR_FLD]) >= SIGNATURE_MATCH_THRES_Q )
            Det_st_Flag = 1 ;
        }    
        
    #endif            
        
/*--------*/
        
//          #if (FEATURE_EXTRACTION_FLD == PQ_FEATURE_EXTRACTION)        
//            if ( ( Match_Wieghtage_percent_P(dv_wieghtage_P) >= SIGNATURE_MATCH_THRES_P ) && ( Match_Wieghtage_percent_Q(dv_wieghtage_Q) >= SIGNATURE_MATCH_THRES_Q ) )
//          #elif (FEATURE_EXTRACTION_FLD == P_FEATURE_EXTRACTION)
//            if ( Match_Wieghtage_percent_P(dv_wieghtage_P) >= SIGNATURE_MATCH_THRES_P )
//          #elif (FEATURE_EXTRACTION_FLD == Q_FEATURE_EXTRACTION)
//            if ( Match_Wieghtage_percent_Q(dv_wieghtage_Q) >= SIGNATURE_MATCH_THRES_Q )
//          #endif            
            if ( Det_st_Flag == 1 )
            {
              Det_st_Flag           = ZERO		;
              match_wieghtage_P     = ZERO		;
              match_wieghtage_Q     = ZERO		;            

//              *Device_Detected    = dv_index		;
              
//              if ( dir == OFF_TO_ON )
//                device_st[dv_index].wt_status	= ON_ST		;			//direction of transition
//              else if ( dir == ON_TO_OFF )
//                device_st[dv_index].wt_status	= OFF_ST	;			//direction of transition
//              
//              device_st[dv_index].trg  = TRIGGERED ;

              
              *Device_Detected    = feature_arr[dev_cnt-1][IDX_FLD]		;                                          
			#if (SIMU_FEATURE_DEBUG_PRINTF == ENABLED)
              printf("Device_Detected = %d\n", *Device_Detected) ;
			#endif
              
              if ( dir == OFF_TO_ON )
                {
            	  device_st[feature_arr[dev_cnt-1][IDX_FLD]].wt_status	= ON_ST		;			//direction of transition
				#if (SIMU_FEATURE_DEBUG_PRINTF == ENABLED)
            	  printf("Device %s is > ON <\n", device_list[feature_arr[dev_cnt-1][IDX_FLD]].name) ;
				#endif
                }

              else if ( dir == ON_TO_OFF )
              {
                device_st[feature_arr[dev_cnt-1][IDX_FLD]].wt_status	= OFF_ST	;			//direction of transition
				#if (SIMU_FEATURE_DEBUG_PRINTF == ENABLED)
                	printf("Device %s is > OFF <\n", device_list[feature_arr[dev_cnt-1][IDX_FLD]].name) ;
				#endif
              }

              device_st[feature_arr[dev_cnt-1][IDX_FLD]].trg  = TRIGGERED ;
              
              // for return
              u8eventFlag = 1;
              
    //              // For logging purpose    
    //              for(uint8 u8tempCnt=0; u8tempCnt<dev_cnt; u8tempCnt++)
    //              { 
    //                if (device_st[u8tempCnt].wt_status == 0x06)
    //                  u8deviceStatus[u8tempCnt]=0;  // Device ON
    //                else if (device_st[u8tempCnt].wt_status == 0x0A)
    //                  u8deviceStatus[u8tempCnt]=1;  // Device OFF
    //                else if (device_st[u8tempCnt].wt_status == 0x01)
    //                  u8deviceStatus[u8tempCnt]=2;  // Unresolved           
    //              }
    //              
    //              LOG(EnvelopState,"%d %d %d %d %d \n",u32iterationCnt,u8deviceStatus[0],u8deviceStatus[1],u8deviceStatus[2],u8deviceStatus[3]);
    //              
    
  }
  
		#if (SIMU_FEATURE_DEBUG_PRINTF == ENABLED)
            printf("device_st.device_idx = %d\ndevice_st.final_status = %d\ndevice_st.status = %d\ndevice_st.trg = %d\ndevice_st.wt_status = %d\n"
            		, device_st[feature_arr[dev_cnt-1][IDX_FLD]].device_idx
            		, device_st[feature_arr[dev_cnt-1][IDX_FLD]].final_status
            		, device_st[feature_arr[dev_cnt-1][IDX_FLD]].status
            		, device_st[feature_arr[dev_cnt-1][IDX_FLD]].trg
            		, device_st[feature_arr[dev_cnt-1][IDX_FLD]].wt_status
            	) ;

            puts("Press any key to exit feature match ...\n") ;
            getch() ;
		#endif

  if(u8eventFlag)
  {
    u8eventNumber = (uint8)(feature_arr[dev_cnt-1][IDX_FLD]+1) ;
    u8dveventCnt  = (uint8)(feature_arr[dev_cnt-1][IDX_FLD]) ;
  }
  else
  {
    u8eventNumber=0; 
  }
  // For logging purpose    
  for(u8tempCnt=0; u8tempCnt<dev_cnt; u8tempCnt++)
  { 
    if (device_st[u8tempCnt].wt_status == 0x06)
      u8deviceStatus[u8tempCnt]=0;  // Device ON
    else if (device_st[u8tempCnt].wt_status == 0x0A)
      u8deviceStatus[u8tempCnt]=1;  // Device OFF
    else if (device_st[u8tempCnt].wt_status == 0x01)
      u8deviceStatus[u8tempCnt]=2;  // Unresolved           
  }
  

#if (SVM == ENABLED)
  if( SVM_learn_st == SVM_LEARNT )
	  main_classify() ;
#endif


#if (LOGGING==ENABLED)
  LOG(EnvelopState,"%d %d %d %d %d %d %d \n",u32iterationCnt,u8eventNumber, device_list[u8dveventCnt].orderOfLearn,u8deviceStatus[0],u8deviceStatus[1],u8deviceStatus[2],u8deviceStatus[3]);
#endif
  
  if(u8eventFlag)
  {
    return 1;
  }
  else
  {
    return 0; 
  }
  
}

