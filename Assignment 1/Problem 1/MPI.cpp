#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // Master process
        printf("Hello from master process.\n");
        int numSlaves;
        printf("Please enter number of slave processes...\n");
        scanf("%d", &numSlaves);

        if (numSlaves >= size) {
            fprintf(stderr, "Error: Number of slaves must be less than total processes.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        printf("Number of slave processes is %d\n", numSlaves);
        int arraySize;
        printf("Please enter size of array...\n");
        scanf("%d", &arraySize);
        int* array = (int*)malloc(arraySize * sizeof(int));
        printf("Please enter array elements ...\n");
        for (int i = 0; i < arraySize; ++i) {
            scanf("%d", &array[i]);
        }
        int chunkSize = arraySize / numSlaves;
        int remainder = arraySize % numSlaves;
        for (int i = 1; i <= numSlaves; ++i) {
            int start = (i - 1) * chunkSize;
            int end = start + chunkSize + (i == numSlaves ? remainder : 0);

            int partitionSize = end - start;
            MPI_Send(&partitionSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&array[start], partitionSize, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        int globalMax = INT_MIN;
        int globalMaxIndex = -1;
        for (int i = 1; i <= numSlaves; ++i) {
            int localMax, localIndex;
            MPI_Recv(&localMax, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&localIndex, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int actualIndex = (i - 1) * chunkSize + localIndex;
            if (localMax > globalMax) {
                globalMax = localMax;
                globalMaxIndex = actualIndex;
            }
        }
        printf("Master process announce the final max which is %d and its index is %d.\n", globalMax, globalMaxIndex);
        printf("Thanks for using our program\n");
        free(array);
    }
    else {
        // Slave process
        int partitionSize;
        MPI_Recv(&partitionSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int* partition = (int*)malloc(partitionSize * sizeof(int));
        MPI_Recv(partition, partitionSize, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int localMax = INT_MIN;
        int localIndex = -1;
        for (int i = 0; i < partitionSize; ++i) {
            if (partition[i] > localMax) {
                localMax = partition[i];
                localIndex = i;
            }
        }
        printf("Hello from slave#%d Max number in my partition is %d and index is %d.\n", rank, localMax, localIndex);
        MPI_Send(&localMax, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&localIndex, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        free(partition);
    }

    MPI_Finalize();
    return 0;
}