#include <mpi.h>

#include <stdio.h>

int main(int argc, char *argv[]) {
    int rango, procesos;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango);
    MPI_Comm_size(MPI_COMM_WORLD, &procesos);

    printf("Hola desde el proceso %d de %d\n", rango, procesos);

    MPI_Finalize();
    return 0;
}