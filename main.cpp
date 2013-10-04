#include <stdio.h>
#include <locale.h>
#include <ncurses.h>
#include <string.h>
#include <panel.h>
#include "chess.h"

// Color
#define COLOR_LIGHTGRAY 21
#define COLOR_DARKBLUE 20

// Panel sizes
#define BOARD_SIZE 12

#define TERMINAL_BUFFER 1<<15
int init_board();
void show_board(WINDOW* win);
void refresh_all();
int move(char* str, int str_length);

Piece board[64];
WINDOW* board_win;
WINDOW* cmdlnborder_win;
WINDOW* commandline_win;
PANEL* board_panel;
PANEL* cmdlnborder_panel;
PANEL* commandline_panel;

int main(){
    setlocale(LC_ALL, ""); 
    init_board();
    initscr();
    int lines = LINES;
    int cols = COLS;
    cbreak(); 
    start_color();
    keypad(commandline_win, TRUE);
    // init board_panel
    board_win = newwin(BOARD_SIZE, 2 * BOARD_SIZE, 
            (lines - BOARD_SIZE) / 4, (cols - 2 * BOARD_SIZE) / 2);
    box(board_win, 0, 0);
    show_board(board_win);
    board_panel = new_panel(board_win);
    immedok(board_win, TRUE);
    // EXTERNAL border around commandline
    cmdlnborder_win = newwin(lines - (BOARD_SIZE + 
            (lines - BOARD_SIZE) / 4), cols, BOARD_SIZE + 
            (lines - BOARD_SIZE) / 4, 0);
    box(cmdlnborder_win, 0, 0);
    cmdlnborder_panel = new_panel(cmdlnborder_win);
    // init commandline_panel
    commandline_win = newwin(lines - (BOARD_SIZE + 
            (lines - BOARD_SIZE) / 4) - 2, cols - 3, BOARD_SIZE + 
            (lines - BOARD_SIZE) / 4 + 1, 1);
    scrollok(commandline_win, TRUE);
    idlok(commandline_win, TRUE);
    immedok(commandline_win, TRUE);
    //leaveok(commandline_win, TRUE);
    commandline_panel = new_panel(commandline_win);
    update_panels();
    doupdate();
    char str[80];
    while(true){
        mvwprintw(commandline_win, lines - (BOARD_SIZE + 
            (lines - BOARD_SIZE) / 4) - 3, 0, ">>> ");
        mvwgetstr(commandline_win, lines - (BOARD_SIZE + 
            (lines - BOARD_SIZE) / 4) - 3, 4, str);
        int len = 0;
        while(*(str + len) != 0)
            len++;
        
        if(!move(str, len))
            show_board(board_win);
    }
    endwin();
    return 0;
}

void show_board(WINDOW* win){
    init_color(COLOR_DARKBLUE, 0, 0, 750);
    init_color(COLOR_LIGHTGRAY, 500, 500, 500);
    init_color(COLOR_BLACK, 0,0,0);
    init_pair(1, COLOR_BLACK, COLOR_DARKBLUE);
    init_pair(2, COLOR_BLACK, COLOR_LIGHTGRAY);
    init_pair(3, COLOR_WHITE, COLOR_DARKBLUE);
    init_pair(4, COLOR_WHITE, COLOR_LIGHTGRAY);
    init_pair(5, COLOR_WHITE, COLOR_BLACK);
    for(int i = 0; i < 64; i++){
        wattron(win, COLOR_PAIR(((i / 8 + i % 8 + 1) % 2) + 1 + 2 *
                    (board[i].color == COLOR_WHITE)));
        mvwprintw(win, i / 8 + 2, 2 * (i % 8) + 4, "%s ", 
                (char*)&(board[i].icon));
        /*init_pair(1, board[i].type & BLACK ? COLOR_BLACK : COLOR_WHITE,
                (i / 8 + i % 8 + 1) % 2 + 20);
        mvwprintw(win, i / 8 + 2, 2 * (i % 8) + 4, "%s ", 
                (char*)&(board[i].icon));
        */
    }
    wattron(win, COLOR_PAIR(5));
    for(int i = 0; i < 8; i++){
        mvwaddch(win, i + 2, 2, 8 - i + 48);
        mvwaddch(win, 10, 2 * i + 4, i + 97);
    }
}

void refresh_all(){
    update_panels();
    doupdate();
}
