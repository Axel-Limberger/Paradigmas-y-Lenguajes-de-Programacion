/*
 * openmp_pin.c
 * Búsqueda de PIN por fuerza bruta usando OpenMP.
 *
 * Compilar:  gcc -fopenmp -o openmp_pin openmp_pin.c
 * Ejecutar:  ./openmp_pin          (usa los hilos por defecto del sistema)
 *            OMP_NUM_THREADS=4 ./openmp_pin   (forzar 4 hilos)
 */

#include <omp.h>
#include <stdio.h>

/* ===== PIN OBJETIVO (modifica este valor para probar) ===== */
#define PIN_OBJETIVO 5831
/* ========================================================== */

int main(void) {
    int pin_hallado = -1;   /* compartido entre hilos */
    int ganador     = -1;   /* hilo que lo encontró   */

    double t_inicio = omp_get_wtime();

    /*
     * La directiva 'parallel' lanza todos los hilos configurados.
     * Cada hilo calcula su propio rango y trabaja solo sobre él.
     */
    #pragma omp parallel shared(pin_hallado, ganador)
    {
        int hilo  = omp_get_thread_num();
        int total_hilos = omp_get_num_threads();
        int total = 10000;

        /* Distribución del rango entre hilos */
        int bloque = total / total_hilos;
        int inicio = hilo * bloque;
        int fin    = (hilo == total_hilos - 1) ? total - 1 : inicio + bloque - 1;

        /* Barrera implícita: todos deben haber calculado su rango antes de imprimir */
        #pragma omp barrier

        /* Solo un hilo a la vez imprime para no mezclar líneas */
        #pragma omp critical
        {
            printf("Hilo %d buscando desde %04d hasta %04d\n", hilo, inicio, fin);
        }

        /* Barrera: esperar a que todos impriman antes de buscar */
        #pragma omp barrier

        /* --- Búsqueda local --- */
        for (int intento = inicio; intento <= fin; intento++) {
            /*
             * Verificamos primero si otro hilo ya encontró el PIN
             * para evitar trabajo innecesario.
             */
            if (pin_hallado != -1) break;

            if (intento == PIN_OBJETIVO) {
                /*
                 * Sección crítica: solo un hilo escribe el resultado
                 * para evitar condiciones de carrera.
                 */
                #pragma omp critical
                {
                    if (pin_hallado == -1) {   /* doble chequeo dentro del critical */
                        pin_hallado = intento;
                        ganador     = hilo;
                    }
                }
            }
        }
    } /* fin del bloque parallel — todos los hilos se unen aquí */

    double t_fin = omp_get_wtime();

    printf("\n");
    if (pin_hallado != -1) {
        printf("PIN encontrado : %04d\n", pin_hallado);
        printf("Encontrado por : Hilo %d\n", ganador);
    } else {
        printf("PIN no encontrado en el rango.\n");
    }
    printf("Tiempo total   : %.6f segundos\n", t_fin - t_inicio);

    /* Mostrar cuántos hilos se usaron (leído desde el entorno) */
    printf("Hilos usados   : %d\n", omp_get_max_threads());

    return 0;
}
