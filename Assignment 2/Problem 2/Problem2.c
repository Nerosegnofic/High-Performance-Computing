#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int* flatten(int **arr, int rows, int cols) {
    int *flat = (int *) malloc(rows * cols * sizeof(int));
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            flat[i * cols + j] = arr[i][j];
    return flat;
}

int** unflatten(int *flat, int rows, int cols) {
    int **arr = (int **) malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; i++) {
        arr[i] = (int *) malloc(cols * sizeof(int));
        for (int j = 0; j < cols; j++) {
            arr[i][j] = flat[i * cols + j];
        }
    }
    return arr;
}

int main(int argc, char **argv) {
    int rank, size;
    int n, m;
    int *flat_a = NULL, *flat_b = NULL, *flat_result = NULL;
    int *sub_a, *sub_b, *sub_result;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Enter number of rows and columns: ");
        scanf("%d%d", &n, &m);

        int **a = (int **) malloc(n * sizeof(int*));
        int **b = (int **) malloc(n * sizeof(int*));
        for (int i = 0; i < n; i++) {
            a[i] = (int *) malloc(m * sizeof(int));
            b[i] = (int *) malloc(m * sizeof(int));
        }

        printf("Enter matrix A:\n");
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                scanf("%d", &a[i][j]);

        printf("Enter matrix B:\n");
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++)
                scanf("%d", &b[i][j]);

        flat_a = flatten(a, n, m);
        flat_b = flatten(b, n, m);

        for (int i = 0; i < n; i++) {
            free(a[i]);
            free(b[i]);
        }
        free(a);
        free(b);

        flat_result = (int *) malloc(n * m * sizeof(int));
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int total_elements = n * m;
    int chunk = (total_elements + size - 1) / size;

    sub_a = (int *) malloc(chunk * sizeof(int));
    sub_b = (int *) malloc(chunk * sizeof(int));
    sub_result = (int *) malloc(chunk * sizeof(int));

    MPI_Scatter(flat_a, chunk, MPI_INT, sub_a, chunk, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(flat_b, chunk, MPI_INT, sub_b, chunk, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < chunk && rank * chunk + i < total_elements; i++) {
        sub_result[i] = sub_a[i] + sub_b[i];
    }

    MPI_Gather(sub_result, chunk, MPI_INT, flat_result, chunk, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        int **result = unflatten(flat_result, n, m);
        printf("Result matrix:\n");
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                printf("%d ", result[i][j]);
            }
            printf("\n");
            free(result[i]);
        }
        free(result);
        free(flat_a);
        free(flat_b);
        free(flat_result);
    }

    free(sub_a);
    free(sub_b);
    free(sub_result);

    MPI_Finalize();
    return 0;
}
