#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

double EPSILON = 10e-6;

struct simplex_t
{
     int m;
     int n;
     int *var;
     double **a;
     double *b;
     double *x;
     double *c;
     double y;
};

struct node_t
{
     int m; /* constraints */
     int n; /* decision variables */
     int k; /* parent branches on x_k */
     int h; /* branch on x_h */
     double xh;
     double ak;   /* parent a_k */
     double bk;   /* parent b_k */
     double *min; /* lower bounds */
     double *max; /* upper bounds */
     double **a;  /* A matris */
     double *b;
     double *x;
     double *c;
     double z; /* The results */
};

struct set_t
{
     struct node_t **nodes;
     int count;
     int alloc;
};

// Declaration

double simplex(int m, int n, double **a, double *b, double *c, double *x, double y);

double xsimplex(int m, int n, double **a, double *b, double *c, double *x, double y, int *var, int h);

void pivot(struct simplex_t *s, int row, int col);

int initial(struct simplex_t *s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var);

int select_nonbasic(struct simplex_t *s);

int init(struct simplex_t *s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var);

int select_nonbasic(struct simplex_t *s)
{
     int selected_col = -1;
     double max_steepest = -INFINITY;
     int approx_rows = s->m > 6 ? 6 : s->m; // Approximate for up to 6 rows

     for (int i = 0; i < s->n; i++)
     {
          if (s->c[i] > EPSILON)
          {
               double norm_approx = 0.0; // Compute the L^1 norm of the column
               int j = 0;

               // Unroll the loop for multiples of 4
               for (; j + 3 < approx_rows; j += 4)
               {
                    norm_approx += fabs(s->a[j][i]) + fabs(s->a[j + 1][i]) +
                                   fabs(s->a[j + 2][i]) + fabs(s->a[j + 3][i]);
               }

               // Handle remaining rows
               for (; j < approx_rows; j++)
               {
                    norm_approx += fabs(s->a[j][i]);
               }

               if (norm_approx > EPSILON)
               {
                    double steepest_value = fabs(s->c[i]) / norm_approx;

                    if (steepest_value > max_steepest)
                    {
                         max_steepest = steepest_value;
                         selected_col = i;
                    }
               }
          }
     }

     return selected_col;
}

struct node_t *initial_node(int m, int n, double **a, double *b, double *c)
{
     struct node_t *p = malloc(1 * sizeof(struct node_t));
     int i, j;

     p->a = malloc((m + 1) * sizeof(double *)); // Allocate memory for each row
     for (i = 0; i < m + 1; i++)
     {
          p->a[i] = malloc((n + 1) * sizeof(double)); // Allocate memory for each column
     }

     p->b = malloc((m + 1) * sizeof(double));
     p->c = malloc((n + 1) * sizeof(double));
     p->x = malloc((n + 1) * sizeof(double));
     p->min = malloc(n * sizeof(double));
     p->max = malloc(n * sizeof(double));
     p->m = m;
     p->n = n;

     // copy a, b, and c parameters to p;
     for (i = 0; i < m; i++)
     {
          memcpy(p->a[i], a[i], n * sizeof(double));
     }
     memcpy(p->b, b, m * sizeof(double));
     memcpy(p->c, c, n * sizeof(double));

     // min and max
     for (i = 0; i < n; i++)
     {
          p->min[i] = -INFINITY;
          p->max[i] = +INFINITY;
     }

     return p;
}

struct node_t *extend(struct node_t *p, int m, int n, double **a, double *b, double *c, int k, double ak, double bk)
{
     struct node_t *q = malloc(sizeof(struct node_t));

     int i, j;

     q->k = k;
     q->ak = ak;
     q->bk = bk;

     if (ak > 0 && p->max[k] < INFINITY)
     {
          q->m = p->m;
     }
     else if (ak < 0 && p->min[k] > 0)
     {
          q->m = p->m;
     }
     else
     {
          q->m = p->m + 1;
     }

     q->n = p->n;
     q->h = -1;
     q->a = malloc((q->m + 1) * sizeof(double *));

     for (i = 0; i < q->m + 1; i++)
     {
          q->a[i] = malloc((q->n + 1) * sizeof(double));
          memset(q->a[i], 0, (q->n + 1) * sizeof(double));
     }

     q->b = malloc((q->m + 1) * sizeof(double));
     q->c = malloc((q->n + 1) * sizeof(double));
     q->x = malloc((q->n + 1) * sizeof(double));
     q->min = malloc(n * sizeof(double));
     q->max = malloc(n * sizeof(double));

     // Copy p.min and p.max to q
     memcpy(q->min, p->min, n * sizeof(double));
     memcpy(q->max, p->max, n * sizeof(double));

     // Copy the first m rows of parameter a to q.a
     for (i = 0; i < m; i++)
     {
          memcpy(q->a[i], a[i], n * sizeof(double));
     }

     // Copy the first m elements of parameter b to q.b
     memcpy(q->b, b, m * sizeof(double));

     // Copy parameter c to q.c
     memcpy(q->c, c, n * sizeof(double));

     if (ak > 0)
     {
          if (q->max[k] == INFINITY || bk < q->max[k])
          {
               q->max[k] = bk;
          }
     }
     else if (q->min[k] == -INFINITY || -bk > q->min[k])
     {
          q->min[k] = -bk;
     }

     for (i = m, j = 0; j < n; j++)
     {
          if (q->min[j] > -INFINITY)
          {
               q->a[i][j] = -1;
               q->b[i] = -q->min[j];
               i++;
          }
          if (q->max[j] < INFINITY)
          {
               q->a[i][j] = 1;
               q->b[i] = q->max[j];
               i++;
          }
     }

     return q;
}

int is_integer(double *xp)
{
     double x = *xp;
     double r = lround(x);

     if (fabs(r - x) < EPSILON)
     {
          *xp = r;
          return 1;
     }
     else
     {
          return 0;
     }
}

int integer(struct node_t *p)
{
     int i;

     for (int i = 0; i < p->n; i += 4)
     {
          if (!is_integer(&p->x[i]) ||
              (i + 1 < p->n && !is_integer(&p->x[i + 1])) ||
              (i + 2 < p->n && !is_integer(&p->x[i + 2])) ||
              (i + 3 < p->n && !is_integer(&p->x[i + 3])))
          {
               return 0;
          }
     }
     return 1;
}

void delete_node(struct node_t *p)
{
     for (int i = 0; i < p->m + 1; i++)
     {
          free(p->a[i]); // free all node elements in the A array
     }
     free(p->a);
     free(p->b);
     free(p->c);
     free(p->x);
     free(p->min);
     free(p->max);
     free(p);
}

void bound(struct node_t *p, struct set_t *h, double *zp, double *x)
{
     if (p->z > *zp)
     {
          *zp = p->z;
          memcpy(x, p->x, (p->n + 1) * sizeof(double));

          // remove and delete all nodes q in h with q.z < p.z
          for (int i = 0; i < h->alloc; i++)
          {
               if (!h->nodes[i] || h->nodes[i]->z >= p->z)
               {
                    continue;
               }

               delete_node(h->nodes[i]);
               h->nodes[i] = NULL; // set it to NULL
               h->count--;         // decrease the count
          }
     }
}

#include <float.h>

#define RELIABILITY_THRESHOLD 3

// Structures to store pseudocosts and branch counts
double *pseudocost_up;
double *pseudocost_down;
int *branch_count;

// Initialize pseudocosts and branch counts
void initialize_branching_data(int num_vars)
{
     pseudocost_up = (double *)malloc(num_vars * sizeof(double));
     pseudocost_down = (double *)malloc(num_vars * sizeof(double));
     branch_count = (int *)malloc(num_vars * sizeof(int));

     for (int i = 0; i < num_vars; i++)
     {
          pseudocost_up[i] = 1.0;   // Default initial pseudocost
          pseudocost_down[i] = 1.0; // Default initial pseudocost
          branch_count[i] = 0;      // No branches yet
     }
}

void update_pseudocosts(int var, double obj_change_up, double obj_change_down)
{
     branch_count[var] += 1;
     pseudocost_up[var] = (pseudocost_up[var] * (branch_count[var] - 1) + obj_change_up) / branch_count[var];
     pseudocost_down[var] = (pseudocost_down[var] * (branch_count[var] - 1) + obj_change_down) / branch_count[var];
}

int branch(struct node_t *q, double z)
{
     if (q->z < z)
     {
          return 0;
     }

     int best_var = -1;
     double best_score = -DBL_MAX;

     for (int h = 0; h < q->n; h++)
     {
          if (!is_integer(&q->x[h]))
          {
               double min = (q->min[h] == -INFINITY) ? 0 : q->min[h];
               double max = q->max[h];

               if (floor(q->x[h]) < min || ceil(q->x[h]) > max)
               {
                    continue;
               }

               double score;
               if (branch_count[h] >= RELIABILITY_THRESHOLD)
               {
                    // Use pseudocosts to estimate impact
                    score = pseudocost_up[h] + pseudocost_down[h];
               }
               else
               {
                    // Use strong branching to estimate impact
                    double obj_change_up = fabs(q->z - floor(q->x[h]));
                    double obj_change_down = fabs(q->z - ceil(q->x[h]));
                    score = obj_change_up + obj_change_down;

                    // Update pseudocosts for future reliability
                    update_pseudocosts(h, obj_change_up, obj_change_down);
               }

               if (score > best_score)
               {
                    best_score = score;
                    best_var = h;
               }
          }
     }

     if (best_var != -1)
     {
          q->h = best_var;
          q->xh = q->x[best_var];
          return 1;
     }

     return 0;
}

void add(struct set_t *h, struct node_t *p)
{
     int i;

     // check if reallocation is needed
     if (h->count >= h->alloc)
     {
          // Allocate on more place size
          h->alloc *= 2;
          struct node_t **new_nodes = (struct node_t **)realloc(h->nodes, h->alloc * sizeof(struct node_t *));
          h->nodes = new_nodes;

          // initialize the newly allocated
          memset(&h->nodes[h->count], 0, (h->alloc - h->count) * sizeof(struct node_t *));

          // Add the new node and update count
          // h->nodes[h->count++] = p;
     }

     for (i = 0; i < h->alloc; i++)
     {
          if ((h->nodes)[i] == NULL)
          {
               h->nodes[i] = p;
               h->count++;
               return;
          }
     }
}

void free_branching_data()
{
     free(pseudocost_up);
     free(pseudocost_down);
     free(branch_count);
}

void succ(struct node_t *p, struct set_t *h, int m, int n, double **a, double *b, double *c, int k, double ak, double bk, double *zp, double *x)
{
     struct node_t *q = extend(p, m, n, a, b, c, k, ak, bk);

     if (q == NULL)
     {
          return;
     }

     q->z = simplex(q->m, q->n, q->a, q->b, q->c, q->x, 0); // solve

     if (isfinite(q->z))
     {
          if (integer(q))
          {
               bound(q, h, zp, x);
          }
          else if (branch(q, *zp))
          {
               add(h, q);
               return;
          }
     }

     delete_node(q);
}

struct set_t *create_set(struct node_t *p)
{
     struct set_t *h = malloc(sizeof(struct set_t)); // Allocating memory for the set structure

     h->alloc = 10;
     h->count = 0;
     h->nodes = malloc(h->alloc * sizeof(struct node_t *)); // Allocating memory for the array of node pointers

     // Use memset to initialize all elements of the array to NULL
     memset(h->nodes, 0, h->alloc * sizeof(struct node_t *));

     add(h, p);

     return h;
}

struct node_t *pop(struct set_t *h)
{
     struct node_t *p;
     for (int i = 0; i < h->alloc; i++)
     {
          if ((h->nodes)[i])
          {
               p = h->nodes[i];
               h->nodes[i] = NULL;
               h->count--;
               break;
          }
     }
     return p;
}

void free_set(struct set_t *h)
{
     free(h->nodes);
     free(h);
}

double intopt(int m, int n, double **a, double *b, double *c, double *x)
{
     initialize_branching_data(n);

     struct node_t *p = initial_node(m, n, a, b, c);
     struct set_t *h = create_set(p);

     double z = -INFINITY; // best integer solution so far

     p->z = simplex(p->m, p->n, p->a, p->b, p->c, p->x, 0);

     if (integer(p) || !isfinite(p->z))
     {
          z = p->z;
          if (integer(p))
          {
               memcpy(x, p->x, (p->n + 1) * sizeof(double));
          }
          delete_node(p);
          free_set(h);
          free_branching_data(); // Free branching data
          return z;
     }

     branch(p, z);

     while (h->count > 0)
     {
          struct node_t *p = pop(h);
          succ(p, h, m, n, a, b, c, p->h, 1, floor(p->xh), &z, x);
          succ(p, h, m, n, a, b, c, p->h, -1, -ceil(p->xh), &z, x);
          delete_node(p);
     }

     free_set(h);
     free_branching_data(); // Free branching data

     return (z == -INFINITY) ? NAN : z;
}

int init(struct simplex_t *s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var)
{
     int i, k;

     s->m = m;     /* nbr constraints */
     s->n = n;     /* nbr decision variables */
     s->var = var; /* variables, not yet allocated array */
     s->a = a;     /* A matrix */
     s->b = b;     /* b array */
     s->x = x;     /* x array */
     s->c = c;     /* c array */
     s->y = y;     /* double */

     if (s->var == NULL)
     {
          s->var = malloc((m + n + 1) * sizeof(int));
          for (i = 0; i < m + n; i++)
          {
               s->var[i] = i;
          }
     }

     for (k = 0, i = 1; i < m; i++)
     {
          if (b[i] < b[k])
          {
               k = i;
          };
     }
     return k;
}

void pivot(struct simplex_t *restrict s, int row, int col)
{
     double **a = s->a;
     double *b = s->b;
     double *c = s->c;
     int m = s->m;
     int n = s->n;

     // Cache pivot-related values
     double pivotValue = a[row][col];
     double pivotInv = 1.0 / pivotValue;
     double cCol = c[col];
     double bRow = b[row];

     // Swap variable indices
     int tmp = s->var[col];
     s->var[col] = s->var[n + row];
     s->var[n + row] = tmp;

     // Update objective value
     s->y += cCol * bRow * pivotInv;

     // Update c vector with unrolling
     for (int i = 0; i < n; i += 4)
     {
          if (i != col)
               c[i] -= cCol * a[row][i] * pivotInv;
          if (i + 1 < n && (i + 1) != col)
               c[i + 1] -= cCol * a[row][i + 1] * pivotInv;
          if (i + 2 < n && (i + 2) != col)
               c[i + 2] -= cCol * a[row][i + 2] * pivotInv;
          if (i + 3 < n && (i + 3) != col)
               c[i + 3] -= cCol * a[row][i + 3] * pivotInv;
     }
     c[col] = -cCol * pivotInv;

     // Update rows other than the pivot row with unrolling
     for (int i = 0; i < m; i++)
     {
          if (i != row)
          {
               double factor = a[i][col] * pivotInv;
               b[i] -= factor * bRow;

               // Unroll loop for updating a[i][j]
               for (int j = 0; j < n; j += 4)
               {
                    if (j != col)
                         a[i][j] -= factor * a[row][j];
                    if (j + 1 < n && (j + 1) != col)
                         a[i][j + 1] -= factor * a[row][j + 1];
                    if (j + 2 < n && (j + 2) != col)
                         a[i][j + 2] -= factor * a[row][j + 2];
                    if (j + 3 < n && (j + 3) != col)
                         a[i][j + 3] -= factor * a[row][j + 3];
               }
               a[i][col] = -factor;
          }
     }

     // Update pivot row with unrolling
     for (int j = 0; j < n; j += 4)
     {
          if (j != col)
               a[row][j] *= pivotInv;
          if (j + 1 < n && (j + 1) != col)
               a[row][j + 1] *= pivotInv;
          if (j + 2 < n && (j + 2) != col)
               a[row][j + 2] *= pivotInv;
          if (j + 3 < n && (j + 3) != col)
               a[row][j + 3] *= pivotInv;
     }
     b[row] *= pivotInv;
     a[row][col] = pivotInv;
}

double xsimplex(int m, int n, double **a, double *b, double *c, double *x, double y, int *var, int h)
{
     struct simplex_t s;
     int i, row, col;

     if (!initial(&s, m, n, a, b, c, x, y, var))
     {
          free(s.var);
          return NAN;
     }

     // Loop until no non-basic column is found
     while ((col = select_nonbasic(&s)) >= 0)
     {
          row = -1;
          double min_ratio = INFINITY;

          // Find the pivot row efficiently
          for (i = 0; i < m; i++)
          {
               if (a[i][col] > EPSILON)
               {
                    double ratio = b[i] / a[i][col];
                    if (ratio < min_ratio)
                    {
                         min_ratio = ratio;
                         row = i;
                    }
               }
          }

          if (row < 0)
          {
               free(s.var);
               return INFINITY;
          }
          pivot(&s, row, col);
     }

     if (h == 0)
     {
          memset(x, 0, n * sizeof(double)); // Set non-basic variables to 0

          for (i = n; i < n + m; i++)
          {
               int var_idx = s.var[i];
               if (var_idx < n)
               {
                    x[var_idx] = s.b[i - n];
               }
          }
          free(s.var);
     }
     else
     {
          memset(x, 0, n * sizeof(double)); // Set non-basic variables to 0
          for (i = n; i < n + m; i++)
          {
               x[i] = s.b[i - n];
          }
     }

     return s.y;
}

double simplex(int m, int n, double **a, double *b, double *c, double *x, double y)
{
     return xsimplex(m, n, a, b, c, x, y, NULL, 0);
}

void prepare(struct simplex_t *s, int k)
{
     int m = s->m;
     int n = s->n;
     int i;

     // Shift variable in the var array
     for (int i = m + n; i > n; i -= 4)
     {
          s->var[i] = s->var[i - 1];
          if (i - 1 > n)
               s->var[i - 1] = s->var[i - 2];
          if (i - 2 > n)
               s->var[i - 2] = s->var[i - 3];
          if (i - 3 > n)
               s->var[i - 3] = s->var[i - 4];
     }

     // Update the var array with the new variable
     s->var[n] = m + n;

     // Increment n
     n = n + 1;

     // Update the last column in the A matrix
     for (i = 0; i < m; i++)
     {
          s->a[i][n - 1] = -1;
     }

     //  Reallocate
     s->x = malloc((m + n) * sizeof(double));
     s->c = malloc(n * sizeof(double));
     memset(s->c, 0, n * sizeof(double));

     s->c[n - 1] = -1;
     s->n = n;

     pivot(s, k, n - 1);
}

int initial(struct simplex_t *s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var)
{
     int i, j, k;
     double w;

     k = init(s, m, n, a, b, c, x, y, var);

     if (b[k] >= 0)
          return 1; // feasible

     prepare(s, k);
     n = s->n;
     s->y = xsimplex(m, n, s->a, s->b, s->c, s->x, 0, s->var, 1);

     // check feasibility
     for (i = 0; i < m + n; i++)
     {
          if (s->var[i] == m + n - 1)
          {
               if (fabs(s->x[i]) > EPSILON)
               {
                    free(s->x);
                    free(s->c);
                    return 0; // infeasible
               }
               break;
          }
     }

     if (i >= n)
     {
          // x_n+m is basic. find good nonbasic.
          for (j = 0, k = 0; k < n; k++)
          {
               if (fabs(s->a[i - n][k]) > fabs(s->a[i - n][j]))
               {
                    j = k;
               }
          }
          pivot(s, i - n, j);
          i = j;
     }

     if (i < n - 1)
     {
          // x_n+m is nonbasic and not last, swap column i and n-1
          k = s->var[i];
          s->var[i] = s->var[n - 1];
          s->var[n - 1] = k;

          for (k = 0; k < m; k++)
          {
               w = s->a[k][n - 1];
               s->a[k][n - 1] = s->a[k][i];
               s->a[k][i] = w;
          }
     }
     // x_n+m is nonbasic and last, forget it

     free(s->c);

     s->c = c;
     s->y = y;

     // shifting variables
     memmove(&s->var[n - 1], &s->var[n], m * sizeof(int));

     n = s->n = s->n - 1;

     // Use stack allocation for temporary calculation becayse of small arrays
     double t[n];
     memset(t, 0, sizeof(t));

     for (k = 0; k < n; k++)
     {
          for (j = 0; j < n; j++)
          {
               if (k == s->var[j])
               {
                    // x_k is nonbasic, add c[k]
                    t[j] += s->c[k];
                    goto next_k;
               }
          }

          // x_k is basic
          int row = -1;
          for (j = 0; j < m; j++)
          {
               if (s->var[n + j] == k)
               {
                    // x_k is at row j
                    row = j;
                    break;
               }
          }

          if (row != -1)
          {
               s->y += s->c[k] * s->b[row];
               for (int i = 0; i < n; i++)
               {
                    t[i] -= s->c[k] * s->a[row][i];
               }
          }
     next_k:;
     }

     // Update objective coefficients
     memcpy(s->c, t, n * sizeof(double));

     // clean up
     free(s->x);
     return 1;
}