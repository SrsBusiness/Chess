#include <ncurses.h>
#include <panel.h>
#include "chess.h"

void clear_square(int index);

extern Piece board[64];
int toMove = 0;

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
    board[0].icon = board[7].icon = board[56].icon = 
        board[63].icon = *(int *)"♜";
    
    // knights-black
    board[1].type = board[6].type = KNIGHT | BLACK;
    board[1].color = board[6].color = COLOR_BLACK;

    // knights-white
    board[57].type = board[62].type = KNIGHT;
    board[57].color = board[62].color = COLOR_WHITE;
    board[1].icon = board[6].icon = board[57].icon = 
        board[62].icon = *(int*)"♞";

    // bishops-black
    board[2].type = board[5].type = BISHOP | BLACK;
    board[2].color = board[5].color = COLOR_BLACK;

    // bishops-white
    board[58].type = board[61].type = BISHOP;
    board[58].color = board[61].color = COLOR_WHITE;
    board[2].icon = board[5].icon = board[58].icon = 
        board[61].icon = *(int*)"♝";

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

int move(char* move_string, int str_length){
    if(str_length < 5)
        return -1;
    int start = *(move_string) - 97 + (8 - (*(move_string+1) - 48)) * 8;
    //move_string++;
    int end = *(move_string+3) - 97 + (8 - (*(move_string+4) - 48)) * 8;
    board[end] = board[start];
    clear_square(start);
    return 0;
}

void clear_square(int index){
    board[index].type = 0;
    board[index].color = 0;
    board[index].icon = 32;
}
