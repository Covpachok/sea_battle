#include "ai.h"

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <ncurses.h>

std::ofstream ai_debug("ai_debug.txt");

AI::AI(AIDifficulty diff) : difficulty(diff) {}

void AI::MakeTurn()
{
    AttackResult at_res;
    do {
        if (enemy_board->GetDestroyedShipsCount() == kMaxShips)
            return;

        if (fo) {
            at_res = FinishOff();
        } else {
            switch (difficulty) {
            case AIDifficulty::Easy:
                at_res = EasyAttack();
                break;
            case AIDifficulty::Normal:
                at_res = NormalAttack();
                break;
            case AIDifficulty::Hard:
                at_res = HardAttack();
                break;
            }
        }
    } while (at_res == AttackResult::ShipCell);
}

void AI::ShipsPlacement(Board &my_board)
{
    int       x, y, size = 1;
    Direction dir;

    for (int i = 0; i < 10; ++i) {
        if (my_board.GetShipsCount(size) == 5 - size)
            ++size;

        do {
            x   = rand() % kBoardSize;
            y   = rand() % kBoardSize;
            dir = static_cast<Direction>(rand() % 4);
        } while (!my_board.PlaceShip(x, y, size, dir));
    }
}

AttackResult AI::EasyAttack()
{
    AttackResult result;
    int          target_x = 0, target_y = 0;

    result = RandomAttack(target_x, target_y);

    return result;
}

AttackResult AI::NormalAttack()
{
    AttackResult result;
    int          target_x = 0, target_y = 0;

    result = RandomAttack(target_x, target_y);

    if (result == AttackResult::ShipCell &&
        enemy_board->IsShipDestroyed(target_x, target_y) !=
            DestroyedCheckResult::Destroyed) {
        fo_x = target_x;
        fo_y = target_y;
        fo   = true;
    }

    return result;
}

AttackResult AI::HardAttack()
{
    AttackResult result;
    int          target_x = 0, target_y = 0;

    if (rand() % 2) {
        Find(Cell::Ship, target_x, target_y);
        result = enemy_board->Attack(target_x, target_y);
    } else {
        result = RandomAttack(target_x, target_y);
    }

    if (result == AttackResult::ShipCell &&
        enemy_board->IsShipDestroyed(target_x, target_y) !=
            DestroyedCheckResult::Destroyed) {
        fo_x = target_x;
        fo_y = target_y;
        fo   = true;
    }

    return result;
}

AttackResult AI::RandomAttack(int &x, int &y)
{
    AttackResult result;
    int          target_x, target_y;

    do {
        target_x = rand() % kBoardSize;
        target_y = rand() % kBoardSize;
        result   = enemy_board->Attack(target_x, target_y);
    } while (result == AttackResult::Unsuccess);

    x = target_x;
    y = target_y;

    return result;
}

AttackResult AI::FinishOff()
{
    ai_debug << __func__ << ": Starting finish off ship at x[" << fo_x << "] y["
             << fo_y << "]"
             << " dir[" << fo_dir << "]" << std::endl;
    AttackResult result = AttackResult::Unsuccess;
    int          xdir, ydir;

    // If no direction is chosen
    if (tried_dir_count == 0) {
        sfo_x = fo_x;
        sfo_y = fo_y;
        ChangeDir();
    }
    ConvertDirection(fo_dir, xdir, ydir);

    // If siutable direction not chosen
    if (!fo_dir_ok) {
        result = enemy_board->Attack(fo_x + xdir, fo_y + ydir);

        if (result == AttackResult::ShipCell) {
            ai_debug << __func__ << ": Success direction[" << fo_dir << "]"
                     << std::endl;
            fo_x += xdir;
            fo_y += ydir;
            fo_dir_ok = true;
        } else if (result == AttackResult::Unsuccess) {
            ai_debug << __func__ << ": Unsuccess, direction[" << fo_dir
                     << "], trying another." << std::endl;
            do {
                ChangeDir();
                ConvertDirection(fo_dir, xdir, ydir);
                result = enemy_board->Attack(fo_x + xdir, fo_y + ydir);
            } while (result == AttackResult::Unsuccess);

            if (result == AttackResult::ShipCell) {
                fo_x += xdir;
                fo_y += ydir;
                fo_dir_ok = true;
            }
        } else {
            ai_debug << __func__ << ": Empty cell, direction[" << fo_dir
                     << "], trying another." << std::endl;
            ChangeDir();
        }
    } else {
        result = enemy_board->Attack(fo_x + xdir, fo_y + ydir);

        if (result != AttackResult::ShipCell) {
            ai_debug << __func__ << ": Trying opposite direction." << std::endl;
            fo_x   = sfo_x;
            fo_y   = sfo_y;
            fo_dir = OppositeDirection(fo_dir);
            if (result == AttackResult::Unsuccess) {
                ConvertDirection(fo_dir, xdir, ydir);
                result = enemy_board->Attack(fo_x + xdir, fo_y + ydir);
            }
        } else {
            fo_x += xdir;
            fo_y += ydir;
        }
    }

    if (enemy_board->IsShipDestroyed(fo_x, fo_y) ==
        DestroyedCheckResult::Destroyed) {
        ai_debug << __func__ << ": Ship at x[" << fo_x << "] y[" << fo_y
                 << "] was destroyed." << std::endl;
        fo              = false;
        tried_dir_count = 0;
        fo_dir_ok       = false;
        for (int i = 0; i < 4; ++i)
            tried_dir[i] = false;
    }

    ai_debug << std::endl;
    return result;
}

void AI::ChangeDir()
{
    for (int i = 0; i < 4; ++i)
        if (!tried_dir[i])
            fo_dir = static_cast<Direction>(i);
    ++tried_dir_count;
    tried_dir[fo_dir] = true;
    ai_debug << __func__ << ": Changed direction to [" << fo_dir << "]"
             << std::endl;
}

void AI::Find(Cell::State st, int &x, int &y)
{
    for (int i = 0; i < kBoardSize; ++i) {
        for (int j = 0; j < kBoardSize; ++j) {
            if (enemy_board->GetCellState(i, j) == st) {
                x = i;
                y = j;
                return;
            }
        }
    }
}
