#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "tic-tac-toe.h"
#include "server.h"

#define MY_PORT 60002

/*
TCP server for playing tic tac toe game
*/
int main(){


	int myListenSocket, clientSocket;
	struct sockaddr_in  myAddr, clientAddr;
  	int i, addrSize, bytesRcv;
    int row, column;
    char winner;

    //game board
    GameBoard gameBoard;

  	char buffer[30];

    srand(time(NULL));

	/* create socket */

  	myListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  	if (myListenSocket < 0) {
   		printf("eek! couldn't open socket\n");
    	exit(-1);
  	}

	/* setup my server address */
  	memset(&myAddr, 0, sizeof(myAddr));
  	myAddr.sin_family = AF_INET;
  	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  	myAddr.sin_port = htons((unsigned short) MY_PORT);

	/* bind my listen socket */

  	i = bind(myListenSocket, (struct sockaddr *) &myAddr, sizeof(myAddr));

  	if (i < 0) {
    	printf("eek! couldn't bind socket\n");
    	exit(-1);
  	}

	/* listen */
  	i = listen(myListenSocket, 5);
  	if (i < 0) {
    	printf("eek! couldn't listen\n");
    	exit(-1);
  	}

	/* wait for connection request */
  	addrSize = sizeof(clientAddr);

  	clientSocket = accept(myListenSocket,(struct sockaddr *) &clientAddr, &addrSize);

  	if (clientSocket < 0) {
    	printf("eek! couldn't accept the connection\n");
    	exit(-1);
  	}

    //setup board
    setup_board(&gameBoard);

    //wait for start
    /* read message from client */
    bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
    buffer[bytesRcv] = 0;

    if (strcmp(buffer, "s") == 0) {

        while (1) {

            //show the board
            display_board(&gameBoard);

            /* read message from client */
            bytesRcv = recv(clientSocket, buffer, sizeof(buffer), 0);
            buffer[bytesRcv] = 0;

            if (strcmp(buffer, "q") == 0) {
                break;
            }

            //parse row, col
            sscanf(buffer, "%d %d", &row, &column);

            //successfully place?
            if (play_turn_client(&gameBoard, 'X', row, column) == 0)
            {
                printf("Added to location of [%d, %d]\n", row, column);

                //check winner
                winner = find_winner(&gameBoard);

                if (winner != 'N')
                {
                    printf("The client wins! Ending game...\n");
                    break;
                }
                else {

                    //computer play
                    // plays a turn for the server
                    play_turn_server(&gameBoard, 'O');

                    //check winner
                    winner = find_winner(&gameBoard);

                    if (winner != 'N')
                    {
                        printf("The server wins! Ending game...\n");
                        break;
                    }
                    else if (is_full(&gameBoard)){ //game is full
                        printf("The game tie! Ending game...\n");
                        break;
                    }
                    else {
                        strcpy(buffer, "OK");
                        send(clientSocket, buffer, strlen(buffer), 0);
                    }
                }
            }
            else {

                sprintf(buffer, "i");//i for invalid
                send(clientSocket, buffer, strlen(buffer), 0);
            }
        }

        //show the board
        display_board(&gameBoard);

        // ends the game and performs cleanup functions.
        strcpy(buffer, "q");
        end_game(&gameBoard, clientSocket, buffer, strlen(buffer), 0);
    }

	/* close sockets */
	printf("The SERVER is shutting down....\n");
	close(myListenSocket);
	close(clientSocket);

	return 0;
}

// saves data about the game to the game_logs.txt text file)
void display_history(GameBoard* board) {

    int i;

    //open file for writting
    FILE* fp = fopen("game_logs.txt", "w");

    //O plays
    fprintf(fp, "The moves of the server are:\n");

    //write to the console
    fprintf(stdout, "The moves of the server are:\n");

    for (i = 0; i < board->num_O; i++) {
        fprintf(fp, "[%d, %d]\n", board->O[i][0], board->O[i][1]);
        fprintf(stdout, "[%d, %d]\n", board->O[i][0], board->O[i][1]);
    }

    //X plays
    fprintf(fp, "The moves of the client are:\n");

    //write to the console
    fprintf(stdout, "The moves of the client are:\n");

    for (i = 0; i < board->num_X; i++) {
        fprintf(fp, "[%d, %d]\n", board->X[i][0], board->X[i][1]);

        //write to the console
        fprintf(stdout, "[%d, %d]\n", board->X[i][0], board->X[i][1]);
    }

    //close file
    fclose(fp);
}

// ends the game and performs cleanup functions. At a minimum, this function should do the following actions:
//	-- save data to the games_logs.txt file
//	-- dealloacte memory used by the program
//	-- send a signal to the client to terminate
void end_game(GameBoard* board, int clientSocket, char* buffer,
    int bufferLength, int flags) {

    //save to file
    display_history(board);

    //send signal to terminal
    send(clientSocket, buffer, bufferLength, flags);
}

//setup board
void setup_board(GameBoard* b) {

    int i, j;

    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            b->board[i][j] = ' ';
        }
    }

    b->num_O = 0;
    b->num_X = 0;
}
