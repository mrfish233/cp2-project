#include "parse_file.h"

int32_t openFile(FILE **file, char *fileName, char *mode) {
    *file = fopen(fileName, mode);

    if (*file == NULL) {
        char err[FILE_SIZE] = {0};
        snprintf(err, FILE_SIZE, "parse_file: '%s' error", fileName);
        perror(err);

        return 1;
    }

    return 0;
}

int32_t readLine(FILE *file, char *buffer, size_t size) {
    if (fgets(buffer, size, file) == NULL) {
        return (feof(file) ? 1 : 2);
    }

    if (buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer) - 1] = '\0';
    }

    return 0;
}

void closeFile(FILE *file) {
    if (file == NULL) {
        return;
    }

    fclose(file);
}
