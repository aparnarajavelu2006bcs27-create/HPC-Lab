Script started on Tuesday 10 February 2026 04:15:23 PM IST
[23bcs002@mepcolinux ex2]$cat prg_final.c
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define MAX_STRING_LENGTH 100
#define MAX_INPUTS 4 

void to_uppercase(char *str) 
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = toupper(str[i]);
    }
}

int is_palindrome(char *str) 
{
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++)
    {
        if (str[i] != str[len - i - 1])
        {
            return 0; // Not a palindrome
        }
    }
    return 1; // Is a palindrome
}

int main(void)
{
    int rank, size;
    char *default_messages[MAX_INPUTS] = {"hello", "madam", "malayalam", "abcd"};
    char message[MAX_STRING_LENGTH];
    char response[MAX_STRING_LENGTH]; 
    MPI_Status status;
    double start_time, end_time, exec_time;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    start_time = MPI_Wtime(); // Start time(entire process)  
    if (rank == 0)
    {
        // Master process
        for (int i = 1; i < size && i <= MAX_INPUTS; i++) 
        {
            MPI_Recv(response, sizeof(response), MPI_CHAR, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("Received from process %d: \"%s\"\n", i, response);
        }
        end_time = MPI_Wtime(); // End time (master process)
        exec_time = (end_time - start_time) * 1000; // Conversion to milliseconds
        printf("Master process execution time: %.3f ms\n", exec_time);
    }
    else if (rank != 0)
    {
        // Worker processes
        strncpy(message, default_messages[rank - 1], MAX_STRING_LENGTH); 
        double worker_start_time = MPI_Wtime();
        if (rank % 2 == 0)
        {
            // Type 2: Check for palindrome
            int palindrome = is_palindrome(message);
            snprintf(response, MAX_STRING_LENGTH, "%s (Palindrome: %s)", message, palindrome ? "Yes" : "No");
        } 
        else
        {
            // Type 1: Convert to uppercase
            to_uppercase(message);
            snprintf(response, MAX_STRING_LENGTH, "%s (Uppercase)", message);
        }
        MPI_Send(response, strlen(response) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        double worker_end_time = MPI_Wtime(); // End time (worker process)
        exec_time = (worker_end_time - worker_start_time) * 1000; // Convert to milliseconds
        printf("Process %d execution time: %.3f ms\n", rank, exec_time);
    }
    MPI_Finalize();
    return 0;
}


III-A1@cil20:~/Aparna/ex2$ mpicc prg2.c
III-A1@cil20:~/Aparna/ex2$ mpiexec -n 5 ./a.out
Process 1 execution time: 0.008 ms
Process 2 execution time: 0.007 ms
Process 3 execution time: 0.010 ms
Process 4 execution time: 0.007 ms
Received from process 1: "HELLO (Uppercase)"
Received from process 2: "madam (Palindrome: Yes)"
Received from process 3: "MALAYALAM (Uppercase)"
Received from process 4: "abcd (Palindrome: No)"
Master process execution time: 0.034 ms


