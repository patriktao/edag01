#include <stdio.h>
#include "poly.h"

static void poly_test(const char *a, const char *b)
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

	printf("End polynomial test of (%s) * (%s)\n", a, b);
}

int main(void)
{
	char a[1024], b[1024]; // Buffers to hold the parsed strings

	// Read input from stdin
	if (scanf("%1023[^,],%1023[^\n]", a, b) != 2)
	{
		fprintf(stderr, "Error: Failed to parse input.\n");
		return 1;
	}

	// Call the poly_test function with the parsed strings
	poly_test(a, b);

	return 0;
}
