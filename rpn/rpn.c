#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

#define N 10

// function to perform operations
typedef int (*operation_func)(int, int);

int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b) { return b == 0 ? 0 : a / b; }

void reset_states(int *idx, int *num, bool *err, bool *building)
{
     *idx = 0;
     *num = 0; // Set to 0 to indicate no current number being parsed
     *err = 0;
     *building = 0;
}

void handle_error(int line, char c)
{
     printf("line %d: error at %c\n", line, c);
}

int main(void)
{
     int stack[N];
     int idx = 0, line = 1;
     int c, num = 0;
     int building = 0; // Tracks if parsing a number

     operation_func operations[128] = {NULL};
     operations['+'] = add;
     operations['-'] = subtract;
     operations['*'] = multiply;
     operations['/'] = divide;

     while ((c = getchar()) != EOF)
     {
          if (isdigit(c))
          {
               num = num * 10 + (c - '0');
               building = 1;
               continue;
          }

          if (building) // Number parsing complete when encountering space or operator
          {
               if (idx >= N) // too many elements in stack
               {
                    handle_error(line, '0' + num);
                    while (c != '\n' && c != EOF)
                         c = getchar(); // Skip line
                    idx = 0;            // Reset stack
                    line++;
                    continue;
               }
               stack[idx++] = num;
               num = 0;
               building = 0;
          }

          if (isspace(c))
          {
               if (c == '\n')
               {
                    if (idx == 1)
                    {
                         printf("line %d: %d\n", line, stack[0]);
                    }
                    else
                    {
                         printf("line %d: error at \\n\n", line);
                    }
                    idx = 0; // Reset stack
                    line++;
               }
               continue;
          }

          if (operations[c]) // execute operation
          {
               if (idx < 2) // not enough elements to pop
               {
                    handle_error(line, c);
                    while (c != '\n' && c != EOF)
                         c = getchar(); // Skip line
                    idx = 0;            // Reset stack
                    line++;
                    continue;
               }

               int op2 = stack[--idx];
               int op1 = stack[--idx];

               if (c == '/' && op2 == 0) // handle division
               {
                    handle_error(line, c);
                    while (c != '\n' && c != EOF)
                         c = getchar(); // Skip line
                    idx = 0;            // Reset stack
                    line++;
                    continue;
               }

               // execute operation and save on stack
               stack[idx++] = operations[c](op1, op2);
          }
          else // invalid character
          {
               handle_error(line, c);
               while (c != '\n' && c != EOF)
                    c = getchar(); // Skip line
               idx = 0;            // Reset stack
               line++;
          }
     }
     return 0;
}
