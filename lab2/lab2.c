#include <stdio.h>
#include <stdlib.h>

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

int init(struct simplex_t *s, int m, int n, int *var, double **a, double *b, double *x, double *c, double y)
{
    int i, k;
    s->m = m;
    s->n = n;
    s->var = var;
    s->a = a;
    s->b = b;
    s->x = x;
    s->c = c;
    s->y = y;

    if (s->var == NULL)
    {
        s->var = malloc(sizeof(m + n + 1));
        for (i = 0; i < m + n; i++)
        {
            s->var[i] = i;
        }
    }
    i = 1;
    for (k = 0; i < m; i++)
    {
        if (b[i] < b[k])
        {
            k = i;
        };
    }
    return k;
}

double xsimplex(int *m, int *n, double **a, double *b, double *c, double *x, double *y, double *var, double *h)
{
    struct simplex_t s;
    int i, row, col;
    if(&initial(&s, m, n, var, a, b, x, c, y) == NULL){
        s.var = NULL;
        return 
    }

}

double simplex(int *m, int *n, double **a, double *b, double *c, double *x, double *y)
{
    return xsimplex(m, n, a, b, c, x, y, NULL, 0);
}

/* int main(int argc, char **argv)
{
} */

/* simplex
• xsimplex
• pivot
• initial and assume bi ≥0 so skip the call to prepare and the rest of
initial
• init
• select_nonbasic */