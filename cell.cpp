#include "cell.h"

void Cell::SetShip(int size, Direction dir)
{
    if (ship)
        delete ship;
    ship  = new ShipInfo(size, dir);
    state = Ship;
}
