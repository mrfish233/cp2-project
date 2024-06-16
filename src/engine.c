#include "UI.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <script-directory>\n", argv[0]);
        return 1;
    }

    return startEngine(argv[1]);
}
