#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_WORD_LEN 10
#define MAX_WORDS 100000

typedef struct
{
     char *word;
     int count;
} WordCount;

bool is_prime(int n)
{
     if (n <= 1)
          return false;
     if (n == 2 || n == 3)
          return true;
     if (n % 2 == 0 || n % 3 == 0)
          return false;

     for (int i = 5; i * i <= n; i += 6)
     {
          if (n % i == 0 || n % (i + 2) == 0)
          {
               return false;
          }
     }
     return true;
}

void add_word(WordCount *words, int *total_words, const char *word)
{
     if (*total_words >= MAX_WORDS) // Check if limit is reached
     {
          fprintf(stderr, "Error: Word limit reached.\n");
          return;
     }

     for (int i = 0; i < *total_words; i++) // Search for existing word
     {
          if (strcmp(words[i].word, word) == 0 && words[i].count > 0)
          {
               printf("counted %s\n", word);
               words[i].count++;
               return;
          }
     }
     printf("added %s\n", word);

     words[*total_words].word = malloc(MAX_WORD_LEN);

     if (words[*total_words].word == NULL)
     {
          fprintf(stderr, "Memory allocation failed.\n");
          exit(1);
     }

     strcpy(words[*total_words].word, word); // Copy the word
     words[*total_words].count = 1;          // we initialize count to 1
     (*total_words)++;                       // Increase total_words by 1 through the pointer
}

void delete_word(WordCount *words, int *total_words, const char *word)
{
     for (int i = 0; i < *total_words; i++)
     {
          if (strcmp(words[i].word, word) == 0)
          {
               if (words[i].count > 0)
               {
                    printf("trying to delete %s: deleted\n", word);
                    words[i].count = 0; // Delete: set count to zero
                    return;
               }
          }
     }
     printf("trying to delete %s: not found\n", word);
}

int main(void)
{
     char word[MAX_WORD_LEN];
     int line_nbr = 1;
     WordCount words[MAX_WORDS] = {0}; // Initialize the array to zero

     int total_words = 0;
     int index = 0;
     int c;

     while ((c = getchar()) != EOF)
     {
          if (c == '\n')
          {
               word[index] = '\0'; // Null-terminate the string

               // Perform action based on line number
               if (!is_prime(line_nbr) || line_nbr == 1)
               {
                    add_word(words, &total_words, word);
               }
               else
               {
                    delete_word(words, &total_words, word);
               }

               // Reset for the next word
               index = 0;
               line_nbr++;
          }
          else if (index < MAX_WORD_LEN - 1)
          {
               word[index++] = (char)c;
          }
     }

     int res_idx = 0;

     for (int i = 0; i < total_words; i++)
     {
          int lexi_cmp = strcmp(words[i].word, words[res_idx].word);
          if (words[i].count > 0 &&
              (words[i].count > words[res_idx].count ||
               (words[i].count == words[res_idx].count && lexi_cmp < 0)))
          {
               res_idx = i;
          }
     }

     printf("result: %s %d\n", words[res_idx].word, words[res_idx].count);

     // Free allocated memory
     for (int i = 0; i < total_words; i++)
     {
          free(words[i].word);
     }

     return 0;
}
