#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "poly.h"

#define MAX_TERMS 10000
#define MAX_POWER 10000
#define INITIAL_CAPACITY 10

typedef struct
{
     int coefficient;
     int power;
} term_t;

struct poly_t
{
     term_t *terms;
     int term_count;
     int capacity;
};

void init_poly(poly_t *poly)
{
     poly->term_count = 0;
     poly->capacity = INITIAL_CAPACITY;
     poly->terms = malloc(poly->capacity * sizeof(term_t));
     if (!poly->terms)
     {
          perror("Failed to allocate memory for polynomial terms");
          exit(EXIT_FAILURE);
     }
}

void free_poly(poly_t *poly)
{
     free(poly->terms);
     free(poly);
}

// allocate more capacity
void ensure_capacity(poly_t *poly)
{
     if (poly->term_count >= poly->capacity)
     {
          poly->capacity *= 2;
          poly->terms = realloc(poly->terms, poly->capacity * sizeof(term_t));
          if (!poly->terms)
          {
               perror("Failed to reallocate memory for polynomial terms");
               exit(EXIT_FAILURE);
          }
     }
}

void add_term(poly_t *poly, int coefficient, int power)
{
     if (coefficient == 0)
          return;

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

     // Insert new term
     ensure_capacity(poly);
     pos = left;

     for (int i = poly->term_count; i > pos; i--)
     {
          poly->terms[i] = poly->terms[i - 1];
     }
     poly->terms[pos].coefficient = coefficient;
     poly->terms[pos].power = power;
     poly->term_count++;
}

poly_t *new_poly_from_string(const char *s)
{
     poly_t *poly = malloc(sizeof(poly_t));

     if (!poly)
     {
          perror("Failed to allocate memory for polynomial");
          return NULL;
     }

     init_poly(poly);

     int coefficient = 0;
     int power = 0;
     bool isNegative = false;
     bool hasCoefficient = false;

     size_t len = strlen(s);

     for (size_t i = 0; i <= len; i++)
     {
          if (s[i] == '-')
          {
               isNegative = 1;
          }
          else if (s[i] == '+')
          {
               isNegative = 0;
          }
          else if (isdigit(s[i]))
          {
               coefficient = coefficient * 10 + (s[i] - '0');
               hasCoefficient = 1;
          }
          else if (s[i] == 'x')
          {
               power = 1;
               if (!hasCoefficient)
               {
                    coefficient = 1;
               }
               if (isNegative)
               {
                    coefficient = -coefficient;
               }
               if (s[i + 1] == '^')
               {
                    i += 2; // Move past
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
               hasCoefficient = 0;
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

// Multiply two polynomials
poly_t *mul(poly_t *a, poly_t *b)
{
     poly_t *result = malloc(sizeof(poly_t));
     if (!result)
     {
          perror("Failed to allocate memory for result polynomial");
          return NULL;
     }
     init_poly(result);

     int coefficients[MAX_POWER] = {0}; // map

     for (int i = 0; i < a->term_count; i++)
     {
          for (int j = 0; j < b->term_count; j++)
          {
               int new_power = a->terms[i].power + b->terms[j].power;
               coefficients[new_power] += a->terms[i].coefficient * b->terms[j].coefficient;
          }
     }

     for (int p = 0; p < MAX_POWER; p++)
     {
          if (coefficients[p] != 0) // lägg bara till de som inte har 0 coefficient.
          {
               add_term(result, coefficients[p], p);
          }
     }

     return result;
}

// Print the polynomial
void print_poly(poly_t *poly)
{
     for (int i = 0; i < poly->term_count; i++)
     {
          int coeff = poly->terms[i].coefficient;
          int power = poly->terms[i].power;

          if (coeff == 0)
               continue;

          if (i > 0 && coeff > 0)
          {
               printf(" + ");
          }
          else if (coeff < 0)
          {
               if (i == 0)
               {
                    printf("- ");
               }
               else
               {
                    printf(" - ");
               }
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
