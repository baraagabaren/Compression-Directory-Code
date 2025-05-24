#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned int lastNumber = 1;

// Check if two FILE* contain the same word
int isSameWord(FILE *a, FILE *b) {
    int ch1, ch2;
    do {
        ch1 = fgetc(a);
        ch2 = fgetc(b);
        if (ch1 != ch2)
            return 0;
    } while (ch1 != EOF && ch2 != EOF && ch1 != ' ' && ch1 != '\n');

    return 1;
}

// Add a new word to SecretWords.txt and Final.txt
void AddToSecretWords(FILE *words, FILE *final, FILE *temp) {
    fseek(temp, 0, SEEK_SET);
    int c;

    while ((c = fgetc(temp)) != EOF) {
        fputc(c, words);
        fputc(c, final);
    }
    fprintf(words, " %d\n", lastNumber++);
    fputc(' ', final);
}

// Compress by replacing duplicate words
void Dublication(FILE *file) {
    FILE *final = fopen("Final.txt", "w");
    if (!final) {
        printf("Failed to open Final.txt\n");
        return;
    }

    FILE *words = fopen("SecretWords.txt", "a+");
    if (!words) {
        printf("Failed to open SecretWords.txt\n");
        fclose(final);
        return;
    }

    int ch;
    FILE *temp = tmpfile(); // holds each word temporarily

    while ((ch = fgetc(file)) != EOF) {
        if (ch == ' ' || ch == '\n' || ch == '\t') {
            if (ftell(temp) > 0) {
                fseek(temp, 0, SEEK_SET);
                rewind(words);

                int found = 0;
                while (!feof(words)) {
                    FILE *wordFile = tmpfile();
                    int ch1;

                    while ((ch1 = fgetc(words)) != ' ' && ch1 != EOF && ch1 != '\n')
                        fputc(ch1, wordFile);

                    fseek(wordFile, 0, SEEK_SET);
                    fseek(temp, 0, SEEK_SET);

                    if (isSameWord(wordFile, temp)) {
                        int num;
                        fscanf(words, "%d", &num);
                        fprintf(final, "%d ", num);
                        found = 1;
                        fclose(wordFile);
                        break;
                    }

                    while (fgetc(words) != '\n' && !feof(words));
                    fclose(wordFile);
                }

                if (!found) {
                    AddToSecretWords(words, final, temp);
                }

                freopen(NULL, "w+", temp); // clear temp
            }
        } else {
            fputc(ch, temp);  // build word
        }
    }

    // Handle last word if any
    if (ftell(temp) > 0) {
        fseek(temp, 0, SEEK_SET);
        rewind(words);
        int found = 0;

        while (!feof(words)) {
            FILE *wordFile = tmpfile();
            int ch1;

            while ((ch1 = fgetc(words)) != ' ' && ch1 != EOF && ch1 != '\n')
                fputc(ch1, wordFile);

            fseek(wordFile, 0, SEEK_SET);
            fseek(temp, 0, SEEK_SET);

            if (isSameWord(wordFile, temp)) {
                int num;
                fscanf(words, "%d", &num);
                fprintf(final, "%d", num);
                found = 1;
                fclose(wordFile);
                break;
            }

            while (fgetc(words) != '\n' && !feof(words));
            fclose(wordFile);
        }

        if (!found) {
            AddToSecretWords(words, final, temp);
        }
    }

    fclose(temp);
    fclose(final);
    fclose(words);
}

int main() {
    FILE *input = fopen("hobbit.txt", "r");
    if (!input) {
        printf("Failed to open input file.\n");
        return 1;
    }

    clock_t start = clock();
    Dublication(input);
    clock_t end = clock();

    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Compression finished in %.6f seconds.\n", elapsed);

    fclose(input);
    return 0;
}
