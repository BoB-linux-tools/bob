#include <ncurses.h>
#include <unistd.h>  // for usleep function

#define ART_WIDTH 80
#define ART_HEIGHT 20
#define DELAY 50000 // Delay in microseconds

// ASCII Art as a multi-line string
const char *art = 
"                                                                                                 \n"
"                                                             *@@@@@@@@@                          \n"
"                                                          @@@@@@@@@@@@@@@@@                      \n"
"                                                        @@@@@@@@@@@@@@@@@@@@@                    \n"
"                                                       @@@@@@@@@@@@@@@@@@@@@@@                   \n"
"                                                      @                    @@@@                  \n"
"                                                      @                    @@@@                  \n"
"                    @@@@ @@@@@@ @@@                   @                    @@@@                  \n"
"               @@@@                  @@@@@            @                    @@@@                  \n"
"           @@@@                           @@@        @@   @@@@@@    @@@@@  @@@@                  \n"
"         @@@                                  @      @                       @@                  \n"
"       @                @ @             @      @     @    @@@@@.   @@@@@@.   @@                  \n"
"      @         @@@@@@@ @ @   @@@@@@@@  @       @    @      @@      @@      @@                   \n"
"     @          @  @    @ @      @  @   @        @   @                      @@                   \n"
"     @          @  @    @@@     @  @    @        @    @                    @@                    \n"
"     @          @  @    @ @    @ @@ @   @        @     @                   @@                    \n"
"     @          @@@@@@@ @ @   @   @  @  @       @      @                  @@                     \n"
"      @                 @ @             @       @       @               --@@                     \n"
"        @                                      @         @  @@@@@@@@@@@  @@                      \n"
"          @                                   @           @     +@@@    @                        \n"
"            @@                        @@ @      @         @@           @@=                       \n"
"               @@@@@@           @@@@@      @@@ @@@       @@ @        @@@@@ @@@@@                 \n"
"                    @@@@@ @@@@@                      @@@@@   @@@@@@@@@@  +@@@@@@@@@@@            \n"
"                                                 @@@@@@@@@     @@@@@:    @@@@@@@@@@@@@@@@        \n"
"                                                @@@@@@@@@@@@           @@@@@@@@@@@@@@@@@@@       \n"
"                                                @@@@@@@@@@@@@@       @@@@@@@@@@@@@@@@@@@@@@      \n"
"                                              @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@    \n"
"                                             @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+  \n"
"                                            @@@@+@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n"
"                                           @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n";

void print_art(WINDOW *win, int startx, int starty) {
    mvwprintw(win, starty, startx, "%s", art);
    wrefresh(win);
}

int main() {
    int x = 0, y = 0;
    int max_x, max_y;
    int dx = 1, dy = 1; // Direction of movement

    initscr();              // Start curses mode
    noecho();               // Don't print input
    curs_set(FALSE);        // Don't display cursor
    keypad(stdscr, TRUE);   // Enable special keys handling
    getmaxyx(stdscr, max_y, max_x); // Get the screen size

    WINDOW *win = newwin(max_y, max_x, 0, 0);
    box(win, 0, 0);         // Draw border around the window

    while (1) {
        werase(win);        // Clear the window
        box(win, 0, 0);     // Redraw border
        print_art(win, x, y);

        // Update position
        x += dx;
        y += dy;

        // Bounce off edges
        if (x <= 0 || x >= max_x - ART_WIDTH) {
            dx = -dx;
        }
        if (y <= 0 || y >= max_y - ART_HEIGHT) {
            dy = -dy;
        }

        usleep(DELAY);      // Sleep for a short time
    }

    endwin();               // End curses mode
    return 0;
}

