
#ifndef __WT_PROT_H__
#define __WT_PROT_H__

#include <math.h>
#include <cpu.h>

//  __FILTERS_H__
extern  void initD2(float **c, float **d,int *support);
extern  void initD4(float **c, float **d,int *support);
extern  void initC6(float **c, float **d,int *support);
extern  void initD6(float **c, float **d,int *support);
extern  void initC12(float **c, float **d,int *support);
extern  void initC18(float **c, float **d,int *support);

extern  void initD2inv(float **c, float **d,int *support);
extern  void initD4inv(float **c, float **d,int *support);
extern  void initC6inv(float **c, float **d,int *support);
extern  void initD6inv(float **c, float **d,int *support);
extern  void initC12inv(float **c, float **d,int *support);
extern  void initC18inv(float **c, float **d,int *support);

//  __MATH_HELPER_H__
float inner_prod(int N, float vec1[],float vec2[]);
int getNumOctaves(int N);
float *dn_sample (float *a, int N);
float *up_sample (float *a, int N);

//  __ONED_ARRAY_H__
int make_1d_array(float **vector, int length );


//
void haar(uint32 *wt_align_buff, uint32 *output_buff)  ;
uint8 wt(uint32 *wt_buff_xx)  ;
uint8 feature_extract(uint32 *buff, uint8 session, uint16 dv_idx, uint8 transition_dir, uint8 Clas_fld)   ;
uint8 feature_match(uint8 dev_cnt, uint8 dir, uint8 *Device_Detected)                ;
void Store_Device_features(uint8 dv_idx)  ;
//void Envelop_create(uint32 Power_W, uint32* envelope_ptr, uint32* pastValuePower_ptr );
void Envelop_create(uint32 Power_W, float* envelope_ptr, uint32* pastValuePower_ptr, uint8 Class_fld);

void Blink_LED(void)	  ;
void Toggle_LED(void)	  ;
void Set_LED(void)        ;
void Clear_LED(void)	  ;
void Toggle_PA8(void)	  ;
//float _abs_d(float _j)  ;

WT_VAR_DATA_TYPE _abs_d(WT_VAR_DATA_TYPE _j)  ;


void    Set_StM_Sess(uint8 Session)		    ;
uint8   Get_StM_Sess(void)			    ;
void    Set_StM_DvTrn(uint8 Trn_State, uint8 cond)  ;
uint8   Get_StM_DvTrn(void)			    ;
uint8   Get_DvTrn_st(void)			    ;
void    Set_StM_DvTrn_2Sig(uint8 Trn_2Sig_ct)	    ;
uint8   Get_StM_DvTrn_2Sig(void)		    ;
void    Set_DvTrn_WT(uint8 wt_store_st)		    ;
uint8   Get_DvTrn_WT(void)			    ;
void    Set_RunSess_WT(uint8 RunSess_wt_st)	    ;
uint8   Get_RunSess_WT(void)			    ;
void    Set_PopRing_WT(uint8 pop_ring_st)	    ;
uint8   Get_PopRing_WT(void)			    ;

void	Set_WT_DevDet_P_St(uint8 wt_det_p_st)       ;
uint8	Get_WT_DevDet_P_St(void)                    ;
void	Set_WT_DevDet_Q_St(uint8 wt_det_q_st)       ;
uint8	Get_WT_DevDet_Q_St(void)                    ;
void	Set_Steady_DevDet_St(uint8 steady_det_st)   ;
uint8	Get_Steady_DevDet_St(void)                  ;
void    Set_DvTrn_AccpRej_Confirm(uint8 accep_rej_st) ;
uint8   Get_DvTrn_AccpRej_Confirm(void)               ;
void    Set_StM_DvTrn_2Sig_Incr(uint8 incr_value)     ;
uint8   Get_DvTrn_Cnt(void)                         ;
uint8 Get_Dv_OFF_to_ON_Flag(void);
uint8 Get_Dv_ON_to_OFF_Flag(void);

#endif
