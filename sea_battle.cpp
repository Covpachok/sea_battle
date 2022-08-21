#include "sea_battle.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>

std::ofstream debug("debug.txt");

const int kCellPair   = 1;
const int kInfMsgPair = 8;
const int kErrMsgPair = 9;
const int kGrnMsgPair = 10;
const int kProtPair   = 16;

const char kCellTile[][3]      = {"  ", "[]", "::", "{}"};
const char kDirectionsStr[][6] = {"Up", "Right", "Down", "Left"};
const char kWinnerStr[][19]    = {"FIRST PLAYER WIN!", "SECOND PLAYER WIN!"};

const int kBoardWidth   = kBoardSize * 2 + 2;
const int kBoardHeight  = kBoardSize + 2;
const int kBoardGap     = 16;
const int kMinScrWidth  = kBoardWidth * 2 + kBoardGap + 2;
const int kMinScrHeight = kBoardHeight + 24;

static void DrawCell(WINDOW *win, int x, int y, const char tile[3], int color)
{
    wattrset(win, color);
    mvwprintw(win, y + 1, x * 2 + 1, "%s", tile);
    wattroff(win, color);
}

static void DrawBoard(WINDOW *win, const Board &board, bool draw_ships)
{
    Cell::State st;
    for (int y = 0; y < kBoardSize; ++y) {
        for (int x = 0; x < kBoardSize; ++x) {
            st = board.GetCellState(x, y);
            if (st == Cell::Ship && !draw_ships)
                DrawCell(win, x, y, kCellTile[0],
                         COLOR_PAIR(kCellPair) | A_BOLD);
            else
                DrawCell(win, x, y, kCellTile[st],
                         COLOR_PAIR(kCellPair + st) | A_BOLD);
        }
    }
    wrefresh(win);
    refresh();
}

static void DrawShipPrototype(WINDOW *win, int x, int y, int size,
                              Direction dir)
{
    if (!AreCoordsValid(x, y) && !IsShipSizeValid(size))
        return;

    int xdir = 0, ydir = 0;
    ConvertDirection(dir, xdir, ydir);

    if (!AreCoordsValid(x + (size - 1) * xdir, y + (size - 1) * ydir))
        return;

    for (int i = 0; i < size; i++)
        DrawCell(win, x + i * xdir, y + i * ydir, kCellTile[Cell::Ship],
                 COLOR_PAIR(kProtPair) | A_DIM);

    wrefresh(win);
    refresh();
}

static void EraseShipPrototype(WINDOW *win, int x, int y, int size,
                               Direction dir)
{
    if (!AreCoordsValid(x, y) && !IsShipSizeValid(size))
        return;

    int xdir = 0, ydir = 0;
    ConvertDirection(dir, xdir, ydir);

    if (!AreCoordsValid(x + (size - 1) * xdir, y + (size - 1) * ydir))
        return;

    for (int i = 0; i < size; i++)
        DrawCell(win, x + i * xdir, y + i * ydir, kCellTile[Cell::Empty],
                 COLOR_PAIR(kCellPair + Cell::Empty));

    wrefresh(win);
    refresh();
}

static void PrintPlacementHelpInfo(const Board &board, int ship_size,
                                   int ship_dir)
{
    attrset(COLOR_PAIR(kInfMsgPair) | A_BOLD);
    mvprintw(0, 0, "Hotkey: [1-4]  Size: %d\t", ship_size);
    mvprintw(1, 0, "Hotkey: [r][R] Direction: %s\t",
             kDirectionsStr[static_cast<int>(ship_dir)]);
    mvprintw(3, 0, "Remaining Ships: ");
    mvprintw(4, 0, "    4: [%d]", 1 - board.GetShipsCount(4));
    mvprintw(5, 0, "    3: [%d]", 2 - board.GetShipsCount(3));
    mvprintw(6, 0, "    2: [%d]", 3 - board.GetShipsCount(2));
    mvprintw(7, 0, "    1: [%d]", 4 - board.GetShipsCount(1));
    attroff(COLOR_PAIR(kInfMsgPair) | A_BOLD);
    refresh();
}

static void PrintPlacementError()
{
    attrset(COLOR_PAIR(kErrMsgPair) | A_BOLD);
    mvprintw(9, 0, "Can't place ship.\t\t\t\t");
    attroff(COLOR_PAIR(kErrMsgPair) | A_BOLD);
    refresh();
}

static void PrintPlacementSuccess()
{
    attrset(COLOR_PAIR(kGrnMsgPair) | A_BOLD);
    mvprintw(9, 0, "Ship successfully placed.\t\t\t\t");
    attroff(COLOR_PAIR(kGrnMsgPair) | A_BOLD);
    refresh();
}

inline static void ConvertToBoardCoords(int &x, int &y, int board_x,
                                        int board_y)
{
    x = (x - board_x - 1) / 2;
    y = y - board_y - 1;
}

static void ErasePlacementMessages()
{
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < kMinScrWidth; j++)
            mvaddch(i, j, ' ');
}

SeaBattle::SeaBattle(AIDifficulty aidiff) : ai(aidiff)
{
    srand(time(nullptr));

    NcursesInit();
    InitColors();

    ai.SetEnemyBoard(&first_board);

    DrawBoards();
}

SeaBattle::~SeaBattle()
{
    curs_set(true);
    endwin();
}

void SeaBattle::Game()
{
    ai.ShipsPlacement(second_board);
    DrawBoards();

    PlayerShipsPlacement();
    if (exit_flag)
        return;

    Turn();
}

void SeaBattle::Turn()
{
    while (true) {
        PlayerTurn();

        if (exit_flag)
            return;

        ai.MakeTurn();

        if (first_board.GetDestroyedShipsCount() == kMaxShips) {
            GameOver(Players::Second);
            return;
        }

        DrawBoards();
    }
}

void SeaBattle::PlayerTurn()
{
    AttackResult result;

    do {
        do {
            HandleInput();
            if (exit_flag)
                return;
        } while (IsInputInRange() != Range::SecondBoard);

        ConvertToBoardCoords(input_x, input_y, second_board_x, board_y);

        result = second_board.Attack(input_x, input_y);

        if (second_board.GetDestroyedShipsCount() == kMaxShips) {
            GameOver(Players::First);
            exit_flag = true;
            return;
        }

        DrawBoards();

    } while (result != AttackResult::EmptyCell);
}

void SeaBattle::PlayerShipsPlacement()
{
    bool      first_click = true, prot_drawn = false;
    int       ship_size = 1, ship_x = -1, ship_y = -1;
    int       prot_size = 1, prot_x = -1, prot_y = -1;
    Direction ship_dir = Direction::Right, prot_dir = Direction::Right;

    while (!first_board.AreAllShipsPlaced()) {
        PrintPlacementHelpInfo(first_board, ship_size, ship_dir);

        HandleInput();
        if (exit_flag)
            return;

        ship_x = input_x;
        ship_y = input_y;
        ConvertToBoardCoords(ship_x, ship_y, first_board_x, board_y);

        // Change ship size
        if (input_key >= '1' && input_key <= '4')
            ship_size = input_key - '0';

        // Change ship direction
        if (input_key == 'r') {
            if (ship_dir == Direction::Left)
                ship_dir = Direction::Up;
            else
                ship_dir =
                    static_cast<Direction>(static_cast<int>(ship_dir) + 1);
        } else if (input_key == 'R') {
            if (ship_dir == Direction::Up)
                ship_dir = Direction::Left;
            else
                ship_dir =
                    static_cast<Direction>(static_cast<int>(ship_dir) - 1);
        }

        if (rmb_click && prot_drawn) {
            EraseShipPrototype(first_win, prot_x, prot_y, prot_size, prot_dir);
            first_click = true;
        }

        if (!first_board.IsShipValid(ship_size)) {
            PrintPlacementError();
            continue;
        }

        if (lmb_click && IsInputInRange() == Range::FirstBoard) {
            // If clicked on ship prototype
            if (!first_click && prot_x == ship_x && prot_y == ship_y &&
                prot_size == ship_size && prot_dir == ship_dir) {
                // If ship placed successfully
                if (first_board.PlaceShip(ship_x, ship_y, ship_size,
                                          ship_dir) == true) {
                    DrawBoard(first_win, first_board, true);

                    prot_drawn  = false;
                    first_click = true;

                    PrintPlacementSuccess();
                }
            } else {
                // Erasing previous prototype
                if (prot_drawn) {
                    EraseShipPrototype(first_win, prot_x, prot_y, prot_size,
                                       prot_dir);
                    prot_drawn = false;
                }

                // Saving prototype information
                prot_x    = ship_x;
                prot_y    = ship_y;
                prot_size = ship_size;
                prot_dir  = ship_dir;

                // If information correct - draw prototype
                if (AreCoordsValid(prot_x, prot_y) &&
                    first_board.CanPlaceShip(prot_x, prot_y, prot_size,
                                             prot_dir)) {
                    DrawShipPrototype(first_win, prot_x, prot_y, prot_size,
                                      prot_dir);
                    prot_drawn = true;
                } else {
                    PrintPlacementError();
                }

                first_click = false;
            }
        }
    }

    ErasePlacementMessages();
    DrawBoards();
}

void SeaBattle::DrawBoards(bool draw_enemy_ships)
{
    DrawBoard(first_win, first_board, true);
    DrawBoard(second_win, second_board, draw_enemy_ships);
}

void SeaBattle::NcursesInit()
{
    initscr();
    cbreak();
    noecho();
    curs_set(false);
    keypad(stdscr, true);
    mouseinterval(0);
    mousemask(ALL_MOUSE_EVENTS, nullptr);
    start_color();
    refresh();

    // Windows initialization
    int max_height, max_width;
    getmaxyx(stdscr, max_height, max_width);

    if (max_height < kMinScrHeight || max_width < kMinScrWidth) {
        endwin();
        std::cerr << "Error: Small screen size " << max_width << "x"
                  << max_height << ", expected " << kMinScrWidth << "x"
                  << kMinScrHeight << std::endl;
        exit(0);
    }

    first_board_x  = (max_width - kBoardGap) / 2 - kBoardWidth;
    second_board_x = first_board_x + kBoardGap + kBoardWidth;
    board_y        = (max_height - kBoardHeight) / 2;

    first_win  = newwin(kBoardHeight, kBoardWidth, board_y, first_board_x);
    second_win = newwin(kBoardHeight, kBoardWidth, board_y, second_board_x);

    box(first_win, 0, 0);
    box(second_win, 0, 0);

    wrefresh(first_win);
    wrefresh(second_win);
    refresh();
}

void SeaBattle::InitColors()
{
    init_pair(kCellPair + Cell::Empty, COLOR_BLACK, COLOR_BLACK);
    init_pair(kCellPair + Cell::Ship, COLOR_YELLOW, COLOR_BLACK);
    init_pair(kCellPair + Cell::Attacked, COLOR_BLACK, COLOR_BLUE);
    init_pair(kCellPair + Cell::AttackedShip, COLOR_YELLOW, COLOR_RED);
    init_pair(kProtPair, COLOR_WHITE, COLOR_BLACK);
    init_pair(kInfMsgPair, COLOR_YELLOW, COLOR_BLACK);
    init_pair(kErrMsgPair, COLOR_RED, COLOR_BLACK);
    init_pair(kGrnMsgPair, COLOR_GREEN, COLOR_BLACK);
}

void SeaBattle::HandleInput()
{
    MEVENT mouse_event;
    bool   success = false;
    int    key;

    lmb_click = false;
    rmb_click = false;
    input_key = 0;

    while (!success) {
        key = getch();
        switch (key) {

        case KEY_MOUSE:
            if (getmouse(&mouse_event) == OK) {
                input_x = mouse_event.x;
                input_y = mouse_event.y;
                if (mouse_event.bstate & BUTTON1_PRESSED) {
                    lmb_click = true;
                    success   = true;
                } else if (mouse_event.bstate & BUTTON3_PRESSED) {
                    rmb_click = true;
                    success   = true;
                }
            }
            break;

        case 'q':
            exit_flag = true;
            success   = true;
            break;

        default:
            input_key = key;
            success   = true;
            break;
        }
    }
}

SeaBattle::Range SeaBattle::IsInputInRange()
{
    Range range = Range::NotInRange;
    if (input_y > board_y && input_y < board_y + kBoardHeight - 1) {
        if (input_x > first_board_x &&
            input_x < first_board_x + kBoardWidth - 1)
            range = Range::FirstBoard;
        if (input_x > second_board_x &&
            input_x < second_board_x + kBoardWidth - 1)
            range = Range::SecondBoard;
    }
    return range;
}

void SeaBattle::GameOver(SeaBattle::Players winner)
{
    attrset(COLOR_PAIR(kInfMsgPair) | A_BOLD);
    mvprintw(5, (getmaxx(stdscr) - 18) / 2, "%s", kWinnerStr[winner]);
    attroff(COLOR_PAIR(kInfMsgPair) | A_BOLD);

    DrawBoards(true);
    refresh();

    while (getch() != 'q') {}
}
