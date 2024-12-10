#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define N 10             // Stack size
#define BUFFER_SIZE 8192 // Buffer size for input

int main(void)
{
    int idx = 0, line = 1, stack[N];
    char buffer[BUFFER_SIZE];

    // Read input in bulk
    size_t bytes_read = fread(buffer, 1, BUFFER_SIZE - 1, stdin);
    if (bytes_read == 0)
        return 0;              // Handle empty input
    buffer[bytes_read] = '\0'; // Null-terminate the buffer

    char *c = buffer;
    while (*c)
    {
        // Skip whitespace efficiently
        while (*c == ' ' || *c == '\t')
            c++;

        // Process numbers
        if (isdigit(*c))
        {
            int num = 0;
            do
            {
                num = num * 10 + (*c - '0');
                c++;
            } while (isdigit(*c));

            if (idx == N)
            {
                printf("line %d: error at %c\n", line, '0' + num);
                idx = 0;
                c = strchr(c, '\n'); // Skip to next line
                if (c)
                    c++; // Move past newline
                line++;
                continue;
            }
            stack[idx++] = num;
        }
        // Handle newlines
        else if (*c == '\n' || *c == '\0')
        {
            if (idx == 1)
            {
                printf("line %d: %d\n", line, stack[0]);
            }
            else
            {
                printf("line %d: error at \\n\n", line);
            }
            idx = 0; // Reset stack for the next line
            if (*c == '\n')
                c++;
            line++;
        }
        // Process operators
        else if (*c == '+' || *c == '-' || *c == '*' || *c == '/')
        {
            if (idx < 2)
            {
                printf("line %d: error at %c\n", line, *c);
                idx = 0;
                c = strchr(c, '\n'); // Skip to next line
                if (c)
                    c++;
                line++;
                continue;
            }

            int op2 = stack[--idx];
            int op1 = stack[--idx];

            if (*c == '/' && op2 == 0)
            {
                printf("line %d: error at %c\n", line, '/');
                idx = 0;
                c = strchr(c, '\n'); // Skip to next line
                if (c)
                    c++;
                line++;
                continue;
            }

            switch (*c)
            {
            case '+':
                stack[idx++] = op1 + op2;
                break;
            case '-':
                stack[idx++] = op1 - op2;
                break;
            case '*':
                stack[idx++] = op1 * op2;
                break;
            case '/':
                stack[idx++] = op1 / op2;
                break;
            }
            c++;
        }
        // Handle invalid characters
        else
        {
            printf("line %d: error at %c\n", line, *c);
            idx = 0;
            c = strchr(c, '\n'); // Skip to next line
            if (c)
                c++;
            line++;
        }
    }
    return 0;
}
