#include "tic-tac-toe.h"
#include <stdio.h>
#include <stdlib.h>

// displays the game board to the screen
void display_board(GameBoard* b) {

	int i, j; //loop control variable

	//separation line
	printf("----------------------------------------------\n");

	//rows
	for (i = 0; i < 3; i++) {
		//cols
		for (j = 0; j < 3; j++) {

			if (j > 0) {
				printf("|");
			}
			//X or O or empty
			printf("    %c    ", b->board[i][j]);
		}
		printf("\n");

		//separation line
		printf("----------------------------------------------\n");
	}

}

// plays a turn for the server
void play_turn_server(GameBoard* b, char play) {

	int r, c; //row and col, used as loop control variable

	//try to win
	for (r = 0; r < 3; r++) {
		//for column
		for (c = 0; c < 3; c++) {

			if (b->board[r][c] == ' ') {

				b->board[r][c] = play;

				if (is_winner(b, play)) {

					//update positions
					update_position(b, play, r + 1, c + 1);

					return;
				}

				//cannot win, restore the previous state
				b->board[r][c] = ' ';
			}
		}
	}

	//play randomly
	while (1) {
		r = rand() % 3;
		c = rand() % 3;

		if (b->board[r - 1][c - 1] == ' ') {
			//update positions
			update_position(b, play, r + 1, c + 1);
			return;
		}
	}

}

// plays a turn for the client. Returns the following values:
//	-- -1: if client is trying to play on a space that is not available (i.e., occupied by an X or O)
//	-- 0: if client is able to successfully place their piece on an empty space on the board
int play_turn_client(GameBoard* b, char play, int row, int col) {

	//validate
	if (row >= 1 && row <= 3 && col >= 1 && col <= 3 && b->board[row - 1][col - 1] == ' ') {

		//update positions
		update_position(b, play, row, col);

		return 0;

	}
	else {
		//invalid position or occupied position
		return -1;
	}

}

// determines the winner of the game (if a winner exists). Returns the following:
//	-- X: if the winner is X
//	-- O: if the winner is O
//	-- N: if there is no winner. There will be no winner in two cases: the game is a tie or there are still spaces on the board left to play
char find_winner(GameBoard* b) {

	//O is winner
	if (is_winner(b, 'O')) {
		return 'O';
	}

	//X is winner
	if (is_winner(b, 'X')) {
		return 'X';
	}

	return 'N'; //tie or board is not full
}

// determines the winner of the game (if a winner exists).
// Returns 1 if win; otherwise, return 0
int is_winner(GameBoard* b, char play) {

	int row, col; //loop control variable

	//check row
	for (row = 0; row < 3; row++) {

		int win = 1;

		//for column
		for (col = 0; win && col < 3; col++) {
			if (b->board[row][col] != play) {
				win = 0;
			}
		}

		if (win) {
			return 1;
		}
	}

	//check column
	for (col = 0; col < 3; col++) {

		int win = 1;

		//for row
		for (row = 0; win && row < 3; row++) {
			if (b->board[row][col] != play) {
				win = 0;
			}
		}

		if (win) {
			return 1;
		}
	}

	//check diagonal: top left to bottom right
	if (b->board[0][0] == play && b->board[1][1] == play && b->board[2][2] == play) {
		return 1;
	}

	//check diagonal: bottom left to top right
	if (b->board[0][2] == play && b->board[1][1] == play && b->board[2][0] == play) {
		return 1;
	}

	return 0;
}

//update the board when the player plays
void update_position(GameBoard* b, char play, int row, int col) {

	b->board[row - 1][col - 1] = play;

	//update positions
	if (play == 'X') {

		b->X[b->num_X][0] = row;
		b->X[b->num_X][1] = col;
		b->num_X += 1;

	}
	else { //player O

		b->O[b->num_O][0] = row;
		b->O[b->num_O][1] = col;
		b->num_O += 1;

	}

}

// check if board is full
// Returns 1 if full; otherwise, return 0
int is_full(GameBoard* b) {

	int row, col;

	//for row
	for (row = 0; row < 3; row++) {
		//for column
		for (col = 0; col < 3; col++) {
			if (b->board[row][col] == ' ') {
				return 0;
			}
		}
	}
	return 1;
}
