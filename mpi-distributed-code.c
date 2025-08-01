#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_LINE 512
#define MAX_TRANSACTIONS 1000000
#define MAX_LOCATIONS 1000

typedef struct {
    char transactionID[20];
    char customerID[20];
    char dob[15];
    char gender[10];
    char location[50];
    double accountBalance;
    char date[15];
    char time[15];
    double amount;
} Transaction;

int loadData(const char *filename, Transaction *data, char uniqueLocations[][50], int *locationCount) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("File open failed");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    int count = 0;
    *locationCount = 0;

    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file) && count < MAX_TRANSACTIONS) {
        sscanf(line, "%19[^,],%19[^,],%14[^,],%9[^,],%49[^,],%lf,%14[^,],%14[^,],%lf",
               data[count].transactionID,
               data[count].customerID,
               data[count].dob,
               data[count].gender,
               data[count].location,
               &data[count].accountBalance,
               data[count].date,
               data[count].time,
               &data[count].amount
        );
        int found = 0;
        for (int i = 0; i < *locationCount; i++) {
            if (strcmp(data[count].location, uniqueLocations[i]) == 0) {
                found = 1;
                break;
            }
        }
        if (!found && *locationCount < MAX_LOCATIONS) {
            strcpy(uniqueLocations[*locationCount], data[count].location);
            (*locationCount)++;
        }

        count++;
    }

    fclose(file);
    return count;
}

int countMatching(Transaction *data, int size, const char *location, const char *gender) {
    int count = 0;
    for (int i = 0; i < size; ++i) {
        if (strcmp(data[i].location, location) == 0 &&
            strcmp(data[i].gender, gender) == 0) {
            count++;
        }
    }
    return count;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv); 

    int rank, size; 
    Transaction *all_data = NULL;
    Transaction *local_data = NULL;

    int total_rows = 0;
    char location[50];
    char gender[10];
    char uniqueLocations[MAX_LOCATIONS][50];
    int locationCount = 0;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        all_data = malloc(sizeof(Transaction) * MAX_TRANSACTIONS);
        total_rows = loadData("transactions.csv", all_data, uniqueLocations, &locationCount);

        printf("\nAvailable Locations:\n");
        for (int i = 0; i < locationCount; ++i) {
            printf("  [%d] %s\n", i + 1, uniqueLocations[i]);
        }

    int choice;
    printf("\nEnter location number to filter: ");
    fflush(stdout);
    scanf("%d", &choice);
    while (choice < 1 || choice > locationCount) {
        printf("Invalid choice. Please enter between 1 and %d: ", locationCount);
        fflush(stdout);
        scanf("%d", &choice);
    }
    strcpy(location, uniqueLocations[choice - 1]);

    printf("Enter gender (Male or Female): ");
    fflush(stdout);
    scanf("%s", gender);
    }

    double start_time = MPI_Wtime();

    MPI_Bcast(&total_rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(location, 50, MPI_CHAR, 0, MPI_COMM_WORLD);
    MPI_Bcast(gender, 10, MPI_CHAR, 0, MPI_COMM_WORLD);

    int chunk_size = (total_rows + size - 1) / size;

    local_data = malloc(sizeof(Transaction) * chunk_size);

    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            int offset = i * chunk_size;
            int send_count = (offset + chunk_size > total_rows) ? (total_rows - offset) : chunk_size;
            if (send_count > 0) {
                MPI_Send(all_data + offset, send_count * sizeof(Transaction), MPI_BYTE, i, 0, MPI_COMM_WORLD);
            }
        }
        int own_count = (chunk_size > total_rows) ? total_rows : chunk_size;
        memcpy(local_data, all_data, own_count * sizeof(Transaction));
    } else {
        MPI_Status status;
        MPI_Recv(local_data, chunk_size * sizeof(Transaction), MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);
    }
    int local_count = chunk_size;
    if (rank == size - 1) {
        int remainder = total_rows % chunk_size;
        if (remainder != 0) {
            local_count = remainder;    
        }
    }

    int local_result = countMatching(local_data, local_count, location, gender);


    int global_result = 0;
    MPI_Reduce(&local_result, &global_result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (rank == 0) {
        printf("\nFilter Applied:\n");
        printf("  Location: %s\n", location);
        printf("  Gender: %s\n", gender);
        printf("  Matching Transactions: %d\n", global_result);
        printf("  Elapsed Time (MPI): %.4f seconds\n", end_time - start_time);
        free(all_data);
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}
