#ifndef SHIP_H
#define SHIP_H

enum Direction { Up, Right, Down, Left };

class ShipInfo {
    int       size;
    Direction dir;

public:
    ShipInfo(int s, Direction d) : size(s), dir(d) {}

    void SetSize(int s) { size = s; }
    void SetDirection(Direction d) { dir = d; }

    int       GetSize() const { return size; }
    Direction GetDirection() const { return dir; }
};

void ConvertDirection(Direction dir, int &xdir, int &ydir);
Direction OppositeDirection(Direction dir);

#endif
