/*
 * mpi_pin.c
 * Busqueda paralela de un PIN usando MPI.
 *
 * Compilar:
 *      mpicc -o mpi_pin mpi_pin.c
 *
 * Ejecutar, por ejemplo, con 4 procesos:
 *      mpirun -np 4 ./mpi_pin
 */

#include <mpi.h>
#include <stdio.h>

/* CONFIGURACION */

#define PIN_DIGITOS 8

// PIN que queremos encontrar
#define PIN_OBJETIVO 87654321LL
#define BLOQUE_CONTROL 100000LL

int main(int argc, char *argv[]) {

    int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    // Calcular automaticamente la cantidad total de PIN posibles

    long long total = 1;

    for (int i = 0; i < PIN_DIGITOS; i++) {
        total *= 10;
    }

    // Validar que el PIN este dentro del rango

    if (PIN_OBJETIVO < 0 || PIN_OBJETIVO >= total) {

        if (rank == 0) {
            printf("Error: el PIN objetivo no corresponde a un PIN de %d digitos.\n",
                   PIN_DIGITOS);
        }

        MPI_Finalize();
        return 1;
    }

    // Distribucion equilibrada del trabajo

    long long bloque_base = total / size;
    long long resto = total % size;

    long long cantidad;
    long long inicio;


    /*
     * Si la division no es exacta, los primeros procesos
     * reciben una combinacion adicional.
     */

    if (rank < resto) {

        cantidad = bloque_base + 1;
        inicio = rank * cantidad;

    } else {

        cantidad = bloque_base;

        inicio =
            resto * (bloque_base + 1) +
            (rank - resto) * bloque_base;
    }


    long long fin = inicio + cantidad - 1;



    // Mostrar que parte le corresponde a cada proceso

    printf(
        "Proceso %d buscando desde %0*lld hasta %0*lld "
        "(%lld combinaciones)\n",
        rank,
        PIN_DIGITOS,
        inicio,
        PIN_DIGITOS,
        fin,
        cantidad
    );

    fflush(stdout);


    /*
     * Esta barrera se conserva solamente para que todos
     * comiencen aproximadamente al mismo tiempo antes
     * de medir el rendimiento.
     */

    MPI_Barrier(MPI_COMM_WORLD);


    double tiempo_inicio = MPI_Wtime();


    // Busqueda del PIN

    int encontrado_local = 0;
    int encontrado_global = 0;

    long long pin_local = -1;

    int ganador_local = -1;


    /*
     * Algunos procesos pueden tener una combinacion mas
     * que otros. Calculamos el mayor tamanio posible para
     * que todos realicen las mismas rondas de comunicacion.
     */

    long long cantidad_maxima =
        bloque_base + (resto > 0 ? 1 : 0);


    /*
     * La busqueda se realiza en bloques.
     *
     * Despues de cada bloque todos los procesos comprueban
     * si alguno encontro el PIN.
     */

    for (
        long long desplazamiento = 0;
        desplazamiento < cantidad_maxima && !encontrado_global;
        desplazamiento += BLOQUE_CONTROL
    ) {

        /*
         * Algunos procesos pueden no tener trabajo en
         * la ultima ronda.
         */

        if (desplazamiento < cantidad) {

            long long comienzo_bloque =
                inicio + desplazamiento;

            long long fin_bloque =
                comienzo_bloque + BLOQUE_CONTROL - 1;


            if (fin_bloque > fin) {
                fin_bloque = fin;
            }


            /* Buscar solamente dentro de este bloque */

            for (
                long long intento = comienzo_bloque;
                intento <= fin_bloque;
                intento++
            ) {

                if (intento == PIN_OBJETIVO) {

                    encontrado_local = 1;
                    pin_local = intento;
                    ganador_local = rank;

                    break;
                }
            }
        }


        /*
         * Cada proceso informa si encontro el PIN.
         *
         * MPI_MAX dara 1 si al menos un proceso
         * tiene encontrado_local == 1.
         */

        MPI_Allreduce(
            &encontrado_local,
            &encontrado_global,
            1,
            MPI_INT,
            MPI_MAX,
            MPI_COMM_WORLD
        );
    }


    // Determinar quien encontro el PIN

    long long pin_global = -1;
    int ganador_global = -1;


    MPI_Allreduce(
        &pin_local,
        &pin_global,
        1,
        MPI_LONG_LONG,
        MPI_MAX,
        MPI_COMM_WORLD
    );


    MPI_Allreduce(
        &ganador_local,
        &ganador_global,
        1,
        MPI_INT,
        MPI_MAX,
        MPI_COMM_WORLD
    );


    // Medicion del tiempo

    double tiempo_fin = MPI_Wtime();

    double tiempo_local =
        tiempo_fin - tiempo_inicio;

    double tiempo_total;


    /*
     * Tomamos el tiempo del proceso que mas tardo.
     * Ese representa mejor el tiempo real de la ejecucion
     * paralela completa.
     */

    MPI_Reduce(
        &tiempo_local,
        &tiempo_total,
        1,
        MPI_DOUBLE,
        MPI_MAX,
        0,
        MPI_COMM_WORLD
    );


    //Resultado

    if (rank == 0) {

        printf("\n");

        if (encontrado_global) {

            printf(
                "PIN encontrado : %0*lld\n",
                PIN_DIGITOS,
                pin_global
            );

            printf(
                "Encontrado por : Proceso %d\n",
                ganador_global
            );

        } else {

            printf(
                "PIN no encontrado en el rango.\n"
            );
        }


        printf(
            "Tiempo total     : %.6f segundos\n",
            tiempo_total
        );

        printf(
            "Procesos usados  : %d\n",
            size
        );
    }

    MPI_Finalize();

    return 0;
}