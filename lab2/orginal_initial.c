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