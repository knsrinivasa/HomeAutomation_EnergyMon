
/*
****************************************************************************************************
*                                             INCLUDES
****************************************************************************************************
*/
#include "main_inf.h"
//#include "nilm_ring.h"

/*
****************************************************************************************************
*                                            #defines
****************************************************************************************************
*/


/*
****************************************************************************************************
*                                            EXTERNS
****************************************************************************************************
*/

/*
****************************************************************************************************
*                                            STATICS
****************************************************************************************************
*/

static uint8 u8_CommSem_P;
static uint8 u8_CommSem_Q;	

static uint16 u16_CurrRingPtr_P;
static uint16 u16_EndDataPtr_P;

static uint16 u16_StopDataPtr_P;
static uint16 u16_RingSize_P;
static uint32 u32_TotalData_P;
static uint8 event_trig_P;
static uint16 u16_Ri_P	= 0 ;
static uint16 u16_NDi_P	= 0 ;


static uint16 u16_CurrRingPtr_Q;
static uint16 u16_EndDataPtr_Q;

static uint16 u16_StopDataPtr_Q;
static uint16 u16_RingSize_Q;
static uint32 u32_TotalData_Q;
static uint8 event_trig_Q;
static uint16 u16_Ri_Q	= 0 ;
static uint16 u16_NDi_Q	= 0 ;

/*
****************************************************************************************************
*                                            FUNCTIONS
****************************************************************************************************
*/

//------------------------------------------------------------------------------
/// Initialize Ring Buffer
////------------------------------------------------------------------------------
//void Ring_Ini(void)
//{
//	// The following variables completely define the ring buffer
//	u16_EndDataPtr 	= RINGBUFF_SIZE - 1;
//	u16_CurrRingPtr = 0;
//	u16_RingSize 	= 0;
//	u32_TotalData 	= 0;
//}


//------------------------------------------------------------------------------
/// Push new data obtained from SPI to the P ring buffer
//------------------------------------------------------------------------------

uint8 PushToRing_P(uint32 * u8_RingBuff_P, uint32 * NewData_P, uint32 u16_Len_P, uint8 event_flag_P, uint8 session_P, uint16 *StartPtr_offset_P, uint8 event_trans_dir)
{
        uint16 pre_smpl_cnt_P;

//	u16_EndDataPtr_P  = 0 ;
//	u16_CurrRingPtr_P = 0 ;
	u16_RingSize_P 	  = 0 ;
	u32_TotalData_P  = 0 ;

//	u16_Ri_P  =u16_EndDataPtr_P;
	u16_NDi_P = 0;

        if ( event_trans_dir == OFF_TO_ON )
          pre_smpl_cnt_P = Event_Pre_Samp_Cnt_OFF_TO_ON_C ;
        else if ( event_trans_dir == ON_TO_OFF )
          pre_smpl_cnt_P = Event_Pre_Samp_Cnt_ON_TO_OFF_C ;
        
	u16_RingSize_P+= u16_Len_P;
	if(u16_RingSize_P > RINGBUFF_SIZE)
	{
		u16_RingSize_P-= u16_Len_P;
		return 2;
 	}

	if ( (event_flag_P==1) && (event_trig_P==0) )
	{
		u16_StopDataPtr_P  = (u16_EndDataPtr_P > pre_smpl_cnt_P)? (u16_EndDataPtr_P - pre_smpl_cnt_P) : ((COEFF_ARR_SIZE+u16_EndDataPtr_P) - pre_smpl_cnt_P);
//		*StartPtr_offset_P =  u16_EndDataPtr_P ;
              *StartPtr_offset_P =  u16_StopDataPtr_P ;
		event_trig_P = 1 ;
	}


	if( (u16_EndDataPtr_P == u16_StopDataPtr_P) && event_trig_P )
	{
          
            event_trig_P = 0	;
                        
//            if(session_P == TRAIN_SESSION)
//              return 1;
//            else if(session_P == RUN_SESSION)
//            {
////	      buff_toggle_flag_P^= 1 ;
//              return 1;
//            }
            
              return 1;            
	}


	while(u16_NDi_P < u16_Len_P)
	{
		if(u16_Ri_P >= RINGBUFF_SIZE)
			u16_Ri_P = 0;

		u8_RingBuff_P[u16_Ri_P] = NewData_P[u16_NDi_P];
		u16_Ri_P++;
		u16_NDi_P++;

		if(u16_Ri_P >= RINGBUFF_SIZE)
			u16_Ri_P = 0;

	}
	u16_EndDataPtr_P = u16_Ri_P;

	u32_TotalData_P += u16_Len_P;
	return 0;
}

//------------------------------------------------------------------------------
/// Push new data obtained from SPI to the Q ring buffer
//------------------------------------------------------------------------------

uint8 PushToRing_Q(uint32 * u8_RingBuff_Q, uint32 * NewData_Q, uint32 u16_Len_Q, uint8 event_flag_Q, uint8 session_Q, uint16 *StartPtr_offset_Q, uint8 event_trans_dir)
{

        uint16 pre_smpl_cnt_Q;
 

//	u16_EndDataPtr_Q  = 0 ;
//	u16_CurrRingPtr_Q = 0 ;
	u16_RingSize_Q 	  = 0 ;
	u32_TotalData_Q  = 0 ;

//	u16_Ri_Q  =u16_EndDataPtr_Q;
	u16_NDi_Q = 0;

        if ( event_trans_dir == OFF_TO_ON )
          pre_smpl_cnt_Q = Event_Pre_Samp_Cnt_OFF_TO_ON_C ;
        else if ( event_trans_dir == ON_TO_OFF )
          pre_smpl_cnt_Q = Event_Pre_Samp_Cnt_ON_TO_OFF_C ;
        
	u16_RingSize_Q+= u16_Len_Q;
	if(u16_RingSize_Q > RINGBUFF_SIZE)
	{
		u16_RingSize_Q-= u16_Len_Q;
		return 2;
 	}

	if ( (event_flag_Q==1) && (event_trig_Q==0) )
	{
		u16_StopDataPtr_Q  = (u16_EndDataPtr_Q > pre_smpl_cnt_Q)? (u16_EndDataPtr_Q - pre_smpl_cnt_Q) : ((COEFF_ARR_SIZE+u16_EndDataPtr_Q) - pre_smpl_cnt_Q);
//		*StartPtr_offset_Q =  u16_EndDataPtr_Q ;                
		*StartPtr_offset_Q =  u16_StopDataPtr_Q ;
		event_trig_Q = 1 ;
	}


	if( (u16_EndDataPtr_Q == u16_StopDataPtr_Q) && event_trig_Q )
	{
          
          event_trig_Q = 0	;          
          
//          if(session_Q == TRAIN_SESSION)
//            return 1;
//          else if(session_Q == RUN_SESSION)
//          {
//            //	buff_toggle_flag_Q^= 1 ;
//            return 1;
//          }

            return 1;          
	}
        
	while(u16_NDi_Q < u16_Len_Q)
	{
		if(u16_Ri_Q >= RINGBUFF_SIZE)
			u16_Ri_Q = 0;

		u8_RingBuff_Q[u16_Ri_Q] = NewData_Q[u16_NDi_Q];
		u16_Ri_Q++;
		u16_NDi_Q++;

		if(u16_Ri_Q >= RINGBUFF_SIZE)
			u16_Ri_Q = 0;

	}
	u16_EndDataPtr_Q = u16_Ri_Q;

	u32_TotalData_Q += u16_Len_Q;
	return 0;
}

//---------------------------------------------------------------------------------
/// Pop data from ring buffer into an array which can be used for flash programming
//---------------------------------------------------------------------------------

uint8 PopFromRing_P(uint32 * u8_RingBuff_P, uint32 * NewData_P, uint16 u16_Len_P, uint16 curr_ptr_offset_P)
{
  
	uint16 u16_Ri_P, u16_NDi_P;

	//	u16_Ri_P  =u16_EndDataPtr_P;
	u16_NDi_P = 0;
       
	u16_EndDataPtr_P 	= 0; //RINGBUFF_SIZE - 1;
	u16_CurrRingPtr_P       = curr_ptr_offset_P;
	u16_RingSize_P 	        = RINGBUFF_SIZE;
	u32_TotalData_P 	= 0;
	
	u8_CommSem_P = TRUE;
	
	u16_Ri_P  = u16_CurrRingPtr_P;
	u16_NDi_P = 0;
	
	if(u16_Len_P > u16_RingSize_P)
	{
		return 2;
	}
	
	u16_RingSize_P -= u16_Len_P;
	
	while(u16_NDi_P < u16_Len_P)
	{
		NewData_P[u16_NDi_P] = u8_RingBuff_P[u16_Ri_P];
		u16_NDi_P++;
		u16_Ri_P++;
		if(u16_Ri_P >= RINGBUFF_SIZE)
		{
			u16_Ri_P = 0;
		}
	}
	u16_CurrRingPtr_P = u16_Ri_P;
	
	u8_CommSem_P = FALSE;
	
	return 1;
}


//---------------------------------------------------------------------------------
/// Pop data from ring buffer into an array which can be used for flash programming
//---------------------------------------------------------------------------------

uint8 PopFromRing_Q(uint32 * u8_RingBuff_Q, uint32 * NewData_Q, uint16 u16_Len_Q, uint16 curr_Qtr_offset_Q)
{
	uint16 u16_Ri_Q, u16_NDi_Q;

	//	u16_Ri_Q  =u16_EndDataPtr_Q;
	u16_NDi_Q = 0;
        
	u16_EndDataPtr_Q 	= 0; //RINGBUFF_SIZE - 1;
	u16_CurrRingPtr_Q = curr_Qtr_offset_Q;
	u16_RingSize_Q 	= RINGBUFF_SIZE;
	u32_TotalData_Q 	= 0;
	
	u8_CommSem_Q = TRUE;
	
	u16_Ri_Q  = u16_CurrRingPtr_Q;
	u16_NDi_Q = 0;
	
	if(u16_Len_Q > u16_RingSize_Q)
	{
		return 2;
	}
	
	u16_RingSize_Q -= u16_Len_Q;
	
	while(u16_NDi_Q < u16_Len_Q)
	{
		NewData_Q[u16_NDi_Q] = u8_RingBuff_Q[u16_Ri_Q];
		u16_NDi_Q++;
		u16_Ri_Q++;
		if(u16_Ri_Q >= RINGBUFF_SIZE)
		{
			u16_Ri_Q = 0;
		}
	}
	u16_CurrRingPtr_Q = u16_Ri_Q;
	
	u8_CommSem_Q = FALSE;
	
	return 1;
}


//void DiscardFromRing(uint16 u16_Len)
//{
//	if(u16_RingSize > 0)
//	{
//		if((sint16)(u16_EndDataPtr - u16_Len) >= 0)
//		{
//			u16_EndDataPtr = (uint16)(u16_EndDataPtr - u16_Len);
//		}
//		else
//		{
//			u16_EndDataPtr = (uint16)(RINGBUFF_SIZE + u16_EndDataPtr - u16_Len);
//		}
//		u16_RingSize = (uint16)((sint16)u16_RingSize - (sint16)u16_Len);
//		
//		u32_TotalData -= u16_Len;
//	}
//}
//
////------------------------------------------------------------------------------
///// Get the current size of the ring buffer
////------------------------------------------------------------------------------
//uint16 GetRingSize(void)
//{
//	return (u16_RingSize);
//}
//
////------------------------------------------------------------------------------
///// Get the Total bytes of data that were pushed into the buffer
////------------------------------------------------------------------------------
//uint32 GetTotalData(void)
//{
//	return (u32_TotalData);
//}
//
//uint32 GetPtrs(void)
//{
//	return (uint32)(u16_EndDataPtr<<16|u16_CurrRingPtr);
//}
