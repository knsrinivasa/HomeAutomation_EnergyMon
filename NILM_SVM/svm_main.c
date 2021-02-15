/*
 * main.c
 *
 *  Created on: Nov 20, 2013
 *      Author: skn2kor
 */


#include "main_inf.h"

/************************************************************************************************************************************************/

int svm_main (void)
{

	main_learn();				// -------- SVM Learn

	main_classify() ;			// -------- SVM Classify

//	printf("\nAfter String copy\n") ;
//	printf("learn_Param_Names[1] = %s\nlearn_Param_Names[3] = %s\nlearn_Param_Names[5] = %s\n", learn_Param_Names[1], learn_Param_Names[3], learn_Param_Names[5]) ;
//	getch() ;

#if (RE_LEARN == ENABLED)
	#if (RE_LEARN_METHOD == INCREMENTAL_RE_LEARN)
		while(ReLearn_flag == 0)
		{
			ReLearn_cnt++;
			if (ReLearn_cnt >= RE_LEARN_CNT_MAX)
				ReLearn_flag = 1 ;

			main_learn();
			main_classify() ;
		}
	#endif
#endif

	return 0;

}

/************************************************************************************************************************************************/


