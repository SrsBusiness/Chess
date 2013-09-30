#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

int main(){
    printf("%d\n", ERR);
    initscr();
    printw("%d\n", can_change_color());
    refresh();
    getchar();
    endwin();
    return 0;
}
