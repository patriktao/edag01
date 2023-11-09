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
     printf("\nMax z:\n");
     for (int i = 0; i < n; i++)
     {
          if (i == n - 1)
          {
          printf("%.3lf %s%d \n", coefficients[i], "x", i);
          }
          else
          {
               printf("%10.3lf %s%d %s", coefficients[i], "x", i, "+ ");
          }
     }

     printf("\nConstraints:\n");
     for (int i = 0; i < m; i++)
     {
          for (int j = 0; j < n; j++)
          {
               if (j == 0)
               {
                    printf("%10.3lf %s%d", matrix[i][j], "x", j);
               }
               else
               {
                    if (matrix[i][j] >= 0)
                    {
                         printf(" + %.3lf %s%d", matrix[i][j], "x", j);
                    }
                    else
                    {
                         printf(" - %.3lf %s%d", -matrix[i][j], "x", j);
                    }
               }
          }
          printf(" %s %.3lf\n", "\u2264", values[i]);
     }
}

void free_matrix(double **matrix, int m)
{
     // Free the allocated memory from each row and the variable
     for (int i = 0; i < m; i++)
     {
          free(matrix[i]);
     }
     free(matrix);
}

void free_array(double *array)
{
     free(array);
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
     double *c_coefficients = (double *)calloc(n, sizeof(double));
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
     double *b_values = (double *)calloc(m, sizeof(double));
     for (int i = 0; i < m; i++)
     {
          scanf("%lf", &b_values[i]);
     }

     printRes(m, n, c_coefficients, b_values, matrix);

     free_matrix(matrix, m);
     free_array(c_coefficients);
     free_array(b_values);

     return 0;
}