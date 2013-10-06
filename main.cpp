#include <stdio.h>
#include <locale.h>
#include <ncurses.h>
#include <string.h>
#include <panel.h>
#include "chess.h"
#include "multiplayer.h"
#include <sys/socket.h>

const char __doc__[] = "Usage:\n\
    chess\n\
    chess --host\n\
    chess --search\n\
    chess -h | --help\n\
\n\
Options:\n\
    -h --help   display this help screen\n\
    --host      host a multiplayer game (LAN only)\n\
    --search    search for a LAN game (if none are found, host a game)";

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
void show_prompt(void);

Piece board[64];
WINDOW* board_win;
WINDOW* cmdlnborder_win;
WINDOW* commandline_win;
PANEL* board_panel;
PANEL* cmdlnborder_panel;
PANEL* commandline_panel;
int turn = 0;

bool multiplayer = false;
int host = 0;
int sockfd, connfd;

int main(int argc, char *argv[]){
    if (argc >= 2) {
        if ((strcmp(argv[1],"-h") == 0) || (strcmp(argv[1],"--help") == 0)) {
            printf("%s\n",__doc__);
            return 0;
        } else if ((strcmp(argv[1],"-s") == 0) || (strcmp(argv[1],"--search") == 0)) {
            multiplayer = true;
            printf("searching for a game...\n");
            if ((sockfd = socket_search()) == -1) {
                printf("let's be the host\n");
                host = 1;
                sockfd = setup_socket();
                if (socket_bind(sockfd) == false) {
                    fprintf(stderr, "socket_bind: unable to bind to port\n");
                    return 2;
                }
                connfd = accept_connection(sockfd);
            } else {
                host = 0;
                connfd = sockfd;
                printf("we connected to someone!\n");
            }
        } else if (strcmp(argv[1],"--host") == 0) {
            printf("let's be the host\n");
            multiplayer = true;
            host = 1;
            sockfd = setup_socket();
            if (socket_bind(sockfd) == false) {
                fprintf(stderr, "socket_bind: unable to bind to port\n");
                return 2;
            }
            connfd = accept_connection(sockfd);
        } else {
            printf("%s\n",__doc__);
            return 0;
        }
    }

    if (multiplayer && host) {
        bool ack = false;
        char str[6];
        while (!ack) {
            memset(str, 0, 6 * sizeof(char));
            recv(connfd, str, 6, 0);
            if (strcmp(str, ping) == 0) {
                send(connfd, pong, 6, 0);
                printf("we connected to someone!\n");
                ack = true;
            }
        }
    }

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
        if (multiplayer) {
            if ((turn+host)%2 == 1) {
                show_prompt();
                mvwgetstr(commandline_win, lines - (BOARD_SIZE + 
                    (lines - BOARD_SIZE) / 4) - 3, 4, str);
                send(connfd, str, 80, 0);
            } else {
                memset(str, 0, 80 * sizeof(char));
                recv(connfd, str, 80, 0);
                if (*str == '\0') {
                    clear();
                    endwin();
                    fprintf(stderr, "error: disconnect\n");
                    return 2;
                }
                strncat(str, "\n", 80);
                show_prompt();
                mvwaddstr(commandline_win, lines - (BOARD_SIZE + 
                    (lines - BOARD_SIZE) / 4) - 3, 4, str);
            }
        } else {
            show_prompt();
            mvwgetstr(commandline_win, lines - (BOARD_SIZE + 
                (lines - BOARD_SIZE) / 4) - 3, 4, str);
        }
        int len = 0;
        while(*(str + len) != 0)
            len++;
        
        if(!move(str, len)) {
            show_board(board_win);
            turn++;
        }
    }
    endwin();
    return 0;
}

void show_prompt(void) {
    mvwprintw(commandline_win, LINES - (BOARD_SIZE + 
        (LINES - BOARD_SIZE) / 4) - 3, 0, ">>> ");
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
