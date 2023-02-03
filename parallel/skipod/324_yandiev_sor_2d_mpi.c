#include <mpi.h>
#include <math.h>
#include <stdio.h>
#define  Max(a,b) ((a)>(b)?(a):(b))

#define  N   30000
double maxeps=0.1e-7, eps, S=0;
int itmax=100;

int main(int argc, char **argv) {
	MPI_Status status[2];
	int myrank, ranksize, startrow, lastrow, nrow;
	double start_time;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &ranksize);
	MPI_Barrier(MPI_COMM_WORLD);
	startrow = (myrank * N) / ranksize;
	lastrow = (((myrank + 1) * N) / ranksize) - 1;
	nrow = lastrow - startrow + 1;
	double A[nrow+2][N], e, loceps;

	start_time = MPI_Wtime();
//init
	for(int i=0; i<=nrow+1; i++)
		for(int j=0; j<=N-1; j++)
			if (myrank==0 && i<=1 || myrank==ranksize-1 && i>=nrow)
				A[i][j] = 0;
			else
				A[i][j] = j == 0 || j == N - 1 ? 0. : 1. + i + j + startrow;
//init

	for(int it=1; it<=itmax/ranksize; it++) {
		loceps = 0.;		
//relax
		if (myrank)
			MPI_Recv(&A[0][0], N, MPI_DOUBLE, myrank-1, 1215, MPI_COMM_WORLD, &status[0]);

		for(int i=1; i<=nrow; i++) {
			if (!(myrank==0 && i==1) && !(myrank==ranksize-1 && i==nrow))
				for(int j=1; j<=N-2; j++) {
					e=A[i][j];
					A[i][j]=(A[i-1][j]+A[i+1][j]+A[i][j-1]+A[i][j+1])/4.;
					loceps=Max(loceps, fabs(e-A[i][j]));
				}
		}
		if (myrank!=ranksize-1) {
			MPI_Send(&A[nrow][0], N, MPI_DOUBLE, myrank+1, 1215, MPI_COMM_WORLD);
			MPI_Recv(&A[nrow+1][0], N, MPI_DOUBLE, myrank+1, 1216, MPI_COMM_WORLD, &status[1]);
		}
		if (myrank!=0)
			MPI_Send(&A[1][0], N, MPI_DOUBLE, myrank-1, 1216, MPI_COMM_WORLD);	
		MPI_Allreduce(&loceps, &eps, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
//relax
		printf("%d: it=%4i   eps=%f\n", myrank, it, eps);
		if (eps < maxeps) 
			break;
	}

//verify
	double s_loc=0;
	for(int i=1; i<=nrow; i++)
		for(int j=0; j<=N-1; j++)
			s_loc += A[i][j]*(i+1)*(j+1)/(N*N);
	MPI_Allreduce(&s_loc, &S, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
//verify

	if (!myrank)
		printf("  S = %f\n", S);
	printf("%d: Time of task = %lf\n", myrank, MPI_Wtime()-start_time);
	MPI_Finalize();
	return 0;
}