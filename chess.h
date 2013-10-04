// Piece masks
#define ROOK 0x01
#define BISHOP 0x02
#define KNIGHT 0x04
#define PAWN 0x08
#define KING 0x10
#define EN_PASSANT 0x20

// Black Type
#define BLACK 0x80

// Masks For Move Struct
typedef struct Piece{
    /*
        type: most significant bit is color: 1 = black; 0 = white;
    */
    unsigned char type;
    int color;  // color mask
    int icon;   // unicode character to be displayed on board
    Piece() : icon(' '){
    }
} Piece;

typedef struct Move{
    char start;
    char end;
    // todo: add vector of Move pointers here
} Move;
