#include "sea_battle.h"
#include <cstdlib>
#include <iostream>
#include <ncurses.h>

/*
void game_test()
{
    Board test, ai_board;
    std::cerr << "1" << std::endl;
    test.PlaceShip(0, 0, 4, Direction::nx); // inv
    std::cerr << "2" << std::endl;
    test.PlaceShip(0, 0, 4, Direction::ny); // inv
    std::cerr << "3" << std::endl;
    test.PlaceShip(0, 0, 4, Direction::x); // v
    std::cerr << "4" << std::endl;
    test.PlaceShip(0, 1, 4, Direction::x); // inv
    std::cerr << "5" << std::endl;
    test.PlaceShip(3, 0, 4, Direction::x); // inv
    std::cerr << "6" << std::endl;
    test.PlaceShip(1, 1, 4, Direction::y); // inv
    std::cerr << "7" << std::endl;
    test.PlaceShip(9, 9, 4, Direction::y); // inv
    std::cerr << "8" << std::endl;
    test.PlaceShip(0, 2, 3, Direction::x); // v
    std::cerr << "9" << std::endl;
    test.PlaceShip(9, 6, 3, Direction::y); // v
    std::cerr << "10" << std::endl;
    test.PlaceShip(9, 0, 2, Direction::ny); // v
    test.debug_write_field();
}
*/

int main(int argc, char **argv)
{
    AIDifficulty ai_diff;

    if (argc < 2) {
        std::cerr << "Usage: ./{game} {AI difficulty}\n"
                  << "\tAI difficulty levels:\n"
                  << "\t\t0 - Easy\n"
                  << "\t\t1 - Normal\n"
                  << "\t\t2 - Hard" << std::endl;
        return 0;
    } else {
        int temp = 0;
        temp     = atoi(argv[1]);

        if (temp < 0 || temp > 2) {
            std::cerr << "Error: Invalid AI difficulty, (" << temp
                      << ") is out of range(0-2)." << std::endl;
            return 0;
        }
        ai_diff = static_cast<AIDifficulty>(temp);
    }

    SeaBattle game(ai_diff);

    game.Game();

    return 0;
}
