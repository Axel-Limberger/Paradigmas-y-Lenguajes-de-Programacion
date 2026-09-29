/*
 * mpi_pin.c
 * Búsqueda de PIN por fuerza bruta usando MPI.
 *
 * Compilar:  mpicc -o mpi_pin mpi_pin.c
 * Ejecutar:  mpirun -np 4 ./mpi_pin
 */

#include <mpi.h>
#include <stdio.h>

/* ===== PIN OBJETIVO (modifica este valor para probar) ===== */
#define PIN_OBJETIVO 5831
/* ========================================================== */

int main(int argc, char *argv[]) {
    int rank, size;
    int encontrado = 0;       /* flag global de éxito           */
    int pin_hallado = -1;     /* valor del PIN cuando se halla  */
    int ganador = -1;         /* rango del proceso que lo halló */

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* --- Distribución del rango 0000-9999 entre procesos --- */
    int total    = 10000;
    int bloque   = total / size;
    int inicio   = rank * bloque;
    int fin      = (rank == size - 1) ? total - 1 : inicio + bloque - 1;

    /* Barrera para que todos impriman antes de que empiece la búsqueda */
    MPI_Barrier(MPI_COMM_WORLD);

    printf("Proceso %d buscando desde %04d hasta %04d\n", rank, inicio, fin);
    fflush(stdout);

    MPI_Barrier(MPI_COMM_WORLD);   /* esperar a que todos impriman */

    /* --- Inicio del cronómetro (solo proceso 0) --- */
    double t_inicio = MPI_Wtime();

    /* --- Búsqueda local --- */
    for (int intento = inicio; intento <= fin && !encontrado; intento++) {
        /*
         * Aquí iría la lógica real de verificación del PIN.
         * Para este ejercicio la "verificación" es una comparación directa.
         */
        if (intento == PIN_OBJETIVO) {
            encontrado  = 1;
            pin_hallado = intento;
        }
    }

    /* --- Comunicar el hallazgo a todos los procesos --- */
    /*
     * Usamos MPI_Allreduce con MAX para que todos sepan si alguien encontró el PIN.
     * El proceso ganador es el que tiene 'encontrado == 1'.
     */
    int encontrado_global = 0;
    MPI_Allreduce(&encontrado, &encontrado_global, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

    /* Determinar qué proceso lo encontró y cuál es el PIN */
    int datos_local[2]  = { encontrado ? rank       : -1,
                             encontrado ? pin_hallado : -1 };
    int datos_global[2] = { -1, -1 };
    MPI_Allreduce(datos_local, datos_global, 2, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

    ganador     = datos_global[0];
    pin_hallado = datos_global[1];

    /* --- Resultado final (solo proceso 0 imprime) --- */
    double t_fin = MPI_Wtime();

    if (rank == 0) {
        printf("\n");
        if (encontrado_global) {
            printf("PIN encontrado : %04d\n", pin_hallado);
            printf("Encontrado por : Proceso %d\n", ganador);
        } else {
            printf("PIN no encontrado en el rango.\n");
        }
        printf("Tiempo total   : %.6f segundos\n", t_fin - t_inicio);
        printf("Procesos usados: %d\n", size);
    }

    MPI_Finalize();
    return 0;
}
