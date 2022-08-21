#include <ncurses.h>

int main()
{
    initscr();
    keypad(stdscr, true);
    mousemask(ALL_MOUSE_EVENTS, nullptr);
    noecho();
    int key;
    while ((key = getch()) != 'q') {
        mvprintw(0, 0, "KEYID: %d\t", key);
        refresh();
    }
    endwin();
    return 0;
}
