
#include "main_inf.h"


/*-----------------------------------------------------------------------------*/


float inner_prod(int N,float vec1[],float vec2[]){
  float inner_prod = 0.;
  int i;
 
  for ( i = 0; i < N; i++)
  {
    inner_prod += vec1[i]*vec2[i];
  }
  return  inner_prod;
}

/*-----------------------------------------------------------------------------*/

int getNumOctaves(int N)
{
  return (int) (log(N)/log(2));
}

/*-----------------------------------------------------------------------------*/

float *dn_sample(float *a, int N)
{
/* Does forwared Mallat scheme swapping */

  float *p = a;
  int i;
//  float *temp;  
  float temp[COEFF_ARR_SIZE];

//  make_1d_array(&temp,N);
//  temp = (float *)( malloc( N * sizeof(float) ) );


  for ( i = 0; i < N; i++)
  {
    if ( 0 == i%2 )
    {
      temp[i/2] = p[i]; 
    }
    else if ( 1 == i%2 )
    {
     temp[(i-1)/2+N/2] = p[i]; 
    }
  }
 
  for ( i = 0; i < N; i++)
  {
    p[i] = temp[i];
  }  
 
//  free( temp );
  
  return p;
}

/*-----------------------------------------------------------------------------*/


float *up_sample (float *a, int N)
{
/* does backward Mallat scheme swapping. */
  float *p = a;
  int i;
//  float *temp;  
  float temp[COEFF_ARR_SIZE];

//  make_1d_array(&temp,N);
  
  
  for ( i = 0; i < N/2; i++)
  {
    temp[2*i] = p[i];
    temp[2*i+1] = p[i+N/2];
  }
 
  for ( i = 0; i < N; i++)
  {
    p[i] = temp[i];
  } 


//  free( temp );
  return p;
}

/*-----------------------------------------------------------------------------*/

int make_1d_array(float **matrix, int length )
{

  *matrix = (float *)malloc( length * sizeof **matrix );

  return 0;

}


/*-----------------------------------------------------------------------------*/


/*----------------------------------------------------*

void Blink_LED(void)
{
static bool Blnk_LED_st;
static uint8 LED_Blnk_Del = LED_BLNK_RAS_CNT;

  if( !(LED_Blnk_Del--) )
    {
      Blnk_LED_st^= 1;

      DIO_Set (PORT_A, 20, !Blnk_LED_st); // on
      LED_Blnk_Del = LED_BLNK_RAS_CNT ;
    }
}

/*----------------------------------------------------*

void Toggle_LED(void)
{
  static bool Tog_LED_st=0 ;
      Tog_LED_st^= 1;
      DIO_Set (PORT_A, 20, Tog_LED_st); // on
}

/*----------------------------------------------------*

void Set_LED(void)
{
	DIO_Set (PORT_A, 20, LED_ON_ST); // on
}

/*----------------------------------------------------*

void Clear_LED(void)
{
	DIO_Set (PORT_A, 20, LED_OFF_ST); // on
}

/*----------------------------------------------------*


void Toggle_PA8(void)
{
  static bool PA8_st=0 ;
      PA8_st^= 1;
      DIO_Set (PORT_A, 8, PA8_st); // on
}

/*----------------------------------------------------*

//    static uint32 New_Time,Old_Time;
//    New_Time = OSTimeGet();
//    Shell_Printf ("%d\n", (New_Time-Old_Time));
//    Old_Time = New_Time;

/*----------------------------------------------------*/



uint32 Srv_AbsDiff32(uint32 x, uint32 y)
{
// (x > y)  ? (return (x - y) ) :  ( return (y - x) ) ;
 if(x > y)
   return (x - y) ;
 else
   return (y - x) ;
}

/*----------------------------------------------------*/

uint32 Srv_PosDiff32(uint32 x, uint32 y)
{
// (x > y)  ?( return (x - y) ) : ( return 0 ) ;
 if(x > y)
   return (x - y) ;
 else
   return 0 ;
}


/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*
void store_data_buff(uint8 *toggle_flag, uint8 Event_Occur_flag, uint32 P_Ins, uint32 Q_Ins)
{
	uint32 *Dest_Buff_ptr;

//if(event detect : t-ti_C and switch ring buffer)

	if(Event_Occur_flag)
	{
		u16_StartDataPtr = u16_EndDataPtr ;
//		u16_StopDataPtr = u16_EndDataPtr - Event_Pre_Samp_Cnt_C	;
		u16_StopDataPtr = (u16_EndDataPtr > Event_Pre_Samp_Cnt_C)?	(u16_EndDataPtr - Event_Pre_Samp_Cnt_C) : ((COEFF_ARR_SIZE+u16_EndDataPtr) - Event_Pre_Samp_Cnt_C);
	}

//	Dest_Buff_ptr = pp_co_eff_buff[toggle_flag].P_Coeff	;
//	Dest_Buff_ptr = pp_co_eff_buff[toggle_flag];

	Dest_Buff_ptr = P_Ring_Buff[*buff_toggle_flag] ;
	PushToRing(Dest_Buff_ptr, P_Ins, BYTES_TO_STORE);

	Dest_Buff_ptr = Q_Ring_Buff[*buff_toggle_flag] ;
	PushToRing(Dest_Buff_ptr, Q_Ins, BYTES_TO_STORE);

	if(u16_EndDataPtr == u16_StopDataPtr)
		{
			(*buff_toggle_flag)^= 1 ;
			return 1;
		}
	return 0;
}
*/

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/


