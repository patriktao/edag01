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
void print(struct simplex_t *s);

double simplex(int m, int n, double **a, double *b, double *c, double *x, double y);

double xsimplex(int m, int n, double **a, double *b, double *c, double *x, double y, int *var, int h);

void pivot(struct simplex_t *s, int row, int col);

int initial(struct simplex_t *s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var);

int select_nonbasic(struct simplex_t *s);

int init(struct simplex_t *s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var);

int select_nonbasic(struct simplex_t *s)
{
     int i;
     for (i = 0; i < s->n; i++)
     {
          if (s->c[i] > EPSILON)
          {
               return i;
          }
     }
     return -1;
}

struct node_t *initial_node(int m, int n, double **a, double *b, double *c)
{
     struct node_t *p = calloc(1, sizeof(struct node_t));
     int i, j;

     p->a = calloc(m + 1, sizeof(double *)); // Allocate memory for each row
     for (i = 0; i < m + 1; i++)
     {
          p->a[i] = calloc(n + 1, sizeof(double)); // Allocate memory for each column
     }

     p->b = calloc(m + 1, sizeof(double));
     p->c = calloc(n + 1, sizeof(double));
     p->x = calloc(n + 1, sizeof(double));
     p->min = calloc(n, sizeof(double));
     p->max = calloc(n, sizeof(double));
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
     struct node_t *q = calloc(1, sizeof(struct node_t));
     int i, j;

     q->k = k;
     q->k = ak;
     q->k = bk;

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

     q->a = calloc(q->m + 1, sizeof(double *));

     for (i = 0; i < q->m + 1; i++)
     {
          q->a[i] = calloc(q->n + 1, sizeof(double));
     }

     q->b = calloc(q->m + 1, sizeof(double));
     q->c = calloc(q->n + 1, sizeof(double));
     q->x = calloc(q->n + 1, sizeof(double));
     q->min = calloc(n, sizeof(double));
     q->max = calloc(n, sizeof(double));

     // copy p.min and p.max to q // each element and not only pointers
     memcpy(q->min, p->min, n * sizeof(double));
     memcpy(q->max, p->max, n * sizeof(double));

     // copy m first rows of parameter a to q.a //each element, possibly wrong
     for (i = 0; i < m; i++)
     {
          memcpy(q->a[i], a[i], n * sizeof(double));
     }

     // copy m first elements of paramenter b to q.b
     memcpy(q->b, b, m * sizeof(double));

     // copy paraemeter c to q.c // each element
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

     for (i = 0; i < p->n; i++)
     {
          if (!is_integer(&p->x[i]))
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

int branch(struct node_t *q, double z)
{
     double min, max;

     if (q->z < z)
     {
          return 0;
     }

     for (int h = 0; h < q->n; h++)
     {
          if (!is_integer(&q->x[h]))
          {
               if (q->min[h] == -INFINITY)
               {
                    min = 0;
               }
               else
               {
                    min = q->min[h];
               }

               max = q->max[h];

               if (floor(q->x[h]) < min || ceil(q->x[h]) > max) // functions defined in math.h
               {
                    continue;
               }

               q->h = h;
               q->xh = q->x[h];

               return 1;
          }
     }
     return 0;
}

void add(struct set_t *h, struct node_t *p)
{
     int i;

     // if there is place, allocate
     if (h->count < h->alloc)
     {
          for (i = 0; i < h->alloc; i++)
          {
               /* Find the first place that is not null and add */
               if ((h->nodes)[i] == NULL)
               {
                    h->nodes[i] = p;
                    h->count++;
                    return;
               }
          }
     }
     else
     {
          // Allocate on more place size
          h->alloc += 1;
          h->nodes = (struct node_t **)realloc(h->nodes, h->alloc * sizeof(struct node_t *));

          // initialize the newly allocated
          for (i = h->count; i < h->alloc; i++)
          {
               h->nodes[i] = NULL;
          }

          // Add the new node and update count
          h->nodes[h->count] = p;
          h->count++;
     }
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
     struct set_t *h = calloc(1, sizeof(struct set_t));

     h->alloc = 10;
     h->count = 0;
     h->nodes = calloc(h->alloc, sizeof(struct node_t *)); // Each array element needs to be allocated memory for a node

     for (int i = 0; i < h->alloc; i++)
     {
          h->nodes[i] = NULL; // Initialize
     }

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

     if (z == -INFINITY)
     {
          return NAN;
     }
     else
     {
          return z;
     }
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
          s->var = calloc(m + n + 1, sizeof(int));
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

void pivot(struct simplex_t *s, int row, int col)
{
     double **a = s->a;
     double *b = s->b;
     double *c = s->c;
     int m = s->m;
     int n = s->n;
     int i, j; // Loop variables

     // avoid rendundancy
     double pivotValue = a[row][col];

     // new changes
     double cCol = c[col];
     double bRow = b[row];

     int t = s->var[col];
     s->var[col] = s->var[n + row];
     s->var[n + row] = t;

     s->y = s->y + cCol * bRow / pivotValue;

     // update c
     for (i = 0; i < n; i++)
     {
          if (i != col)
          {
               c[i] = c[i] - cCol * a[row][i] / pivotValue;
          }
     }

     c[col] = -cCol / pivotValue;

     // update b and a at the same time
     for (i = 0; i < m; i++)
     {
          if (i != row)
          {
               double aICol = a[i][col];
               b[i] -= aICol * bRow / pivotValue;

               for (j = 0; j < n; j++)
               {
                    if (j != col)
                    {
                         a[i][j] = a[i][j] - aICol * a[row][j] / pivotValue;
                    }
               }
               a[i][col] = -aICol / pivotValue;
          }
     }

     // Update a[row][i] and b[row]
     for (i = 0; i < n; i++)
     {
          if (i != col)
          {
               a[row][i] /= pivotValue;
          }
     }

     b[row] /= pivotValue;
     a[row][col] = 1 / pivotValue;
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

     while ((col = select_nonbasic(&s)) >= 0)
     {
          row = -1;

          for (i = 0; i < m; i++)
          {
               if (a[i][col] > EPSILON && (row < 0 || b[i] / a[i][col] < b[row] / a[row][col]))
               {
                    row = i;
               }
          }

          if (row < 0)
          {
               free(s.var);
               return INFINITY;
          }

          pivot(&s, row, col);
     }

     if (h == 0) // Finalization baserat på variabel h, uppdatera s
     {
          for (i = 0; i < n; i++)
          {
               if (s.var[i] < n)
               {
                    x[s.var[i]] = 0;
               }
          }
          for (i = 0; i < m; i++)
          {
               if (s.var[n + i] < n)
               {
                    x[s.var[n + i]] = s.b[i];
               }
          }
          free(s.var);
     }
     else
     {
          for (i = 0; i < n; i++)
          {
               x[i] = 0;
          }
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
     for (i = m + n; i > n; i--)
     {
          s->var[i] = s->var[i - 1];
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
     s->x = calloc(m + n, sizeof(double));
     s->c = calloc(n, sizeof(double));

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
     {
          return 1;
     }

     prepare(s, k);

     n = s->n;

     s->y = xsimplex(m, n, s->a, s->b, s->c, s->x, 0, s->var, 1);

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
               else
               {
                    break;
               }
          }
     }

     if (i >= n)
     {
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
     else
     {
          // x_n+m is nonbasic and last, forget it
     }

     free(s->c);

     s->c = c;
     s->y = y;
     for (k = n - 1; k < n + m - 1; k++)
     {
          s->var[k] = s->var[k + 1];
     }

     n = s->n = s->n - 1;

     double *t = calloc(n, sizeof(double));

     for (k = 0; k < n; k++)
     {
          for (j = 0; j < n; j++)
          {
               if (k == s->var[j])
               {
                    // x_k is nonbasic, add c[k]
                    t[j] = t[j] + s->c[k];
                    goto next_k;
               }
          }

          // x_k is basic
          for (j = 0; j < m; j++)
          {
               if (s->var[n + j] == k)
               {
                    // x_k is at row j
                    break;
               }
          }

          s->y = s->y + s->c[k] * s->b[j];

          for (i = 0; i < n; i++)
          {
               t[i] = t[i] - s->c[k] * s->a[j][i];
          }

     next_k:;
     }

     for (i = 0; i < n; i++)
     {
          s->c[i] = t[i];
     }

     free(t);
     free(s->x);
     return 1;
}

void print(struct simplex_t *s)
{
     printf("\n---------------------------------\n");
     printf("\nMax z: ");
     for (int i = 0; i < s->n; i++)
     {
          if (i == s->n - 1)
          {
               printf("%.3lf %s%d \n", s->c[i], "x", i);
          }
          else
          {
               printf("%.3lf %s%d %s", s->c[i], "x", i, "+ ");
          }
     }

     printf("\nConstraints:\n");
     for (int i = 0; i < s->m; i++)
     {
          for (int j = 0; j < s->n; j++)
          {
               if (j == 0)
               {
                    printf("%10.3lf %s%d", s->a[i][j], "x", j);
               }
               else
               {
                    if (s->a[i][j] >= 0)
                    {
                         printf(" + %.3lf %s%d", s->a[i][j], "x", j);
                    }
                    else
                    {
                         printf(" - %.3lf %s%d", -s->a[i][j], "x", j);
                    }
               }
          }
          printf(" %s %.3lf\n", "\u2264", s->b[i]);
     }
     printf("\n---------------------------------\n");
}

int main()
{
     int m, n;
     double **a;
     double *b;
     double *c;
     double *x;
     int i, j;

     // Scan input
     scanf("%d", &m);
     scanf("%d", &n);

     // Allocate memory for arrays
     a = calloc(m, sizeof(double *));
     b = calloc(m, sizeof(double));
     c = calloc(n, sizeof(double));
     x = calloc(n + 1, sizeof(double));

     // Read the coefficients
     for (i = 0; i < n; i++)
     {
          scanf("%lf", &c[i]);
     }

     // Create and fill matrix
     for (i = 0; i < m; i++)
     {
          a[i] = calloc(n + 1, sizeof(double));
          for (j = 0; j < n; j++)
          {
               scanf("%lf", &a[i][j]);
          }
     }

     // Read b-values
     for (i = 0; i < m; i++)
     {
          scanf("%lf", &b[i]);
     }

     // Execute algorithm
     double max_z = intopt(m, n, a, b, c, x);
     printf("result: %f\n\n", max_z);

     // Terminate
     for (i = 0; i < m; i++)
     {
          free(a[i]);
     }
     free(a);
     free(b);
     free(c);
     free(x);
}