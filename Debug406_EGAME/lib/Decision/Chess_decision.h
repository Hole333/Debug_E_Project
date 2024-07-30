//调用getBestMove函数即可，参考AIGo()

#ifndef _CHESS_DECISION_H
#define _CHESS_DECISION_H
#include <Arduino.h>

#define EMPTY 0
#define Black 2
#define White 1
#define SIZE 3

uint8_t checkWin(uint8_t board[SIZE][SIZE], uint8_t player);

uint8_t evaluate(uint8_t board[SIZE][SIZE], uint8_t player);

void getBestMove(uint8_t board[SIZE][SIZE], uint8_t player, uint8_t *bestRow, uint8_t *bestCol) ;

void AIGo(uint8_t board[SIZE][SIZE], uint8_t player);

#endif // !_KEY_DRIVER

