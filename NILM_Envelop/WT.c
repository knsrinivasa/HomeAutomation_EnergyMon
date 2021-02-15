
#include "main_inf.h"


/*---------------------------------------------------------------------*/

int file_read( float **x, float **y, int *N )
{
		FILE *ptr_file;
//		char buf[1000];
		  float col1;
		  float col2;
		  int count = 0;

		ptr_file =fopen("input.txt","r");
		if (!ptr_file)
		{
//			printf("\nFile not found\n");
    		return 1;
    	}
/*
		for (i=0; i<LEN;i++)
			{
				fscanf(ptr_file, "%lf", &temp) ;
//	    		printf("%lf\n",temp);
	    		input[i] = temp ;
			}
*/
		  *x = NULL;
		  *y = NULL;
		  while ( (EOF != fscanf(ptr_file, "%f %f \n",&col1,&col2) ) )
		  {
		    ++count;
		    if ( NULL == (*x = (float *) realloc( *x,count * sizeof(float) ) ) ) exit(EXIT_FAILURE);
		    if ( NULL == (*y = (float *) realloc( *y,count * sizeof(float) ) ) ) exit(EXIT_FAILURE);
		    (*x)[count-1] = col1;
		    (*y)[count-1] = col2;
//		    printf("%lf\t%lf \n",col1, col2);
		  }

		  *N = count;
	fclose(ptr_file);
		return 0;
}

/*---------------------------------------------------------------------*/


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/

uint8 wt(uint32 *wt_align_buff)
{
#if( ( READ_PATH == PIPE_IN ) || ( READ_PATH == FILE_READ )  )
  float *index;
  int octaves;
#endif  
  float *c,*d;
  
//  float *signal;
//  float *short_signal;
//  float *temp;

  float signal[COEFF_ARR_SIZE];
  float short_signal[COEFF_ARR_SIZE];
  float temp[COEFF_ARR_SIZE];
  
  int N;
  int i,j,k;

  int support = 0;
  int offset = 0;

//	int D, C;
//  char filter_type[5];
//  int c_opts;

    switch (D)
    {
      case 2:
        initD2(&c,&d,&support);
        break;  
      case 4:
        initD4(&c,&d,&support);
        break;
      case 6:
        initD6(&c,&d,&support);
        break;
    }

/*
    switch (C)
    {
      case 6:
        initC6(&c,&d,&support);
        break;
      case 12:
        initC12(&c,&d,&support);
        break;
      case 18:
        initC18(&c,&d,&support);
        break;

   }   
*/


#if ( READ_PATH == PIPE_IN )
  pipein( &index, &signal, &N );

#elif ( READ_PATH == FILE_READ )
  file_read( &index, &signal, &N );

#elif ( READ_PATH == PQ_ARRAY_FEED )

  N = COEFF_ARR_SIZE ;

//  signal = (float*) ( malloc( N * sizeof(float)) )  ;
  
  for ( i = 0; i < N; i++)
    signal[i] = (float)wt_align_buff[i] ;
  
#endif
#if( ( READ_PATH == PIPE_IN ) || ( READ_PATH == FILE_READ )  )
  octaves = getNumOctaves(N);
#endif  

//  if ( N < support)  
//  { 
////    puts("Signal length should be at as long as the filter used.");
//    abort();
//  }
 
//  make_1d_array(&short_signal,support);
//  make_1d_array(&temp,N);  
//  short_signal = (float *)( malloc( support * sizeof(float) ) );
//  temp = (float *)( malloc( N * sizeof(float) ) );

  for( k = 1; k <= N/2; k <<= 1 )
  {
  
// Make copy of signal at each octave and do offset if necessary 
    for ( i = 0; i < N/k; i++)
    {
      temp[(i+offset)%(N/k)]  = signal[i]; // Back wrap to front or vice versa
    }

// Take inner product with wavelet filter, doing loop round each octave. 
    for ( i = 0; i < N/2/k; i++)
    {
      for ( j = 0; j < support; j++)
      {
        short_signal[j] = temp[(2*i+j)%(N/k)];
      }
      signal[2*i]   = inner_prod(support,c,short_signal);
      signal[2*i+1] = inner_prod(support,d,short_signal);
    }

// Do Mallat down sampling to group approx and detail coefficients in resulting array. 
    dn_sample(signal,N/k);

// Repeat until signal is decimated. 
  }

//  for ( i = 0; i < N; i++)
//    printf("%d %20.16f\n",i+1,signal[i]);
  
  for ( i = 0; i < N; i++)
    wt_align_buff[i] = (uint32)(signal[i])  ;
  
/*  
  free( short_signal )  ;
  free( signal )        ;
  free( temp )          ;
#if( ( READ_PATH == PIPE_IN ) || ( READ_PATH == FILE_READ )  )
  free( index )         ;
#endif  
*/
  
  return 1;

}

/*---------------------------------------------------------------------*//*---------------------------------------------------------------------*/
