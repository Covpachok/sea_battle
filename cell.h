#ifndef CELL_H
#define CELL_H

#include "ship.h"

const char kCellChars[][3] = {"  ", "[]", "::", "{}"};
const int  kCellColor      = 1;
const int  kPrototypeColor = 16;

class Cell {
public:
    enum State { Empty, Ship, Attacked, AttackedShip };

private:
    State     state;
    ShipInfo *ship;

public:
    Cell() : state(Empty), ship(nullptr) {}
    ~Cell()
    {
        if (ship)
            delete ship;
    }

    void Attack() { state = state == State::Ship ? AttackedShip : Attacked; }
    void SetState(State new_state) { state = new_state; }
    void SetShip(int size, Direction dir);

    State GetState() const { return state; }
    bool  IsEmpty() const { return state == Empty; }
    bool  IsShip() const { return state == Ship; }
    bool  IsAttackedEmpty() const { return state == Attacked; }
    bool  IsAttackedShip() const { return state == AttackedShip; }
    bool  IsAttacked() const { return IsAttackedEmpty() || IsAttackedShip(); }
    bool  IsAnyShip() const { return IsShip() || IsAttackedShip(); }

    Direction GetShipDirection() const { return ship->GetDirection(); }
    int       GetShipSize() const { return ship->GetSize(); }
};

#endif
