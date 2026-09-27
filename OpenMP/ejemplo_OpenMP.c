#include <omp.h>
#include <stdio.h>

int main(void) {
	#pragma omp parallel
	{
		int hilo = omp_get_thread_num();
		int total = omp_get_num_threads();

		printf("Hola desde el hilo %d de %d\n", hilo, total);
	}

	return 0;
}
