#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define STRING_SIZE 256
#define NUM_SENTENCES 10
#define NUM_READERS 5

char shared_string[STRING_SIZE] = "lorem ipsum";

// List of test sentences
const char *sentences[NUM_SENTENCES] = {
    "The quick brown fox jumps over the lazy dog.",
    "All that glitters is not gold.",
    "A watched pot never boils.",
    "Beauty is in the eye of the beholder.",
    "When in Rome, do as the Romans do.",
    "Birds of a feather flock together.",
    "Practice makes perfect.",
    "Honesty is the best policy.",
    "A journey of a thousand miles begins with a single step.",
    "Don't put all your eggs in one basket."
};

void *writer_function(void *arg)
{
    int i = 0;
    while (1)
    {
        const char *sentence = sentences[i];
        size_t len = strlen(sentence);

        // Simulate long write (character by character)
        for (size_t j = 0; j < len && j < STRING_SIZE - 1; ++j)
        {
            shared_string[j] = sentence[j];
            usleep(1000); // tiny delay between characters
        }
        shared_string[len] = '\0';

        i = (i + 1) % NUM_SENTENCES;
        sleep(1); // new sentence every second
    }
    return NULL;
}

void *reader_function(void *arg)
{
    int id = *(int *)arg;
    char filename[32];
    snprintf(filename, sizeof(filename), "reader_%d.txt", id);

    FILE *f = fopen(filename, "w");
    if (!f)
    {
        perror("fopen");
        pthread_exit(NULL);
    }

    while (1)
    {
        fprintf(f, "%s\n", shared_string);
        fflush(f);      // immediately flush to file
        usleep(100000); // read 10 times per second
    }

    fclose(f);
    return NULL;
}

int main()
{
    pthread_t writer;
    pthread_t readers[NUM_READERS];
    int ids[NUM_READERS];

    // create writer
    pthread_create(&writer, NULL, writer_function, NULL);

    // create readers
    for (int i = 0; i < NUM_READERS; i++)
    {
        ids[i] = i;
        pthread_create(&readers[i], NULL, reader_function, &ids[i]);
    }

    // wait (infinite)
    pthread_join(writer, NULL);
    for (int i = 0; i < NUM_READERS; i++)
    {
        pthread_join(readers[i], NULL);
    }

    return 0;
}
