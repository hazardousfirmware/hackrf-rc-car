#include <stdio.h>
#include <curses.h>

#include "rf.h"
#include "car.h"

int main(int argc, const char** argv)
{
    //Control the 27MHz Blitzer car


    int result = init_hackrf();
    if (result != 0)
    {
        printf("Hackrf error: %s\n", hackrf_error_name(result));
        return 1;
    }

    set_direction(STOP);

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
        //clear();
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
            set_direction(FWD);
            printw("\rDirection: forward                               ");
            break;
        case 'q':
            set_direction(FWD_LEFT);
            printw("\rDirection: forward left                          ");
            break;
        case 'e':
            set_direction(FWD_RIGHT);
            printw("\rDirection: forward right                         ");
            break;
        case 's':
            set_direction(REV);
            printw("\rDirection: reverse                               ");
            break;
        case 'a':
            set_direction(REV_LEFT);
            printw("\rDirection: reverse left                          ");
            break;
        case 'd':
            set_direction(REV_RIGHT);
            printw("\rDirection: reverse right                         ");
            break;
        case 'z':
            set_direction(LEFT);
            printw("\rDirection: left                                  ");
            break;
        case 'x':
            set_direction(RIGHT);
            printw("\rDirection: right                                 ");
            break;
        case ' ':
            set_direction(STOP);
            printw("\rDirection: stop                                  ");
            break;
        }

    }

    set_direction(STOP);

    delwin(w);
    endwin();

    shutdown_hackrf();

    return 0;
}
