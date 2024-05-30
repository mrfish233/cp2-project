#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "script.h"
#include "parse_file.h"
#include "parse_script.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <script-directory>\n", argv[0]);
        return 1;
    }

    Script script = {0};
    char script_file[FILE_SIZE] = {0};

    if (snprintf(script_file, FILE_SIZE, "%s/script.toml", argv[1]) < 0) {
        perror("main: snprintf error");
        return 1;
    }

    if (parseTomlScript(script_file, &script) != 0) {
        return 1;
    }

    clearScript(&script);

    return 0;
}
