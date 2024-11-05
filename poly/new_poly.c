#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TERMS 10

typedef struct
{
     int coefficient;
     int power;
} term_t;

typedef struct
{
     term_t terms[MAX_TERMS];
     int term_count;
} poly_t;

void add_term(poly_t *poly, int coefficient, int power)
{
     printf("\ncoeff: %d, power: %d\n", coefficient, power);

     // add to already existing term with same power
     for (int i = 0; i < poly->term_count; i++)
     {
          if (poly->terms[i].power == power)
          {
               poly->terms[i].power += coefficient;
               return;
          }
     }

     // otherwise, add a new term
     if (poly->term_count < MAX_TERMS)
     {
          poly->terms[poly->term_count].coefficient = coefficient;
          poly->terms[poly->term_count].power = power;
          poly->term_count++;
     }
}

poly_t *new_poly_from_string(const char *s)
{
     poly_t *poly = malloc(sizeof(poly_t));

     if (!poly)
     {
          perror("Failed to allocate memory for polynomial");
          return NULL;
     }

     poly->term_count = 0;
     int coefficient, power = 0;
     bool isNegative, hasCoefficient = false;

     for (size_t i = 0; i < strlen(s); i++)
     {
          if (s[i] == '-')
          {
               isNegative = true;
          }
          else if (s[i] == '+')
          {
               isNegative = false;
          }
          else if (isdigit(s[i]))
          {
               coefficient = coefficient * 10 + (s[i] - '0');
               hasCoefficient = true;
          }
          else if (s[i] == 'x')
          {
               power = 1;
               if (!hasCoefficient)
               {
                    coefficient = 1; // default coeffieicnt fo x
               }
               if (isNegative)
               {
                    coefficient = -coefficient;
               }
               if (s[i + 1] == '^')
               {
                    i += 2; // Move past '^'
                    power = 0;
                    while (isdigit(s[i])) // iteratively build the power digit
                    {
                         power = power * 10 + (s[i] - '0');
                         i++;
                    }
                    i--; // Compensate for extra increment in loop
               }
               add_term(poly, coefficient, power); // add term to poly
               // reset
               coefficient = 0;
               power = 0;
               hasCoefficient = false;
          }
          else if (isspace(s[i]))
          {
               continue;
          }
          else
          {
               if (hasCoefficient || isNegative)
               {
                    add_term(poly, isNegative ? -coefficient : coefficient, 0);
                    // reset
                    coefficient = 0;
                    isNegative = false;
                    hasCoefficient = false;
               }
          }
     }

     if (hasCoefficient)
     {
          add_term(poly, isNegative ? -coefficient : coefficient, 0);
     }

     return poly;
}

void print_poly(const poly_t *poly)
{
     for (int i = 0; i < poly->term_count; i++)
     {
          int coeff = poly->terms[i].coefficient;
          int power = poly->terms[i].power;

          if (i > 0 && coeff > 0)
          {
               printf(" + ");
          }
          else if (coeff < 0)
          {
               printf(" - ");
               coeff = -coeff;
          }

          if (coeff != 1 || power == 0)
          {
               printf("%d", coeff);
          }
          if (power > 0)
          {
               printf("x");
               if (power > 1)
               {
                    printf("^%d", power);
               }
          }
     }
     printf("\n");
}

void free_poly(poly_t *);

poly_t *mul(poly_t *a, poly_t *b)
{
     poly_t *result = malloc(sizeof(poly_t));

     for (int i = 0; i < a->term_count; i++)
     {
          for (int j = 0; j < b->term_count; j++)
          {
               int new_power = a->terms[i].power + b->terms[j].power;
               int coefficient = a->terms[i].coefficient * b->terms[j].coefficient;
               add_term(result, coefficient, new_power);
          }
     }
     return result;
}

int main()
{
     char *a = "x + 1";
     char *b = "x - 1";

     poly_t *p;
     poly_t *q;
     poly_t *r;

     printf("Begin polynomial test of (%s) * (%s)\n", a, b);

     p = new_poly_from_string(a);
     q = new_poly_from_string(b);

     print_poly(p);
     print_poly(q);

     r = mul(p, q);
     print_poly(r);
}