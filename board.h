#ifndef BOARD_H
#define BOARD_H

#include "cell.h"

const int kBoardSize   = 10;
const int kMaxShips    = 10;
const int kMaxShipSize = 4;

enum class AttackResult { Unsuccess, EmptyCell, ShipCell };
enum class DestroyedCheckResult { NotAShip, NotDestroyed, Destroyed };

class Board {
    Cell cells[kBoardSize][kBoardSize];
    int  placed_ships_count[kMaxShipSize] = {0, 0, 0, 0};
    int  destroyed_ships_count            = 0;

public:
    Board() {}

    AttackResult Attack(int x, int y);
    void         SetCellState(int x, int y, Cell::State new_state);
    bool         PlaceShip(int x, int y, int size, Direction dir);

    bool CanPlaceShip(int x, int y, int size, Direction dir) const;

    Cell::State          GetCellState(int x, int y) const;
    DestroyedCheckResult IsShipDestroyed(int x, int y) const;

    int GetDestroyedShipsCount() const { return destroyed_ships_count; }
    int GetShipsCount(int size) const { return placed_ships_count[size - 1]; }

    bool IsShipValid(int size) const;
    bool AreAllShipsPlaced() const;

private:
    void ProcessDestroyedShip(int x, int y);
    void SubprocessingDestroyedShip(int x, int y);
};

inline bool IsCoordValid(int x)
{
    return x >= 0 && x < kBoardSize;
}

inline bool AreCoordsValid(int x, int y)
{
    return IsCoordValid(x) && IsCoordValid(y);
}

inline bool IsShipSizeValid(int size)
{
    return size >= 1 && size <= kMaxShipSize;
}

#endif
