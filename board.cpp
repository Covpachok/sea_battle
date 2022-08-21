#include "board.h"

AttackResult Board::Attack(int x, int y)
{
    if (!AreCoordsValid(x, y) || cells[y][x].IsAttacked())
        return AttackResult::Unsuccess;

    cells[y][x].Attack();

    if (IsShipDestroyed(x, y) == DestroyedCheckResult::Destroyed) {
        ++destroyed_ships_count;
        ProcessDestroyedShip(x, y);
    }

    return cells[y][x].IsAttackedEmpty() ? AttackResult::EmptyCell
                                         : AttackResult::ShipCell;
}

void Board::SetCellState(int x, int y, Cell::State new_state)
{
    if (!AreCoordsValid(x, y))
        return;

    cells[y][x].SetState(new_state);
}

bool Board::PlaceShip(int x, int y, int size, Direction dir)
{
    if (!AreCoordsValid(x, y) || !IsShipValid(size))
        return false;

    int xdir = 0, ydir = 0;
    ConvertDirection(dir, xdir, ydir);

    if (!AreCoordsValid(x + (size - 1) * xdir, y + (size - 1) * ydir))
        return false;

    // Check all cells around a ship
    if (!CanPlaceShip(x, y, size, dir))
        return false;

    for (int i = 0; i < size; ++i)
        cells[y + i * ydir][x + i * xdir].SetShip(size, dir);

    ++placed_ships_count[size - 1];

    return true;
}

bool Board::CanPlaceShip(int x, int y, int size, Direction dir) const
{
    int xdir = 0, ydir = 0;
    ConvertDirection(dir, xdir, ydir);

    int cx, cy;
    for (int i = 0; i < size; ++i) {
        cx = x + i * xdir;
        cy = y + i * ydir;
        if (cells[cy][cx].IsShip() ||
            (IsCoordValid(cx - 1) && cells[cy][cx - 1].IsShip()) ||
            (IsCoordValid(cx + 1) && cells[cy][cx + 1].IsShip()) ||
            (IsCoordValid(cy - 1) && cells[cy - 1][cx].IsShip()) ||
            (IsCoordValid(cy + 1) && cells[cy + 1][cx].IsShip()) ||
            (AreCoordsValid(cx - 1, cy - 1) &&
             cells[cy - 1][cx - 1].IsShip()) ||
            (AreCoordsValid(cx + 1, cy - 1) &&
             cells[cy - 1][cx + 1].IsShip()) ||
            (AreCoordsValid(cx - 1, cy + 1) &&
             cells[cy + 1][cx - 1].IsShip()) ||
            (AreCoordsValid(cx + 1, cy + 1) && cells[cy + 1][cx + 1].IsShip()))
            return false;
    }
    return true;
}

void Board::ProcessDestroyedShip(int x, int y)
{
    if (!cells[y][x].IsAnyShip())
        return;

    int xdir = 0, ydir = 0;
    ConvertDirection(cells[y][x].GetShipDirection(), xdir, ydir);

    for (int i = 0; i < cells[y][x].GetShipSize(); ++i) {
        if (cells[y + i * ydir][x + i * xdir].IsAttackedShip()) {
            SubprocessingDestroyedShip(x + i * xdir, y + i * ydir);
        }
    }

    for (int i = 0; i < cells[y][x].GetShipSize(); ++i) {
        if (cells[y - i * ydir][x - i * xdir].IsAttackedShip()) {
            SubprocessingDestroyedShip(x - i * xdir, y - i * ydir);
        }
    }
}

void Board::SubprocessingDestroyedShip(int x, int y)
{
    if (IsCoordValid(y - 1) && cells[y - 1][x].IsEmpty())
        cells[y - 1][x].Attack();
    if (IsCoordValid(y + 1) && cells[y + 1][x].IsEmpty())
        cells[y + 1][x].Attack();
    if (IsCoordValid(x - 1) && cells[y][x - 1].IsEmpty())
        cells[y][x - 1].Attack();
    if (IsCoordValid(x + 1) && cells[y][x + 1].IsEmpty())
        cells[y][x + 1].Attack();
    if (AreCoordsValid(x - 1, y - 1) && cells[y - 1][x - 1].IsEmpty())
        cells[y - 1][x - 1].Attack();
    if (AreCoordsValid(x - 1, y + 1) && cells[y + 1][x - 1].IsEmpty())
        cells[y + 1][x - 1].Attack();
    if (AreCoordsValid(x + 1, y - 1) && cells[y - 1][x + 1].IsEmpty())
        cells[y - 1][x + 1].Attack();
    if (AreCoordsValid(x + 1, y + 1) && cells[y + 1][x + 1].IsEmpty())
        cells[y + 1][x + 1].Attack();
}

Cell::State Board::GetCellState(int x, int y) const
{
    if (!AreCoordsValid(x, y))
        return Cell::Empty;

    return cells[y][x].GetState();
}

DestroyedCheckResult Board::IsShipDestroyed(int x, int y) const
{
    if (!AreCoordsValid(x, y) || !cells[y][x].IsAnyShip())
        return DestroyedCheckResult::NotAShip;

    int ship_parts_count = 0, destroyed_cells_count = 0;
    int xdir = 0, ydir = 0;
    ConvertDirection(cells[y][x].GetShipDirection(), xdir, ydir);

    for (int i = 0; i < cells[y][x].GetShipSize(); ++i) {
        if (AreCoordsValid(x + xdir * i, y + ydir * i)) {
            if (cells[y + ydir * i][x + xdir * i].IsAnyShip()) {
                ++ship_parts_count;
                if (cells[y + ydir * i][x + xdir * i].IsAttackedShip())
                    ++destroyed_cells_count;
            } else {
                break;
            }
        }
    }

    for (int i = 0; i < cells[y][x].GetShipSize(); ++i) {
        if (AreCoordsValid(x - xdir * i, y - ydir * i)) {
            if (cells[y - ydir * i][x - xdir * i].IsAnyShip()) {
                ++ship_parts_count;
                if (cells[y - ydir * i][x - xdir * i].IsAttackedShip())
                    ++destroyed_cells_count;
            } else {
                break;
            }
        }
    }

    return ship_parts_count == destroyed_cells_count
               ? DestroyedCheckResult::Destroyed
               : DestroyedCheckResult::NotDestroyed;
}

bool Board::IsShipValid(int size) const
{
    return IsShipSizeValid(size) && placed_ships_count[size - 1] != 5 - size;
}

bool Board::AreAllShipsPlaced() const
{
    int placed_count = 0;
    for (int i = 0; i < kMaxShipSize; ++i)
        placed_count += placed_ships_count[i];

    return placed_count == kMaxShips;
}
