#include <mpi.h>
#include <mpi-ext.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <stdbool.h>

#define N 30000
#define MAX_ITER 100
#define CKPT_STEP 10
#define Max(a, b) ((a)>(b) ? (a) : (b))
#define world (worldc[worldi])

static void app_reload_ckpt(MPI_Comm comm);
static int app_needs_repair(MPI_Comm comm);
static void errhandler_respawn(MPI_Comm* pcomm, int* errcode, ...);
static int MPIX_Comm_replace(MPI_Comm comm, MPI_Comm *newcomm);

char** gargv;
static int iteration = 0, ckpt_iteration = 0, last_dead = MPI_PROC_NULL;
static jmp_buf stack_jmp_buf;

static MPI_Comm worldc[2] = { MPI_COMM_NULL, MPI_COMM_NULL };
static int worldi = 0;

int main(int argc, char **argv) {
    gargv = argv;

    int rank, size, startrow, lastrow, nrow;
    double start_time;
    double maxeps = 0.1e-7, eps, S = 0;
    MPI_Request req[2] = {MPI_REQUEST_NULL, MPI_REQUEST_NULL};
    MPI_Status status[2];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Barrier(MPI_COMM_WORLD);

    startrow = (rank * N) / size;
    lastrow = (((rank + 1) * N) / size) - 1;
    nrow = lastrow - startrow + 1;
    double A[nrow+2][N], B[nrow+2][N], e, loceps;

    //init
    for(int i=0; i<=nrow+1; i++)
        for(int j=0; j<=N-1; j++)
            if (rank==0 && i<=1 || rank==size-1 && i>=nrow)
                A[i][j] = 0;
            else
                A[i][j] = j == 0 || j == N - 1 ? 0. : 1. + i + j + startrow;
    //init

    int allowed_to_kill = 1;
    MPI_Errhandler errh;
    MPI_Comm parent;
    int do_recover = 0;
    double *bckpt = NULL;

    MPI_Comm_create_errhandler(&errhandler_respawn, &errh);

    MPI_Comm_get_parent( &parent );
    if( MPI_COMM_NULL == parent ) {
        MPI_Comm_dup( MPI_COMM_WORLD, &world );
    } else {
        allowed_to_kill = 0;
        ckpt_iteration = MAX_ITER;
        app_needs_repair(MPI_COMM_NULL);
    }

    MPI_Comm_rank(world, &rank);
    MPI_Comm_size(world, &size);

    bckpt = (double*)malloc(sizeof(double) * (nrow+2) * N);

    for(int i=0; i<=nrow+1; i++)
        for(int j=0; j<=N-1; j++)
            B[i][j] = A[i][j];

// restart:
    do_recover = _setjmp(stack_jmp_buf);
    MPI_Comm_set_errhandler( world, errh );

    if( do_recover || (MPI_COMM_NULL != parent)) {
        MPI_Irecv(&B, (nrow+2) * N, MPI_DOUBLE, (rank + 1) % size, 111, world, &req[0]);

        if( rank == last_dead ) {
            MPI_Send(bckpt, 0, MPI_DOUBLE, (rank - 1 + size) % size, 111, world);
        } else {
            MPI_Send(bckpt, (nrow+2) * N, MPI_DOUBLE, (rank - 1 + size) % size, 111, world);
        }
        MPI_Wait(&req[0], MPI_STATUS_IGNORE);
        goto do_sor;
    }

    start_time = MPI_Wtime();
    for(iteration=1; iteration<=MAX_ITER/size; iteration++) {
        loceps = 0.;
//relax
        if (rank)
            MPI_Recv(&A[0][0], N, MPI_DOUBLE, rank-1, 1215, world, &status[0]);

        for(int i=1; i<=nrow; i++) {
            if (!(rank==0 && i==1) && !(rank==size-1 && i==nrow))
                for(int j=1; j<=N-2; j++) {
                    e=A[i][j];
                    A[i][j]=(A[i-1][j]+A[i+1][j]+A[i][j-1]+A[i][j+1])/4.;
                    loceps=Max(loceps, fabs(e-A[i][j]));
                }
        }
        if (rank!=size-1) {
            MPI_Send(&A[nrow][0], N, MPI_DOUBLE, rank+1, 1215, world);
            MPI_Recv(&A[nrow+1][0], N, MPI_DOUBLE, rank+1, 1216, world, &status[1]);
        }
        if (rank!=0)
            MPI_Send(&A[1][0], N, MPI_DOUBLE, rank-1, 1216, world);

        if( allowed_to_kill && (iteration == 50) ) {
            allowed_to_kill = 0;
            if( rank == 1 )
              raise(SIGKILL);
        }
//relax
        MPI_Barrier(world);

        if( (iteration != 0) && ((iteration % CKPT_STEP != 0)) ) {
            MPI_Irecv(bckpt, (nrow+2) * N, MPI_DOUBLE, (rank - 1 + size) % size, 111, world, &req[0]);
            MPI_Send(&A, (nrow+2) * N, MPI_DOUBLE, (rank + 1) % size, 111, world);
            MPI_Wait(&req[0], MPI_STATUS_IGNORE);
            ckpt_iteration = iteration;
        }

    do_sor:
        for(int i=0; i<=nrow+1; i++)
            for(int j=0; j<=N-1; j++)
                A[i][j] = B[i][j];

        MPI_Allreduce(&loceps, &eps, 1, MPI_DOUBLE, MPI_MAX, world);

        printf("%d: it=%4i   eps=%f\n", rank, iteration, eps);
        if (eps < maxeps)
            break;
    }

//verify
    double s_loc=0;
    for(int i=1; i<=nrow; i++)
        for(int j=0; j<=N-1; j++)
            s_loc += A[i][j]*(i+1)*(j+1)/(N*N);
    MPI_Allreduce(&s_loc, &S, 1, MPI_DOUBLE, MPI_SUM, world);
//verify

    if (!rank)
        printf("  S = %f\n", S);
    printf("%d: Time of task = %lf\n", rank, MPI_Wtime()-start_time);
    MPI_Finalize();
    return 0;
}

static void app_reload_ckpt(MPI_Comm comm) {
    MPI_Allreduce(&ckpt_iteration, &iteration, 1, MPI_INT, MPI_MIN, comm);
    iteration++;
}

static int app_needs_repair(MPI_Comm comm) {
    if( comm == world ) {
        worldi = (worldi+1) % 2;
        if( MPI_COMM_NULL != world ) {
            MPI_Comm_free(&world);
        }
        MPIX_Comm_replace(comm, &world);
        app_reload_ckpt(world);
        if( MPI_COMM_NULL == comm ) {
            return false;
        }
        _longjmp( stack_jmp_buf, 1 );
    }
    return true;
}

static void errhandler_respawn(MPI_Comm* pcomm, int* errcode, ...) {
    int eclass;
    MPI_Error_class(*errcode, &eclass);

    if( MPIX_ERR_PROC_FAILED != eclass &&
        MPIX_ERR_REVOKED != eclass ) {
        MPI_Abort(MPI_COMM_WORLD, *errcode);
    }
    MPIX_Comm_revoke(world);

    app_needs_repair(world);
}

static int MPIX_Comm_replace(MPI_Comm comm, MPI_Comm *newcomm) {
    MPI_Comm icomm, scomm, mcomm;
    MPI_Group cgrp, sgrp, dgrp;
    int rc, flag, rflag, i, nc, ns, nd, crank, srank, drank;

 redo:
    if( comm == MPI_COMM_NULL ) {
        MPI_Comm_get_parent(&icomm);
        scomm = MPI_COMM_WORLD;
        MPI_Recv(&crank, 1, MPI_INT, 0, 1, icomm, MPI_STATUS_IGNORE);
    } else {
        MPIX_Comm_shrink(comm, &scomm);
        MPI_Comm_size(scomm, &ns);
        MPI_Comm_size(comm, &nc);
        nd = nc-ns;
        if( 0 == nd ) {
            MPI_Comm_free(&scomm);
            *newcomm = comm;
            return MPI_SUCCESS;
        }
        MPI_Comm_set_errhandler( scomm, MPI_ERRORS_RETURN );

        rc = MPI_Comm_spawn(gargv[0], &gargv[1], nd, MPI_INFO_NULL,
                            0, scomm, &icomm, MPI_ERRCODES_IGNORE);
        flag = (MPI_SUCCESS == rc);
        MPIX_Comm_agree(scomm, &flag);
        if( !flag ) {
            if( MPI_SUCCESS == rc ) {
                MPIX_Comm_revoke(icomm);
                MPI_Comm_free(&icomm);
            }
            MPI_Comm_free(&scomm);
            goto redo;
        }

        MPI_Comm_rank(comm, &crank);
        MPI_Comm_rank(scomm, &srank);

        if(0 == srank) {
            MPI_Comm_group(comm, &cgrp);
            MPI_Comm_group(scomm, &sgrp);
            MPI_Group_difference(cgrp, sgrp, &dgrp);

            for(i=0; i<nd; i++) {
                MPI_Group_translate_ranks(dgrp, 1, &i, cgrp, &drank);
                MPI_Send(&drank, 1, MPI_INT, i, 1, icomm);
                last_dead = drank;
            }
            MPI_Group_free(&cgrp); MPI_Group_free(&sgrp); MPI_Group_free(&dgrp);
        }
    }

    rc = MPI_Intercomm_merge(icomm, 1, &mcomm);
    rflag = flag = (MPI_SUCCESS==rc);
    MPIX_Comm_agree(scomm, &flag);
    if( MPI_COMM_WORLD != scomm ) MPI_Comm_free(&scomm);
    MPIX_Comm_agree(icomm, &rflag);
    MPI_Comm_free(&icomm);
    if( !(flag && rflag) ) {
        if( MPI_SUCCESS == rc ) {
            MPI_Comm_free(&mcomm);
        }
        goto redo;
    }

    rc = MPI_Comm_split(mcomm, 1, crank, newcomm);
    flag = (MPI_SUCCESS==rc);
    MPIX_Comm_agree(mcomm, &flag);
    MPI_Comm_free(&mcomm);
    if( !flag ) {
        if( MPI_SUCCESS == rc ) {
            MPI_Comm_free( newcomm );
        }
        goto redo;
    }

    if( MPI_COMM_NULL != comm ) {
        MPI_Errhandler errh;
        MPI_Comm_get_errhandler( comm, &errh );
        MPI_Comm_set_errhandler( *newcomm, errh );
    }

    return MPI_SUCCESS;
}