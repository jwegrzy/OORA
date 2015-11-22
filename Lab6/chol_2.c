#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "wrapper.c"

/* for Fortran - j*n + i */
//#define IDX(i, j, n)	(((i) * (n)) + (j))
#define IDX(i, j, n) (((j)+ (i)*(n)))

static double gtod_ref_time_sec = 0.0;

/* Adapted from the bl2_clock() routine in the BLIS library */

double dclock()
{
	double         the_time, norm_sec;
	struct timeval tv;
	gettimeofday( &tv, NULL );
	if ( gtod_ref_time_sec == 0.0 )
		gtod_ref_time_sec = ( double ) tv.tv_sec;
	norm_sec = ( double ) tv.tv_sec - gtod_ref_time_sec;
	the_time = norm_sec + tv.tv_usec * 1.0e-6;
	return the_time;
}

int
chol(double *A, unsigned int n)
{
	register unsigned int i;
	register unsigned int j;
	register unsigned int k;

	for (j = 0; j < n; j++) {
		for (i = j; i < n; i++) {
			for (k = 0; k < j; ++k) {
				A[IDX(i, j, n)] -= A[IDX(i, k, n)] *
				A[IDX(j, k, n)];
			}
		}

		if (A[IDX(j, j, n)] < 0.0) {
			return (1);
		}

		A[IDX(j, j, n)] = sqrt(A[IDX(j, j, n)]);
		for (i = j + 1; i < n; i++)
			A[IDX(i, j, n)] /= A[IDX(j, j, n)];
	}

	return (0);
}

double* load_matrix(char* filename, int n){

	char *line = NULL;
	char *token = NULL;
	size_t len = 0;
	ssize_t read;
	FILE *fp;
	double *A;
	char newline[2] = "\n";
	int i = 0;
	int j = 0;


	fp = fopen(filename, "r");
	if (fp == NULL){
		printf("Cannot open input file\n");
		exit(1);
	}


	A = calloc(n*n, sizeof(double));
	assert(A != NULL);

	while ((read = getline(&line, &len, fp)) != -1){
		while ((token = strsep(&line, ",")) != NULL){
			if (strcmp(token, newline) != 0){
				A[IDX(i, j, n)] = atof(token);
				j++;
				if (j == n){
					j = 0;
					i++;
				}
			}
		}
	}

	fclose(fp);
	if (line)
		free(line);

	return A;
}

void check(double* A, int n){
	int i, j;
	double check=0.0;
  	for(i=0;i<n;i++){
    	for(j=0;j<n;j++){
      		check+=A[IDX(i,j,n)];
    	}
  	}
  	fprintf(stderr, "check %le \n",check); 
}

int
main(int argc, char** argv)
{
	double *A;
	int n, ret, event;
	double startTime;
	double endTime;
	long long value;

	n = atoi(argv[2]);
	A = load_matrix(argv[1], n);
	event = atoi(argv[3]);
	if (event != 5) {
  		papi_init(event);
	  	papi_start();
	} else {
		startTime = dclock();
	}
	ret = chol(A, n);
	if (event != 5) {
		value = papi_stop();
		printf("%lld\n", value);
	} else {
		endTime = dclock();
		printf("%lf\n", endTime - startTime);
	}
	fprintf(stderr, "RET:%d\n", ret);
	check(A,n);
	free(A);
	return 0;
}

