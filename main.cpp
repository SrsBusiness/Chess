#include <stdio.h>
#include <locale.h>
#include <ncurses.h>
#include "chess.h"

// Color
#define COLOR_LIGHTGRAY 20
#define COLOR_DARKBLUE 21

int init_board();
void show_board();

Piece board[64];

int main(){
    setlocale(LC_ALL, ""); 
    
    init_board();
    for(int i = 0; i < 64; i++){
        if(board[i].type == (PAWN | BLACK) || board[i].type == PAWN){
            printf("%d\n", board[i].color);
        }
    }
    initscr();
    cbreak();
    start_color();
    
    show_board();
    //display_board();
    
    
    refresh();
    getchar();
    endwin();
    return 0;
}

int init_board(){
    // pawns-black
    for(int i = 8; i < 16; i++){
        board[i].type = PAWN | BLACK;
        board[i].color = COLOR_BLACK;
        board[i].icon = *(int *)"♟";
    } 
    // pawns-white
    for(int i = 48; i < 56; i++){
        board[i].type = PAWN;
        board[i].color = COLOR_WHITE;
        board[i].icon = *(int*)"♟";
    }
    // rooks-black
    board[0].type = board[7].type = ROOK | BLACK;
    board[0].color = board[7].color = COLOR_BLACK;
    
    // rooks-white
    board[56].type = board[63].type = ROOK;
    board[56].color = board[63].color = COLOR_WHITE;
    board[0].icon = board[7].icon = board[56].icon = board[63].icon = *(int *)"♜";
    
    // knights-black
    board[1].type = board[6].type = KNIGHT | BLACK;
    board[1].color = board[6].color = COLOR_BLACK;

    // knights-white
    board[57].type = board[62].type = KNIGHT;
    board[57].color = board[62].color = COLOR_WHITE;
    board[1].icon = board[6].icon = board[57].icon = board[62].icon = *(int*)"♞";

    // bishops-black
    board[2].type = board[5].type = BISHOP | BLACK;
    board[2].color = board[5].color = COLOR_BLACK;

    // bishops-white
    board[58].type = board[61].type = BISHOP;
    board[58].color = board[61].color = COLOR_WHITE;
    board[2].icon = board[5].icon = board[58].icon = board[61].icon = *(int*)"♝";

    // queens
    board[3].type = BISHOP | ROOK | BLACK;
    board[3].color = COLOR_BLACK;
    board[59].type = BISHOP | ROOK;
    board[59].color = COLOR_WHITE;
    board[3].icon = board[59].icon = *(int*)"♛";
    
    // kings
    board[4].type = KING | BLACK;
    board[4].color = COLOR_BLACK;
    board[60].type = KING;
    board[60].color = COLOR_WHITE;
    board[4].icon = board[60].icon = *(int*)"♚";
    return 0;
}

void show_board(){
    init_color(COLOR_DARKBLUE, 0, 0, 750);
    init_color(COLOR_LIGHTGRAY, 500, 500, 500);
    init_pair(1, COLOR_BLACK, COLOR_DARKBLUE);
    init_pair(2, COLOR_BLACK, COLOR_LIGHTGRAY);
    init_pair(3, COLOR_WHITE, COLOR_DARKBLUE);
    init_pair(4, COLOR_WHITE, COLOR_LIGHTGRAY);
    for(int i = 0; i < 64; i++){
        attron(COLOR_PAIR(((i / 8 + i % 8 + 1) % 2) + 1 + 2 * board[i].color/COLOR_WHITE));
        mvprintw(i / 8, 2 * (i % 8), "%s ", (char*)&(board[i].icon));
    }
}
