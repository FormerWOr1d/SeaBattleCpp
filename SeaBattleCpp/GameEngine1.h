#pragma once
#include <random>

using namespace System;
using namespace System::Collections::Generic;

public enum class CellState { Empty, Ship, Hit, Miss };
public enum class GameMode { PvP, PvC_Medium, PvC_Hard };

public ref class GameEngine
{
private:
    static const int SIZE = 10;
    array<CellState, 2>^ player1Board;
    array<CellState, 2>^ player2Board;

    List<int>^ shipLengths;
    List<List<Tuple<int, int>^>^>^ player1Ships;
    List<List<Tuple<int, int>^>^>^ player2Ships;
    List<bool>^ player1Sunk;
    List<bool>^ player2Sunk;

    GameMode mode;
    bool isPlayer1Turn;
    bool gameOver;
    int winner;

    System::Random^ rng;
    List<Tuple<int, int>^>^ availableShots;

    bool CanPlaceShip(array<CellState, 2>^ board, int row, int col, int length, bool horizontal);
    void PlaceShip(array<CellState, 2>^ board, List<List<Tuple<int, int>^>^>^ ships, int row, int col, int length, bool horizontal);
    void MarkAroundSunk(array<CellState, 2>^ board, List<Tuple<int, int>^>^ ship);

public:
    GameEngine(GameMode gm);
    void NewGame();

    bool PlaceShipManual(int player, int row, int col, int length, bool horizontal);
    void AutoPlaceShips(int player);

    bool MakeMove(int player, int row, int col, bool% hit, bool% sunk, int% sunkLength);

    Tuple<int, int>^ GetComputerMove();

    bool IsGameOver() { return gameOver; }
    int GetWinner() { return winner; }
    int GetCurrentPlayer() { return isPlayer1Turn ? 1 : 2; }
    GameMode GetMode() { return mode; }
    array<CellState, 2>^ GetBoard(int player) { return (player == 1) ? player1Board : player2Board; }

    List<int>^ GetShipLengths() { return shipLengths; }
    int GetRemainingShipsCount(int player);

    void ForceGameOver(int player) { gameOver = true; winner = player; }

    void SaveGame(String^ filename) {}
    bool LoadGame(String^ filename) { return false; }
};

// ------------------------- Реализация -------------------------
GameEngine::GameEngine(GameMode gm) : mode(gm), isPlayer1Turn(true), gameOver(false), winner(0), rng(gcnew System::Random())
{
    shipLengths = gcnew List<int>();
    shipLengths->Add(4); shipLengths->Add(3); shipLengths->Add(3); shipLengths->Add(2);
    shipLengths->Add(2); shipLengths->Add(2); shipLengths->Add(1); shipLengths->Add(1);
    shipLengths->Add(1); shipLengths->Add(1);
    NewGame();
}

void GameEngine::NewGame()
{
    player1Board = gcnew array<CellState, 2>(SIZE, SIZE);
    player2Board = gcnew array<CellState, 2>(SIZE, SIZE);
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            player1Board[i, j] = CellState::Empty;
            player2Board[i, j] = CellState::Empty;
        }
    }
    player1Ships = gcnew List<List<Tuple<int, int>^>^>();
    player2Ships = gcnew List<List<Tuple<int, int>^>^>();
    player1Sunk = gcnew List<bool>();
    player2Sunk = gcnew List<bool>();
    for (int i = 0; i < shipLengths->Count; i++) {
        player1Sunk->Add(false);
        player2Sunk->Add(false);
    }
    isPlayer1Turn = true;
    gameOver = false;
    winner = 0;

    availableShots = gcnew List<Tuple<int, int>^>();
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            availableShots->Add(Tuple::Create(i, j));
}

bool GameEngine::CanPlaceShip(array<CellState, 2>^ board, int row, int col, int length, bool horizontal)
{
    if (horizontal && col + length > SIZE) return false;
    if (!horizontal && row + length > SIZE) return false;
    for (int i = 0; i < length; i++) {
        int r = row + (horizontal ? 0 : i);
        int c = col + (horizontal ? i : 0);
        if (board[r, c] != CellState::Empty) return false;
        for (int dr = -1; dr <= 1; dr++)
            for (int dc = -1; dc <= 1; dc++) {
                int nr = r + dr, nc = c + dc;
                if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE && board[nr, nc] == CellState::Ship)
                    return false;
            }
    }
    return true;
}

void GameEngine::PlaceShip(array<CellState, 2>^ board, List<List<Tuple<int, int>^>^>^ ships, int row, int col, int length, bool horizontal)
{
    List<Tuple<int, int>^>^ cells = gcnew List<Tuple<int, int>^>();
    for (int i = 0; i < length; i++) {
        int r = row + (horizontal ? 0 : i);
        int c = col + (horizontal ? i : 0);
        board[r, c] = CellState::Ship;
        cells->Add(Tuple::Create(r, c));
    }
    ships->Add(cells);
}

bool GameEngine::PlaceShipManual(int player, int row, int col, int length, bool horizontal)
{
    auto board = (player == 1) ? player1Board : player2Board;
    auto ships = (player == 1) ? player1Ships : player2Ships;
    if (CanPlaceShip(board, row, col, length, horizontal)) {
        PlaceShip(board, ships, row, col, length, horizontal);
        return true;
    }
    return false;
}

void GameEngine::AutoPlaceShips(int player)
{
    auto board = (player == 1) ? player1Board : player2Board;
    auto ships = (player == 1) ? player1Ships : player2Ships;
    auto sunk = (player == 1) ? player1Sunk : player2Sunk;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i, j] == CellState::Ship)
                board[i, j] = CellState::Empty;
    ships->Clear();
    for (int i = 0; i < sunk->Count; i++) sunk[i] = false;

    for each (int len in shipLengths) {
        bool placed = false;
        while (!placed) {
            int row = rng->Next(SIZE);
            int col = rng->Next(SIZE);
            bool horiz = rng->Next(2) == 0;
            if (CanPlaceShip(board, row, col, len, horiz)) {
                PlaceShip(board, ships, row, col, len, horiz);
                placed = true;
            }
        }
    }
}

void GameEngine::MarkAroundSunk(array<CellState, 2>^ board, List<Tuple<int, int>^>^ ship)
{
    for each (Tuple<int, int> ^ cell in ship) {
        int r = cell->Item1, c = cell->Item2;
        for (int dr = -1; dr <= 1; dr++)
            for (int dc = -1; dc <= 1; dc++) {
                int nr = r + dr, nc = c + dc;
                if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE && board[nr, nc] == CellState::Empty)
                    board[nr, nc] = CellState::Miss;
            }
    }
}

bool GameEngine::MakeMove(int player, int row, int col, bool% hit, bool% sunk, int% sunkLength)
{
    hit = false;
    sunk = false;
    sunkLength = 0;
    if (gameOver) return false;
    if ((player == 1 && !isPlayer1Turn) || (player == 2 && isPlayer1Turn)) return false;

    auto enemyBoard = (player == 1) ? player2Board : player1Board;
    auto enemyShips = (player == 1) ? player2Ships : player1Ships;
    auto enemySunk = (player == 1) ? player2Sunk : player1Sunk;

    if (enemyBoard[row, col] == CellState::Hit || enemyBoard[row, col] == CellState::Miss)
        return false;

    if (enemyBoard[row, col] == CellState::Ship) {
        enemyBoard[row, col] = CellState::Hit;
        hit = true;

        for (int idx = 0; idx < enemyShips->Count; idx++) {
            if (enemySunk[idx]) continue;
            auto ship = enemyShips[idx];
            bool found = false;
            for each (Tuple<int, int> ^ cell in ship) {
                if (cell->Item1 == row && cell->Item2 == col) { found = true; break; }
            }
            if (found) {
                bool allHit = true;
                for each (Tuple<int, int> ^ cell in ship) {
                    if (enemyBoard[cell->Item1, cell->Item2] != CellState::Hit) { allHit = false; break; }
                }
                if (allHit) {
                    sunk = true;
                    sunkLength = ship->Count;
                    enemySunk[idx] = true;
                    MarkAroundSunk(enemyBoard, ship);
                }
                break;
            }
        }
        if (!sunk) return true;
    }
    else {
        enemyBoard[row, col] = CellState::Miss;
        hit = false;
    }

    // Проверка победы
    bool allSunk = true;
    for each (List<Tuple<int, int>^> ^ ship in enemyShips) {
        bool shipSunk = true;
        for each (Tuple<int, int> ^ cell in ship) {
            if (enemyBoard[cell->Item1, cell->Item2] != CellState::Hit) { shipSunk = false; break; }
        }
        if (!shipSunk) { allSunk = false; break; }
    }
    if (allSunk) {
        gameOver = true;
        winner = player;
        return true;
    }

    if (!hit)
        isPlayer1Turn = !isPlayer1Turn;
    return true;
}

int GameEngine::GetRemainingShipsCount(int player)
{
    auto ships = (player == 1) ? player1Ships : player2Ships;
    return shipLengths->Count - ships->Count;
}

Tuple<int,int>^ GameEngine::GetComputerMove()
{
    // Удаляем уже обстрелянные клетки (проходим с конца)
    for (int i = availableShots->Count - 1; i >= 0; i--) {
        Tuple<int,int>^ p = availableShots[i];
        if (player2Board[p->Item1, p->Item2] == CellState::Hit ||
            player2Board[p->Item1, p->Item2] == CellState::Miss) {
            availableShots->RemoveAt(i);
        }
    }
    if (availableShots->Count == 0) return nullptr;
    int idx = rng->Next(availableShots->Count);
    Tuple<int,int>^ move = availableShots[idx];
    availableShots->RemoveAt(idx);
    return move;
}