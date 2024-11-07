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
     // if term already exists, ensure same power and add coefficients
     for (int i = 0; i < poly->term_count; i++)
     {
          if (poly->terms[i].power == power)
          {
               poly->terms[i].coefficient += coefficient;
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
     poly_t *poly = calloc(1, sizeof(poly_t));

     if (!poly)
     {
          perror("Failed to allocate memory for polynomial");
          return NULL;
     }

     int coefficient, power = 0;
     bool isNegative, hasCoefficient = false;

     for (size_t i = 0; s[i] != '\0'; i++)
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
               power = 1; // default power is 1
               if (!hasCoefficient)
               {
                    coefficient = 1; // // Default coefficient is 1 if none is specified
               }
               if (isNegative)
               {
                    coefficient = -coefficient;
               }
               if (s[i + 1] == '^')
               {
                    i += 2; // Move past '^'
                    power = 0;
                    while (isdigit(s[i])) // iteratively build the power number
                    {
                         power = power * 10 + (s[i] - '0');
                         i++;
                    }
                    i--; // Compensate for extra increment in loop
               }
               add_term(poly, coefficient, power);
               coefficient = 0;
               power = 0;
               hasCoefficient = false;
          }
          else if (isspace(s[i]))
          {
               continue;
          }
     }

     if (hasCoefficient) // add constant
     {
          add_term(poly, isNegative ? -coefficient : coefficient, 0);
          // reset
          coefficient = 0;
          isNegative = false;
          hasCoefficient = false;
     }

     return poly;
}

void free_poly(poly_t *poly)
{
     free(poly);
}

poly_t *mul(poly_t *a, poly_t *b)
{
     poly_t *result = calloc(1, sizeof(poly_t));

     if (!result)
     {
          perror("Failed to allocate memory for result polynomial");
          return NULL;
     }

     for (int i = 0; i < a->term_count; i++)
     {
          for (int j = 0; j < b->term_count; j++)
          {
               int new_power = a->terms[i].power + b->terms[j].power;
               int new_coefficient = a->terms[i].coefficient * b->terms[j].coefficient;
               add_term(result, new_coefficient, new_power);
          }
     }
     return result;
}

void print_poly(const poly_t *poly)
{
     for (int i = 0; i < poly->term_count; i++)
     {
          int coeff = poly->terms[i].coefficient;
          int power = poly->terms[i].power;

          // skip zero coefficient terms
          if (coeff == 0)
               continue;

          // print neg or pos symbol
          if (i > 0 && coeff > 0)
          {
               printf(" + ");
          }
          else if (coeff < 0)
          {
               printf(" - ");
               coeff = -coeff;
          }

          // print the term
          if (coeff != 1 || power == 0)
          {
               printf("%d ", coeff);
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

static void poly_test(char *a, char *b)
{
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

     free_poly(p);
     free_poly(q);
     free_poly(r);

     printf("End polynomial test of (%s) * (%s)\n\n\n", a, b);
}

int main(void)
{
     poly_test("x^2 - 7x + 1", "3x + 2");
     poly_test("x^10000000 + 2", "2x^2 + 3x + 4");
     return 0;
}