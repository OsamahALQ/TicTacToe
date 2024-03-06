#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "client.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 60002

/*
TCP client for playing tic tac toe game
*/
int main() {
    int mySocket;
    struct sockaddr_in  addr;
    char command = ' ';
    int i;
    int bytesRcv;
    int row, column;

    char inStr[10];
    char buffer[10];

    /* create socket */
    mySocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mySocket < 0) {
        printf("eek! couldn't open socket\n");
        exit(-1);
    }

    /* setup address */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    addr.sin_port = htons((unsigned short)SERVER_PORT);

    /* connect to server */

    i = connect(mySocket, (struct sockaddr*)&addr, sizeof(addr));

    if (i < 0) {
        printf("client could not connect!\n");
        exit(-1);
    }

    //show menu
    show_menu();

    //read a line
    fgets(buffer, sizeof(buffer), stdin);
    command = buffer[0];

    //play
    if (command == 'p' || command == 'P') {

        //start 's' for start
        strcpy(buffer, "s");
        send(mySocket, buffer, strlen(buffer), 0);

        while (1) { //play until tie or has winner

            sleep(1);

            //read row
            printf("Enter the row: ");

            //read a line
            fgets(inStr, sizeof(inStr), stdin);
            inStr[strlen(inStr) - 1] = 0;
            strcpy(buffer, inStr);

            //convert to int
            sscanf(buffer, "%d", &row);

            //read column
            printf("Enter the column: ");

            //read a line
            fgets(inStr, sizeof(inStr), stdin);
            inStr[strlen(inStr) - 1] = 0;
            strcpy(buffer, inStr);

            //convert to int
            sscanf(buffer, "%d", &column);

            //validate row, col
            if (row >= 1 && row <= 3 && column >= 1 && column <= 3) {

                sprintf(buffer, "%d %d\n", row, column);
                send(mySocket, buffer, strlen(buffer), 0);

                //check the return
                // Get response from server
                bytesRcv = recv(mySocket, buffer, 10, 0);
                buffer[bytesRcv] = 0; // put a 0 at the end so we can display the string

                //quit the program
                if (strcmp(buffer, "q") == 0) {
                    break;
                }
                else if (strcmp(buffer, "i") == 0) //i for invalid
                {
                    printf("The location of [%d, %d] is invalid. Please try again.\n", row, column);
                }
            }
            else {
                printf("The location of [%d, %d] is invalid. Please try again.\n", row, column);
            }

        }

    }
    else {//quit the game

        //send the quit signal to server to terminal
        strcpy(buffer, "q");
        send(mySocket, buffer, strlen(buffer), 0);

    }

    /* close the socket */
    printf("The CLIENT is shutting down....\n");
    close(mySocket);

    return 0;
}

// shows a menu to the user
void show_menu() {

    printf("*****************************\n");
    printf(" What would you like to do\n");
    printf("*****************************\n");
    printf("Play a turn (P)\n");
    printf("Quit (Q)\n");
    printf("-----------------------------\n");
    printf("Enter a choice from the menu (P, Q): ");
}
