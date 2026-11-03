#include <mpi.h>
#include <stdio.h>

/* Configuración de la búsqueda. */
#define PIN_DIGITOS 8
#define PIN_OBJETIVO 87654321LL
#define BLOQUE_CONTROL 100000LL

int main(int argc, char *argv[]) {
    int rango_proceso;
    int cantidad_procesos;
    long long total_combinaciones = 1;
    long long combinaciones_base;
    long long procesos_con_combinacion_extra;
    long long combinaciones_locales;
    long long inicio_local;
    long long fin_local;
    long long pin_local = -1;
    long long pin_global = -1;
    int encontrado_local = 0;
    int encontrado_global = 0;
    int proceso_ganador_local = -1;
    int proceso_ganador_global = -1;
    double tiempo_inicio;
    double tiempo_fin;
    double tiempo_local;
    double tiempo_total;

    // Inicializa MPI y obtiene la identidad y el número total de procesos.
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rango_proceso);
    MPI_Comm_size(MPI_COMM_WORLD, &cantidad_procesos);

    // Un PIN de N dígitos produce 10^N combinaciones, incluyendo los ceros iniciales.
    for (int i = 0; i < PIN_DIGITOS; i++) {
        total_combinaciones = total_combinaciones * 10;
    }

    // Verifica que el PIN configurado pueda representarse con la cantidad de dígitos indicada.
    if (PIN_OBJETIVO < 0 || PIN_OBJETIVO >= total_combinaciones) {
        if (rango_proceso == 0) {
            printf("Error: el PIN objetivo no corresponde a un PIN de %d digitos.\n", PIN_DIGITOS);
        }
        MPI_Finalize();
        return 1;
    }

    /* Divide las combinaciones entre los procesos. Los primeros procesos reciben
     * una combinación adicional cuando la división no es exacta. */
    combinaciones_base = total_combinaciones / cantidad_procesos;
    procesos_con_combinacion_extra = total_combinaciones % cantidad_procesos;

    if (rango_proceso < procesos_con_combinacion_extra) {
        combinaciones_locales = combinaciones_base + 1;
        inicio_local = rango_proceso * combinaciones_locales;
    } else {
        combinaciones_locales = combinaciones_base;
        inicio_local = procesos_con_combinacion_extra * (combinaciones_base + 1) +
                       (rango_proceso - procesos_con_combinacion_extra) * combinaciones_base;
    }

    fin_local = inicio_local + combinaciones_locales - 1;
    printf("Proceso %d buscando desde %0*lld hasta %0*lld\n",
           rango_proceso, PIN_DIGITOS, inicio_local, PIN_DIGITOS, fin_local);
    fflush(stdout);

    /* Sincroniza la salida de todos los procesos antes de iniciar la medición. */
    MPI_Barrier(MPI_COMM_WORLD);
    tiempo_inicio = MPI_Wtime();

    /* Todos los procesos participan en el mismo número de rondas. Este número
     * alcanza para cubrir el rango más grande asignado a cualquier proceso. */
    long long combinaciones_maximas = combinaciones_base;
    if (procesos_con_combinacion_extra > 0) {
        combinaciones_maximas++;
    }
    long long desplazamiento = 0;

    /* Se procesa un bloque y luego se informa a todos los procesos si alguno
     * encontró el PIN. Así se evita continuar innecesariamente en rondas futuras. */
    while (desplazamiento < combinaciones_maximas &&
           !encontrado_global) {
        if (desplazamiento < combinaciones_locales) {
            long long inicio_bloque = inicio_local + desplazamiento;
            long long fin_bloque = inicio_bloque + BLOQUE_CONTROL - 1;
            if (fin_bloque > fin_local) {
                fin_bloque = fin_local;
            }

            // Busca secuencialmente el PIN dentro del bloque local actual.
            for (long long intento = inicio_bloque; intento <= fin_bloque; intento++) {
                if (intento == PIN_OBJETIVO) {
                    encontrado_local = 1;
                    pin_local = intento;
                    proceso_ganador_local = rango_proceso;
                    break;
                }
            }
        }

        // MPI_MAX funciona como un OR para estas banderas (0 = falso, 1 = verdadero).
        MPI_Allreduce(&encontrado_local, &encontrado_global, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

        desplazamiento += BLOQUE_CONTROL;
    }

    // Reúne el PIN y el proceso que lo encontró. -1 representa "no encontrado".
    MPI_Allreduce(&pin_local, &pin_global, 1, MPI_LONG_LONG_INT, MPI_MAX, MPI_COMM_WORLD);
    MPI_Allreduce(&proceso_ganador_local, &proceso_ganador_global, 1, MPI_INT, MPI_MAX,
                  MPI_COMM_WORLD);

    // El tiempo total es el de la búsqueda del proceso más lento.
    tiempo_fin = MPI_Wtime();
    tiempo_local = tiempo_fin - tiempo_inicio;
    MPI_Reduce(&tiempo_local, &tiempo_total, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    // Solo el proceso 0 muestra el resultado global para evitar mensajes duplicados.
    if (rango_proceso == 0) {
        printf("\n");
        if (encontrado_global) {
            printf("PIN encontrado: %0*lld\n", PIN_DIGITOS, pin_global);
            printf("Encontrado por: Proceso %d\n", proceso_ganador_global);
        } else {
            printf("PIN no encontrado en el rango.\n");
        }
        printf("Tiempo total: %.6f segundos\n", tiempo_total);
        printf("Procesos usados: %d\n", cantidad_procesos);
    }

    // Libera los recursos de MPI antes de finalizar el programa.
    MPI_Finalize();
    return 0;
}