#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 4          /* A C T G */

/* ---------------- */
/* ------MISC------ */
/* ---------------- */

void *safe_malloc(unsigned long size) {
    void *ptr = (void *) malloc(size);

    if (ptr == NULL) {
        perror("malloc error");
        exit(1);
    }

    return ptr;
}

void *safe_realloc(void *ptr, unsigned long size) {
    void *new_ptr = (void *) realloc(ptr, size);

    if (new_ptr == NULL) {
        perror("realloc error");
        exit(1);
    }
    
    return new_ptr;
}

void safe_scanf(char *format, void *ptr) {
    if (scanf(format, ptr) <= 0) {
        perror("scanf error");
        exit(1);
    }
}

void free_strings(char **strings, unsigned long size) {
    unsigned long i;

    for (i = 0; i < size; i++) {
        free(strings[i]);
    }

    free(strings);
}

void print_table(unsigned long *table, unsigned long N) {
    unsigned long i;
    for (i = 0; i < N; i++) {
        printf("%ld ", table[i]);
    }

    printf("\n");
}

long int max(long int s1, long int s2) {
    return (s1 > s2) ? s1 : s2;
}

/* --------------- */
/* -----INPUT----- */
/* --------------- */

char *read_line() {
    unsigned long str_size;
    char *buffer;

    safe_scanf("%lu", &str_size);

    buffer = (char *) safe_malloc((str_size + 1 + 1) * sizeof(char));       /* \n + \0 */
    safe_scanf("%s", buffer);

    buffer[str_size] = '\0';                                                /* Remove \n */
    return buffer;
}

/* -------------- */
/* -----MAIN----- */
/* -------------- */

int main() {
    unsigned long i, K;
    char **strings = NULL;

    safe_scanf("%lu", &K);
    strings = (char **) safe_malloc(sizeof(char *) * K);

    for (i = 0; i < K; i++) {
        strings[i] = read_line();
    }
    
    free_strings(strings, K);
    return 0;
}