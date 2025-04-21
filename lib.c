#include "lib.h"

void cleanString(char *buffer) {
    int len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n')) {
        buffer[len - 1] = '\0';
        len--;
    }
}

void clearCharArray(char* str) {
    for(int i = 0; str[i] != '\0'; i++) {
        str[i] = '\0';
    }
}
