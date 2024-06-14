#include "UI.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <script-directory>\n", argv[0]);
        return 1;
    }

    return startEngine(argv[1]);

    // Script  script  = {0};
    // Display display = {0};

    // if (initGame(&script, &display, argv[1]) != 0) {
    //     return 1;
    // }

    // while (1) {
    //     if (updateDialogue(&script, &display) != 0) {
    //         break;
    //     }

    //     if (strlen(display.character) > 0) {
    //         printf("%s: ", display.character);
    //     }

    //     printf("%s\n", display.dialogue);

    //     if (display.update_flag) {
    //         for (int32_t i = 0; i < display.update_size; i++) {
    //             printf("%s\n", display.updates[i]);
    //         }
    //     }

    //     if (display.option_flag) {
    //         for (int32_t i = 0; i < display.option_size; i++) {
    //             printf("    %d. %s\n", i + 1, display.options[i]);
    //         }

    //         int32_t option = 0;
    //         printf("Select an option: ");
    //         scanf("%d", &option);

    //         while (option < 1 || option > display.option_size) {
    //             printf("Invalid option, please select again: ");
    //             scanf("%d", &option);
    //         }

    //         display.option_select = option;
    //     }

    //     if (display.end_flag) {
    //         break;
    //     }

    //     usleep(500000);
    // }

    // if (saveScript(&script, AUTO) != 0) {
    //     return 1;
    // }

    // clearScript(&script);

    // return 0;
}
