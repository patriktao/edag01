#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_WORD_LEN 100
#define MAX_WORDS 1000

typedef struct
{
     char word;
     int count;
} WordCount;

bool is_prime(int n)
{
     if (n <= 1)
          return false;

     if (n <= 3)
          return true;

     if (n % 2 == 0 || n % 3 == 0)
          return false;

     // no nbr greater than sqrt(n) can be a divisor of n
     for (int i = 5; i <= (int)sqrt(n); i += 6)
     {
          if (n % i == 0 || n % (i + 2) == 0) // if line nbr is prime nbr
          {
               return false;
          }
     }
     return true;
}

void add_word(WordCount words[], int *word_count, const char *word)
{
     // Check if the word already exists
     for (int i = 0; i < *word_count; i++)
     {
          if (strcmp(words[i].word, word) == 0)
          {
               words[i].count++;
               return;
          }
     }

     // Add new word
     strcpy(words[*word_count].word, word);
     words[*word_count].count = 1;
     (*word_count)++;
}

void delete_word(WordCount words[]);

int main(void)
{
     char word[256];      // Buffer to store each line
     int line_number = 1; // Initialize line counter
     WordCount words[MAX_WORDS];
     int word_count = 0;

     while (fgets(word, sizeof(word), stdin) != NULL) // for each line
     {
          word[strcspn(word, "\n")] = '\0';
          printf("%s", word);

          if (is_prime(line_number))
          {
               add_word(words, &word_count, word);
          }
          else
          {
                    }

          line_number++;
     }
}