#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define N 10
#define MAX_LEN 256

void reset_states(int *idx, int *num, bool *building, bool *err)
{
     *idx = 0;
     *num = 0;
     *building = false;
     *err = false;
}

void handle_error(int line, char c)
{
     printf("line %d: error at %c\n", line, c);
}

int main(void)
{
     unsigned int stack[N];          // Stack to store numbers and results
     int idx = 0, num = 0, calc = 0; // Stack index and calculation variable
     int c;
     bool building = false;
     bool err = false;
     int line = 1;

     while ((c = getchar()) != EOF)
     {
          if (err)
          {
               if (c == '\n')
               {
                    reset_states(&idx, &num, &err, &building);
                    line++;
               }
               continue;
          }

          if (isdigit(c))
          {
               building = true;
               num = num * 10 + (c - '0');
               continue;
          }

          if (building)
          {
               if (idx == N)
               {
                    handle_error(line, '0' + num); // Use '0' + num to show parsed number
                    err = true;
               }
               else
               {
                    stack[idx++] = num;
               }
               num = 0;
               building = false;
          }

          if (c == ' ')
          {
               continue;
          }
          else if (c == '\n')
          {
               if (idx != 1)
               {
                    printf("line %d: error at \\n\n", line);
               }
               else
               {
                    printf("line %d: %d\n", line, stack[0]);
               }
               line++;
               reset_states(&idx, &num, &building, &err);
               continue;
          }
          else if (c == '+' || c == '-' || c == '*' || c == '/')
          {
               if (idx < 2)
               {
                    handle_error(line, c);
                    err = true;
                    continue;
               }

               int op2 = stack[--idx]; // pop 1
               int op1 = stack[--idx]; // pop 2

               switch (c)
               {
               case '+':
                    calc = op1 + op2;
                    break;
               case '-':
                    calc = op1 - op2;
                    break;
               case '*':
                    calc = op1 * op2;
                    break;
               case '/':
                    if (op2 == 0)
                    {
                         printf("line %d: error at /\n", line);
                         err = true;
                         continue;
                    }
                    calc = op1 / op2;
                    break;
               }
               stack[idx++] = calc; // add res
          }
          else
          {
               handle_error(line, c);
               err = true;
          }
     }
     return 0;
}
