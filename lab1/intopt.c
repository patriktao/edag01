/* 1. Hello Word */
#include <stdio.h>
#include <stdlib.h>

double **make_matrix(int m, int n)
{
     /* (*matrix) points to an array of pointers, and each of the pointers points to an array of integers, forming the rows of the matrix */
     double **matrix;

     /* Allocate memory for the Matrix variable */
     matrix = calloc(m, sizeof(double *));

     /* Dynamically allocate memory for each cell */
     for (int i = 0; i < m; i += 1)
     {
          matrix[i] = calloc(n, sizeof(double));
     }

     return matrix;
}

void printRes(int m, int n, double *coefficients, double *values, double **matrix)
{

     printf("\nC-coefficients:\n");
     for (int i = 0; i < m; i++)
     {
          printf("%10.3lf", coefficients[i]);
     }

     printf("\nB-values:\n");
     for (int i = 0; i < m; i++)
     {
          printf("%10.3lf", values[i]);
     }

     // Print Matrix
     printf("\nMatrix:\n");
     for (int i = 0; i < m; i++)
     {
          for (int j = 0; j < m; j++)
          {
               printf("%10.3lf", matrix[i][j]);
          }
          printf("\n");
     }
}

int main(int argc, char **argv)
{
     int m, n;
     // m is number of constraints
     // n is number of decision values

     // Read the dimensions of the matrix from the file
     if (scanf("%d %d", &m, &n) != 2)
     {
          printf("Invalid input in the file. Please check the format.\n");
          return 1;
     }

     // Read  the c-coefficients
     double c_coefficients[n];
     for (int i = 0; i < n; i++)
     {
          scanf("%lf", &c_coefficients[i]);
     }

     // Create matrix
     double **matrix = make_matrix(m, n);
     
     // Fill matrix
     for (int i = 0; i < m; i++)
     {
          for (int j = 0; j < n; j++)
          {
               if (scanf("%lf", &matrix[i][j]) != 1)
               {
                    printf("Invalid input. Please enter an integer.\n");
                    return 1;
               }
          }
     }

     // Read  the c-coefficients
     double b_values[m];
     for (int i = 0; i < m; i++)
     {
          scanf("%lf", &b_values[i]);
     }

     printRes(m, n, c_coefficients, b_values, matrix);

     // Free the allocated memory from each row and the variable
     for (int i = 0; i < m; i++)
     {
          free(matrix[i]);
     }
     free(matrix);

     return 0;
}
