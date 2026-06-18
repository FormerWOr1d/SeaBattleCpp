#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <algorithm>
#include <ctime>

using namespace System;
using namespace System::Collections::Generic;

public enum class CellState { Empty, Ship, Hit, Miss };
public enum class GameMode { PvP, PvC_Medium, PvC_Hard };

public ref class GameEngine
{
private:
    static const int SIZE = 10;
    array<array<CellState, SIZE>, SIZE>^ player1Board;
    array<array<CellState, SIZE>, SIZE>^ player2Board;

    std::vector<int> shipLengths;
    std::vector<std::vector<std::pair<int, int>>> player1Ships;
    std::vector<std::vector<std::pair<int, int>>> player2Ships;
    std::vector<bool> player1Sunk;
    std::vector<bool> player2Sunk;

    GameMode mode;
    bool isPlayer1Turn;
    bool gameOver;
    int winner;

    std::mt19937 rng;
    std::vector<std::pair<int, int>> randomAvailableShots;
    std::vector<std::pair<int, int>> smartAvailableShots;
    std::vector<std::pair<int, int>> smartTargetQueue;

    bool CanPlaceShip(array<array<CellState, SIZE>, SIZE>^ board, int row, int col, int length, bool horizontal);
    void PlaceShip(array<array<CellState, SIZE>, SIZE>^ board, std::vector<std::vector<std::pair<int, int>>>& ships, int row, int col, int length, bool horizontal);
    void MarkAroundSunk(array<array<CellState, SIZE>, SIZE>^ board, const std::vector<std::pair<int, int>>& ship);

public:
    GameEngine(GameMode gm);
    void NewGame();

    bool PlaceShipManual(int player, int row, int col, int length, bool horizontal);
    void AutoPlaceShips(int player);

    bool MakeMove(int player, int row, int col, bool% hit, bool% sunk, int% sunkLength);

    std::pair<int, int> GetRandomComputerMove();
    std::pair<int, int> GetSmartComputerMove();
    void NotifySmartResult(int row, int col, bool hit, bool sunk);

    bool IsGameOver() { return gameOver; }
    int GetWinner() { return winner; }
    int GetCurrentPlayer() { return isPlayer1Turn ? 1 : 2; }
    GameMode GetMode() { return mode; }
    array<array<CellState, SIZE>, SIZE>^ GetBoard(int player) { return (player == 1) ? player1Board : player2Board; }

    std::vector<int> GetShipLengths() { return shipLengths; }
    int GetRemainingShipsCount(int player);

    void ForceGameOver(int player) { gameOver = true; winner = player; }

    void SaveGame(String^ filename);
    bool LoadGame(String^ filename);
};

// ------------------------- Реализация -------------------------
GameEngine::GameEngine(GameMode gm) : mode(gm), isPlayer1Turn(true), gameOver(false), winner(0), rng(std::time(0))
{
    shipLengths = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
    NewGame();
}

void GameEngine::NewGame()
{
    player1Board = gcnew array<array<CellState, SIZE>, SIZE>(SIZE);
    player2Board = gcnew array<array<CellState, SIZE>, SIZE>(SIZE);
    for (int i = 0; i < SIZE; i++) {
        player1Board[i] = gcnew array<CellState, SIZE>(SIZE);
        player2Board[i] = gcnew array<CellState, SIZE>(SIZE);
        for (int j = 0; j < SIZE; j++) {
            player1Board[i][j] = CellState::Empty;
            player2Board[i][j] = CellState::Empty;
        }
    }
    player1Ships.clear();
    player2Ships.clear();
    player1Sunk.assign(shipLengths.size(), false);
    player2Sunk.assign(shipLengths.size(), false);
    isPlayer1Turn = true;
    gameOver = false;
    winner = 0;

    randomAvailableShots.clear();
    smartAvailableShots.clear();
    smartTargetQueue.clear();
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) {
            randomAvailableShots.push_back({ i, j });
            smartAvailableShots.push_back({ i, j });
        }
}

bool GameEngine::CanPlaceShip(array<array<CellState, SIZE>, SIZE>^ board, int row, int col, int length, bool horizontal)
{
    if (horizontal && col + length > SIZE) return false;
    if (!horizontal && row + length > SIZE) return false;
    for (int i = 0; i < length; i++) {
        int r = row + (horizontal ? 0 : i);
        int c = col + (horizontal ? i : 0);
        if (board[r][c] != CellState::Empty) return false;
        for (int dr = -1; dr <= 1; dr++)
            for (int dc = -1; dc <= 1; dc++) {
                int nr = r + dr, nc = c + dc;
                if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE && board[nr][nc] == CellState::Ship)
                    return false;
            }
    }
    return true;
}

void GameEngine::PlaceShip(array<array<CellState, SIZE>, SIZE>^ board, std::vector<std::vector<std::pair<int, int>>>& ships, int row, int col, int length, bool horizontal)
{
    std::vector<std::pair<int, int>> cells;
    for (int i = 0; i < length; i++) {
        int r = row + (horizontal ? 0 : i);
        int c = col + (horizontal ? i : 0);
        board[r][c] = CellState::Ship;
        cells.push_back({ r, c });
    }
    ships.push_back(cells);
}

bool GameEngine::PlaceShipManual(int player, int row, int col, int length, bool horizontal)
{
    auto board = (player == 1) ? player1Board : player2Board;
    auto& ships = (player == 1) ? player1Ships : player2Ships;
    if (CanPlaceShip(board, row, col, length, horizontal)) {
        PlaceShip(board, ships, row, col, length, horizontal);
        return true;
    }
    return false;
}

void GameEngine::AutoPlaceShips(int player)
{
    auto board = (player == 1) ? player1Board : player2Board;
    auto& ships = (player == 1) ? player1Ships : player2Ships;
    auto& sunk = (player == 1) ? player1Sunk : player2Sunk;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == CellState::Ship)
                board[i][j] = CellState::Empty;
    ships.clear();
    for (size_t i = 0; i < sunk.size(); i++) sunk[i] = false;

    for (int len : shipLengths) {
        bool placed = false;
        while (!placed) {
            int row = std::uniform_int_distribution<>(0, SIZE - 1)(rng);
            int col = std::uniform_int_distribution<>(0, SIZE - 1)(rng);
            bool horiz = std::uniform_int_distribution<>(0, 1)(rng) == 0;
            if (CanPlaceShip(board, row, col, len, horiz)) {
                PlaceShip(board, ships, row, col, len, horiz);
                placed = true;
            }
        }
    }
}

void GameEngine::MarkAroundSunk(array<array<CellState, SIZE>, SIZE>^ board, const std::vector<std::pair<int, int>>& ship)
{
    for (auto [r, c] : ship) {
        for (int dr = -1; dr <= 1; dr++)
            for (int dc = -1; dc <= 1; dc++) {
                int nr = r + dr, nc = c + dc;
                if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE && board[nr][nc] == CellState::Empty)
                    board[nr][nc] = CellState::Miss;
            }
    }
}

bool GameEngine::MakeMove(int player, int row, int col, bool% hit, bool% sunk, int% sunkLength)
{
    hit = sunk = false;
    sunkLength = 0;
    if (gameOver) return false;
    if ((player == 1 && !isPlayer1Turn) || (player == 2 && isPlayer1Turn)) return false;

    auto enemyBoard = (player == 1) ? player2Board : player1Board;
    auto& enemyShips = (player == 1) ? player2Ships : player1Ships;
    auto& enemySunk = (player == 1) ? player2Sunk : player1Sunk;

    if (enemyBoard[row][col] == CellState::Hit || enemyBoard[row][col] == CellState::Miss)
        return false;

    if (enemyBoard[row][col] == CellState::Ship) {
        enemyBoard[row][col] = CellState::Hit;
        hit = true;

        for (size_t idx = 0; idx < enemyShips.size(); idx++) {
            if (enemySunk[idx]) continue;
            auto it = std::find(enemyShips[idx].begin(), enemyShips[idx].end(), std::make_pair(row, col));
            if (it != enemyShips[idx].end()) {
                bool allHit = true;
                for (auto [r, c] : enemyShips[idx])
                    if (enemyBoard[r][c] != CellState::Hit) { allHit = false; break; }
                if (allHit) {
                    sunk = true;
                    sunkLength = enemyShips[idx].size();
                    enemySunk[idx] = true;
                    MarkAroundSunk(enemyBoard, enemyShips[idx]);
                }
                break;
            }
        }
        if (!sunk) return true;
    }
    else {
        enemyBoard[row][col] = CellState::Miss;
        hit = false;
    }

    bool allSunk = true;
    for (auto& ship : enemyShips) {
        bool shipSunk = true;
        for (auto [r, c] : ship)
            if (enemyBoard[r][c] != CellState::Hit) { shipSunk = false; break; }
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
    auto& ships = (player == 1) ? player1Ships : player2Ships;
    return shipLengths.size() - ships.size();
}

std::pair<int, int> GameEngine::GetRandomComputerMove()
{
    randomAvailableShots.erase(
        std::remove_if(randomAvailableShots.begin(), randomAvailableShots.end(),
            [this](const std::pair<int, int>& cell) {
                return player2Board[cell.first][cell.second] == CellState::Hit ||
                    player2Board[cell.first][cell.second] == CellState::Miss;
            }),
        randomAvailableShots.end()
    );
    if (randomAvailableShots.empty()) return { -1, -1 };
    std::uniform_int_distribution<> dist(0, randomAvailableShots.size() - 1);
    int idx = dist(rng);
    auto move = randomAvailableShots[idx];
    randomAvailableShots.erase(randomAvailableShots.begin() + idx);
    return move;
}

std::pair<int, int> GameEngine::GetSmartComputerMove()
{
    smartAvailableShots.erase(
        std::remove_if(smartAvailableShots.begin(), smartAvailableShots.end(),
            [this](const std::pair<int, int>& cell) {
                return player2Board[cell.first][cell.second] == CellState::Hit ||
                    player2Board[cell.first][cell.second] == CellState::Miss;
            }),
        smartAvailableShots.end()
    );
    smartTargetQueue.erase(
        std::remove_if(smartTargetQueue.begin(), smartTargetQueue.end(),
            [this](const std::pair<int, int>& cell) {
                return player2Board[cell.first][cell.second] == CellState::Hit ||
                    player2Board[cell.first][cell.second] == CellState::Miss;
            }),
        smartTargetQueue.end()
    );

    if (!smartTargetQueue.empty()) {
        auto move = smartTargetQueue.front();
        smartTargetQueue.erase(smartTargetQueue.begin());
        auto it = std::find(smartAvailableShots.begin(), smartAvailableShots.end(), move);
        if (it != smartAvailableShots.end()) smartAvailableShots.erase(it);
        return move;
    }
    if (smartAvailableShots.empty()) return { -1, -1 };
    std::uniform_int_distribution<> dist(0, smartAvailableShots.size() - 1);
    int idx = dist(rng);
    auto shot = smartAvailableShots[idx];
    smartAvailableShots.erase(smartAvailableShots.begin() + idx);
    return shot;
}

void GameEngine::NotifySmartResult(int row, int col, bool hit, bool sunk)
{
    if (hit && !sunk) {
        for (auto [dr, dc] : std::vector<std::pair<int, int>>{ {-1,0},{1,0},{0,-1},{0,1} }) {
            int nr = row + dr, nc = col + dc;
            if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE) {
                if (player2Board[nr][nc] == CellState::Empty) {
                    if (std::find(smartTargetQueue.begin(), smartTargetQueue.end(), std::make_pair(nr, nc)) == smartTargetQueue.end())
                        smartTargetQueue.push_back({ nr, nc });
                }
            }
        }
    }
    else if (hit && sunk) {
        smartTargetQueue.clear();
    }
}

void GameEngine::SaveGame(String^ filename) { /* заглушка */ }
bool GameEngine::LoadGame(String^ filename) { return false; }