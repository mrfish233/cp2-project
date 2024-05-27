#include "parse_script.h"

int32_t parseTomlScript(char *filename, Script *script) {
    FILE *file = NULL;
    if (openFile(&file, filename, "r") != 0) {
        return 1;
    }

    char buffer[STR_SIZE] = {0};
    while (readLine(file, buffer, STR_SIZE) == 0) {
        if (strlen(buffer) == 0 || buffer[0] == '#') {
            continue;
        }

        printf("%s\n", buffer);
    }

    closeFile(file);
    return 0;
}
