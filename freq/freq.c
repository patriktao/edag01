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
     char word[MAX_WORD_LEN];
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

     for (int i = 5; i <= (int)sqrt(n); i += 6)
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
     for (int i = 0; i < *total_words; i++)
     {
          if (strcmp(words[i].word, word) == 0 && words[i].count > 0)
          {
               printf("counted %s\n", word);
               words[i].count++;
               return;
          }
     }
     printf("added %s\n", word);
     strcpy(words[*total_words].word, word); // Copy word to words[*total_words].word
     words[*total_words].count = 1;
     (*total_words)++; // Increase total_words by 1 through the pointer
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
                    words[i].count--; // Decrease the count if more than 1
                    return;
               }
          }
     }
     printf("trying to delete %s: not found\n", word);
}

int main(void)
{
     char word[256];
     int line_nbr = 1;
     WordCount words[MAX_WORDS];
     memset(words, 0, sizeof(words));

     int total_words = 0;

     // add first word
     fgets(word, sizeof(word), stdin);
     word[strcspn(word, "\n")] = '\0'; // Remove \n from word
     add_word(words, &total_words, word);
     line_nbr++;

     while (fgets(word, sizeof(word), stdin) != NULL)
     {
          word[strcspn(word, "\n")] = '\0';
          if (!is_prime(line_nbr))
          {
               add_word(words, &total_words, word);
          }
          else
          {
               delete_word(words, &total_words, word);
          }

          line_nbr++;
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
     return 0;
}
