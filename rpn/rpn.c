#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define N (10)
#define MAX_LEN 256

int main(void)
{
	char line[MAX_LEN]; // Increase buffer size to accommodate MAX_LEN
	int nbrs[N];
	char ops[N];

	// for each line
	{
		char c;

		while (fgets(line, MAX_LEN, stdin))
		{
			int nbr_index = 0;
			int ops_index = 0;
			int num = 0;
			printf("Line: %s", line);

			// Print parsed numbers and operators
			printf("\n----------------------------------------------\n");
			printf("Numbers: ");
			for (int i = 0; i < nbr_index; i++)
			{
				printf("%d ", nbrs[i]);
			}

			printf("\nOperators: ");
			for (int i = 0; i < ops_index; i++)
			{
				printf("%c ", ops[i]);
			}

			// check whether operators and lines are empty:
			if (ops_index == 0 || nbr_index == 0)
			{
				printf("\nerror at: \\n");
			}
			printf("\n----------------------------------------------\n");
			printf("\n\n"); // readability
		}
	}
	return 0;
}
