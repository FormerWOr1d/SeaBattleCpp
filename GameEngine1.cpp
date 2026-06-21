#include "GameEngine1.h"
#include <random>
#include <ctime>

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace SeaBattleCpp;

GameEngine::GameEngine(GameMode gm) : mode(gm), isPlayer1Turn(true), gameOver(false), winner(0)
{
    rng = gcnew Random();
    gameLog = gcnew List<String^>();
    NewGame();
}

void GameEngine::NewGame()
{
    player1Board = gcnew array<CellState>(SIZE * SIZE);
    player2Board = gcnew array<CellState>(SIZE * SIZE);
    for (int i = 0; i < SIZE * SIZE; i++) {
        player1Board[i] = CellState::Empty;
        player2Board[i] = CellState::Empty;
    }

    player1Ships = gcnew List<List<System::ValueTuple<int, int>>^>();
    player2Ships = gcnew List<List<System::ValueTuple<int, int>>^>();
    player1Sunk = gcnew List<bool>();
    player2Sunk = gcnew List<bool>();

    smartTargetQueue = gcnew List<System::ValueTuple<int, int>>();
    smartAvailableShots = gcnew List<System::ValueTuple<int, int>>();
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            smartAvailableShots->Add(System::ValueTuple<int, int>(i, j));

    remainingLengths1 = gcnew List<int>();
    remainingLengths1->Add(4);
    remainingLengths1->Add(3);
    remainingLengths1->Add(3);
    remainingLengths1->Add(2);
    remainingLengths1->Add(2);
    remainingLengths1->Add(2);
    remainingLengths1->Add(1);
    remainingLengths1->Add(1);
    remainingLengths1->Add(1);
    remainingLengths1->Add(1);

    remainingLengths2 = gcnew List<int>();
    remainingLengths2->Add(4);
    remainingLengths2->Add(3);
    remainingLengths2->Add(3);
    remainingLengths2->Add(2);
    remainingLengths2->Add(2);
    remainingLengths2->Add(2);
    remainingLengths2->Add(1);
    remainingLengths2->Add(1);
    remainingLengths2->Add(1);
    remainingLengths2->Add(1);

    isPlayer1Turn = true;
    gameOver = false;
    winner = 0;

    gameLog->Clear();
}

array<String^>^ GameEngine::SplitNonEmpty(String^ line)
{
    array<String^>^ parts = line->Split(' ');
    List<String^>^ list = gcnew List<String^>();
    for each (String ^ part in parts) {
        if (part->Length > 0)
            list->Add(part);
    }
    return list->ToArray();
}

bool GameEngine::CanPlaceShip(array<CellState>^ board, int row, int col, int length, bool horizontal)
{
    if (horizontal && col + length > SIZE) return false;
    if (!horizontal && row + length > SIZE) return false;
    for (int i = 0; i < length; i++) {
        int r = row + (horizontal ? 0 : i);
        int c = col + (horizontal ? i : 0);
        int idx = GetIndex(r, c);
        if (board[idx] != CellState::Empty) return false;
        for (int dr = -1; dr <= 1; dr++)
            for (int dc = -1; dc <= 1; dc++) {
                int nr = r + dr, nc = c + dc;
                if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE) {
                    int nidx = GetIndex(nr, nc);
                    if (board[nidx] == CellState::Ship) return false;
                }
            }
    }
    return true;
}

void GameEngine::PlaceShip(array<CellState>^ board, List<List<System::ValueTuple<int, int>>^>^ ships, int row, int col, int length, bool horizontal)
{
    List<System::ValueTuple<int, int>>^ cells = gcnew List<System::ValueTuple<int, int>>();
    for (int i = 0; i < length; i++) {
        int r = row + (horizontal ? 0 : i);
        int c = col + (horizontal ? i : 0);
        board[GetIndex(r, c)] = CellState::Ship;
        cells->Add(System::ValueTuple<int, int>(r, c));
    }
    ships->Add(cells);
}

bool GameEngine::PlaceShipManual(int player, int row, int col, int length, bool horizontal)
{
    auto board = (player == 1) ? player1Board : player2Board;
    auto ships = (player == 1) ? player1Ships : player2Ships;
    auto sunk = (player == 1) ? player1Sunk : player2Sunk;
    if (CanPlaceShip(board, row, col, length, horizontal)) {
        PlaceShip(board, ships, row, col, length, horizontal);
        sunk->Add(false);
        return true;
    }
    return false;
}

void GameEngine::AutoPlaceShips(int player)
{
    auto board = (player == 1) ? player1Board : player2Board;
    auto ships = (player == 1) ? player1Ships : player2Ships;
    auto sunk = (player == 1) ? player1Sunk : player2Sunk;
    auto remaining = GetRemainingList(player);

    for (int i = 0; i < SIZE * SIZE; i++)
        if (board[i] == CellState::Ship)
            board[i] = CellState::Empty;
    ships->Clear();
    sunk->Clear();

    remaining->Clear();
    remaining->Add(4);
    remaining->Add(3);
    remaining->Add(3);
    remaining->Add(2);
    remaining->Add(2);
    remaining->Add(2);
    remaining->Add(1);
    remaining->Add(1);
    remaining->Add(1);
    remaining->Add(1);

    while (remaining->Count > 0) {
        int len = remaining[0];
        bool placed = false;
        while (!placed) {
            int row = rng->Next(0, SIZE);
            int col = rng->Next(0, SIZE);
            bool horiz = rng->Next(0, 2) == 0;
            if (CanPlaceShip(board, row, col, len, horiz)) {
                PlaceShip(board, ships, row, col, len, horiz);
                placed = true;
            }
        }
        remaining->RemoveAt(0);
    }

    for (int i = 0; i < ships->Count; i++)
        sunk->Add(false);
}

void GameEngine::MarkAroundSunk(array<CellState>^ board, List<System::ValueTuple<int, int>>^ ship)
{
    for each (auto cell in ship) {
        for (int dr = -1; dr <= 1; dr++)
            for (int dc = -1; dc <= 1; dc++) {
                int nr = cell.Item1 + dr, nc = cell.Item2 + dc;
                if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE) {
                    int idx = GetIndex(nr, nc);
                    if (board[idx] == CellState::Empty)
                        board[idx] = CellState::Miss;
                }
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
    auto enemyShips = (player == 1) ? player2Ships : player1Ships;
    auto enemySunk = (player == 1) ? player2Sunk : player1Sunk;

    int idx = GetIndex(row, col);
    if (enemyBoard[idx] == CellState::Hit || enemyBoard[idx] == CellState::Miss)
        return false;

    bool result = false; // для логирования

    if (enemyBoard[idx] == CellState::Ship) {
        enemyBoard[idx] = CellState::Hit;
        hit = true;

        for (int i = 0; i < enemyShips->Count; i++) {
            if (enemySunk[i]) continue;
            List<System::ValueTuple<int, int>>^ ship = enemyShips[i];
            bool found = false;
            for each (auto cell in ship) {
                if (cell.Item1 == row && cell.Item2 == col) { found = true; break; }
            }
            if (found) {
                bool allHit = true;
                for each (auto cell in ship) {
                    if (enemyBoard[GetIndex(cell.Item1, cell.Item2)] != CellState::Hit) {
                        allHit = false;
                        break;
                    }
                }
                if (allHit) {
                    sunk = true;
                    sunkLength = ship->Count;
                    enemySunk[i] = true;
                    MarkAroundSunk(enemyBoard, ship);
                }
                break;
            }
        }
        // Логируем попадание (с потоплением или без)
        if (sunk) {
            AddLogEntry(player, row, col, "Потоплен корабль длины " + sunkLength.ToString());
        }
        else {
            AddLogEntry(player, row, col, "Попадание");
        }
    }
    else {
        enemyBoard[idx] = CellState::Miss;
        hit = false;
        AddLogEntry(player, row, col, "Промах");
    }

    // Проверяем, потоплены ли все корабли противника
    bool allSunk = true;
    for (int i = 0; i < enemyShips->Count; i++) {
        if (enemySunk[i]) continue;
        List<System::ValueTuple<int, int>>^ ship = enemyShips[i];
        bool shipSunk = true;
        for each (auto cell in ship) {
            if (enemyBoard[GetIndex(cell.Item1, cell.Item2)] != CellState::Hit) {
                shipSunk = false;
                break;
            }
        }
        if (!shipSunk) { allSunk = false; break; }
    }
    if (allSunk) {
        gameOver = true;
        winner = player;
        AddLogEntry(player, row, col, "Победа! Все корабли потоплены");
        return true;
    }

    // Если промах – переключить ход
    if (!hit)
        isPlayer1Turn = !isPlayer1Turn;

    return true;
}

void GameEngine::RemoveLastShip(int player)
{
    auto ships = (player == 1) ? player1Ships : player2Ships;
    auto board = (player == 1) ? player1Board : player2Board;
    auto sunk = (player == 1) ? player1Sunk : player2Sunk;
    if (ships->Count == 0) return;
    List<System::ValueTuple<int, int>>^ lastShip = ships[ships->Count - 1];
    for each (auto cell in lastShip) {
        board[GetIndex(cell.Item1, cell.Item2)] = CellState::Empty;
    }
    ships->RemoveAt(ships->Count - 1);
    if (sunk->Count > 0) sunk->RemoveAt(sunk->Count - 1);
}

bool GameEngine::RemoveShipAt(int player, int row, int col)
{
    auto ships = (player == 1) ? player1Ships : player2Ships;
    auto board = (player == 1) ? player1Board : player2Board;
    auto remaining = GetRemainingList(player);
    int idx = GetIndex(row, col);
    if (board[idx] != CellState::Ship) return false;

    for (int i = 0; i < ships->Count; i++) {
        List<System::ValueTuple<int, int>>^ ship = ships[i];
        for each (auto cell in ship) {
            if (cell.Item1 == row && cell.Item2 == col) {
                int length = ship->Count;
                for each (auto c in ship) {
                    board[GetIndex(c.Item1, c.Item2)] = CellState::Empty;
                }
                ships->RemoveAt(i);
                auto sunk = (player == 1) ? player1Sunk : player2Sunk;
                if (sunk->Count > i) sunk->RemoveAt(i);
                remaining->Add(length);
                return true;
            }
        }
    }
    return false;
}

System::ValueTuple<int, int> GameEngine::GetRandomComputerMove()
{
    for (int i = smartAvailableShots->Count - 1; i >= 0; i--) {
        int r = smartAvailableShots[i].Item1;
        int c = smartAvailableShots[i].Item2;
        if (player1Board[GetIndex(r, c)] == CellState::Hit ||
            player1Board[GetIndex(r, c)] == CellState::Miss)
            smartAvailableShots->RemoveAt(i);
    }
    if (smartAvailableShots->Count == 0) return System::ValueTuple<int, int>(-1, -1);
    int idx = rng->Next(0, smartAvailableShots->Count);
    auto move = smartAvailableShots[idx];
    smartAvailableShots->RemoveAt(idx);
    return move;
}

System::ValueTuple<int, int> GameEngine::GetSmartComputerMove()
{
    for (int i = smartAvailableShots->Count - 1; i >= 0; i--) {
        int r = smartAvailableShots[i].Item1;
        int c = smartAvailableShots[i].Item2;
        if (player1Board[GetIndex(r, c)] == CellState::Hit ||
            player1Board[GetIndex(r, c)] == CellState::Miss)
            smartAvailableShots->RemoveAt(i);
    }
    for (int i = smartTargetQueue->Count - 1; i >= 0; i--) {
        int r = smartTargetQueue[i].Item1;
        int c = smartTargetQueue[i].Item2;
        if (player1Board[GetIndex(r, c)] == CellState::Hit ||
            player1Board[GetIndex(r, c)] == CellState::Miss)
            smartTargetQueue->RemoveAt(i);
    }

    if (smartTargetQueue->Count > 0) {
        auto move = smartTargetQueue[0];
        smartTargetQueue->RemoveAt(0);
        for (int i = smartAvailableShots->Count - 1; i >= 0; i--) {
            if (smartAvailableShots[i].Item1 == move.Item1 && smartAvailableShots[i].Item2 == move.Item2) {
                smartAvailableShots->RemoveAt(i);
                break;
            }
        }
        return move;
    }
    if (smartAvailableShots->Count == 0) return System::ValueTuple<int, int>(-1, -1);
    int idx = rng->Next(0, smartAvailableShots->Count);
    auto shot = smartAvailableShots[idx];
    smartAvailableShots->RemoveAt(idx);
    return shot;
}

void GameEngine::NotifySmartResult(int row, int col, bool hit, bool sunk)
{
    if (hit && !sunk) {
        bool upHit = (row > 0 && player1Board[GetIndex(row - 1, col)] == CellState::Hit);
        bool downHit = (row < SIZE - 1 && player1Board[GetIndex(row + 1, col)] == CellState::Hit);
        bool leftHit = (col > 0 && player1Board[GetIndex(row, col - 1)] == CellState::Hit);
        bool rightHit = (col < SIZE - 1 && player1Board[GetIndex(row, col + 1)] == CellState::Hit);

        List<ValueTuple<int, int>>^ candidates = gcnew List<ValueTuple<int, int>>();

        if (upHit || downHit) {
            if (row > 0) {
                CellState state = player1Board[GetIndex(row - 1, col)];
                if (state != CellState::Hit && state != CellState::Miss)
                    candidates->Add(ValueTuple<int, int>(row - 1, col));
            }
            if (row < SIZE - 1) {
                CellState state = player1Board[GetIndex(row + 1, col)];
                if (state != CellState::Hit && state != CellState::Miss)
                    candidates->Add(ValueTuple<int, int>(row + 1, col));
            }
        }
        else if (leftHit || rightHit) {
            if (col > 0) {
                CellState state = player1Board[GetIndex(row, col - 1)];
                if (state != CellState::Hit && state != CellState::Miss)
                    candidates->Add(ValueTuple<int, int>(row, col - 1));
            }
            if (col < SIZE - 1) {
                CellState state = player1Board[GetIndex(row, col + 1)];
                if (state != CellState::Hit && state != CellState::Miss)
                    candidates->Add(ValueTuple<int, int>(row, col + 1));
            }
        }
        else {
            array<ValueTuple<int, int>>^ dirs = {
                ValueTuple<int,int>(-1,0),
                ValueTuple<int,int>(1,0),
                ValueTuple<int,int>(0,-1),
                ValueTuple<int,int>(0,1)
            };
            for each (auto delta in dirs) {
                int nr = row + delta.Item1;
                int nc = col + delta.Item2;
                if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE) {
                    CellState state = player1Board[GetIndex(nr, nc)];
                    if (state != CellState::Hit && state != CellState::Miss)
                        candidates->Add(ValueTuple<int, int>(nr, nc));
                }
            }
        }

        for each (auto cell in candidates) {
            bool exists = false;
            for each (auto c in smartTargetQueue) {
                if (c.Item1 == cell.Item1 && c.Item2 == cell.Item2) {
                    exists = true;
                    break;
                }
            }
            if (!exists)
                smartTargetQueue->Add(cell);
        }
    }
    else if (hit && sunk) {
        smartTargetQueue->Clear();
    }
}

void GameEngine::SaveGame(String^ filename)
{
    try {
        StreamWriter^ sw = gcnew StreamWriter(filename);
        sw->WriteLine(((int)mode).ToString());
        sw->WriteLine(isPlayer1Turn.ToString());
        sw->WriteLine(gameOver.ToString());
        sw->WriteLine(winner.ToString());

        sw->WriteLine(player1Ships->Count.ToString());
        for each (auto ship in player1Ships) {
            sw->Write(ship->Count.ToString());
            for each (auto cell in ship) {
                sw->Write(" " + cell.Item1.ToString() + " " + cell.Item2.ToString());
            }
            sw->WriteLine();
        }
        for each (bool b in player1Sunk) {
            sw->Write(b ? "1 " : "0 ");
        }
        sw->WriteLine();
        for each (int val in remainingLengths1) {
            sw->Write(val.ToString() + " ");
        }
        sw->WriteLine();

        sw->WriteLine(player2Ships->Count.ToString());
        for each (auto ship in player2Ships) {
            sw->Write(ship->Count.ToString());
            for each (auto cell in ship) {
                sw->Write(" " + cell.Item1.ToString() + " " + cell.Item2.ToString());
            }
            sw->WriteLine();
        }
        for each (bool b in player2Sunk) {
            sw->Write(b ? "1 " : "0 ");
        }
        sw->WriteLine();
        for each (int val in remainingLengths2) {
            sw->Write(val.ToString() + " ");
        }
        sw->WriteLine();

        for (int i = 0; i < SIZE * SIZE; i++) {
            sw->Write(((int)player1Board[i]).ToString() + " ");
        }
        sw->WriteLine();
        for (int i = 0; i < SIZE * SIZE; i++) {
            sw->Write(((int)player2Board[i]).ToString() + " ");
        }
        sw->WriteLine();

        sw->WriteLine(smartAvailableShots->Count.ToString());
        for each (auto cell in smartAvailableShots) {
            sw->WriteLine(cell.Item1.ToString() + " " + cell.Item2.ToString());
        }
        sw->WriteLine(smartTargetQueue->Count.ToString());
        for each (auto cell in smartTargetQueue) {
            sw->WriteLine(cell.Item1.ToString() + " " + cell.Item2.ToString());
        }

        sw->Close();
        delete sw;
    }
    catch (Exception^ ex) {
        System::Windows::Forms::MessageBox::Show("Ошибка сохранения: " + ex->Message, "Ошибка");
    }
}

bool GameEngine::LoadGame(String^ filename)
{
    try {
        StreamReader^ sr = gcnew StreamReader(filename);

        int modeInt = Int32::Parse(sr->ReadLine());
        mode = (GameMode)modeInt;
        isPlayer1Turn = Boolean::Parse(sr->ReadLine());
        gameOver = Boolean::Parse(sr->ReadLine());
        winner = Int32::Parse(sr->ReadLine());

        player1Ships->Clear();
        player1Sunk->Clear();
        remainingLengths1->Clear();
        player2Ships->Clear();
        player2Sunk->Clear();
        remainingLengths2->Clear();

        int count1 = Int32::Parse(sr->ReadLine());
        for (int i = 0; i < count1; i++) {
            String^ line = sr->ReadLine();
            array<String^>^ parts = SplitNonEmpty(line);
            if (parts->Length == 0) continue;
            int length = Int32::Parse(parts[0]);
            List<System::ValueTuple<int, int>>^ ship = gcnew List<System::ValueTuple<int, int>>();
            for (int j = 0; j < length; j++) {
                int row = Int32::Parse(parts[1 + j * 2]);
                int col = Int32::Parse(parts[2 + j * 2]);
                ship->Add(System::ValueTuple<int, int>(row, col));
            }
            player1Ships->Add(ship);
        }
        array<String^>^ sunkParts1 = SplitNonEmpty(sr->ReadLine());
        for each (String ^ s in sunkParts1) {
            player1Sunk->Add(s == "1");
        }
        array<String^>^ remParts1 = SplitNonEmpty(sr->ReadLine());
        for each (String ^ s in remParts1) {
            remainingLengths1->Add(Int32::Parse(s));
        }

        int count2 = Int32::Parse(sr->ReadLine());
        for (int i = 0; i < count2; i++) {
            String^ line = sr->ReadLine();
            array<String^>^ parts = SplitNonEmpty(line);
            if (parts->Length == 0) continue;
            int length = Int32::Parse(parts[0]);
            List<System::ValueTuple<int, int>>^ ship = gcnew List<System::ValueTuple<int, int>>();
            for (int j = 0; j < length; j++) {
                int row = Int32::Parse(parts[1 + j * 2]);
                int col = Int32::Parse(parts[2 + j * 2]);
                ship->Add(System::ValueTuple<int, int>(row, col));
            }
            player2Ships->Add(ship);
        }
        array<String^>^ sunkParts2 = SplitNonEmpty(sr->ReadLine());
        for each (String ^ s in sunkParts2) {
            player2Sunk->Add(s == "1");
        }
        array<String^>^ remParts2 = SplitNonEmpty(sr->ReadLine());
        for each (String ^ s in remParts2) {
            remainingLengths2->Add(Int32::Parse(s));
        }

        String^ boardLine1 = sr->ReadLine();
        array<String^>^ board1 = SplitNonEmpty(boardLine1);
        if (board1->Length != SIZE * SIZE) {
            System::Windows::Forms::MessageBox::Show("Ошибка: доска 1 содержит " + board1->Length + " элементов, ожидается " + (SIZE * SIZE), "Ошибка");
            return false;
        }
        for (int i = 0; i < SIZE * SIZE; i++) {
            player1Board[i] = (CellState)Int32::Parse(board1[i]);
        }

        String^ boardLine2 = sr->ReadLine();
        array<String^>^ board2 = SplitNonEmpty(boardLine2);
        if (board2->Length != SIZE * SIZE) {
            System::Windows::Forms::MessageBox::Show("Ошибка: доска 2 содержит " + board2->Length + " элементов, ожидается " + (SIZE * SIZE), "Ошибка");
            return false;
        }
        for (int i = 0; i < SIZE * SIZE; i++) {
            player2Board[i] = (CellState)Int32::Parse(board2[i]);
        }

        smartAvailableShots->Clear();
        int availCount = Int32::Parse(sr->ReadLine());
        for (int i = 0; i < availCount; i++) {
            array<String^>^ coords = SplitNonEmpty(sr->ReadLine());
            int r = Int32::Parse(coords[0]);
            int c = Int32::Parse(coords[1]);
            smartAvailableShots->Add(System::ValueTuple<int, int>(r, c));
        }

        smartTargetQueue->Clear();
        int targetCount = Int32::Parse(sr->ReadLine());
        for (int i = 0; i < targetCount; i++) {
            array<String^>^ coords = SplitNonEmpty(sr->ReadLine());
            int r = Int32::Parse(coords[0]);
            int c = Int32::Parse(coords[1]);
            smartTargetQueue->Add(System::ValueTuple<int, int>(r, c));
        }

        sr->Close();
        delete sr;
        return true;
    }
    catch (Exception^ ex) {
        System::Windows::Forms::MessageBox::Show("Ошибка загрузки: " + ex->Message + "\n\n" + ex->StackTrace, "Ошибка");
        return false;
    }
}

// ==================== ЛОГИРОВАНИЕ СТАТИСТИКИ ====================

void GameEngine::AddLogEntry(int player, int row, int col, String^ result)
{
    String^ entry = String::Format("Игрок {0}: ({1},{2}) - {3}", player, row, col, result);
    gameLog->Add(entry);
}

void GameEngine::SaveStatistics(String^ filename)
{
    try {
        StreamWriter^ sw = gcnew StreamWriter(filename);
        sw->WriteLine("=== СТАТИСТИКА ИГРЫ ===");
        sw->WriteLine("Дата и время: " + DateTime::Now.ToString());
        sw->WriteLine("Режим: " + mode.ToString());
        sw->WriteLine("Победитель: Игрок " + winner.ToString());
        sw->WriteLine();
        sw->WriteLine("--- ЛОГ ХОДОВ ---");
        for each (String ^ line in gameLog) {
            sw->WriteLine(line);
        }
        sw->WriteLine();
        sw->WriteLine("--- ИТОГИ ---");
        int shots1 = 0, shots2 = 0;
        int hits1 = 0, hits2 = 0;
        int misses1 = 0, misses2 = 0;
        int sunk1 = 0, sunk2 = 0;
        for each (String ^ line in gameLog) {
            if (line->StartsWith("Игрок 1:")) {
                shots1++;
                if (line->Contains("Попадание")) hits1++;
                else if (line->Contains("Промах")) misses1++;
                else if (line->Contains("Потоплен")) { hits1++; sunk1++; }
            }
            else if (line->StartsWith("Игрок 2:")) {
                shots2++;
                if (line->Contains("Попадание")) hits2++;
                else if (line->Contains("Промах")) misses2++;
                else if (line->Contains("Потоплен")) { hits2++; sunk2++; }
            }
        }
        sw->WriteLine("Игрок 1: выстрелов {0}, попаданий {1}, промахов {2}, потоплено кораблей {3}", shots1, hits1, misses1, sunk1);
        sw->WriteLine("Игрок 2: выстрелов {0}, попаданий {1}, промахов {2}, потоплено кораблей {3}", shots2, hits2, misses2, sunk2);
        sw->Close();
        delete sw;
    }
    catch (Exception^) {
        // Игнорируем ошибки записи
    }
}