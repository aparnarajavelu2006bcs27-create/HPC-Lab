#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int **A, **B;
int **sum;
int **product;
int SIZE;

double get_elapsed_ms(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;
}

void* add_matrices(void* arg) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            sum[i][j] = A[i][j] + B[i][j];
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double *elapsed = malloc(sizeof(double));
    *elapsed = get_elapsed_ms(start, end);

    printf("\n[Thread 1] Matrix Addition Complete.\n");
    pthread_exit(elapsed);
}

void* multiply_matrices(void* arg) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            product[i][j] = 0;
            for (int k = 0; k < SIZE; k++) {
                product[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double *elapsed = malloc(sizeof(double));
    *elapsed = get_elapsed_ms(start, end);

    printf("[Thread 2] Matrix Multiplication Complete.\n");
    pthread_exit(elapsed);
}

int main() {
    pthread_t thread1, thread2;
    double *time1, *time2;

    printf("Enter matrix size (N for NxN): ");
    scanf("%d", &SIZE);

    srand(time(NULL));

    A = malloc(SIZE * sizeof(int *));
    B = malloc(SIZE * sizeof(int *));
    sum = malloc(SIZE * sizeof(int *));
    product = malloc(SIZE * sizeof(int *));
    for (int i = 0; i < SIZE; i++) {
        A[i] = malloc(SIZE * sizeof(int));
        B[i] = malloc(SIZE * sizeof(int));
        sum[i] = malloc(SIZE * sizeof(int));
        product[i] = malloc(SIZE * sizeof(int));
        
        for (int j = 0; j < SIZE; j++) {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }
    }

    pthread_create(&thread1, NULL, add_matrices, NULL);
    pthread_create(&thread2, NULL, multiply_matrices, NULL);

    pthread_join(thread1, (void**)&time1);
    pthread_join(thread2, (void**)&time2);

    printf("\n--- Execution Times ---");
    printf("\nAddition Thread: %.4f ms", *time1);
    printf("\nMultiplication Thread: %.4f ms\n", *time2);

    if (SIZE < 6) {
        printf("\n--- Matrix A ---\n");
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                printf("%2d ", A[i][j]); // Adjusted for better alignment
            }
            printf("\n");
        }
        printf("\n--- Matrix B ---\n");
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                printf("%2d ", B[i][j]); // Adjusted for better alignment
            }
            printf("\n");
        }
        printf("\n--- Result of Addition ---\n");
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                printf("%2d ", sum[i][j]); // Adjusted for better alignment
            }
            printf("\n");
        }
        printf("\n--- Result of Multiplication ---\n");
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                printf("%2d ", product[i][j]); // Adjusted for better alignment
            }
            printf("\n");
        }
    } else {
        printf("\nMatrix size is %dx%d. Skipping matrix display (Size >= 6x6).\n", SIZE, SIZE);
    }

    for (int i = 0; i < SIZE; i++) {
        free(A[i]); free(B[i]); free(sum[i]); free(product[i]);
    }
    free(A); free(B); free(sum); free(product);
    free(time1); free(time2);

    return 0;
}
III-A1@cil20:~/Aparna/ex5$ cc ex5.c -lpthread
III-A1@cil20:~/Aparna/ex5$ ./a.out
Enter matrix size (N for NxN): 5 5

[Thread 1] Matrix Addition Complete.
[Thread 2] Matrix Multiplication Complete.

--- Execution Times ---
Addition Thread: 0.0012 ms
Multiplication Thread: 0.0039 ms

--- Matrix A ---
 1  6  4  0  1 
 4  2  3  9  6 
 2  3  9  8  3 
 8  4  1  2  8 
 7  7  7  8  2 

--- Matrix B ---
 4  4  5  9  4 
 2  3  7  9  8 
 7  2  5  6  8 
 7  4  9  3  3 
 2  9  3  7  6 

--- Result of Addition ---
 5 10  9  9  5 
 6  5 10 18 14 
 9  5 14 14 11 
15  8 10  5 11 
 9 16 10 15  8 

--- Result of Multiplication ---
46 39 70 94 90 
116 118 148 141 119 
139 94 157 144 146 
77 126 115 176 126 
151 113 197 206 176

