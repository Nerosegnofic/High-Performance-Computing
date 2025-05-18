#include <stdio.h>
#include <stdlib.h>
#include <omp.h>


void matrix_vector_mult(int n, int **A, int *v, int *r) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        r[i] = 0.0;
        for (int j = 0; j < n; j++) {
            r[i] += A[i][j] * v[j];
        }
    }
}

int main() {
    int n;
    int **A; 
    int *v;  
    int *r;  
    
    printf("Enter the size of the matrix (n for n x n matrix): ");
    scanf("%d", &n);
    
    if (n <= 0) {
        printf("Error: Size must be a positive integer.\n");
        return 1;
    }

    A = (int **)malloc(n * sizeof(int *));
    
    
    for (int i = 0; i < n; i++) {
        A[i] = (int *)malloc(n * sizeof(int));
    
    }

    v = (int *)malloc(n * sizeof(int));
    r = (int *)malloc(n * sizeof(int));


    printf("Enter the values for the %d x %d matrix A (row by row):\n", n, n);
    for (int i = 0; i < n; i++) {
        printf("Row %d: ", i+1);
        for (int j = 0; j < n; j++) {
            scanf("%d", &A[i][j]);
        }
    }


    printf("Enter the values for the vector v (size %d):\n", n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &v[i]);
    }


    printf("\nMatrix A:\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    printf("Vector v:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    printf("\n\n");

    int num_threads;
    printf("Enter the number of threads to use (or 0 for system default): ");
    scanf("%d", &num_threads);
    
    if (num_threads > 0) {
        omp_set_num_threads(num_threads);
    }

    matrix_vector_mult(n, A, v, r);

    printf("\nResulting vector r:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", r[i]);
    }
    printf("\n\n");

    printf("Number of threads used: %d\n", omp_get_max_threads());

    // Free allocated memory
    for (int i = 0; i < n; i++) {
        free(A[i]);
    }
    free(A);
    free(v);
    free(r);

    return 0;
}
