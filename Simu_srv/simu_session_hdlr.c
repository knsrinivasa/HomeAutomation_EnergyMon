/*
 * simu_session_hdlr.c
 *
 *  Created on: Nov 15, 2013
 *      Author: skn2kor
 */

/*****************************************************************************************************************/
#include "main_inf.h"
#include "main_macros.h"


/*****************************************************************************************************************/

void simu_session_hdlr(void)
{
	static uint8 sess_choice, Train_option, Train_load_option, Delete_option, Run_option, Status_option, Train_acc_rej_opt, Next_Run_Event_Read;
	static uint8 GUI_Key_input = 0 ;

	uint8 u8caliReturn, Dv_trn_start_end_st, Train_New_Dev_start_st ;

	uint8 GUI_ip_st ;
	GUI_ip_st = Get_GUI_IP_Req_St() ;
//	printf("\n1) _________GUI_ip_st = %d_______\n", GUI_ip_st) ;

	while(GUI_ip_st == GUI_IP_REQUIRED)
  {
	if( GUI_Key_input == 0 )
	{
		puts("Enter Session Intended:\n------------------------\n") ;
		puts("\tTraining Session =>\t( 1 )\n\tRun Session =>\t\t( 2 )\n\tExit =>\t\t\t( 0 )");
		scanf("%d", &sess_choice);
	}

	Dv_trn_start_end_st = Get_StM_DvTrn() ;
	Train_New_Dev_start_st = Get_New_DvTrn_Start() ;

	if( (Train_New_Dev_start_st != DEV_TRAIN_START) && (Dv_trn_start_end_st == DV_TRN_END) && (GUI_Key_input == TRAIN_DEVICE) )
	{
		puts("Device Training is Done\n\t> Accept\t( 1 )\n\t> Reject\t( 2 )\n");
		scanf("%d", &Train_acc_rej_opt ) ;

		(Train_acc_rej_opt == 1)? ( gui_hdlr(TRAINING_ACCEPTED) ) : ( gui_hdlr(TRAINING_REJECTED) ) ;

		Train_acc_rej_opt = 0 ;
		GUI_Key_input = TRAINING_SESSION ;
	}

//	else
	{
		switch(sess_choice)
		{
			case 1:
			{
				puts("--------------------- In Training Session ---------------------\n") ;

				GUI_Key_input = TRAINING_SESSION ;
				gui_hdlr(TRAINING_SESSION) ;

				#if (PQ_ZERO_CAL == ENABLED)
					puts("---- System Zero Calibration is going on: ----\n");
					u8caliReturn = PQ_Zero_Calib( WattValue, VarValue, &P_Calib, &Q_Calib ) ;
					printf("Zero Calibrated Values:\tP_Calib = %d\tQ_Calib = %d\n\n", P_Calib, Q_Calib );
				#endif

				puts("---- Enter choice:\n"
						"\tTo Train a Device\t( 1 )\n"
						"\tDelete Device\t\t( 2 )\n"
						"\tZero Calibration\t( 3 )\n"
						"\tRuntime Info\t\t( 4 )\n"
						"\tDevice List\t\t( 5 )\n"
						"\tBack\t\t\t( 6 )\n"
				) ;
				scanf("%d", &Train_option) ;

				switch(Train_option)
				{
					case 1:
					{
						puts("---- To Train a Device\n") ;
						puts("---- Enter choice:\n"
								"To Train New Device\t\t\t( 1 )\n"
								"To Load offline Device trained Data\t( 2 )\n"
								"Back\t\t\t\t\t( 3 )\n"
						) ;
						scanf("%d", &Train_load_option) ;

						switch(Train_load_option)
						{
						case 1:
							puts("---- Train New Device\n");
							GUI_Key_input = TRAIN_DEVICE ;
							break ;

						case 2:
							puts("---- Load offline Device trained Data\n");
							GUI_Key_input = OFFLINE_TRAIN_LOAD ;
							break ;
						}

						if(Train_load_option == 3)
							gui_hdlr(BACK) ;
						else
						{
							Set_New_DvTrn_Start(DEV_TRAIN_START) ;
							gui_hdlr(TRAIN_DEVICE) ;
						}
						break ;
					}

					case 2:
					{
						puts("---- Delete Device\n") ;
						puts("---- Enter choice:\n"
								"\tTo Delete a Single Device\t( 1 )\n"
								"\tTo Delete all Devices\t\t( 2 )\n"
						) ;
						scanf("%d", &Delete_option) ;

						switch(Delete_option)
						{
							case 1:
								puts("----Delete Single Device\n") ;
								GUI_Key_input = SELECT_DELETE ;
								gui_hdlr(SELECT_DELETE) ;
								break ;

							case 2:
								puts("----Delete All Devices\n") ;
								GUI_Key_input = DELETE ;
								gui_hdlr(DELETE) ;
								break ;
						}

						break ;
					}

					case 3:
						puts("---- Zero Calibration\n") ;
						GUI_Key_input = CALIBRATION ;
						gui_hdlr(CALIBRATION) ;
						break ;

					case 4:
						puts("---- Runtime Info\n") ;
						GUI_Key_input = RUN_TIME_INFO ;
						gui_hdlr(RUN_TIME_INFO) ;
						break ;

					case 5:
						puts("---- Device List\n") ;
						GUI_Key_input = DEVICE_LIST ;
						gui_hdlr(DEVICE_LIST) ;
						break ;

					case 6:
						puts("---- Back to Main Session \n") ;
						GUI_Key_input = 0 ;
						gui_hdlr(BACK) ;
						break ;

					default:
						break ;
				}					//	switch(Train_option)

				break ;
			}

			case 2:
			{
//				Set_New_DvTrn_Start(DEV_TRAIN_START) ;

				GUI_Key_input = RE_INIT_SESSION ;
		        Set_StM_Sess(RE_INIT_SESSION) ;                           // SYSTEM Mode set to -> RE_INIT_SESSION  //  to indicate re-init session after adding new device

				puts("--------------------- In Run Session ---------------------\n") ;
				puts("---- Enter choice:\n"
						"\tNext Event\t( 0 )\n"
						"\tDevice Status\t( 1 )\n"
						"\tDevice List\t( 2 )\n"
						"\tSystem Info\t( 3 )\n"
						"\tAbout\t\t( 4 )\n"
						"\tBack\t\t( 5 )\n"
				) ;
				scanf("%d", &Run_option) ;

//		        if ( (Run_option == 0) && (Next_Run_Event_Read == 0) )
//		        {
//		        	Set_Simu_Run_PQ_Read_St (START_READ_RUN_PQ) ;
//		        	Set_GUI_IP_Req_St (GUI_IP_ENTERED) ;
//
//		        	Next_Run_Event_Read = 1 ;
//		        }

//		        if (Next_Run_Event_Read == 1)
		        {
					switch(Run_option)
					{

					case 0:
						Set_Simu_Run_PQ_Read_St (START_READ_RUN_PQ) ;
						Set_GUI_IP_Req_St (GUI_IP_ENTERED) ;
						break;

					case 1:
					{
						puts("---- Device Status\n") ;

						/*
						puts("---- Enter choice:\n"
								"\tEnvelop Device Status\t\t( 1 )\n"
								"\tSteady State Device status\t( 2 )\n"
								"\tFinal Status\t\t\t( 3 )\n"
								"\tBack\t\t\t\t( 4 )\n"
						) ;
						scanf("%d", &Status_option) ;
*/

/*
						switch(Status_option)
						{
						case 1:
							puts("----Envelope Device Status\t\n");
							GUI_Key_input = WT_STATUS ;
							gui_hdlr(WT_STATUS) ;
	//						DeviceStatus(device_list, device_st, WT_ST )  ;
							break ;
						case 2:
							puts("----Steady State Device Status\t\n");
							GUI_Key_input = ST_STATUS ;
							gui_hdlr(ST_STATUS) ;
	//						DeviceStatus(device_list, device_st, STD_ST )  ;
							break ;
						case 3:
							puts("----Final Device Status\t\n");
							GUI_Key_input = FN_STATUS ;
							gui_hdlr(FN_STATUS) ;
	//						DeviceStatus(device_list, device_st, FNL_ST)  ;
							break ;
						case 4:
						default:
							gui_hdlr(BACK) ;
							break ;
						}
*/


						puts("----Envelope Device Status\t\n");
						GUI_Key_input = WT_STATUS ;
						gui_hdlr(WT_STATUS) ;
/*

						puts("----Steady State Device Status\t\n");
						GUI_Key_input = ST_STATUS ;
						gui_hdlr(ST_STATUS) ;

						puts("----Final Device Status\t\n");
						GUI_Key_input = FN_STATUS ;
						gui_hdlr(FN_STATUS) ;
*/

						Next_Run_Event_Read = 0 ;
						break ;
					}

					case 2:
						puts("---- Device List\n") ;
						GUI_Key_input = DEVICE_LIST ;
						gui_hdlr(DEVICE_LIST) ;
	//					DeviceList(device_list);
						break ;

					case 3:
						puts("---- System Info\n") ;
						GUI_Key_input = SYSTEM_INFO ;
						gui_hdlr(SYSTEM_INFO) ;
	//					SystemInfo(device_list);
						break ;

					case 4:
						GUI_Key_input = ABOUT ;
						gui_hdlr(ABOUT) ;
	//					print_about() ;
						break ;

					case 5:
						GUI_Key_input = 0 ;
						gui_hdlr(BACK) ;
						break ;

					default:
						GUI_Key_input = ABOUT ;
						gui_hdlr(ABOUT) ;
	//					print_about() ;
						break ;
					}

				}
				break ;
			}

			case 0:
				exit (-1) ;

			default:
				GUI_Key_input = ABOUT ;
				gui_hdlr(ABOUT) ;
//				print_about() ;
				break ;
		}						//		switch(sess_choice)
	}
	GUI_ip_st = Get_GUI_IP_Req_St() ;
//	printf("\n2) _________GUI_ip_st = %d_______\n", GUI_ip_st) ;
  }

}

/*****************************************************************************************************************/

