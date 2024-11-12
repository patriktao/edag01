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

void add_word(WordCount *words[], int total_words, const char *word)
{
     // increase word count if it already exist on stack
     for (int i = 0; i < total_words; i++)
     {
          if (words[i]->word == *word)
          {
               words[i]->count++; // increase word count by 1
               return;
          }
     }

     // otherwise add the word to stack
     strcpy(words[total_words]->word, *word); // add 'word' to words[total_words].word
     words[total_words]->count = 1;           // initialize the word count to 1
     (total_words)++;                         // increase total_words by 1
}

void delete_word(WordCount *words[], int total_words, const char *word)
{
}

int main(void)
{
     char word[256];             // Buffer to store each line
     int line_nbr = 1;           // Initialize line counter
     WordCount words[MAX_WORDS]; // Stack of words
     int total_words = 0;

     while (fgets(word, sizeof(word), stdin) != NULL) // for each line
     {
          word[strcspn(word, "\n")] = '\0'; // removes \n from the string
          printf("%s", word);

          if (is_prime(line_nbr))
          {
               add_word(words, total_words, word);
          }
          else
          {
               delete_word(words, total_words, word);
          }

          line_nbr++;
     }
}