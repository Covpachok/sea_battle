#include "ship.h"

void ConvertDirection(Direction dir, int &xdir, int &ydir)
{
    xdir = dir == Direction::Right ? 1 : dir == Direction::Left ? -1 : 0;
    ydir = dir == Direction::Down ? 1 : dir == Direction::Up ? -1 : 0;
}

Direction OppositeDirection(Direction dir)
{
    return dir == Direction::Down   ? Direction::Up
           : dir == Direction::Up   ? Direction::Down
           : dir == Direction::Left ? Direction::Right
                                    : Direction::Left;
}
