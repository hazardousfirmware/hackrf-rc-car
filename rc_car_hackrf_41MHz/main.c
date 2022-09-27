#include <stdio.h>
#include <curses.h>

#include "rf.h"
#include "car.h"

int main(int argc, const char** argv)
{
    //Control the 40MHz NIKKO car


    int result = init_hackrf();
    if (result != 0)
    {
        printf("Hackrf error: %s\n", hackrf_error_name(result));
        return 1;
    }

    set_direction(stop);

    WINDOW* w = initscr();
    //noecho();

    printw("W - forward\n"
           "Q - forward/left\n"
           "E - forward/right\n"
           "S - reverse\n"
           "A - reverse/left\n"
           "D - reverse/right\n"
           "Z - left\n"
           "X - right\n"
           "[space] - stop\n"
           "[escape] - quit\n");

    while(1)
    {
        int c = getch();
        clear();
        if (c == 27 || c == ERR)
        {
            break; //esc key
        }

        if (c >= 'A' && c <= 'Z')
        {
            c+= 0x20;
        }

        switch (c)
        {
        case 'w':
            set_direction(fwd);
            printw("Direction: forward\n");
            break;
        case 'q':
            set_direction(fwd_left);
            printw("Direction: forward left\n");
            break;
        case 'e':
            set_direction(fwd_right);
            printw("Direction: forward right\n");
            break;
        case 's':
            set_direction(rev);
            printw("Direction: reverse\n");
            break;
        case 'a':
            set_direction(rev_left);
            printw("Direction: reverse left\n");
            break;
        case 'd':
            set_direction(rev_right);
            printw("Direction: reverse right\n");
            break;
        case 'z':
            set_direction(left);
            printw("Direction: left\n");
            break;
        case 'x':
            set_direction(right);
            printw("Direction: right\n");
            break;
        case ' ':
            set_direction(stop);
            printw("Direction: stop\n");
            break;
        }

    }

    set_direction(stop);

    delwin(w);
    endwin();

    shutdown_hackrf();

    return 0;
}
