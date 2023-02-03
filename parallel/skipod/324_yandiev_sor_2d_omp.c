#include <stdio.h>
#include <math.h>
#include <omp.h>

#define  max(a,b) ((a)>(b)?(a):(b))
#define  min(a,b) ((a)<(b)?(a):(b))

#define  N   10000
double   maxeps = 0.1e-7;
int itmax = 100;
double eps;

double A [N][N];

void relax();
void init();
void verify(); 

int main(int an, char **as)
{
	double start_time, end_time; 
	start_time = omp_get_wtime();

	init();

	for(int it=1; it<=itmax; it++) {
		eps = 0.;
		relax();
		printf( "it=%4i   eps=%f\n", it, eps);
		if (eps < maxeps) 
			break;
	}

	verify();
	
	end_time = omp_get_wtime();
	printf("Total time was %f seconds.\n", end_time - start_time);
	return 0;
}


void init() {
	#pragma omp parallel for default(none) shared(A) if (N > 2000)
	{
		for(int i=0; i<=N-1; i++)
			for(int j=0; j<=N-1; j++)
				A[i][j] = i == 0 || i == N - 1 || j == 0 || j == N - 1 ? 0. : 1. + i + j;
	}
} 


void relax() {
	int *isync = (int *) malloc(omp_get_max_threads()*sizeof(int));
	int iam, numt, limit;
	double e;
	#pragma omp parallel private(iam, numt, limit, e)
	{
		double loceps;
		iam = omp_get_thread_num();
		numt = omp_get_num_threads();
		limit = min(numt-1, N-4);
		isync[iam]=0;
		#pragma omp barrier
	 	for (int i=1; i<N-2; i++) {
	 		if ((iam>0) && (iam<=limit)) {
	 			for (;isync[iam-1]==0;) {
	 				#pragma omp flush (isync)
				}
				isync[iam-1]=0;
	 			#pragma omp flush (isync)
			}
			#pragma omp flush (eps)
			loceps = eps;
			#pragma omp for schedule(static) nowait
	 			for (int j=1; j<N-2; j++) {
	 				e=A[i][j];
					A[i][j] = (A[i-1][j] + A[i+1][j] + A[i][j-1] + A[i][j+1]) / 4.;
					loceps = max(loceps, fabs(e-A[i][j]));
				}
			#pragma omp flush (eps)
			eps = max(eps, loceps);
			#pragma omp flush (eps)
			if (iam<limit) {
				for (;isync[iam]==1;) {
					#pragma omp flush (isync)
				}
				isync[iam]=1;
				#pragma omp flush (isync)
			}
		}
	}
	free(isync);
}


void verify() { 
	double s=0.;
	#pragma omp parallel for default(none) shared(A) reduction(+: s) if (N > 2000)
	{
		for(int i=0; i<=N-1; i++)
			for(int j=0; j<=N-1; j++)
				s += A[i][j]*(i+1)*(j+1)/(N*N);
	}
	printf("  S = %f\n", s);
} 