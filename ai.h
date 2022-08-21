#ifndef AI_H
#define AI_H

#include "board.h"

enum class AIDifficulty { Easy, Normal, Hard };

class AI {
    const int kMaxAttackRepetion = 15;

    AIDifficulty difficulty;

    Board *enemy_board;

    bool fo = false;

    int       fo_x = -1, fo_y = -1, sfo_x = -1, sfo_y = -1;
    int       tried_dir_count = 0;
    bool      tried_dir[4]    = {false, false, false, false};
    bool      fo_dir_ok       = false;
    Direction fo_dir          = Direction::Up;

public:
    AI(AIDifficulty diff);

    void MakeTurn();
    void ShipsPlacement(Board &my_board);
    void SetEnemyBoard(Board *en_b) { enemy_board = en_b; }

    void SetDifficulty(AIDifficulty diff) { difficulty = diff; }

private:
    AttackResult EasyAttack();
    AttackResult NormalAttack();
    AttackResult HardAttack();

    AttackResult RandomAttack(int &x, int &y);

    AttackResult FinishOff();
    void         ChangeDir();
    void         Find(Cell::State st, int &x, int &y);
};

#endif
