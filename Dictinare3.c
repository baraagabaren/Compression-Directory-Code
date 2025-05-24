#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORDS 10000
#define MAX_LEN 64

typedef struct {
    char word[MAX_LEN];
    int code;
} WordEntry;

WordEntry dict[MAX_WORDS];
int dictSize = 0;

// Check if word exists in dictionary
int findWord(const char *word) {
    for (int i = 0; i < dictSize; i++) {
        if (strcmp(dict[i].word, word) == 0)
            return dict[i].code;
    }
    return -1;
}

// Add word to dictionary
int addWord(const char *word) {
    if (dictSize >= MAX_WORDS) return -1;
    strcpy(dict[dictSize].word, word);
    dict[dictSize].code = dictSize + 1;
    return dict[dictSize++].code;
}

void compress(FILE *in, FILE *out) {
    char word[MAX_LEN];
    int c, pos = 0;

    while ((c = fgetc(in)) != EOF) {
        if (c == ' ' || c == '\n' || c == '\t') {
            if (pos > 0) {
                word[pos] = '\0';
                int code = findWord(word);
                if (code == -1)
                    code = addWord(word);
                fprintf(out, "%d", code);
                pos = 0;
            }
            fputc(c, out);
        } else {
            if (pos < MAX_LEN - 1)
                word[pos++] = c;
        }
    }

    if (pos > 0) {
        word[pos] = '\0';
        int code = findWord(word);
        if (code == -1)
            code = addWord(word);
        fprintf(out, "%d", code);
    }
}

int main() {
    FILE *input = fopen("hobbit.txt", "r");
    if (!input) {
        printf("Failed to open hobbit.txt\n");
        return 1;
    }

    FILE *output = fopen("Final.txt", "w");
    if (!output) {
        printf("Failed to create Final.txt\n");
        fclose(input);
        return 1;
    }

    compress(input, output);

    fclose(input);
    fclose(output);

    printf("Compression complete. Dictionary size: %d entries.\n", dictSize);
    return 0;
}
