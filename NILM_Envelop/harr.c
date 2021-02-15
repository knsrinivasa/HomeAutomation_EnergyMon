
#include "main_inf.h"

/*
 ============================================================================
 Name        :
 Author      : skn2kor
 Version     :
 Copyright   :
 Description :
 ============================================================================
 */


/***************************************************************/

int write_data(void)
{
	FILE *ptr_file;
//	int x;

	ptr_file =fopen("output.txt", "w");

	if (!ptr_file)
	{
//		printf("\nFile not found\n");
		return EXIT_FAILURE;
	}


//	for (x=1; x<=10; x++)
//		fprintf(ptr_file,"%d\n", x);

	fclose(ptr_file);

	return  EXIT_SUCCESS;
}

/***************************************************************/

int read_data(float *input, float *output)
{
	FILE *ptr_file;
//	int buf[COEFF_ARR_SIZE]={0};
	float temp ;
	int i;

#ifdef FILE_READ
	ptr_file = fopen("input.txt","r");

		if (!ptr_file)
		{
//			printf("\nFile not found\n");
			return EXIT_FAILURE;
    	}

//		while (fgets(buf,COEFF_ARR_SIZE, ptr_file)!=NULL)
		for (i=0; i<COEFF_ARR_SIZE;i++)
		{
		  fscanf(ptr_file, "%f", &temp) ;
//    		  printf("%lf\n",temp);
    		  input[i] = temp ; 

		}

	fclose(ptr_file);

#else
	for(i=0; i<COEFF_ARR_SIZE; i++)
		input[i] = i+1 ;
#endif

//	printf("\n");
//	for(i=0;i<COEFF_ARR_SIZE;i++)
//		printf("input[%i] = %lf\n",i, input[i]);

	return EXIT_SUCCESS;
}

/***************************************************************/


void haar(uint32 *input, uint32 *output)
{

sint32 sum, difference ;
uint16 i, length, itr ;

//printf("\n\n============================= In HAAR ======================= \n");

itr = HAAR_ITR_DEPTH;

    // This function assumes that input.length=2^n, n>1

  do
  {
    for (length = COEFF_ARR_SIZE >> 1; ; length >>= 1) 
    {
        // length = input.length / 2^n, WITH n INCREASING to log(input.length) / log(2)
        for (i = 0; i < length; ++i) 
        {
            sum = input[i * 2] + input[i * 2 + 1];
            difference = input[i * 2] - input[i * 2 + 1];
            output[i] = sum;
            output[length + i] = difference;
        }
        
        if (length == 1) 
        {
            break;
        }

 
        //Swap arrays to do next iteration
//        System.arraycopy(output, 0, input, 0, length << 1);
		for(i=0; i< (length << 1) ; i++)
			 input[i] = output[i] ;

    }
//printf("output =\n");
//for(i=0;i<COEFF_ARR_SIZE;i++)
//	printf("%d\n",output[i]);


  }while(itr--);


//printf("\n\n");

}

/***************************************************************/

