#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

// Student structure
struct Student {
    char name[50];
    int roll;
    int marks[5];
    int total;
    char grade;
};

// Function to read input from the file
int get_input(struct Student students[]) {
    FILE *fp = fopen("students.txt", "r");
    int n = 0;

    if(fp == NULL) {
        printf("File not found\n");
        return 0;
    }

    while(fscanf(fp, "%s %d %d %d %d %d %d",
        students[n].name,
        &students[n].roll,
        &students[n].marks[0],
        &students[n].marks[1],
        &students[n].marks[2],
        &students[n].marks[3],
        &students[n].marks[4]) != EOF) {
            n++;
    }

    fclose(fp);
    return n;
}

// Function to calculate total and grade
void calculateResult(struct Student *s) {
    s->total = 0;

    for(int i = 0; i < 5; i++)
        s->total += s->marks[i];

    float avg = s->total / 5.0;

    if(avg >= 90) s->grade = 'A';
    else if(avg >= 75) s->grade = 'B';
    else if(avg >= 60) s->grade = 'C';
    else if(avg >= 50) s->grade = 'D';
    else s->grade = 'F';
}

int main(int argc, char *argv[]) {
    int rank, size, n;
    struct Student students[MAX];
    struct Student local;

    MPI_Datatype MPI_STUDENT;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // -------- Create Derived Datatype --------

    int blocklengths[5] = {50, 1, 5, 1, 1};
    MPI_Datatype types[5] = {MPI_CHAR, MPI_INT, MPI_INT, MPI_INT, MPI_CHAR};
    MPI_Aint displacements[5];

    struct Student temp;
    MPI_Aint base;

    MPI_Get_address(&temp, &base);
    MPI_Get_address(&temp.name, &displacements[0]);
    MPI_Get_address(&temp.roll, &displacements[1]);
    MPI_Get_address(&temp.marks, &displacements[2]);
    MPI_Get_address(&temp.total, &displacements[3]);
    MPI_Get_address(&temp.grade, &displacements[4]);

    for(int i = 0; i < 5; i++)
        displacements[i] -= base;

    MPI_Type_create_struct(5, blocklengths, displacements, types, &MPI_STUDENT);
    MPI_Type_commit(&MPI_STUDENT);

    double start_time, end_time;

    // -------- Master reads input --------
    if(rank == 0) {
        start_time = MPI_Wtime(); // Start timing
        n = get_input(students);
        if (n == 0) {
            MPI_Abort(MPI_COMM_WORLD, 1); // Abort if no data read
        }
        printf("Number of students read: %d\n", n);
    }

    // Broadcast number of students
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(n == 0) {
        MPI_Finalize();
        return 0; // Exit if no valid students
    }

    // Scatter one student to each process
    MPI_Scatter(students, 1, MPI_STUDENT,
                &local, 1, MPI_STUDENT,
                0, MPI_COMM_WORLD);

    // Each process calculates result
    double process_start_time = MPI_Wtime(); // Start process timing
    calculateResult(&local);
    double process_end_time = MPI_Wtime(); // End process timing
    double process_time = (process_end_time - process_start_time) * 1000; // Convert to milliseconds

    // Gather results back
    MPI_Gather(&local, 1, MPI_STUDENT,
               students, 1, MPI_STUDENT,
               0, MPI_COMM_WORLD);

    // -------- Master writes output --------
    if(rank == 0) {
        FILE *out = fopen("grades.txt", "w");

        for(int i = 0; i < n; i++) {
            fprintf(out, "%s %d Total=%d Grade=%c\n",
                students[i].name,
                students[i].roll,
                students[i].total,
                students[i].grade);
        }

        fclose(out);
        printf("Grades written to grades.txt\n");

        end_time = MPI_Wtime(); // End timing
        printf("Total execution time for Process 0: %f milliseconds\n", (end_time - start_time) * 1000);
    }

    // Print execution time for each process
    double total_time;
    MPI_Reduce(&process_time, &total_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if(rank != 0) {
        printf("Process %d execution time: %f milliseconds\n", rank, process_time);
    }

    MPI_Type_free(&MPI_STUDENT);
    MPI_Finalize();
    return 0;
}
III-A1@cil20:~/Aparna/ex4$ cat students.txt
Aparna 1 80 75 90 85 88
Rahul 2 60 70 65 55 72
Divya 3 95 92 96 94 90
Kiran 4 45 50 40 48 52
Sneha 5 78 82 74 80 76
Arun 6 88 84 79 91 87

III-A1@cil20:~/Aparna/ex4$ mpicc ex4_new.c
III-A1@cil20:~/Aparna/ex4$ mpiexec ./a.out
Number of students read: 6
Process 1 execution time: 0.000246 milliseconds
Process 2 execution time: 0.000233 milliseconds
Process 4 execution time: 0.000254 milliseconds
Process 3 execution time: 0.000343 milliseconds
Process 5 execution time: 0.000221 milliseconds
Grades written to grades.txt
Total execution time for Process 0: 0.688570 milliseconds

III-A1@cil20:~/Aparna/ex4$ cat students.txt
Aparna 1 80 75 90 85 88
Rahul 2 60 70 65 55 72
Divya 3 95 92 96 94 90
Kiran 4 45 50 40 48 52
Sneha 5 78 82 74 80 76
Arun 6 88 84 79 91 87

III-A1@cil20:~/Aparna/ex4$ cat grades.txt
Aparna 1 Total=418 Grade=B
Rahul 2 Total=322 Grade=C
Divya 3 Total=467 Grade=A
Kiran 4 Total=235 Grade=F
Sneha 5 Total=390 Grade=B
Arun 6 Total=429 Grade=B


