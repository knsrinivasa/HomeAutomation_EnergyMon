

#ifndef _RING_H_
#define _RING_H_

#include "cpu.h"

void 	Ring_Ini (void);
void 	DiscardFromRing (uint16 u16_Len);

uint8 PushToRing_P(uint32 * u8_RingBuff_P, uint32 * NewData_P, uint32 u16_Len_P, uint8 event_flag_P, uint8 session_P, uint16 *StartPtr_offset_P, uint8 event_trans_dir);
uint8 PushToRing_Q(uint32 * u8_RingBuff_Q, uint32 * NewData_Q, uint32 u16_Len_Q, uint8 event_flag_Q, uint8 session_Q, uint16 *StartPtr_offset_Q, uint8 event_trans_dir);

uint8 PopFromRing_P(uint32 * u8_RingBuff_P, uint32 * NewData_P, uint16 u16_Len_P, uint16 curr_ptr_offset_P);
uint8 PopFromRing_Q(uint32 * u8_RingBuff_Q, uint32 * NewData_Q, uint16 u16_Len_Q, uint16 curr_Qtr_offset_Q);

uint16 	GetRingSize (void);
uint32 	GetTotalData(void);
uint32 GetPtrs(void);

#endif //_RING_H_

