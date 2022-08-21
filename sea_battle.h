#ifndef SEA_BATTLE_H
#define SEA_BATTLE_H

#include "ai.h"
#include "board.h"

#include <ncurses.h>

class SeaBattle {
    enum Players { First, Second };
    enum Range { NotInRange, FirstBoard, SecondBoard };

    Players current_turn = Players::First;
    WINDOW *first_win, *second_win;
    AI      ai;
    Board   first_board, second_board;

    int  first_board_x, second_board_x, board_y;
    int  input_x = -1, input_y = -1, input_key = -1;
    bool exit_flag = false, lmb_click = false, rmb_click = false;

public:
    SeaBattle(AIDifficulty aidiff);
    ~SeaBattle();

    void Game();

private:
    void Turn();

    void PlayerShipsPlacement();
    void PlayerTurn();

    void DrawBoards(bool draw_enemy_ships = false);

    void NcursesInit();
    void InitColors();

    void HandleInput();

    Range IsInputInRange();

    void GameOver(Players winner);
};

#endif
