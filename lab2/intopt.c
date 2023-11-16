#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

// Declaration
double simplex(int m, int n, double **a, double *b, double *c, double *x, double y);

double xsimplex(int m, int n, double **a, double *b, double *c, double *x, double y, int *var, int h);

void pivot(struct simplex_t *s, int row, int col);

int initial(struct simplex_t **s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var);

int select_nonbasic(struct simplex_t *s);

int init(struct simplex_t **s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var);

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

int init(struct simplex_t **s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var)
{
    int i, k;

    *s = (struct simplex_t *)malloc(sizeof(struct simplex_t));

    (*s)->m = m;     /* nbr constraints */
    (*s)->n = n;     /* nbr decision variables */
    (*s)->var = var; /* variables, Needs to be freed */
    (*s)->a = a;     /* A matrix */
    (*s)->b = b;     /* b array */
    (*s)->x = x;     /* x array */
    (*s)->c = c;     /* c array */
    (*s)->y = y;     /* double */

    if ((*s)->var == NULL)
    {
        (*s)->var = malloc((m + n + 1) * sizeof(int));
        for (i = 0; i < m + n; i++)
        {
            (*s)->var[i] = i;
        }
    }

    for (k = 0; i < m; i++)
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
    int i, j, t; // Loop variables

    t = s->var[col];
    s->var[col] = s->var[n + row];
    s->var[n + row] = t;
    s->y = s->y + c[col] * b[row] / a[row][col];

    for (i = 0; i < n; i++)
    {
        if (i != col)
        {
            c[i] = c[i] - c[col] * a[row][i] / a[row][col];
        }
    }
    c[col] = -c[col] / a[row][col];

    for (i = 0; i < m; i++)
    {
        if (i != row)
        {
            b[i] = b[i] - a[i][col] * b[row] / a[row][col];
        }
    }

    for (i = 0; i < m; i++)
    {
        if (i != row)
        {
            for (j = 0; j < n; j++)
            {
                if (j != col)
                {
                    a[i][j] = a[i][j] - a[i][col] * a[row][j] / a[row][col];
                }
            }
        }
    }

    for (i = 0; i < m; i++)
    {
        if (i != row)
        {
            a[i][col] = -a[i][col] / a[row][col];
        }
    }

    for (i = 0; i < n; i++)
    {
        if (i != col)
        {
            a[row][i] = a[row][i] / a[row][col];
        }
    }

    b[row] = b[row] / a[row][col];
    a[row][col] = 1 / a[row][col];
    printf("pivot: ");
    printf("row: %d, ", row);
    printf("col: %d\n", col);
}

double xsimplex(int m, int n, double **a, double *b, double *c, double *x, double y, int *var, int h)
{
    struct simplex_t *s;
    int i, row, col;

    if (!initial(&s, m, n, a, b, c, x, y, var)) // if init fails
    {
        free(s->var);
        return NAN;
    }

    print(s); // print the inital stage of the table

    // pain method
    // iterates as long as there exists a nonbasic variable (col) that can be selected to improve the objective function
    /* In each iteration, it finds a suitable basic variable (row) for pivoting based on the ratios of coefficients.
        If no suitable basic variable is found, it returns INFINITY indicating an unbounded solution.
        Otherwise, it performs the pivot operation on the tableau and prints the updated tableau. */
    while ((col = select_nonbasic(s)) >= 0)
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
            free(s->var);
            return INFINITY;
        }

        pivot(s, row, col);
        print(s);
    }

    if (h == 0) /* Finalization baserat på variabel h, uppdatera s */
    {
        for (i = 0; i < n; i++)
        {
            if (s->var[i] < n)
            {
                x[s->var[i]] = 0;
            }
        }
        for (i = 0; i < m; i++)
        {
            if (s->var[n + 1] < n)
            {
                x[s->var[n + i]] = s->b[i];
            }
        }
        free(s->var);
    }
    else
    {
        for (i = 0; i < n; i++)
        {
            x[i] = 0;
        }
        for (i = n; i < n + m; i++)
        {
            x[i] = s->b[i - n];
        }
    }

    double result = s->y;

    free(s);
    return result;
}

double simplex(int m, int n, double **a, double *b, double *c, double *x, double y)
{
    return xsimplex(m, n, a, b, c, x, y, NULL, 0);
}

int initial(struct simplex_t **s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var)
{
    int i, j, k;
    double w;
    k = init(s, m, n, a, b, c, x, y, var);

    return 1; // we assume that b[k] >  0

    /*     if (b[k] >= 0)
        {
            return 1;
        } */
    /* prepare(s, k); */

    /*
    n = s->n;
    s->y = xsimplex(m, n, s->a, s->b, s->c, s->x, 0, s->var, 1);
    for (i = 0; i < m + n; i++)
    {
        if (s->var[i] = m + n - 1)
        {
            if (fabs(s->x[i]) > EPSILON)
            {
                free(s->x);
                free(s->c);
                return 0;
            }
            else
            {
                break;
            }
        }
    }

    if (i >= n)
    {
        for (j = k = 0; k < n; k++)
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
        free(s->c);
        s->c = c;
        s->y = y;
        for (k = n - 1; k < n + m + 1; k++)
        {
            s->var[k] = s->var[k + 1];
        }
        n = s->n = s->n - 1;
        int *t = malloc(n * sizeof(double));
        for (k = 0; k < n; k++)
        {
            for (j = 0; j < n; j++)
            {
                if (k == s->var[j])
                {
                    t[j] = t[j] + s->c[k];
                }
            }
            for (j = 0; j < m; j++)
            {
                if (s->var[n + j] == k)
                {
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
    */
}

void free_matrix(double **a, int m)
{
    for (int i = 0; i < m; i++)
    {
        free(a[i]);
    }
    free(a);
}

void print(struct simplex_t *s)
{
    printf("\n---------------------------------\n");
    printf("\nMax z:\n");
    for (int i = 0; i < s->n; i++)
    {
        if (i == s->n - 1)
        {
            printf("%.3lf %s%d \n", s->c[i], "x", i);
        }
        else
        {
            printf("%10.3lf %s%d %s", s->c[i], "x", i, "+ ");
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
        printf(" %s %.3lf\n", "\u2264", *s->a[i]);
    }
}

int main()
{
    int m, n;
    double **a;
    double *b;
    double *c;
    double *x;
    size_t i;

    // Scan input
    scanf("%d", &m);
    scanf("%d", &n);

    a = (double **)calloc(m, sizeof(double *));
    b = (double *)calloc(m, sizeof(double));
    c = (double *)calloc(n, sizeof(double));
    x = (double *)calloc(n + 1, sizeof(double));

    // create matrix
    for (int i = 0; i < m; i += 1)
    {
        a[i] = calloc(n, sizeof(double));
    }

    // Read  the coefficients
    for (int i = 0; i < n; i++)
    {
        scanf("%lf", &c[i]);
    }

    // Fill matrix
    for (int i = 0; i < m; i++)
    {
        for (size_t j = 0; j < n; j++)
        {
            scanf("%lf", &a[i][j]);
        }
    }

    // Read b-values
    for (int i = 0; i < m; i++)
    {
        scanf("%lf", &b[i]);
    }

    // Execute
    printf("result: %f\n", simplex(m, n, a, b, c, x, 0));

    // terminate
    free(b);
    free(c);
    free(x);
    free_matrix(a, m);
}