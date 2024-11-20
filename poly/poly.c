#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "poly.h"

#define MAX_TERMS 10000

typedef struct
{
     int coefficient;
     int power;
} term_t;

struct poly_t
{
     term_t terms[MAX_TERMS];
     int term_count;
};

void add_term(poly_t *poly, int coefficient, int power)
{
     if (coefficient == 0)
          return; // Ignore zero terms

     // Binary search to find the correct position (if terms are sorted)
     int pos = -1;
     int left = 0;
     int right = poly->term_count - 1;

     while (left <= right)
     {
          int mid = left + (right - left) / 2;
          if (poly->terms[mid].power == power)
          {
               pos = mid;
               break;
          }
          else if (poly->terms[mid].power > power)
          {
               left = mid + 1;
          }
          else
          {
               right = mid - 1;
          }
     }

     // Update the coefficient if power already exists
     if (pos != -1)
     {
          poly->terms[pos].coefficient += coefficient;

          // Remove the term if the coefficient becomes zero
          if (poly->terms[pos].coefficient == 0)
          {
               for (int i = pos; i < poly->term_count - 1; i++)
               {
                    poly->terms[i] = poly->terms[i + 1];
               }
               poly->term_count--;
          }
          return;
     }

     // determine insertion pos
     pos = left;

     // Otherwise: insert a new term with the power
     if (poly->term_count < MAX_TERMS)
     {
          // the terms after pos are shifted one pos to right to make room for new term
          // poly->term_count index increases the array by one, so after this index (pos) we shift the elements forward to make space for the new term
          for (int i = poly->term_count; i > pos; i--)
          {
               poly->terms[i] = poly->terms[i - 1];
          }
          poly->terms[pos].coefficient = coefficient;
          poly->terms[pos].power = power;
          poly->term_count++;
          return;
     }
     else
     {
          {
               fprintf(stderr, "Error: Maximum term count exceeded\n");
          }
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

     int coefficient = 0;
     int power = 0;
     bool isNegative = false;
     bool hasCoefficient = false;

     size_t len = strlen(s);

     for (size_t i = 0; i <= len; i++)
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
          else if (isspace(s[i]) || s[i] == '\0') // Handle end of term or input
          {
               if (hasCoefficient) // If parsing a constant term
               {
                    add_term(poly, isNegative ? -coefficient : coefficient, 0);
                    coefficient = 0;
                    isNegative = false;
                    hasCoefficient = false;
               }
          }
          else if (s[i] != '\0') // Unexpected characters
          {
               fprintf(stderr, "Warning: Ignoring unexpected character '%c'\n", s[i]);
          }
     }
     return poly;
}

void free_poly(poly_t *poly)
{
     free(poly);
}

poly_t *mul(poly_t *a, poly_t *b)
{
     poly_t *result = malloc(sizeof(poly_t));

     if (!result)
     {
          perror("Failed to allocate memory for result polynomial");
          return NULL;
     }

     result->term_count = 0;

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

void print_poly(poly_t *poly)
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
