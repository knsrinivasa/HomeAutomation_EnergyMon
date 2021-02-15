/*
 * read_data.c
 *
 *  Created on: Nov 6, 2013
 *      Author: skn2kor
 */

#include "main_inf.h"

/*****************************************************************************************************************/

/*
#if( ( SIMU_READ_PATH == PIPE_IN ) || ( SIMU_READ_PATH == FILE_READ )  )


#elif ( SIMU_READ_PATH == PQ_ARRAY_FEED )

	float signal[COEFF_ARR_SIZE];
	  float short_signal[COEFF_ARR_SIZE];
	  float temp[COEFF_ARR_SIZE];
	  int Simu_read_data_Num;
#endif
*/

/******************************************************************************************************************************/

int pipein( double **x, double **y, int *read_data_Num )
{
  double col1;
  double col2;
  int count = 0;

  *x = NULL;
  *y = NULL;
  while ( (EOF != fscanf(stdin,"%lf %lf \n",&col1,&col2) ) )
  {
    ++count;
    if ( NULL == (*x = (double *) realloc( *x,count * sizeof(double) ) ) ) exit(EXIT_FAILURE);
    if ( NULL == (*y = (double *) realloc( *y,count * sizeof(double) ) ) ) exit(EXIT_FAILURE);
    (*x)[count-1] = col1;
    (*y)[count-1] = col2;
  }
  *read_data_Num = count;

  return 0;
}


/*****************************************************************************************************************/


void Get_PQ_File(void)
{

#if ( SIMU_READ_PATH == PIPE_IN )
	float *index ;
	pipein(&index, &signal_P, &signal_Q, &Simu_read_data_Num );

#elif ( SIMU_READ_PATH == FILE_READ )
  int file_read_ret = 0 ;
  float *index ;

  file_read_ret = file_read_PQ( &index, &signal_P, &signal_Q, &Simu_read_data_Num );

  //  for(i=0; i<Simu_read_data_Num; i++)
  //	  printf("idx = %d\tcol1 = %lf\tcol2 = %lf\tcol3 = %lf\n", i+1, index[i], signal_P[i], signal_Q[i]);

#elif ( SIMU_READ_PATH == PQ_ARRAY_FEED )

  Simu_read_data_Num = COEFF_ARR_SIZE ;

//  signal = (float*) ( malloc( Simu_read_data_Num * sizeof(float)) )  ;

#endif


//	printf("start_clock = %d\tcurrent_clock=%d\tdiff_clock = %d\t\t", start_clock, current_clock, diff_clock) ;
/*

  for ( i = 0; i < Simu_read_data_Num; i++)
	{
		printf("idx = %d\tcol1 = %lf\tcol2 = %lf\tcol3 = %lf\n", i+1, index[i], signal_P[i], signal_Q[i]);
		i++;
	}
*/


}

/*****************************************************************************************************************/

int file_read_PQ( float **x, float **y, float **z, int *read_data_Num )
{

	//	char buf[1000];
	float col1;
	float col2;
	float col3;
	int count = 0;
	FILE *PQ_ptr_file;
	char PQ_input_file_name[] = "input_PQ.txt" ;		// PQ_input_3dev.txt

	PQ_ptr_file = fopen(PQ_input_file_name,"r");

	if (!PQ_ptr_file)
	{
		printf("\ninput_PQ.txt File not found\n");
		return 2;
	}
	else
//		printf("\ninput_PQ.txt File found\n");

	*x = NULL;
	*y = NULL;
	*z = NULL;

	while ( (EOF != fscanf(PQ_ptr_file, "%f %f %f\n",&col1, &col2 ,&col3) ) )
	{
		++count;

		if ( NULL == (*x = (float *) realloc( *x,count * sizeof(float) ) ) ) exit(EXIT_FAILURE);
		if ( NULL == (*y = (float *) realloc( *y,count * sizeof(float) ) ) ) exit(EXIT_FAILURE);
		if ( NULL == (*z = (float *) realloc( *z,count * sizeof(float) ) ) ) exit(EXIT_FAILURE);

		(*x)[count-1] = col1;
		(*y)[count-1] = col2;
		(*z)[count-1] = col3;

//		printf("idx=%d\tcol1=%lf\tcol2=%lf\tcol3=%lf\n",count, col1, col2, col3);

	}
	*read_data_Num = count;
	printf("Simu_read_data_Num = %d\n", *read_data_Num) ;
//	puts("\nPress any Key ...\n");
//	getch();

	fclose(PQ_ptr_file);

	puts("file read exit");
	return (-1) ;
}

/*****************************************************************************************************************/


