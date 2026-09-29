# Búsqueda de PIN por Fuerza Bruta — MPI vs OpenMP

Actividad práctica para la asignatura **Paradigmas y Lenguajes de Programación**.  
Demuestra cómo distribuir una misma tarea de búsqueda usando dos modelos de paralelismo diferentes:

| Archivo | Modelo | Unidad de trabajo |
|---|---|---|
| `mpi_pin.c` | MPI | Procesos independientes |
| `openmp_pin.c` | OpenMP | Hilos dentro de un proceso |

---

## Descripción del problema

Se simula la búsqueda de un PIN numérico de 4 dígitos (0000–9999) mediante **fuerza bruta**.  
El rango completo se divide equitativamente entre los procesos o hilos disponibles.  
Cada uno trabaja únicamente sobre su porción del rango hasta encontrar el PIN objetivo.

---

## Requisitos

### Para la versión MPI (`mpi_pin.c`)
- Una implementación de MPI instalada:
  - **Linux/macOS:** OpenMPI (`sudo apt install openmpi-bin libopenmpi-dev`) o MPICH.
  - **Windows:** Microsoft MPI (MS-MPI) o MPICH desde WSL.
- Compilador C compatible con MPI (`mpicc`).

### Para la versión OpenMP (`openmp_pin.c`)
- GCC 4.2 o superior (incluido por defecto en la mayoría de sistemas Linux/macOS).
- En **Windows** se puede usar MinGW-w64 o compilar desde WSL.

---

## Modificar el PIN objetivo

En ambos archivos, busca la línea:

```c
#define PIN_OBJETIVO 5831
```

Cambia `5831` por cualquier número entre `0` y `9999` y recompila.

---

## Compilar

```bash
# Versión MPI
mpicc -o mpi_pin mpi_pin.c

# Versión OpenMP
gcc -fopenmp -o openmp_pin openmp_pin.c
```

---

## Ejecutar

### MPI — variar la cantidad de procesos

```bash
mpirun -np 1 ./mpi_pin
mpirun -np 2 ./mpi_pin
mpirun -np 4 ./mpi_pin
mpirun -np 8 ./mpi_pin
```

> En Windows con MS-MPI usa `mpiexec` en lugar de `mpirun`.

### OpenMP — variar la cantidad de hilos

```bash
# Opción 1: variable de entorno (recomendada)
OMP_NUM_THREADS=1 ./openmp_pin
OMP_NUM_THREADS=2 ./openmp_pin
OMP_NUM_THREADS=4 ./openmp_pin
OMP_NUM_THREADS=8 ./openmp_pin

# Opción 2: dejar que el sistema elija (usa todos los núcleos disponibles)
./openmp_pin
```

---

## Ejemplo de salida esperada

### MPI con 4 procesos

```
Proceso 0 buscando desde 0000 hasta 2499
Proceso 1 buscando desde 2500 hasta 4999
Proceso 2 buscando desde 5000 hasta 7499
Proceso 3 buscando desde 7500 hasta 9999

PIN encontrado : 5831
Encontrado por : Proceso 2
Tiempo total   : 0.000123 segundos
Procesos usados: 4
```

### OpenMP con 4 hilos

```
Hilo 0 buscando desde 0000 hasta 2499
Hilo 1 buscando desde 2500 hasta 4999
Hilo 2 buscando desde 5000 hasta 7499
Hilo 3 buscando desde 7500 hasta 9999

PIN encontrado : 5831
Encontrado por : Hilo 2
Tiempo total   : 0.000098 segundos
Hilos usados   : 4
```

> El orden de impresión de los procesos/hilos puede variar entre ejecuciones; esto es normal y forma parte del comportamiento paralelo.

---

## Preguntas para reflexionar

1. ¿Cambia el tiempo de ejecución al aumentar el número de procesos/hilos? ¿Por qué?
2. ¿Siempre encuentra el PIN el mismo proceso o hilo? ¿Qué lo determina?
3. ¿Qué diferencia hay entre la memoria compartida (OpenMP) y la memoria distribuida (MPI)?
4. ¿Qué sucedería si el rango fuera mucho más grande (por ejemplo, PINs de 8 dígitos)?

---

## Diferencias clave entre ambos modelos

| Aspecto | MPI | OpenMP |
|---|---|---|
| Memoria | Distribuida (cada proceso tiene la suya) | Compartida (todos los hilos comparten memoria) |
| Comunicación | Explícita mediante mensajes (`MPI_Send`, `MPI_Allreduce`, …) | Implícita mediante variables compartidas y directivas |
| Escalabilidad | Múltiples máquinas en red (cluster) | Una sola máquina (multinúcleo) |
| Sincronización | Barreras y colectivas MPI | `#pragma omp barrier`, `#pragma omp critical` |
| Complejidad | Mayor (hay que gestionar la comunicación) | Menor (el compilador gestiona los hilos) |
