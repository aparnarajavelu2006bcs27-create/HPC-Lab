#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main()
{
    int rank, size;
    int search_num;
    int n = 16;  // total elements (adjusted to be divisible by number of processes)
    int *array = NULL;
    int *sub_array;
    int local_n;
    int local_count = 0;
    int total_count = 0;

    MPI_Init(NULL, NULL);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Start timing
    double start_time = MPI_Wtime();

    local_n = n / size;  // Will work correctly with n = 16

    sub_array = (int *)malloc(local_n * sizeof(int));

    /* Master process */
    if (rank == 0)
    {
        array = (int *)malloc(n * sizeof(int));

        printf("Enter the number to find frequency: ");
        fflush(stdout); // Ensure this output is printed immediately
        scanf("%d", &search_num);

        printf("Array elements: ");
        for (int i = 0; i < n; i++)
        {
            array[i] = rand() % 20; // Random elements between 0 and 19
            printf("%d ", array[i]);
        }
        printf("\n");
        fflush(stdout); // Ensure this output is printed immediately
    }

    /* Broadcast search number */
    MPI_Bcast(&search_num, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /* Scatter array to all processes */
    MPI_Scatter(array, local_n, MPI_INT,
                sub_array, local_n, MPI_INT,
                0, MPI_COMM_WORLD);

    /* Print the elements of the scattered array */
    printf("Process %d received elements: ", rank);
    for (int i = 0; i < local_n; i++)
    {
        printf("%d ", sub_array[i]);
    }
    printf("\n");
    fflush(stdout); // Ensure this output is printed immediately

    /* Each process counts frequency */
    for (int i = 0; i < local_n; i++)
    {
        if (sub_array[i] == search_num)
            local_count++;
    }

    // Master collects local counts from other processes
    if (rank != 0) {
        // Send local count to master
        MPI_Send(&local_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    } else {
        total_count = local_count; 
        
        for (int i = 1; i < size; i++) {
            int sub_local_count;
            MPI_Recv(&sub_local_count, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_count += sub_local_count;
            printf("Process %d found %d occurrences of %d.\n", i, sub_local_count, search_num);
            fflush(stdout);
        }
    }

    // Total frequency output from the master
    if (rank == 0) {
        printf("Total frequency of %d is: %d\n", search_num, total_count);
        fflush(stdout); // Ensure this output is printed immediately
    }

    // End timing
    double end_time = MPI_Wtime();
    double execution_time_ms = (end_time - start_time) * 1000; // Convert to milliseconds

    printf("Process %d finished execution in %f milliseconds.\n", rank, execution_time_ms);
    fflush(stdout); // Ensure this output is printed immediately

    // Cleanup
    if (rank == 0) {
        free(array);
    }
    free(sub_array);

    MPI_Finalize();
    return 0;
}

III-A1@cil20:~/Aparna/ex3$ mpiexec -n 5 ./a.out
Enter the number to find frequency: 12
Array elements: 33 36 27 15 43 35 36 42 49 21 12 27 40 9 13 26 
Process 2 received elements: 36 42 49 
Process 4 received elements: 40 9 13 
Process 0 received elements: 33 36 27 
Process 1 found 0 occurrences of 12.
Process 2 found 0 occurrences of 12.
Process 1 received elements: 15 43 35 
Process 1 finished execution in 5588.959121 milliseconds.
Process 4 finished execution in 5588.939323 milliseconds.
Process 2 finished execution in 5588.838855 milliseconds.
Process 3 found 1 occurrences of 12.
Process 4 found 0 occurrences of 12.
Total frequency of 12 is: 1
Process 0 finished execution in 5589.428068 milliseconds.
Process 3 received elements: 21 12 27 
Process 3 finished execution in 5589.322341 milliseconds.


