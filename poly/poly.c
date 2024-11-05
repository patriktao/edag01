#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "error.h"
#include "poly.h"

struct poly_t
{
     int constant;
     const char *string;
     int variables[];
};

poly_t *
new_poly_from_string(const char *s)
{
     struct poly_t *poly = calloc(1, sizeof(struct poly_t));
     poly->string = s;

     int hasConstant = 1;
     int isNegative = 0;
     int isPower = 0;
     int isX = 0;

     int multiple = 1;
     int constant = 1;

     // for every character in the string
     for (int i = 0; i < sizeof(s); i++)
     {
          // print char
          char c = s[i];
          printf("%c \n", c);

          if (isspace(c))
          {
               if (!isX)
               {
                    continue;
               }
               else if (isPower && isX && hasConstant) //-3x^2 and 3x^2
               {
                    isNegative
                        ? (poly->variables[multiple] = -constant)
                        : (poly->variables[multiple] = constant);
               }
               else if (!isPower && isX && hasConstant) //-3x and 3x
               {
                    isNegative ? (poly->variables[multiple] = -constant)
                               : (poly->variables[multiple] = constant);
               }
               else if (!isPower && isX && !hasConstant) // x and -x
               {
                    isNegative ? (poly->variables[1] = -1)
                               : (poly->variables[1] = 1);
               }
               else if (hasConstant)
               {
                    isNegative ? (poly->variables[0] = -c)
                               : (poly->variables[0] = c);
               }
               else
               {
                    printf("error");
               }

               // FIX: this is being run each time there is a whitespace
               constant = 1;
               multiple = 1;
               hasConstant = 0;
               isX = 0;
               isPower = 0;
               isNegative = 0;
          }

          // read characters
          if (c == '-')
          {
               isNegative = 1;
          }
          else if (c == '+')
          {
               isNegative = 0;
          }
          else if (isdigit(c))
          {
               if (isPower)
               {
                    multiple = c;
               }
               else
               {
                    constant = c;
                    hasConstant = 1;
               }
          }
          else if (c == 'x')
          {
               isX = 1;
          }
          else if (c == '^')
          {
               isPower = 1;
          }
     }
     return poly;
}

void free_poly(poly_t *);

poly_t *mul(poly_t *, poly_t *);

void print_poly(poly_t *);

int main()
{
     poly_t *poly = new_poly_from_string("x^2");
}