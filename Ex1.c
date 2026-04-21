[23bcs002@mepcolinux ex1_new]$cat prg1.c
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<time.h>

int main() {
    int N;
    printf("Enter the value of N for NxN matrix: ");
    scanf("%d", &N);
    int i,j,k;
    // Allocate memory for matrices
    int **A = (int **)malloc(N * sizeof(int *));
    int **B = (int **)malloc(N * sizeof(int *));
    int **C = (int **)malloc(N * sizeof(int *));
    int (*p2c)[2] = (int (*)[2])malloc(N * sizeof(int[2]));

    for (i = 0; i < N; i++)
    {
        A[i] = (int *)malloc(N * sizeof(int));
        B[i] = (int *)malloc(N * sizeof(int));
        C[i] = (int *)malloc(N * sizeof(int));
    }
    // Seed for random number generation
    //srand(time(0));
    if(N<6)
    {
    printf("Matrix A:\n");
    // Generate random matrix A
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            A[i][j] = rand() % 10;
            printf("%d\t",A[i][j]);
        }
        printf("\n");
    }
   printf("Matrix B:\n");
    // Generate random matrix B
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            B[i][j] = rand() % 10;
            printf("%d\t",B[i][j]);
        }
        printf("\n");
    }
    }
    // Create pipes
    for (i = 0; i < N; i++) {
        pipe(p2c[i]);
    }

    // Create child processes
    for (i = 0; i < N; i++) {
        int pid = fork();
        if (pid == 0) { // Child process
            int *result = (int *)malloc(N * sizeof(int));
            struct timeval start, end;
            double time_used;
            int j,k;
            gettimeofday(&start, NULL);
            for (j = 0; j < N; j++) {
                result[j] = 0;
                for (k = 0; k < N; k++) {
                    result[j] += A[i][k] * B[k][j];
                }
            }
            gettimeofday(&end, NULL);

            time_used = (end.tv_sec - start.tv_sec) * 1e6;
            time_used = (time_used + (end.tv_usec - start.tv_usec)) * 1e-6;

            close(p2c[i][0]); // Close read end
            write(p2c[i][1], result, N * sizeof(int));
            write(p2c[i][1], &time_used, sizeof(time_used));
            close(p2c[i][1]); // Close write end
            free(result);
            return 0; // Exit child process
        }
    }

    // Parent process
    double max_time = 0;
    for (i = 0; i < N; i++) {
        int *result = (int *)malloc(N * sizeof(int));
        double child_time;
        int j;
        close(p2c[i][1]); // Close write end
        read(p2c[i][0], result, N * sizeof(int));
        read(p2c[i][0], &child_time, sizeof(child_time));
        for (j = 0; j < N; j++) {
            C[i][j] = result[j];
        }
        if (child_time > max_time)
            max_time = child_time;
        close(p2c[i][0]); // Close read end
        free(result);
    }

    // Wait for all child processes to finish
    for (i = 0; i < N; i++) {
        wait(NULL);
    }
    struct timeval s, e;
    double time_used;
    gettimeofday(&s, NULL);
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
                C[i][j] = 0;
                for (k = 0; k < N; k++) {
                   C[i][j] += A[i][k] * B[k][j];
                }
        }
    }
    gettimeofday(&e, NULL);
    time_used = (e.tv_sec - s.tv_sec) * 1e6;
    time_used = (time_used + (e.tv_usec - s.tv_usec)) * 1e-6;

    // Convert to milliseconds
    max_time *= 1000;
    time_used *= 1000;

    printf("Maximum execution time among child processes (PARALLEL EXECUTION): %f milliseconds\n", max_time);
    printf("Maximum execution time (Serial execution): %f milliseconds\n", time_used);
    if(N<6)
    {
    printf("Resultant matrix:\n");
    for(i=0;i<N;i++)
    {
       for(j=0;j<N;j++)
       {
          printf("%d\t",C[i][j]);
       }
       printf("\n");
    }
    }
    // Free allocated memory
    for (i = 0; i < N; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);
    free(p2c);

    return 0;
}

[23bcs002@mepcolinux ex1_new]$cc -std=c99 prg1.c
[23bcs002@mepcolinux ex1_new]$./a.out
Enter the value of N for NxN matrix: 5
Matrix A:
3	6	7	5	3	
5	6	2	9	1	
2	7	0	9	3	
6	0	6	2	6	
1	8	7	9	2	
Matrix B:
0	2	3	7	5	
9	2	2	8	9	
7	3	6	1	2	
9	3	1	9	4	
7	8	4	5	0	
Maximum execution time among child processes (PARALLEL EXECUTION): 0.001000 milliseconds
Maximum execution time (Serial execution): 0.002000 milliseconds
Resultant matrix:
169	78	80	136	103	
156	63	52	171	119	
165	69	41	166	109	
102	84	80	96	50	
216	82	78	169	127	
[23bcs002@mepcolinux ex1_new]$./a.out
Enter the value of N for NxN matrix: 50
Maximum execution time among child processes (PARALLEL EXECUTION): 0.047000 milliseconds
Maximum execution time (Serial execution): 1.995000 milliseconds
[23bcs002@mepcolinux ex1_new]$./a.out
Enter the value of N for NxN matrix: 500
Maximum execution time among child processes (PARALLEL EXECUTION): 6.482000 milliseconds
Maximum execution time (Serial execution): 3188.690000 milliseconds
[23bcs002@mepcolinux ex1_new]$./a.out
Enter the value of N for NxN matrix: 1000
Maximum execution time among child processes (PARALLEL EXECUTION): 43.965000 milliseconds
Maximum execution time (Serial execution): 29485.223000 milliseconds
[23bcs002@mepcolinux ex1_new]$cat prg2_new.c
#define _POSIX_C_SOURCE 199310L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#define ITERATIONS 100

double get_time_sec()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main()
{
    int N;
    printf("Enter the size of the square matrix (N): ");
    scanf("%d", &N);

    if (N <= 0)
    {
        fprintf(stderr, "Matrix size must be a positive integer.\n");
        return 1;
    }

    int (*A)[N] = malloc(sizeof(int) * N * N);
    int (*B)[N] = malloc(sizeof(int) * N * N);
    if (A == NULL || B == NULL)
    {
        perror("malloc failed for A or B");
        return 1;
    }

    int shmid_add, shmid_sub;
    int (*Add)[N];
    int (*Sub)[N];

    shmid_add = shmget(IPC_PRIVATE, sizeof(int) * N * N, IPC_CREAT | 0666);
    if (shmid_add == -1) {
        perror("shmget for Add failed");
        free(A);
        free(B);
        return 1;
    }
    Add = shmat(shmid_add, NULL, 0);
    if (Add == (void *)-1) {
        perror("shmat for Add failed");
        shmctl(shmid_add, IPC_RMID, NULL);
        free(A);
        free(B);
        return 1;
    }

    shmid_sub = shmget(IPC_PRIVATE, sizeof(int) * N * N, IPC_CREAT | 0666);
    if (shmid_sub == -1) {
        perror("shmget for Sub failed");
        shmdt(Add);
        shmctl(shmid_add, IPC_RMID, NULL);
        free(A);
        free(B);
        return 1;
    }
    Sub = shmat(shmid_sub, NULL, 0);
    if (Sub == (void *)-1) {
        perror("shmat for Sub failed");
        shmdt(Add);
        shmctl(shmid_add, IPC_RMID, NULL);
        shmctl(shmid_sub, IPC_RMID, NULL);
        free(A);
        free(B);
        return 1;
    }

    srand(time(NULL));
    if (N <= 10)
    {
        printf("Matrix A:\n");
    }
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            A[i][j] = rand() % 10;
            if (N <= 10) {
                printf("%d ", A[i][j]);
            }
        }
        if (N <= 10) {
            printf("\n");
        }
    }

    if (N <= 10) {
        printf("\nMatrix B:\n");
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            B[i][j] = rand() % 10;
            if (N <= 10) {
                printf("%d ", B[i][j]);
            }
        }
        if (N <= 10) {
            printf("\n");
        }
    }

    // ---------------- SERIAL EXECUTION ----------------
    double serial_add_start = get_time_sec();
    for (int r = 0; r < ITERATIONS; r++) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                Add[i][j] = A[i][j] + B[i][j];
            }
        }
    }
    double serial_add_end = get_time_sec();
    double serial_add_time = serial_add_end - serial_add_start;

    double serial_sub_start = get_time_sec();
    for (int r = 0; r < ITERATIONS; r++) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                Sub[i][j] = A[i][j] - B[i][j];
            }
        }
    }
    double serial_sub_end = get_time_sec();
    double serial_sub_time = serial_sub_end - serial_sub_start;
    double serial_total_time = serial_add_time + serial_sub_time;

    // ---------------- PARALLEL EXECUTION ----------------
    double parallel_add_time = 0;
    double parallel_sub_time = 0;
    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe failed");
        shmdt(Add); shmctl(shmid_add, IPC_RMID, NULL);
        shmdt(Sub); shmctl(shmid_sub, IPC_RMID, NULL);
        free(A); free(B);
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        close(fd[0]); close(fd[1]);
        shmdt(Add); shmctl(shmid_add, IPC_RMID, NULL);
        shmdt(Sub); shmctl(shmid_sub, IPC_RMID, NULL);
        free(A); free(B);
        return 1;
    } else if (pid == 0) { // Child process
        close(fd[0]);

        double start = get_time_sec();
        for (int r = 0; r < ITERATIONS; r++) {
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    Sub[i][j] = A[i][j] - B[i][j];
                }
            }
        }
        double end = get_time_sec();
        parallel_sub_time = end - start;

        write(fd[1], &parallel_sub_time, sizeof(double)); 
        close(fd[1]); 

        shmdt(Add);
        shmdt(Sub);
        free(A);
        free(B);
        exit(0); // Child exits
    } else { // Parent process
        close(fd[1]);

        double start = get_time_sec();
        for (int r = 0; r < ITERATIONS; r++)
        {
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    Add[i][j] = A[i][j] + B[i][j];
                }
            }
        }
        double end = get_time_sec();
        parallel_add_time = end - start;
        printf("\nParent (Addition) Execution Time:%.3f ms\n", parallel_add_time * 1000);
        read(fd[0], &parallel_sub_time, sizeof(double));
        close(fd[0]); 

        printf("Child (Subtraction) Execution Time:%.3f ms\n", parallel_sub_time * 1000);
       // wait(NULL); // Wait for the child to fully terminate.
    }

    // ---------------- PARALLEL TOTAL TIME (MAX of the two parallel tasks) ----------------
    double parallel_total_time = (parallel_add_time > parallel_sub_time) ? parallel_add_time : parallel_sub_time;

    // ---------------- RESULTS ----------------
    if (N <= 10) {
        printf("\nAddition Result Matrix:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++)
                printf("%d ", Add[i][j]);
            printf("\n");
        }
        printf("\nSubtraction Result Matrix:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++)
                printf("%d ", Sub[i][j]);
            printf("\n");
        }
    } else {
        printf("\nMatrices are too large to print. Displaying execution times only.\n");
    }

    // ---------------- TIME COMPARISON ----------------
    printf("\n------ TIME COMPARISON ------\n");
    printf("Serial Execution Time (Add + Sub)   : %.3f ms\n", serial_total_time * 1000);
    printf("Parallel Execution Time (MAX of Add, Sub) : %.3f ms\n", parallel_total_time * 1000);
    printf("Time Difference (Serial - Parallel) : %.3f ms\n", (serial_total_time - parallel_total_time) * 1000);

    shmdt(Add);
    shmctl(shmid_add, IPC_RMID, NULL);
    shmdt(Sub);
    shmctl(shmid_sub, IPC_RMID, NULL);
    free(A);
    free(B);

    return 0;
}
[23bcs002@mepcolinux ex1_new]$cc -std=c99 prg2_new.c
[23bcs002@mepcolinux ex1_new]$./a.out
Enter the size of the square matrix (N): 5
Matrix A:
1 1 2 5 6 
4 3 5 3 2 
9 9 8 9 2 
5 8 4 9 7 
5 2 9 9 9 

Matrix B:
2 2 5 2 4 
8 6 7 0 3 
3 6 6 8 2 
0 7 1 1 8 
5 6 6 9 5 

Parent (Addition) Execution Time:0.019 ms
Child (Subtraction) Execution Time:0.023 ms

Addition Result Matrix:
3 3 7 7 10 
12 9 12 3 5 
12 15 14 17 4 
5 15 5 10 15 
10 8 15 18 14 

Subtraction Result Matrix:
-1 -1 -3 3 2 
-4 -3 -2 3 -1 
6 3 2 1 0 
5 1 3 8 -1 
0 -4 3 0 4 

------ TIME COMPARISON ------
Serial Execution Time (Add + Sub)   : 0.054 ms
Parallel Execution Time (MAX of Add, Sub) : 0.023 ms
Time Difference (Serial - Parallel) : 0.031 ms
[23bcs002@mepcolinux ex1_new]$./a.out
Enter the size of the square matrix (N): 50

Parent (Addition) Execution Time:1.826 ms
Child (Subtraction) Execution Time:1.861 ms

Matrices are too large to print. Displaying execution times only.

------ TIME COMPARISON ------
Serial Execution Time (Add + Sub)   : 3.595 ms
Parallel Execution Time (MAX of Add, Sub) : 1.861 ms
Time Difference (Serial - Parallel) : 1.734 ms
[23bcs002@mepcolinux ex1_new]$./a.out
Enter the size of the square matrix (N): 500

Parent (Addition) Execution Time:300.272 ms
Child (Subtraction) Execution Time:295.579 ms

Matrices are too large to print. Displaying execution times only.

------ TIME COMPARISON ------
Serial Execution Time (Add + Sub)   : 522.647 ms
Parallel Execution Time (MAX of Add, Sub) : 300.272 ms
Time Difference (Serial - Parallel) : 222.375 ms
[23bcs002@mepcolinux ex1_new]$./a.out
Enter the size of the square matrix (N): 1000

Parent (Addition) Execution Time:1209.722 ms
Child (Subtraction) Execution Time:1131.340 ms

Matrices are too large to print. Displaying execution times only.

------ TIME COMPARISON ------
Serial Execution Time (Add + Sub)   : 2102.668 ms
Parallel Execution Time (MAX of Add, Sub) : 1209.722 ms
Time Difference (Serial - Parallel) : 892.946 ms

