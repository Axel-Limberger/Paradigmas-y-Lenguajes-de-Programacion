#include <mpi.h>
#include <stdio.h>

#define PIN_DIGITOS 8
#define PIN_OBJETIVO 58312344LL
#define BLOQUE_CONTROL 100000LL

int main(int argc, char *argv[]) {
    int rank, size;
    long long total = 1, bloque_base, resto, cantidad, inicio, fin;
    long long pin_local = -1, pin_global = -1;
    int encontrado_local = 0, encontrado_global = 0;
    int ganador_local = -1, ganador_global = -1;
    double tiempo_inicio, tiempo_fin, tiempo_local, tiempo_total;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (int i = 0; i < PIN_DIGITOS; i++) total *= 10;

    if (PIN_OBJETIVO < 0 || PIN_OBJETIVO >= total) {
        if (rank == 0)
            printf("Error: el PIN objetivo no corresponde a un PIN de %d digitos.\n", PIN_DIGITOS);
        MPI_Finalize();
        return 1;
    }

    bloque_base = total / size;
    resto = total % size;

    if (rank < resto) {
        cantidad = bloque_base + 1;
        inicio = rank * cantidad;
    } else {
        cantidad = bloque_base;
        inicio = resto * (bloque_base + 1) + (rank - resto) * bloque_base;
    }

    fin = inicio + cantidad - 1;
    printf("Proceso %d buscando desde %0*lld hasta %0*lld (%lld combinaciones)\n",
           rank, PIN_DIGITOS, inicio, PIN_DIGITOS, fin, cantidad);
    fflush(stdout);

    MPI_Barrier(MPI_COMM_WORLD);
    tiempo_inicio = MPI_Wtime();

    for (long long desplazamiento = 0;
         desplazamiento < bloque_base + (resto > 0 ? 1 : 0) && !encontrado_global;
         desplazamiento += BLOQUE_CONTROL) {

        if (desplazamiento < cantidad) {
            long long comienzo = inicio + desplazamiento;
            long long fin_bloque = comienzo + BLOQUE_CONTROL - 1;
            if (fin_bloque > fin) fin_bloque = fin;

            for (long long intento = comienzo; intento <= fin_bloque; intento++) {
                if (intento == PIN_OBJETIVO) {
                    encontrado_local = 1;
                    pin_local = intento;
                    ganador_local = rank;
                    break;
                }
            }
        }

        MPI_Allreduce(&encontrado_local, &encontrado_global, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    }

    MPI_Allreduce(&pin_local, &pin_global, 1, MPI_LONG_LONG_INT, MPI_MAX, MPI_COMM_WORLD);
    MPI_Allreduce(&ganador_local, &ganador_global, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

    tiempo_fin = MPI_Wtime();
    tiempo_local = tiempo_fin - tiempo_inicio;
    MPI_Reduce(&tiempo_local, &tiempo_total, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("\n");
        if (encontrado_global) {
            printf("PIN encontrado: %0*lld\n", PIN_DIGITOS, pin_global);
            printf("Encontrado por: Proceso %d\n", ganador_global);
        } else {
            printf("PIN no encontrado en el rango.\n");
        }
        printf("Tiempo total: %.6f segundos\n", tiempo_total);
        printf("Procesos usados: %d\n", size);
    }

    MPI_Finalize();
    return 0;
}