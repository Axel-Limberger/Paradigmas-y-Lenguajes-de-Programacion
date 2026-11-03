/*
 * openmp_pin.c
 * Búsqueda de PIN usando OpenMP.
 *
 * Compilar:  gcc -fopenmp -o openmp_pin openmp_pin.c
 * Ejecutar:  ./openmp_pin
 *            OMP_NUM_THREADS=4 ./openmp_pin
 */

#include <omp.h>
#include <stdio.h>

/* ===== PIN OBJETIVO (8 dígitos) ===== */
#define PIN_OBJETIVO 87654321

int main(void) {

    int pin_hallado  = -1;   // compartido entre hilos
    int ganador      = -1;   // hilo que lo encontró
    int encontrado   = 0;    // bandera de corte
    int hilos_usados = 1;    // cantidad real utilizada

    double t_inicio = omp_get_wtime();

    /* La directiva parallel lanza todos los hilos configurados.
     * Cada hilo calcula su propio rango y trabaja solo sobre él. */
    #pragma omp parallel shared(pin_hallado, ganador, encontrado, hilos_usados)
    {
        int hilo = omp_get_thread_num();
        int total_hilos = omp_get_num_threads();
        int total = 100000000; // 10^8 combinaciones posibles

        /* Todos los hilos conocen total_hilos.
         * Solo el hilo 0 guarda el dato para mostrarlo al final.*/
        if (hilo == 0) {
            hilos_usados = total_hilos;
        }

        /* Distribución del rango entre hilos */
        int bloque = total / total_hilos;
        int inicio = hilo * bloque;
        int fin = (hilo == total_hilos - 1)
                    ? total - 1
                    : inicio + bloque - 1;

        // Todos deben haber calculado su rango antes de imprimir
        #pragma omp barrier

        // Solo un hilo a la vez imprime para evitar mezclar líneas
        #pragma omp critical
        {
            printf(
                "Hilo %d buscando desde %08d hasta %08d\n",
                hilo,
                inicio,
                fin
            );
        }

        // Esperar a que todos impriman antes de buscar
        #pragma omp barrier

        // -- Búsqueda local
        for (int intento = inicio; intento <= fin; intento++) {

            /* Lectura sincronizada de la bandera compartida.
             * Si otro hilo encontró el PIN, se deja de buscar. */
            int detener;

            #pragma omp atomic read
            detener = encontrado;

            if (detener) {
                break;
            }

            if (intento == PIN_OBJETIVO) {

                // Solo un hilo puede registrar el resultado dentro de la sección crítica.
                #pragma omp critical
                {
                    if (pin_hallado == -1) {
                        pin_hallado = intento;
                        ganador = hilo;

                        // Se informa de forma sincronizada al resto de los hilos que el PIN fue hallado.
                        #pragma omp atomic write
                        encontrado = 1;
                    }
                }
            }
        }

    }

    double t_fin = omp_get_wtime();

    printf("\n");

    if (pin_hallado != -1) {
        printf("PIN encontrado : %08d\n", pin_hallado);
        printf("Encontrado por : Hilo %d\n", ganador);
    } else {
        printf("PIN no encontrado en el rango.\n");
    }

    printf("Tiempo total    : %.6f segundos\n", t_fin - t_inicio);
    printf("Hilos usados    : %d\n", hilos_usados);

    return 0;
}