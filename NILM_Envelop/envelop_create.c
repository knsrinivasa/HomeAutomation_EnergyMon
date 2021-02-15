
#include "main_inf.h"
#include "global_variables.h"



//static uint32 Envelop_P_Value  ;
//static uint32 Envelop_Q_Value  ;
//
//
//void Envelope_createIni()
//{
//
// Envelop_P_Value = 0;
// Envelop_Q_Value = 0;
//
//}

/*
 ============================================================================
 Name        : feature_extract.c
 Author      : 
 Created on  : Apr 24, 2013
 Version     :
 Copyright   :
 Description :	To Store Features to device structure:
 ============================================================================
 */

void Envelop_create(uint32 Power_W, float* envelope_ptr, uint32* pastValuePower_ptr, uint8 Class_fld )
{
  float envelopeValue = *envelope_ptr;
  float envelop_temp ;

  uint16	envelope_riseup_rate		   	;
  uint16	rapid_response_threshold_rise_w		;
  uint16	rapid_response_threshold_fall_w		;
  uint16	envelope_slopedown_rate		   	;
  
  
  envelop_temp = (float)*envelope_ptr;
  
  if ( Class_fld == DAQ_PWR_VAR_ENVELOP )
  {
    envelope_riseup_rate		 = DAQ_ENVELOPE_RISEUP_RATE			        ;
    rapid_response_threshold_rise_w	 = DAQ_RAPID_RESPONSE_THRESHOLD_RISE_W		        ;
    rapid_response_threshold_fall_w	 = DAQ_RAPID_RESPONSE_THRESHOLD_FALL_W		        ;
    envelope_slopedown_rate		 = DAQ_ENVELOPE_SLOPEDOWN_RATE			        ;
  } 
  else if ( Class_fld == NORM_PWR_VAR_ENVELOP )
  {
    envelope_riseup_rate		 = NORM_ENVELOPE_RISEUP_RATE			        ;
    rapid_response_threshold_rise_w	 = NORM_RAPID_RESPONSE_THRESHOLD_RISE_W		        ;
    rapid_response_threshold_fall_w	 = NORM_RAPID_RESPONSE_THRESHOLD_FALL_W		        ;
    envelope_slopedown_rate		 = NORM_ENVELOPE_SLOPEDOWN_RATE			        ;
  }  
  else if ( Class_fld == STD_ST_PWR_VAR_ENVELOP )
  {
    envelope_riseup_rate		 = STD_ST_ENVELOPE_RISEUP_RATE			        ;
    rapid_response_threshold_rise_w	 = STD_ST_RAPID_RESPONSE_THRESHOLD_RISE_W		;
    rapid_response_threshold_fall_w	 = STD_ST_RAPID_RESPONSE_THRESHOLD_FALL_W		;
    envelope_slopedown_rate		 = STD_ST_ENVELOPE_SLOPEDOWN_RATE			;
  }  
  
  
  //   if( nilm(i,3) >envelope(i-1) ) %If there is a rise.
  //If there is a rise in power compared to current envelope jump to meet it.
  if( Power_W > envelopeValue)
  {
//          if(nilm(i,3)- nilm(i-1,3) < 200)
//         envelope(i)=envelope(i-1) + ( (-envelope(i-1) + nilm(i,3))/2 ) ;  
 //      else
   //       envelope(i)= nilm(i,3); 

    if( (Power_W - *pastValuePower_ptr )< rapid_response_threshold_rise_w)
    {
       envelop_temp = envelop_temp + (float)((float)(-envelop_temp + Power_W)/ envelope_riseup_rate);
    }
    else
    {
       *envelope_ptr = Power_W;
    }
  }
  // If there is a fall, decide whether to jump or slope down.
  else
  {
    //Measure the fall  and check if it is falling very fast
    if(	 (*pastValuePower_ptr - Power_W) < rapid_response_threshold_fall_w)
    {
      //Not a rapid fall, so slide gently as per slope indicated by 
     // *envelope_ptr = *envelope_ptr - (( *envelope_ptr- Power_W)/ envelope_slopedown_rate);
       envelop_temp = envelop_temp - (float)((float)(envelop_temp - Power_W)/ envelope_slopedown_rate);
       *envelope_ptr = envelop_temp;
    }
    else
    {
      //This is a rapid fall, so we need to jump to value instead of slide
      *envelope_ptr = Power_W;
    }
    
  }
  *pastValuePower_ptr = Power_W ;
}

