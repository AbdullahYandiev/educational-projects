// В транспьютерной матрице размером 4*4, в каждом узле которой находится один процесс, необходимо выполнить операцию редукции MPI_MAXLOC, определить глобальный максимум и соответствующих ему индексов. Каждый процесс предоставляет свое значение и свой номер в группе. Для всех процессов операция редукции должна возвратить значение максимума и номер первого процесса с этим значением.
// Реализовать программу, моделирующую выполнение данной операции на транспьютерной матрице при помощи пересылок MPI типа точка-точка.
// Оценить сколько времени потребуется для выполнения операции редукции, если все процессы выдали эту операцию редукции одновременно. Время старта равно 100, время передачи байта равно 1 (Ts=100,Tb=1). Процессорные операции, включая чтение из памяти и запись в память, считаются бесконечно быстрыми.

#include <mpi.h>
#include <stdio.h>

#define N 4
#define MAX(X, Y) (X[0] == Y[0] ? (X[1] < Y[1] ? X : Y) : (X[0] > Y[0] ? X : Y))

int main(int argc, char **argv) {
    int rank, size;
    int send[2], recv[2];
    int* max;
    MPI_Status status;
    int A[N][N] = {{5, 6, 7, 4},
                   {4, 7, 2, 1},
                   {7, 6, 8, 4},
                   {4, 3, 8, 1}};

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    send[0] = A[rank / 4][rank % 4];
    send[1] = rank;

    if (rank % 2 == 0) {
        MPI_Send(&send, 2, MPI_INT, rank + 1, 100, MPI_COMM_WORLD);
    } else {
        MPI_Recv(&recv, 2, MPI_INT, rank - 1, 100, MPI_COMM_WORLD, &status);
        max = MAX(send, recv);
        send[0] = max[0], send[1] = max[1];
    }

    if (rank % 2 == 1) {
        if (rank % 4 != 3) {
            MPI_Send(&send, 2, MPI_INT, rank + 2, 101, MPI_COMM_WORLD);
        } else {
            MPI_Recv(&recv, 2, MPI_INT, rank - 2, 101, MPI_COMM_WORLD, &status);
            max = MAX(send, recv);
            send[0] = max[0], send[1] = max[1];
        }
    }

    if (rank % 4 == 3) {
        if (rank % 8 != 7) {
            MPI_Send(&send, 2, MPI_INT, rank + 4, 102, MPI_COMM_WORLD);
            MPI_Recv(&recv, 2, MPI_INT, rank + 4, 103, MPI_COMM_WORLD, &status);
        } else {
            MPI_Send(&send, 2, MPI_INT, rank - 4, 103, MPI_COMM_WORLD);
            MPI_Recv(&recv, 2, MPI_INT, rank - 4, 102, MPI_COMM_WORLD, &status);
        }
        max = MAX(send, recv);
        send[0] = max[0], send[1] = max[1];

        if (rank < 8) {
            MPI_Send(&send, 2, MPI_INT, rank + 8, 104, MPI_COMM_WORLD);
            MPI_Recv(&recv, 2, MPI_INT, rank + 8, 105, MPI_COMM_WORLD, &status);
        } else {
            MPI_Send(&send, 2, MPI_INT, rank - 8, 105, MPI_COMM_WORLD);
            MPI_Recv(&recv, 2, MPI_INT, rank - 8, 104, MPI_COMM_WORLD, &status);
        }
        max = MAX(send, recv);
        send[0] = max[0], send[1] = max[1];
        MPI_Send(&send, 2, MPI_INT, rank - 1, 106, MPI_COMM_WORLD);
    }

    if (rank % 4 == 2) {
        MPI_Recv(&send, 2, MPI_INT, rank + 1, 106, MPI_COMM_WORLD, &status);
    }

    if (rank % 4 == 2 || rank % 4 == 3) {
        MPI_Send(&send, 2, MPI_INT, rank - 2, 107, MPI_COMM_WORLD);
    } else {
        MPI_Recv(&send, 2, MPI_INT, rank + 2, 107, MPI_COMM_WORLD, &status);
    }

    if (rank == 0) {
        printf("MAX = %d\nRANK = %d\n", send[0], send[1]);
    }

    MPI_Finalize();
    return 0;
}