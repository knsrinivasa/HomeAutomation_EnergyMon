/***********************************************************************/
/*                                                                     */
/*   svm_classify.c                                                    */
/*                                                                     */
/*   Classification module of Support Vector Machine.                  */
/*                                                                     */
/*   Author: Thorsten Joachims                                         */
/*   Date: 02.07.02                                                    */
/*                                                                     */
/*   Copyright (c) 2002  Thorsten Joachims - All rights reserved       */
/*                                                                     */
/*   This software is available for non-commercial use only. It must   */
/*   not be modified and distributed without prior permission of the   */
/*   author. The author is not responsible for implications from the   */
/*   use of this software.                                             */
/*                                                                     */
/************************************************************************/

#include "svm_common.h"
#include "conio.h"
#include "main_inf.h"

/************************************************************************************************************************************************/

//int main (int argc, char* argv[])
void main_classify()
{

	static double pred_arr[200][200];
	int dissagr_arr[200];

	int classify_ret;
	int classify_itr_cnt = 0;
	int max_docs_cnt=0, max_comment_cnt=0 ;

	int classify_argc = 4;
	char* classify_argv[4];

	classify_argv[0] = classify_Param_Names[0] ;
	classify_argv[1] = classify_Param_Names[1] ;

	classify_itr_cnt=0;

	for(classify_itr_cnt=0; classify_itr_cnt<MAX_MODEL_CNT; classify_itr_cnt++)
	{
		classify_argv[2] = classify_Param_Names[(classify_itr_cnt*2)+2] ;
		classify_argv[3] = classify_Param_Names[(classify_itr_cnt*2)+3] ;

		printf("\n_____________________________\n") ;
		printf("classify_argc = %d\nargv[0] = %s\nargv[1] = %s\nargv[2] = %s\nargv[3] = %s\n", classify_argc, classify_argv[0], classify_argv[1], classify_argv[2], classify_argv[3]) ;

		classify_ret = SVM_classify(classify_argc, classify_argv, classify_itr_cnt, pred_arr, dissagr_arr, &max_docs_cnt, &max_comment_cnt);

	}

	print_pred_matrix (pred_arr, dissagr_arr, max_docs_cnt, max_comment_cnt) ;

	find_max_wt_pred (pred_arr, dissagr_arr, max_docs_cnt, max_comment_cnt) ;

	print_confu_matrix() ;

#if (RE_LEARN == ENABLED)
	if(ReLearn_flag == 0)
	{

	add_to_model(dissagr_arr, classify_Param_Names, max_docs_cnt, max_comment_cnt) ;

		#if (RE_LEARN_METHOD == INCREMENTAL_RE_LEARN)
			for(classify_itr_cnt=0; classify_itr_cnt<MAX_MODEL_CNT; classify_itr_cnt++)
			{
				//	puts("------RE Learn function\n") ;
				prep_mod_file(classify_Param_Names[(classify_itr_cnt*2)+2]) ;
			}

			//	relearn_model(model_file_name, recall_pointer) ;
	//			strcpy(learn_Param_Names[1],"1_mixer_model.txt") ;
	//			strcpy(learn_Param_Names[3],"2_micro_model.txt") ;
	//			strcpy(learn_Param_Names[5],"2_fridge_model.txt") ;

			learn_Param_Names[1]  =  learn_Param_Names[2] ;
			learn_Param_Names[3]  =  learn_Param_Names[4] ;
			learn_Param_Names[5]  =  learn_Param_Names[6] ;

		#endif
	}
#endif

}

/************************************************************************************************************************************************/

int SVM_classify(int argc, char* argv[], int itr, double pred_arr[200][200], int *dissagr_arr,	int *max_docs_cnt, int *max_comment_cnt )
{
	int i;
	DOC *doc; /* test example */
	WORD *words;
	int max_docs, max_words_doc, lld, comment_cnt = 0 ;
	int totdoc = 0, queryid, slackid;
	int correct = 0, incorrect = 0, no_accuracy = 0;
	int res_a = 0, res_b = 0, res_c = 0, res_d = 0, wnum, pred_format;
	int j;
	double t1, runtime = 0;
	double dist, doc_label, costfactor;
	char *line, *comment;
	FILE *predfl, *docfl;
	MODEL *model;

//	static double pred_arr[200][200];
	int row = 0, col = 0, flag = 1;
	int col_i ;

	row = 0 ;
	col = itr;

	read_input_parameters_classify(argc, argv, classify_docfile, classify_modelfile, predictionsfile,	&verbosity, &pred_format);

	nol_ll(classify_docfile, &max_docs, &max_words_doc, &lld); /* scan size of input file */
	max_words_doc += 2;
	lld += 2;

	line = (char *) my_malloc(sizeof(char) * lld);
	words = (WORD *) my_malloc(sizeof(WORD) * (max_words_doc + 10));

	printf("\nLine count = %i\nmax_docs = %i\n", lld, max_docs);

	model = read_model(classify_modelfile);

	if (model->kernel_parm.kernel_type == 0) { /* linear kernel */
		/* compute weight vector */
		add_weight_vector_to_linear_model(model);
	}

	if (verbosity >= 2) {
		printf("Classifying test examples..");
		fflush(stdout);
	}

	if ((docfl = fopen(classify_docfile, "r")) == NULL) {
		perror(classify_docfile);
		exit(1);
	}
	if ((predfl = fopen(predictionsfile, "w")) == NULL) {
		perror(predictionsfile);
		exit(1);
	}

	while ((!feof(docfl)) && fgets(line, (int) lld, docfl)) {

		if (line[0] == '#') {
			comment_cnt++;
			continue;
		} /* line contains comments */

//		printf("comment_cnt = %d\n", comment_cnt);

//---------------------
		//printf("%s\n", line) ;
//		strcpy(relearn_add_string, line) ;			//	Only in case of single recall vector

//---------------------

		parse_document(line, words, &doc_label, &queryid, &slackid,
				&costfactor, &wnum, max_words_doc, &comment);
		totdoc++;
		if (model->kernel_parm.kernel_type == 0) { /* linear kernel */
			for (j = 0; (words[j]).wnum != 0; j++) { /* Check if feature numbers   */
				if ((words[j]).wnum > model->totwords) /* are not larger than in     */
					(words[j]).wnum = 0; /* model. Remove feature if   */
			} /* necessary.                 */
			doc = create_example(-1, 0, 0, 0.0,
					create_svector(words, comment, 1.0));
			t1 = get_runtime();
			dist = classify_example_linear(model, doc);
			runtime += (get_runtime() - t1);
			free_example(doc, 1);
		} else { /* non-linear kernel */
			doc = create_example(-1, 0, 0, 0.0,
					create_svector(words, comment, 1.0));
			t1 = get_runtime();
			dist = classify_example(model, doc);
			runtime += (get_runtime() - t1);
			free_example(doc, 1);
		}
		if (dist > 0) {
			if (pred_format == 0) { /* old weired output format */
				fprintf(predfl, "%.8g:+1 %.8g:-1\n", dist, -dist);
			}
			if (doc_label > 0)
				correct++;
			else
				incorrect++;
			if (doc_label > 0)
				res_a++;
			else
				res_b++;
		} else {
			if (pred_format == 0) { /* old weired output format */
				fprintf(predfl, "%.8g:-1 %.8g:+1\n", -dist, dist);
			}
			if (doc_label < 0)
				correct++;
			else
				incorrect++;
			if (doc_label > 0)
				res_c++;
			else
				res_d++;
		}

/*----------------------------------------------------------*/
		if (pred_format == 1) { /* output the value of decision function */
//			printf("%d:%d Printing format Sri\n", itr, i++);
			fprintf(predfl, "%.8g\n", dist);

//			(dist > 0.0) ? (pred_arr[row++][col] = dist) : (pred_arr[row++][col] = (0.0 - dist));
			pred_arr[row++][col] = (dist > 0.0) ? (dist) : (0.0 - dist);

		}
/*----------------------------------------------------------*/

		if ((int) (0.01 + (doc_label * doc_label)) != 1) {
			no_accuracy = 1;
		} /* test data is not binary labeled */
		if (verbosity >= 2) {
			if (totdoc % 100 == 0) {
				printf("%ld..", totdoc);
				fflush(stdout);
			}
		}
	}

	fclose(predfl);
	fclose(docfl);
	free(line);
	free(words);
	free_model(model, 1);

	if (verbosity >= 2) {
		printf("done\n");

		/*   Note by Gary Boone                     Date: 29 April 2000        */
		/*      o Timing is inaccurate. The timer has 0.01 second resolution.  */
		/*        Because classification of a single vector takes less than    */
		/*        0.01 secs, the timer was underflowing.                       */
		printf("Runtime (without IO) in cpu-seconds: %.2f\n",
				(float) (runtime / 100.0));

	}
	if ((!no_accuracy) && (verbosity >= 1)) {
		printf(
				"Accuracy on test set: %.2f%% (%ld correct, %ld incorrect, %ld total)\n",
				(float) (correct) * 100.0 / totdoc, correct, incorrect, totdoc);

		printf("Precision/recall on test set: %.2f%%/%.2f%%\n",
				(float) (res_a) * 100.0 / (res_a + res_b),
				(float) (res_a) * 100.0 / (res_a + res_c));
	}

	*max_docs_cnt = max_docs;
	*max_comment_cnt = comment_cnt ;

	return (0);
}

/************************************************************************************************************************************************/


void read_input_parameters_classify(int argc, char **argv, char *classify_docfile,
		char *classify_modelfile, char *predictionsfile, long int *verbosity,
		long int *pred_format) {
	long i;

	//-------------------
	/* set default */
	strcpy(classify_modelfile, "svm_model");
	strcpy(predictionsfile, "svm_predictions");

	(*verbosity) = 2;
	(*pred_format) = 1;

	for (i = 1; (i < argc) && ((argv[i])[0] == '-'); i++) {
		switch ((argv[i])[1]) {
		case 'h':
			print_help_classify();
			exit(0);
		case 'v':
			i++;
			(*verbosity) = atol(argv[i]);
			break;
		case 'f':
			i++;
			(*pred_format) = atol(argv[i]);
			break;
		default:
			printf("\nUnrecognized option %s!\n\n", argv[i]);
			print_help_classify();
			exit(0);
		}
	}

	if ((i + 1) >= argc) {
		printf("\nNot enough input parameters!\n\n");
		print_help_classify();
		exit(0);
	}

	//-------------------
	strcpy(classify_docfile, argv[i]);
	strcpy(classify_modelfile, argv[i + 1]);
	if ((i + 2) < argc) {
		strcpy(predictionsfile, argv[i + 2]);
	}

	if (((*pred_format) != 0) && ((*pred_format) != 1)) {
		printf("\nOutput format can only take the values 0 or 1!\n\n");
		print_help_classify();
		exit(0);
	}
}

/************************************************************************************************************************************************/

void print_help_classify(void) {
	printf(
			"\nSVM-light %s: Support Vector Machine, classification module     %s\n",
			VERSION, VERSION_DATE);
	copyright_notice();
	printf(
			"   usage: svm_classify [options] example_file model_file output_file\n\n");
	printf("options: -h         -> this help\n");
	printf("         -v [0..3]  -> verbosity level (default 2)\n");
	printf("         -f [0,1]   -> 0: old output format of V1.0\n");
	printf(
			"                    -> 1: output the value of decision function (default)\n\n");
}


/************************************************************************************************************************************************/

void print_pred_matrix(double pred_arr[200][200], int *dissagr_arr, int max_docs, int comment_cnt)
{
	int row_idx, col_idx ;

	//	for (row_idx = 0; row_idx < (max_docs - comment_cnt); row_idx++)
	//		printf("pred_arr[%d][%d] = %0.8g\n", row_idx, col_idx, pred_arr[row_idx][col_idx]);

		printf("\n---------- Full Matrix ----------\n") ;
		for (row_idx = 0; row_idx < (max_docs - comment_cnt); row_idx++)
		{
			for (col_idx = 0; col_idx < MAX_MODEL_CNT; col_idx++)
				printf("pred_arr[%d][%d] = %0.8g\t", row_idx, col_idx, pred_arr[row_idx][col_idx]);
			printf("\n") ;
		}

}

/************************************************************************************************************************************************/

void find_max_wt_pred(double pred_arr[200][200], int *dissagr_arr, int max_docs, int comment_cnt)
{
int row_idx ;

	for (row_idx = 0; row_idx < (max_docs - comment_cnt); row_idx++)
		dissagr_arr[row_idx] = ( pred_arr[row_idx][0]>pred_arr[row_idx][1] ) ? ( (pred_arr[row_idx][0]>pred_arr[row_idx][2])? 1 : 3 ) :  ( (pred_arr[row_idx][1]>pred_arr[row_idx][2])? 2:3 ) ;


//	if(itr == MAX_MODEL_CNT-1)			//	After all model files have been traversed through
		for (row_idx = 0; row_idx < (max_docs - comment_cnt); row_idx++)
		{
			(dissagr_arr[row_idx]==1)? printf("Mixer") : ((dissagr_arr[row_idx]==2)? printf("Micro"): printf("Fridge"))  ;
			printf("\t\tdissagr_arr[%d] = %d\n", row_idx, dissagr_arr[row_idx]);
		}

}

/************************************************************************************************************************************************/

void print_confu_matrix(void)
{


}

/************************************************************************************************************************************************/
//-------- Incremental learning addition ------

int add_to_model(int *dissagr_arr, char* classify_Param_Names[], int max_docs, int comment_cnt)
{
	int row_idx;
	char model_file_name[20] ;
	static int init_flag = 0;
	FILE *model_fp, *recall_fp ;
	char line[200] ;

	recall_fp = fopen(classify_Param_Names[1], "r");

	if( !recall_fp )
	{
		printf("\nRecall file %s File not found\n", classify_Param_Names[1]);
		getch() ;
		return -1 ;
	}

//	printf("%s\n", classify_Param_Names[1]) ;

//	printf("max_docs = %d\ncomment_cnt = %d", max_docs, comment_cnt) ;

	for( row_idx=0 ; row_idx < (max_docs - comment_cnt) ; row_idx++)
	{
		if (row_idx == (max_docs-comment_cnt-1) )
			init_flag = 2;

		fgets(line, 200, recall_fp) ;

		if (line[0] == '#')			/* line contains comments */
		{
			row_idx-- ;
			continue;
		}

		switch(dissagr_arr[row_idx])
		{
			case 1:	//	Mixer
//				puts("------Model addition Mixer\n") ;
				strcpy(model_file_name, classify_Param_Names[2] ) ;
				break ;

			case 2:	//	Micro
//				puts("------Model addition Micro\n") ;
				strcpy(model_file_name, classify_Param_Names[4] ) ;
				break ;

			case 3:	//	Fridge
//				puts("------Model addition Fridge\n") ;
				strcpy(model_file_name, classify_Param_Names[6] ) ;
				break ;

			default: //	Unknown/New device
				break ;
		}

		model_fp = fopen(model_file_name,"a");

		if (!model_fp)
		{
			printf("\nModel file %s File not found\n", model_file_name);
			getch () ;
			return -1 ;
		}

//		fprintf(model_fp,"%s", relearn_add_string)	;					//	Only in case of single recall vector
//		printf("%s\n", line) ;
		fprintf(model_fp,"%s", line)	;

		fclose(model_fp) ;
	}

	fclose(recall_fp) ;

return 0 ;
}

/************************************************************************************************************************************************/

int prep_mod_file(char *model_file_name)
{
	FILE *model_file_ptr;
	char model_line[200] ;
	int pos, pos_prev ;

	model_file_ptr = fopen(model_file_name, "r+");

	if (!model_file_ptr)
	{
		printf("\nModel file for Re-learn %s File not found\n", model_file_name);
		getch() ;
		return -1 ;
	}

	while (!feof(model_file_ptr) )
	{
		pos_prev = pos = ftell(model_file_ptr) ;
		fgets(model_line, 200, model_file_ptr) ;
		pos = ftell(model_file_ptr) ;

//		printf("\n\t======> In File : %s\t#%d\tline : %s", model_file_name, pos, model_line) ;
//		model_line[0] = '#' ;
//		printf("\tModified line : %s", model_line) ;

		fseek(model_file_ptr, pos_prev, SEEK_SET);
		fputc('#', model_file_ptr) ;
//		fputs(model_line, model_file_ptr) ;
		fseek(model_file_ptr, pos, SEEK_SET);

		if( strstr( (char *)model_line, "threshold") )
			break ;
	}
	fclose(model_file_ptr) ;
	return 0 ;
}

/************************************************************************************************************************************************/



/************************************************************************************************************************************************/

