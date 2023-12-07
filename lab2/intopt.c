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

int init(struct simplex_t *s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var)
{
    int i, k;

    s->m = m;     /* nbr constraints */
    s->n = n;     /* nbr decision variables */
    s->var = var; /* variables, Needs to be freed */
    s->a = a;     /* A matrix */
    s->b = b;     /* b array */
    s->x = x;     /* x array */
    s->c = c;     /* c array */
    s->y = y;     /* double */

    print(&s);

    if (s->var == NULL)
    {
        s->var = malloc((m + n + 1) * sizeof(int));
        for (i = 0; i < m + n; i++)
        {
            s->var[i] = i;
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
    int i, j, t;

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
    struct simplex_t s;
    int i, row, col;

    if (!initial(&s, m, n, a, b, c, x, y, var)) // if init fails
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

    if (h == 0) /* Finalization baserat på variabel h, uppdatera s */
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
            if (s.var[n + 1] < n)
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

int initial(struct simplex_t *s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var)
{
    int i, j, k;
    double w;
    k = init(&s, m, n, a, b, c, x, y, var);

    return 1; // we assume that b[k] >  0
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
        printf(" %s %.3lf\n", "\u2264", s->b[i]);
    }
    printf("\n");
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

    // Create matrix
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
    for (int i = 0; i < m; i++)
    {
        free(a[i]);
    }
    free(a);
}