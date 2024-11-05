#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "error.h"
#include "poly.h"

struct poly_t
{
     char *string;
     int variables[];
     int max_pow;
};

void reset_state(bool *isNeg, bool *isPow, bool *isBuilding, int *num, int *pow)
{
     *isNeg = false;
     *isPow = false;
     *isBuilding = false;
     *num = 1;
     *pow = 0;
}

poly_t *
new_poly_from_string(const char *s)
{
     poly_t *poly = (poly_t *)calloc(1, sizeof(poly_t));
     poly->string = strdup(s);

     poly->max_pow = 0;

     bool isNeg = false;
     bool isPow = false;
     bool isBuilding = false;
     int num = 1;
     int pow = 0;

     // loop through each char
     for (int i = 0; i < strlen(s); i++)
     {
          char c = s[i]; // character

          if (c == '-')
          {
               isNeg = true;
          }
          else if (isdigit(c))
          {
               if (isPow)
               {
                    pow = pow * 10 + (c - '0');
                    if (pow > poly->max_pow)
                    {
                         poly->max_pow = pow;
                    }
                    continue;
               }
               num = num * 10 + (c - '0');
               isBuilding = true;
          }
          else if (c == 'x')
          {
               pow = 1;
               isBuilding = true;
          }
          else if (isBuilding && (isspace(c) || c == '\n' || c == EOF)) // push the e.g. 2x^2 to the array
          {
               isNeg ? (poly->variables[pow] = -num) : (poly->variables[pow] = num);
               reset_state(&isNeg, &isPow, &isBuilding, &num, &pow); // reset values
               if (c == '+')
                    isNeg = false; // Reset sign if '+' is encountered
          }
          else if (c == '^')
          {
               isPow = true;
          }
          else if (isspace(c))
          {
               continue;
          }
          else if (c == '+')
          {
               isNeg = false;
          }
          else
          {
               printf("Error: Unexpected character '%c'\n", c);
               free(poly->variables);
               free(poly->string);
               free(poly);
               return NULL;
          }
     }

     return poly;
}

void free_poly(poly_t *poly)
{
     if (poly != NULL)
     {
          free(poly->string);
          free(poly->variables);
          free(poly);
     }
}

poly_t *mul(poly_t *, poly_t *);

void print_poly(poly_t *poly)
{
     for (int i = poly->max_pow; i >= 0; i--)
     {
          int coeff = poly->variables[i];
          if (coeff == 0)
               continue; // Skip zero coefficients
          int pow = i;

          if (pow == 0)
          {
               printf("%d", coeff);
               continue;
          }
     };
}
