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

int main(int argc, char *argv[]) {
    int x, y;
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Enter lower bound (x):");
        scanf("%d", &x);

        printf("Enter upper bound (y):");
        scanf("%d", &y);

        if (x > y) {
            printf("Invalid range: lower bound is greater than upper bound.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&y, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_count = 0;

    for (int i = x + rank; i <= y; i += size) {
        if (is_prime(i)) {
            ++local_count;
        }
    }

    int global_count = 0;
    MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Count of prime numbers occurring between %d and %d is: %d\n", x, y, global_count);
    }

    MPI_Finalize();
    return 0;
}
