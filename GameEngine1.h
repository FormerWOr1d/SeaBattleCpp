#ifndef GAMEENGINE1_H
#define GAMEENGINE1_H

#include "GameMode.h"
using namespace SeaBattleCpp;

using namespace System;
using namespace System::Collections::Generic;

public enum class CellState { Empty, Ship, Hit, Miss };

public ref class GameEngine
{
private:
    static const int SIZE = 10;
    array<CellState>^ player1Board;
    array<CellState>^ player2Board;

private:
    static array<String^>^ SplitNonEmpty(String^ line);

    List<String^>^ gameLog;
    List<List<System::ValueTuple<int, int>>^>^ player1Ships;
    List<List<System::ValueTuple<int, int>>^>^ player2Ships;
    List<bool>^ player1Sunk;
    List<bool>^ player2Sunk;

    List<int>^ remainingLengths1;
    List<int>^ remainingLengths2;

    GameMode mode;
    bool isPlayer1Turn;
    bool gameOver;
    int winner;

    Random^ rng;

    List<System::ValueTuple<int, int>>^ smartTargetQueue;
    List<System::ValueTuple<int, int>>^ smartAvailableShots;

    bool CanPlaceShip(array<CellState>^ board, int row, int col, int length, bool horizontal);
    void PlaceShip(array<CellState>^ board, List<List<System::ValueTuple<int, int>>^>^ ships, int row, int col, int length, bool horizontal);
    void MarkAroundSunk(array<CellState>^ board, List<System::ValueTuple<int, int>>^ ship);
    int GetIndex(int row, int col) { return row * SIZE + col; }

    List<int>^ GetRemainingList(int player) { return (player == 1) ? remainingLengths1 : remainingLengths2; }

public:
    GameEngine(GameMode gm);
    void NewGame();

    void AddLogEntry(int player, int row, int col, String^ result);
    void SaveStatistics(String^ filename);

    bool PlaceShipManual(int player, int row, int col, int length, bool horizontal);
    void AutoPlaceShips(int player);

    bool MakeMove(int player, int row, int col, bool% hit, bool% sunk, int% sunkLength);

    System::ValueTuple<int, int> GetRandomComputerMove();
    System::ValueTuple<int, int> GetSmartComputerMove();
    void NotifySmartResult(int row, int col, bool hit, bool sunk);

    bool IsGameOver() { return gameOver; }
    int GetWinner() { return winner; }
    int GetCurrentPlayer() { return isPlayer1Turn ? 1 : 2; }
    GameMode GetMode() { return mode; }
    array<CellState>^ GetBoard(int player) { return (player == 1) ? player1Board : player2Board; }

    int GetRemainingShipsCount(int player) { return GetRemainingList(player)->Count; }
    int GetRemainingLength(int player, int index) { return GetRemainingList(player)[index]; }
    void RemoveRemainingLengthAt(int player, int index) { GetRemainingList(player)->RemoveAt(index); }
    void AddRemainingLength(int player, int length) { GetRemainingList(player)->Add(length); }

    void RemoveLastShip(int player);
    bool RemoveShipAt(int player, int row, int col);

    void ForceGameOver(int player) { gameOver = true; winner = player; }

    void SaveGame(String^ filename);
    bool LoadGame(String^ filename);
};

#endif 