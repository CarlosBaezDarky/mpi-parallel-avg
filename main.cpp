#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int main(int argc, char** argv) {
    int rank, size, N;
    double *local_values, local_sum = 0.0, total_sum = 0.0, average;

    // Inicialización de MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Proceso raíz obtiene el tamaño N del usuario
    if (rank == 0) {
        printf("Ingrese el numero de valores por proceso (N): ");
        scanf("%d", &N);

        // Validación de entrada
        if (N <= 0) {
            printf("Error: N debe ser un numero positivo.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // Broadcast de N a todos los procesos
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Cada proceso genera N valores aleatorios y calcula su suma local
    local_values = (double*)malloc(N * sizeof(double));
    srand(time(NULL) + rank); // Semilla diferente para cada proceso

    for (int i = 0; i < N; i++) {
        local_values[i] = (double)rand() / RAND_MAX; // Valores entre 0 y 1
        local_sum += local_values[i];
    }

    // Reducción de las sumas locales al proceso raíz
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Proceso raíz calcula el promedio y lo distribuye
    if (rank == 0) {
        average = total_sum / (N * size);
        printf("\nProceso %d: Suma total = %.4f, Promedio calculado = %.4f\n",
               rank, total_sum, average);
    }

    // Broadcast del promedio a todos los procesos
    MPI_Bcast(&average, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Cada proceso imprime el promedio recibido
    if (rank != 0) {
        printf("Proceso %d: Promedio recibido = %.4f\n", rank, average);
    }

    // Liberar memoria y finalizar
    free(local_values);
    MPI_Finalize();

    return 0;
}
