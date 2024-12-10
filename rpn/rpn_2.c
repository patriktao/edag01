#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define N 10
#define MAX_LINE_LEN 50 // Maximum line length

int main(void)
{
     int idx = 0, num = 0, line = 1, stack[N];
     bool building = 0;
     char buffer[MAX_LINE_LEN];

     while (fgets(buffer, MAX_LINE_LEN, stdin))
     {
          idx = 0;
          num = 0;
          building = 0;

          for (char *c = buffer; *c != '\0'; c++) // Iterate until '\0'
          {
               if (*c >= '0' && *c <= '9') // Start building a number
               {
                    num = 0;                       // Reset number
                    while (*c >= '0' && *c <= '9') // Build number in one pass
                    {
                         num = num * 10 + (*c - '0');
                         c++; // Move to the next character
                    }
                    building = 1;
                    c--; // Step back to reprocess the non-digit character
               }
               else
               {
                    if (building)
                    {
                         if (idx == N)
                         {
                              printf("line %d: error at %c\n", line, '0' + num);
                              line++;
                              break;
                         }
                         stack[idx++] = num; // add to stack
                         building = 0;
                    }

                    if (*c == ' ')
                         continue;
                    else if (*c == '\n' || *c == '\0') // End of line
                    {
                         if (idx != 1)
                              printf("line %d: error at \\n\n", line);
                         else
                              printf("line %d: %d\n", line, stack[0]); // print result
                         line++;
                         break;
                    }
                    else if (*c == '+' || *c == '-' || *c == '*' || *c == '/') // Execute operation
                    {
                         if (idx < 2)
                         {
                              printf("line %d: error at %c\n", line, *c);
                              line++;
                              break;
                         }

                         int op2 = stack[--idx];
                         int op1 = stack[--idx];

                         if (*c == '/' && op2 == 0)
                         {
                              printf("line %d: error at %c\n", line, '/');
                              line++;
                              break;
                         }

                         stack[idx++] = (*c == '+') ? (op1 + op2) : (*c == '-') ? (op1 - op2)
                                                                : (*c == '*')   ? (op1 * op2)
                                                                                : (op1 / op2);
                    }
                    else // Invalid character
                    {
                         printf("line %d: error at %c\n", line, *c);
                         line++;
                         break;
                    }
               }
          }
     }
     return 0;
}