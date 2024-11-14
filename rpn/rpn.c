#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

#define N 10

void reset_states(int *idx, int *num, bool *err, bool *building)
{
     *idx = 0;
     *num = 0; // Set to 0 to indicate no current number being parsed
     *err = false;
     *building = false;
}

void handle_error(int line, char c)
{
     printf("line %d: error at %c\n", line, c);
}

int main(void)
{
     unsigned int stack[N];
     int idx = 0, num = 0, calc = 0; // num initialized to 0 to indicate no number yet
     int c;
     bool err = false;
     bool building;
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
          else if (isdigit(c))
          {
               num = num * 10 + (c - '0');
               building = true;
               continue;
          }
          else if (building) // Number parsing complete when encountering space or operator
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
               building = false;
               num = 0;
          }

          if (c == ' ') // skip white space
          {
               continue;
          }
          else if (c == '\n') // Reached end of line, print result
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
               reset_states(&idx, &num, &err, &building);
               continue;
          }
          else if (c == '+' || c == '-' || c == '*' || c == '/') // execute operation
          {
               if (idx < 2)
               {
                    handle_error(line, c);
                    err = true;
                    continue;
               }

               int op2 = stack[--idx];
               int op1 = stack[--idx];

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
               stack[idx++] = calc;
          }
          else
          {
               handle_error(line, c);
               err = true;
          }
     }
     return 0;
}
