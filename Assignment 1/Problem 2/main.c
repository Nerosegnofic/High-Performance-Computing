#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>

void encrypt(const char* input, int size, char* result) {
    for (int i = 0; i < size; ++i) {
        if (isupper(input[i])) {
            result[i] = 'A' + (input[i] - 'A' + 3) % 26;
        } else if (islower(input[i])) {
            result[i] = 'a' + (input[i] - 'a' + 3) % 26;
        } else {
            result[i] = input[i];
        }
    }
}

char take_input_mode() {
    char input_mode;
    printf("Choose input mode: C or c for console, F or f for file.\n");
    do {
        scanf(" %c", &input_mode);
        if (input_mode != 'F' && input_mode != 'f' && input_mode != 'C' && input_mode != 'c') {
            printf("Wrong input. Try again.\n");
        }
    } while (input_mode != 'F' && input_mode != 'f' && input_mode != 'C' && input_mode != 'c');
    return input_mode;
}

void decrypt(const char* input, int size, char* result) {
    for (int i = 0; i < size; ++i) {
        if (isupper(input[i])) {
            result[i] = 'A' + (input[i] - 'A' - 3 + 26) % 26;
        } else if (islower(input[i])) {
            result[i] = 'a' + (input[i] - 'a' - 3 + 26) % 26;
        } else {
            result[i] = input[i];
        }
    }
}

char take_encryption_mode() {
    char encryption_mode;
    printf("Choose encryption mode: E or e for encryption, D or d for decryption.\n");
    do {
        scanf(" %c", &encryption_mode);
        if (encryption_mode != 'E' && encryption_mode != 'e' && encryption_mode != 'D' && encryption_mode != 'd') {
            printf("Wrong input. Try again.\n");
        }
    } while (encryption_mode != 'E' && encryption_mode != 'e' && encryption_mode != 'D' && encryption_mode != 'd');
    return encryption_mode;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    char *text = NULL;
    char *output = NULL;
    int size;

    char encryption_mode = take_encryption_mode();
    char input_mode = take_input_mode();

    if (input_mode == 'C' || input_mode == 'c') {
        printf("Enter the size of your text: ");
        scanf("%d", &size);

        text = (char*) malloc((size + 1) * sizeof(char));
        output = (char*) malloc((size + 1) * sizeof(char));

        printf("Enter your text: ");
        scanf("%s", text);

        if (text == NULL || output == NULL) {
            printf("Memory allocation failed.\n");
            free(text);
            free(output);
            return 1;
        }

        text[size] = '\0';
        output[size] = '\0';

    } else {
        FILE *file = fopen("input.txt", "r");
        if (file == NULL) {
            printf("Failed to open input.txt\n");
            return 1;
        }

        fseek(file, 0, SEEK_END);
        size = ftell(file);
        rewind(file); // This will bring the "cursor" (so to speak) to the beginning.

        text = (char*) malloc((size + 1) * sizeof(char));
        output = (char*) malloc((size + 1) * sizeof(char));

        if (text == NULL || output == NULL) {
            printf("Memory allocation failed.\n");
            fclose(file);
            free(text);
            free(output);
            return 1;
        }

        fread(text, sizeof(char), size, file);
        text[size] = '\0';
        output[size] = '\0';

        fclose(file);
    }

    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);


    int chunk = size / num_procs;
    int rem = size % num_procs;

    char *local_input;
    char *local_output;
    int local_size;

    if (rank == 0) {
        int index = 0;
        for (int i = 1; i < num_procs; ++i) {
            int send_size = chunk + (i < rem ? 1 : 0);
            MPI_Send(&text[index], send_size, MPI_CHAR, i, 0, MPI_COMM_WORLD);
            index += send_size;
        }

        local_size = chunk + (0 < rem ? 1 : 0);
        local_input = malloc(local_size);
        local_output = malloc(local_size);
        memcpy(local_input, text, local_size);
    } else {
        local_size = chunk + (rank < rem ? 1 : 0);
        local_input = malloc(local_size);
        local_output = malloc(local_size);
        MPI_Recv(local_input, local_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if (encryption_mode == 'E' || encryption_mode == 'e') {
        encrypt(local_input, local_size, local_output);
    } else {
        decrypt(local_input, local_size, local_output);
    }

    if (rank == 0) {
        int offset = local_size;
        memcpy(output, local_output, local_size);

        for (int i = 1; i < num_procs; ++i) {
            int recv_size = chunk + (i < rem ? 1 : 0);
            MPI_Recv(output + offset, recv_size, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            offset += recv_size;
        }

        output[size] = '\0';
        printf("Result: %s\n", output);
        free(output);
        free(text);
    } else {
        MPI_Send(local_output, local_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    free(local_input);
    free(local_output);

    MPI_Finalize();
    return 0;
}
