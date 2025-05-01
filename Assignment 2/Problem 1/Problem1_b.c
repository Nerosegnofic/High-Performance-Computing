#include <stdio.h>
#include <mpi.h>

bool is_prime(const int n) {
    if (n == 2) {
        return true;
    }

    if (n <= 1 || !(n & 1)) {
        return false;
    }

    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

int main(int argc, char** argv) {
    int rank, size;
    int x, y;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Enter lower bound (x):");
        scanf("%d", &x);
        printf("Enter upper bound (y):");
        scanf("%d", &y);

        if (x > y) {
            printf("Invalid range: lower bound greater than upper bound.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        for (int i = 1; i < size; ++i) {
            MPI_Send(&x, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&y, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    int local_count = 0;
    for (int i = x + rank; i <= y; i += size) {
        if (is_prime(i)) {
            ++local_count;
        }
    }

    if (rank != 0) {
        MPI_Send(&local_count, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    } else {
        int total_count = local_count;
        int received_count;
        for (int i = 1; i < size; ++i) {
            MPI_Recv(&received_count, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_count += received_count;
        }
        printf("Count of prime numbers occurring between %d and %d is: %d\n", x, y, total_count);
    }

    MPI_Finalize();
    return 0;
}
