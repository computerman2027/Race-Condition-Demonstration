#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define STRING_SIZE 256
#define NUM_SENTENCES 10
#define NUM_READERS 5

char shared_string[STRING_SIZE] = "lorem ipsum";

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
        strncpy(shared_string, sentences[i], STRING_SIZE - 1);
        shared_string[STRING_SIZE - 1] = '\0';
        i = (i + 1) % NUM_SENTENCES;
        sleep(5);
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
        fflush(f);      // flush immediately to file
        usleep(200000); // reader reads 5 times per second
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

    // create multiple readers
    for (int i = 0; i < NUM_READERS; i++)
    {
        ids[i] = i;
        pthread_create(&readers[i], NULL, reader_function, &ids[i]);
    }

    // wait (infinite loop anyway, but keep program running)
    pthread_join(writer, NULL);
    for (int i = 0; i < NUM_READERS; i++)
    {
        pthread_join(readers[i], NULL);
    }

    return 0;
}