#include<Chess_decision.h>

uint8_t board[SIZE][SIZE] = {
	{EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY}
};

uint8_t corners[4][2] = { {0, 0}, {0, 2}, {2, 0}, {2, 2} };

uint8_t checkWin(uint8_t board[SIZE][SIZE], uint8_t player) {
		uint8_t s = 0;
		for (int i = 0; i < SIZE; i++) {
			if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
				(board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
				return 1;
			}
		}

		if ((board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
			(board[0][2] == player && board[1][1] == player && board[2][0] == player)) {
			return 1;
		}
		return 0;

	}

uint8_t evaluate(uint8_t board[SIZE][SIZE], uint8_t player) {
	uint8_t score = 0;
	uint8_t playerCountRow = 0;
	uint8_t playerCountCol = 0;
	uint8_t playerCountDiag1 = 0;
	uint8_t playerCountDiag2 = 0;

	// 赢得比赛
	if (checkWin(board, player)) {
		score += 100;
	}
	// 行和列两个连续
	for (uint8_t i = 0; i < SIZE - 1; i++) {
		playerCountRow = 0;
		playerCountCol = 0;
		for (int j = 0; j < SIZE - 1; j++) {
			if (board[i][j] == player && board[i][j+1] == player)   playerCountRow++;
			if (board[j][i] == player && board[j+1][i] == player)   playerCountCol++;
		}
		if (playerCountRow >= 1) score += 10;
		if (playerCountCol >= 1) score += 10;
	}
	// 对角线两个连续
	for (uint8_t i = 0; i < SIZE - 1; i++) {
		if (board[i][i] == player && board[i+1][i+1] == player)   playerCountDiag1++;
		if (board[i][SIZE - i - 1] == player && board[i+1][SIZE - i-2] == player)   playerCountDiag2++;
	}
	if (playerCountDiag1 >= 1) score += 10;
	if (playerCountDiag2 >= 1) score += 10;
	// 中心点
	if (board[1][1] == player) score += 20;
	// 四角点
	for (uint8_t k = 0; k < 4; k++) {
		uint8_t x = corners[k][0];
		uint8_t y = corners[k][1];
		if (board[x][y] == player) score += 2;
	}

	return score;
}

void getBestMove(uint8_t board[SIZE][SIZE], uint8_t player, uint8_t *bestRow, uint8_t *bestCol) {
    uint8_t opponent = (player == Black) ? White : Black;
	uint8_t bestScore = -1000;
	uint8_t score_p;
	uint8_t score_o;
	for (uint8_t i = 0; i < SIZE; i++) {
		for (uint8_t j = 0; j < SIZE; j++) {
			if (board[i][j] == EMPTY) {
				// 将位置设置为自己的标志并评估得分
				board[i][j] = player;
				score_p = evaluate(board, player);
				// 将位置设置为对手的标志并评估得分
				board[i][j] = opponent;
				score_o = evaluate(board, opponent)/2;

				board[i][j] = EMPTY;

				if (score_p + score_o > bestScore) {
					bestScore = score_p + score_o;
					*bestRow = i;
					*bestCol = j;
				}
			}
		}
	}
}

void AIGo(uint8_t board[SIZE][SIZE], uint8_t player)
{
	uint8_t bestRow, bestCol;
	getBestMove(board, player, &bestRow, &bestCol);
	board[bestRow][bestCol] = player;
}

