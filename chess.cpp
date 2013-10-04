#include <ncurses.h>
#include <panel.h>
#include "chess.h"
#include <cmath>

using namespace std;

// the board
extern Piece board[64];

// stack of captured pieces, for move take-backs
Piece captured[32];
char sp;

// each bit determines whether square is guarded by each color
// useful for determining check and checkmate
unsigned long guarded_white;
unsigned long guarded_black;

// the color to move;
char toMove = 0;

void clear_square(char index);
char can_capture(Move m);
char distance(char start, char end);
char same_color(char start, char end);

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

char check_legal(Move m){
    // assume for now the move is formatted correctly, 
    // squares are in the board, etc
    char type = board[m.start].type;
    char result = 0;
    if(type & ROOK){
        result |= ((m.start - m.end) % 8 == 0 || m.start / 8 == m.end / 8) &&
            can_capture(m);
    }
    if(type & BISHOP){
        return (result | ((m.start - m.end) % 7 == 0) || 
            ((m.start - m.end) % 9 == 0)) && can_capture(m) && 
            same_color(m.start, m.end);
    }else if(type & KNIGHT){
        return distance(m.start, m.end) == 5 && can_capture(m);
    }else if(type & PAWN){
        return 1;
    }else if(type & KING){
        char d = distance(m.start, m.end);
        return (d == 1 || d == 2) && can_capture(m);
        
    }else{
        return result;
    }
}

char same_color(char start, char end){
    return (start / 8 + start % 8) % 2 == 
        (end / 8 + end % 8) % 2;
}

char distance(char start, char end){
    return (char)(pow(start % 8 - end % 8, 2) + pow(start / 8 - end / 8, 2));
}

char can_capture(Move m){
    if(board[m.start].type & PAWN){
        return 1;
    }
    return !board[m.end].type || ((board[m.start].type ^ 
        board[m.end].type) & BLACK);
    //return (!board[m.end].type) || board[m.start].color != board[m.end].color;
    
}

char move(char* move_string, int str_length){
    if(str_length < 5)
        return -1;
    char start = *(move_string) - 97 + (8 - (*(move_string+1) - 48)) * 8;
    //move_string++;
    char end = *(move_string+3) - 97 + (8 - (*(move_string+4) - 48)) * 8;
    Move m = {start, end};
    if(!check_legal(m))
        return -1;
    board[end] = board[start];
    clear_square(start);
    return 0;
}

void clear_square(char index){
    board[index].type = 0;
    board[index].color = 0;
    board[index].icon = 32;
}
