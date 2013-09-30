// Piece masks
#define ROOK 0x01
#define BISHOP 0x02
#define KNIGHT 0x04
#define PAWN 0x08
#define KING 0x10

// Black Type
#define BLACK 0x80

typedef struct Piece{
    /*
        type: most significant bit is color: 1 = black; 0 = white;
    */
    unsigned char type;
    // color mask
    int color;
    int icon; // unicode character to be displayed on board
    Piece() : icon(32){
    }
} Piece;

