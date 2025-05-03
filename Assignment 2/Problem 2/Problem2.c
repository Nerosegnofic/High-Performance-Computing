#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>

int *flatten(int **arr, int row, int col) {
  int *new_arr = (int *) malloc(sizeof(int) * row * col);
  int ptr = 0;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      new_arr[ptr] = arr[i][j];
      ptr++;
    }
  }
  return new_arr;
}

int **unflatten(int *flat_arr, int row, int col) {
  int **twoD_arr = (int **) malloc(sizeof(int*) * row);
  for (int i = 0; i < row; i++)
    twoD_arr[i] = (int *) malloc(sizeof(int) * col);

  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      twoD_arr[i][j] = flat_arr[i * col + j];
    }
  }
  return twoD_arr;
}

int main(int argc, char **argv) {
  int size, rank;
  int n, m;
  int **arr_a, **arr_b, **output_arr;

  MPI_Init(NULL, NULL);

  int *flatten_arr_a, *flatten_arr_b, *sub_arr_a, *sub_arr_b, *output_sub_arr, *flatten_output_arr;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);


  // input from user
  if (rank == 0) {
    printf("Write the number of rows and the number of columns\n");
    scanf("%d%d", &n, &m);

    arr_a = (int **) malloc(sizeof(int*) * n);
    for (int i = 0; i < n; i++)
      arr_a[i] = (int *) malloc(sizeof(int) * m);

    arr_b = (int **) malloc(sizeof(int*) * n);
    for (int i = 0; i < n; i++)
      arr_b[i] = (int *) malloc(sizeof(int) * m);

    output_arr = (int **) malloc(sizeof(int*) * n);
    for (int i = 0; i < n; i++)
      output_arr[i] = (int *) malloc(sizeof(int) * m);

    flatten_arr_a = (int *) malloc(sizeof(int) * n * m);
    flatten_arr_b = (int *) malloc(sizeof(int) * n * m);
    
    flatten_output_arr = (int *) malloc(sizeof(int) * n * m);

    printf("Enter the number of the matrix a\n");
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) { 
        scanf("%d", &arr_a[i][j]);
      }
    }

    printf("Enter the number of the matrix b\n");
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        scanf("%d", &arr_b[i][j]);
      }
    }

    flatten_arr_a = flatten(arr_a, n, m);
    flatten_arr_b = flatten(arr_b, n, m);
  }

  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

  sub_arr_a = (int *) malloc(sizeof(int) * (n * m + size - 1) / size);
  sub_arr_b = (int *) malloc(sizeof(int) * (n * m + size - 1) / size);

  output_sub_arr = (int *) malloc(sizeof(int) * (n * m + size - 1) / size);

  // scatter the array
  MPI_Scatter(flatten_arr_a, (n * m + size - 1) / size, MPI_INT, sub_arr_a, (n * m + size - 1) / size, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Scatter(flatten_arr_b, (n * m + size - 1) / size, MPI_INT, sub_arr_b, (n * m + size - 1) / size, MPI_INT, 0, MPI_COMM_WORLD);

  // process
  for (int i = 0; i < (n * m + size - 1) / size; i++) {
    output_sub_arr[i] = sub_arr_a[i] + sub_arr_b[i];
  }

  // gather
  MPI_Gather(output_sub_arr, (n * m + size - 1) / size, MPI_INT, flatten_output_arr, (n * m + size - 1) / size, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    output_arr = unflatten(flatten_output_arr, n, m);
    printf("The result matrix is: \n");
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        printf("%d ", output_arr[i][j]);
      }
      printf("\n");
    }
  }

  MPI_Finalize();
}