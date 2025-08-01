#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

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
        exit(1);
    }

    char line[MAX_LINE];
    int count = 0;
    *locationCount = 0;

    fgets(line, sizeof(line), file); // skip header
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
        for (int i = 0; i < *locationCount; ++i) {
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

int  countMatchingParallel(Transaction *data, int size, const char *location, const char *gender) {
    int count = 0;

    //Split the iterations of the following for loop across multiple threads.
    #pragma omp parallel for reduction(+:count) num_threads(12)
    for (int i = 0; i < size; ++i) {
        if (strcmp(data[i].location, location) == 0 &&
            strcmp(data[i].gender, gender) == 0) {
            count++;
        }
    }

    return count;
}

int main() {
    Transaction *data = malloc(sizeof(Transaction) * MAX_TRANSACTIONS);
    char uniqueLocations[MAX_LOCATIONS][50];
    int locationCount = 0;

    int total = loadData("transactions.csv", data, uniqueLocations, &locationCount);

    printf("\nAvailable Locations:\n");
    for (int i = 0; i < locationCount; ++i) {
        printf("  [%d] %s\n", i + 1, uniqueLocations[i]);
    }

    int locationChoice;
    printf("\nEnter the number of the location to filter: ");
    scanf("%d", &locationChoice);

    while (locationChoice < 1 || locationChoice > locationCount) {
        printf("Invalid input. Enter a number between 1 and %d: ", locationCount);
        scanf("%d", &locationChoice);
    }

    char gender[10];
    printf("Enter gender (Male or Female): ");
    scanf("%s", gender);

    while (strcmp(gender, "Male") != 0 && strcmp(gender, "Female") != 0) {
        printf("Invalid gender. Enter 'Male' or 'Female': ");
        scanf("%s", gender);
    }

    const char *selectedLocation = uniqueLocations[locationChoice - 1];

    double start = omp_get_wtime();
    int result = countMatchingParallel(data, total, selectedLocation, gender);
    double end = omp_get_wtime();

    printf("\nFiltered Results:\n");
    printf("  Location: %s\n", selectedLocation);
    printf("  Gender: %s\n", gender);
    printf("  Matching Transactions: %d\n", result);
    printf("  Time taken (parallel): %.4f seconds\n", end - start);

    free(data);
    return 0;
}