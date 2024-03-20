#ifndef CONVERTER_H
#define CONVERTER_H
#include <ctype.h>
#include <string.h>

void convertConsonantsToUpper(char *text) {
    char vowels[] = "aeiouAEIOU";
    for (int i = 0; text[i] != '\0'; i++) {
        if (isalpha(text[i]) && !strchr(vowels, text[i])) {
            text[i] = toupper(text[i]);
        }
    }
}

#endif
