#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for sleep()

unsigned int lastNumber = 1;
FILE *words;

// Compare two file pointers word by word
int isSameWord(FILE *a, FILE *b) {
    rewind(a);
    rewind(b);
    int ca, cb;
    while (1) {
        ca = getc(a);
        cb = getc(b);

        if ((ca == EOF || ca == ' ' || ca == '\n' || ca == '\t') &&
            (cb == EOF || cb == ' ' || cb == '\n' || cb == '\t'))
            return 1;

        if (ca != cb)
            return 0;
    }
}

// Adds a new word to SecretWords.txt and updates lastNumber
void AddToSecretWordsFile(FILE *wordFile) {
    FILE *temp = fopen("SecretWords.txt", "a");
    if (!temp) return;

    rewind(wordFile);
    int ch;
    while ((ch = getc(wordFile)) != EOF && ch != ' ' && ch != '\n' && ch != '\t')
        fputc(ch, temp);

    fprintf(temp, " %d\n", lastNumber++);
    fclose(temp);
}

// Build dictionary from file
void *FindDublication(FILE *file) {
    int ch = fgetc(file);
    int count = 0;
    int dotState = 0;

    while (ch != EOF) {
        if (ch == ' ' || ch == '\n' || ch == '\t') {
            ch = fgetc(file);
            continue;
        }

        if (++count % 100 == 0) {
            printf("\rBuilding dictionary from hobbit.txt");
            for (int i = 0; i < dotState; i++) printf(".");
            fflush(stdout);
            dotState = (dotState + 1) % 4;
        }

        FILE *wordFile = tmpfile();
        if (!wordFile) return NULL;

        fputc(ch, wordFile);
        while ((ch = fgetc(file)) != EOF && ch != ' ' && ch != '\n' && ch != '\t')
            fputc(ch, wordFile);

        rewind(wordFile);
        rewind(words);

        int found = 0;
        int wc;
        while ((wc = getc(words)) != EOF) {
            FILE *dictWord = tmpfile();
            while (wc != EOF && wc != ' ' && wc != '\n' && wc != '\t') {
                fputc(wc, dictWord);
                wc = getc(words);
            }
            rewind(dictWord);
            rewind(wordFile);

            if (isSameWord(wordFile, dictWord)) {
                found = 1;
                fclose(dictWord);
                break;
            }

            while (wc != '\n' && wc != EOF) wc = getc(words);
            fclose(dictWord);
        }

        if (!found)
            AddToSecretWordsFile(wordFile);

        fclose(wordFile);
    }

    printf("\n");
    return NULL;
}

// Replace each word with number from dictionary
FILE *Dublication(FILE *file) {
    FILE *final = fopen("Final.txt", "w+");
    if (!final) {
        printf("ERROR: Failed to create Final.txt\n");
        return NULL;
    }

    rewind(words);  // very important

    int ch;
    while ((ch = getc(file)) != EOF) {
        if (ch == ' ' || ch == '\n' || ch == '\t') {
            fputc(ch, final);
            continue;
        }

        FILE *temp = tmpfile();
        fputc(ch, temp);
        while ((ch = getc(file)) != EOF && ch != ' ' && ch != '\n' && ch != '\t')
            fputc(ch, temp);
        rewind(temp);

        rewind(words);
        int found = 0;
        int number = 0;
        while (!feof(words)) {
            FILE *dictWord = tmpfile();

            int c = getc(words);
            while (c != EOF && c != ' ' && c != '\n')
                fputc(c, dictWord), c = getc(words);

            rewind(dictWord);
            rewind(temp);

            if (isSameWord(temp, dictWord)) {
                fscanf(words, "%d", &number);
                fprintf(final, "%d", number);
                found = 1;
                fclose(dictWord);
                break;
            }

            while (c != '\n' && c != EOF) c = getc(words);
            fclose(dictWord);
        }

        if (!found) {
            AddToSecretWordsFile(temp);
            fprintf(final, "%d", lastNumber - 1);  // just added
        }

        fputc(' ', final);
        fclose(temp);

        if (ch != EOF && ch != ' ' && ch != '\n' && ch != '\t')
            ungetc(ch, file);
    }

    return final;
}

int main() {
    FILE *input = fopen("halfaa", "r+");
    if (!input) {
        input = fopen("hobbit.txt", "w+");
        if (!input) {
            printf("Failed to open or create hobbit.txt\n");
            return 1;
        }
        printf("Created empty hobbit.txt file.\n");
        fclose(input);
        return 0;
    }

    words = fopen("SecretWords.txt", "r+");
    if (!words) {
        words = fopen("SecretWords.txt", "w+");
        if (!words) {
            printf("Failed to create SecretWords.txt\n");
            fclose(input);
            return 1;
        }
    }

    printf("Building dictionary from hobbit.txt");
    fflush(stdout);
    FindDublication(input);
    rewind(input);

    fclose(words); // close & reopen to reset
    words = fopen("SecretWords.txt", "r+");

    printf("\nGenerating Final.txt with word numbers...\n");
    Dublication(input);

    fclose(input);
    fclose(words);
    printf("Done. Final.txt created.\n");
    return 0;
}
