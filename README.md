# Búsqueda de PIN por Fuerza Bruta — MPI vs OpenMP

Trabajo Práctico para la asignatura **Paradigmas y Lenguajes de Programación** (2026).  
Comparación práctica entre los paradigmas de **memoria distribuida** (MPI) y **memoria compartida** (OpenMP).

---

## Integrantes
- Ernst Milagros Shaiel
- Kluka Jorge Favio
- Limberger Axel Agustín
- Verón Juan Manuel

---

## Archivos del proyecto

| Archivo | Modelo | Unidad de trabajo | Mecanismo de parada |
|---|---|---|---|
| `mpi_pin.c` | MPI | Procesos independientes | Reducción colectiva por tandas (`MPI_Allreduce`) |
| `openmp_pin.c` | OpenMP | Hilos (threads) dentro de un proceso | Variable compartida con exclusión mutua (`#pragma omp critical`) |

---

## Descripción del problema

Se simula la búsqueda de un PIN numérico de **8 dígitos** (`00000000`–`99999999`, 100 millones de combinaciones) mediante **fuerza bruta** (paralelismo de datos por descomposición de dominio).

El rango total se reparte de forma equitativa entre las unidades de procesamiento disponibles:
- **Enfoque MPI (Memoria Distribuida):** Cada proceso trabaja en su espacio de direcciones aislado. Para evitar sobrecargar la red con mensajes en cada iteración individual, la búsqueda se organiza en bloques de **100.000 combinaciones**. Al término de cada bloque, los procesos se sincronizan mediante `MPI_Allreduce` con `MPI_MAX` para verificar si alguno ya halló el PIN y detener la búsqueda tempranamente.
- **Enfoque OpenMP (Memoria Compartida):** Múltiples hebras conviven dentro del mismo proceso y leen directamente la variable compartida `pin_hallado`. Al encontrar el valor, entran a una sección crítica (`#pragma omp critical`) para registrar el hallazgo de manera atómica, permitiendo que las demás hebras interrumpan su ciclo inmediatamente.

> **Nota sobre seguridad (Hashing):** En un entorno de producción real, las contraseñas nunca se almacenan en texto plano, sino mediante funciones hash criptográficas unidireccionales (SHA-256, bcrypt, etc.). En este caso práctico comparamos enteros directamente en memoria con fines pedagógicos; no obstante, el reparto de dominio y la coordinación paralela se rigen bajo el mismo principio.

---

## Requisitos

### Para la versión MPI (`mpi_pin.c`)
- Implementación de MPI instalada:
  - **Linux / macOS:** OpenMPI (`sudo apt install openmpi-bin libopenmpi-dev`) o MPICH.
  - **Windows:** Microsoft MPI (MS-MPI) o MPICH vía WSL.
- Compilador de MPI: `mpicc`.

### Para la versión OpenMP (`openmp_pin.c`)
- GCC 4.2 o superior (con soporte para OpenMP vía flag `-fopenmp`).

---

## Modificar el PIN objetivo

En ambos archivos C puedes ajustar el valor de prueba modificando la constante:

```c
#define PIN_OBJETIVO ........
```

Cambia el valor por cualquier número entre `0` y `99999999` y recompila.
#define PIN_OBJETIVO 87654321

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
Proceso 2 buscando desde 50000000 hasta 74999999 (25000000 combinaciones)
Proceso 3 buscando desde 75000000 hasta 99999999 (25000000 combinaciones)
Proceso 0 buscando desde 00000000 hasta 24999999 (25000000 combinaciones)
Proceso 1 buscando desde 25000000 hasta 49999999 (25000000 combinaciones)

PIN encontrado : 87654321
Encontrado por : Proceso 3
Tiempo total   : 0.051859 segundos
Procesos usados: 4
```

### OpenMP con 4 hilos

```
Hilo 2 buscando desde 50000000 hasta 74999999
Hilo 3 buscando desde 75000000 hasta 99999999
Hilo 0 buscando desde 00000000 hasta 24999999
Hilo 1 buscando desde 25000000 hasta 49999999

PIN encontrado : 87654321
Encontrado por : Hilo 3
Tiempo total   : 0.069593 segundos
Hilos usados   : 4
```

> El orden de impresión de los procesos/hilos puede variar entre ejecuciones; esto es normal y forma parte del comportamiento paralelo.

---

## Preguntas para reflexionar

1. ¿Cambia el tiempo de ejecución al aumentar el número de procesos/hilos? ¿Por qué?
2. ¿Siempre encuentra el PIN el mismo proceso o hilo? ¿Qué lo determina?
3. ¿Qué diferencia hay entre la memoria compartida (OpenMP) y la memoria distribuida (MPI)?
---

## Diferencias clave entre ambos modelos

| Aspecto | MPI | OpenMP |
|---|---|---|
| Memoria | Distribuida (cada proceso tiene la suya) | Compartida (todos los hilos comparten memoria) |
| Comunicación | Explícita mediante mensajes (`MPI_Send`, `MPI_Allreduce`, …) | Implícita mediante variables compartidas y directivas |
| Escalabilidad | Múltiples máquinas en red (cluster) | Una sola máquina (multinúcleo) |
| Sincronización | Barreras y colectivas MPI | `#pragma omp barrier`, `#pragma omp critical` |
| Complejidad | Mayor (hay que gestionar la comunicación) | Menor (el compilador gestiona los hilos) |
