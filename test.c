#include <curses.h>
#include <string.h>

int main() {
    char str[80] = {0}; // Initialize the buffer to zero

    initscr();          // Initialize the screen
    cbreak();           // Disable line buffering, but allow Ctrl-C
    echo();             // Echo input

    mvprintw(10, 10, "What is 2+3? "); // Ask the question

    getstr(str);        // Get the user's input

    if (strcmp("5", str) == 0)
        mvprintw(11, 10, "Correct");   // Display correct if the answer is 5
    else
        mvprintw(11, 10, "Wrong");     // Display wrong otherwise

    mvprintw(12, 10, "Press any key to quit..."); // Instructions to quit
    getch();            // Wait for user input
    endwin();           // Restore terminal settings

    return 0;
}
