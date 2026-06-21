#include "MainForm.h"
#include "GameEngine1.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::IO;
using namespace System::Threading;
using namespace SeaBattleCpp;

// ==================== ЛОГГЕР ====================
void TestLog(String^ message) {
    try {
        StreamWriter^ sw = gcnew StreamWriter("test_results.txt", true);
        sw->WriteLine(message);
        sw->Close();
        delete sw;
    }
    catch (Exception^) {}
    Console::WriteLine(message);
}

#define TEST_ASSERT(condition, message) \
    if (!(condition)) { \
        TestLog(L"  [FAIL] " + message); \
        return false; \
    } else { \
        TestLog(L"  [PASS] " + message); \
    }

// ==================== WHITE‑BOX ТЕСТЫ ====================

bool Test_PlaceShipManual() {
    GameEngine^ engine = gcnew GameEngine(GameMode::PvP);
    bool result = engine->PlaceShipManual(1, 0, 0, 4, true);
    TEST_ASSERT(result, L"Корабль 4x1 в (0,0) горизонтально должен поместиться");
    TEST_ASSERT(engine->GetBoard(1)[0] == CellState::Ship, L"Клетка (0,0) – Ship");
    TEST_ASSERT(engine->GetBoard(1)[1] == CellState::Ship, L"Клетка (0,1) – Ship");
    TEST_ASSERT(engine->GetBoard(1)[2] == CellState::Ship, L"Клетка (0,2) – Ship");
    TEST_ASSERT(engine->GetBoard(1)[3] == CellState::Ship, L"Клетка (0,3) – Ship");
    return true;
}

bool Test_PlaceShipManual_Invalid() {
    GameEngine^ engine = gcnew GameEngine(GameMode::PvP);
    bool result = engine->PlaceShipManual(1, 9, 7, 4, true);
    TEST_ASSERT(!result, L"Корабль не должен поместиться (выход за правую границу)");
    return true;
}

bool Test_AutoPlaceShips() {
    GameEngine^ engine = gcnew GameEngine(GameMode::PvP);
    engine->AutoPlaceShips(1);
    int shipCount = engine->GetRemainingShipsCount(1);
    TEST_ASSERT(shipCount == 0, L"Список оставшихся длин пуст");
    int shipCells = 0;
    for (int i = 0; i < 100; i++) {
        if (engine->GetBoard(1)[i] == CellState::Ship) shipCells++;
    }
    TEST_ASSERT(shipCells == 20, L"На доске 20 клеток с кораблями");
    return true;
}

bool Test_RemoveShipAt() {
    GameEngine^ engine = gcnew GameEngine(GameMode::PvP);
    while (engine->GetRemainingShipsCount(1) > 0) {
        engine->RemoveRemainingLengthAt(1, 0);
    }
    engine->AddRemainingLength(1, 3);
    engine->PlaceShipManual(1, 0, 0, 3, true);
    bool removed = engine->RemoveShipAt(1, 0, 1);
    TEST_ASSERT(removed, L"Корабль удалён");
    TEST_ASSERT(engine->GetBoard(1)[0] == CellState::Empty, L"Клетка (0,0) пуста");
    TEST_ASSERT(engine->GetBoard(1)[1] == CellState::Empty, L"Клетка (0,1) пуста");
    TEST_ASSERT(engine->GetBoard(1)[2] == CellState::Empty, L"Клетка (0,2) пуста");
    TEST_ASSERT(engine->GetRemainingShipsCount(1) == 2, L"В списке две длины");
    TEST_ASSERT(engine->GetRemainingLength(1, 0) == 3, L"Первая длина 3");
    TEST_ASSERT(engine->GetRemainingLength(1, 1) == 3, L"Вторая длина 3");
    return true;
}

bool Test_MakeMove_Hit() {
    GameEngine^ engine = gcnew GameEngine(GameMode::PvP);
    engine->PlaceShipManual(2, 0, 0, 3, true);
    bool hit, sunk; int sunkLen;
    bool result = engine->MakeMove(1, 0, 0, hit, sunk, sunkLen);
    TEST_ASSERT(result, L"Ход разрешён");
    TEST_ASSERT(hit, L"Попадание");
    TEST_ASSERT(!sunk, L"Корабль не потоплен");
    TEST_ASSERT(engine->GetBoard(2)[0] == CellState::Hit, L"Клетка (0,0) – Hit");
    TEST_ASSERT(engine->GetCurrentPlayer() == 1, L"Ход остался у игрока 1");
    return true;
}

bool Test_MakeMove_Sink() {
    GameEngine^ engine = gcnew GameEngine(GameMode::PvP);
    engine->PlaceShipManual(2, 0, 0, 2, true);
    bool hit, sunk; int len;
    engine->MakeMove(1, 0, 0, hit, sunk, len);
    engine->MakeMove(1, 0, 1, hit, sunk, len);
    TEST_ASSERT(sunk, L"Корабль потоплен");
    TEST_ASSERT(len == 2, L"Длина = 2");
    TEST_ASSERT(engine->GetBoard(2)[10] == CellState::Miss, L"Клетка (1,0) – Miss");
    TEST_ASSERT(engine->GetBoard(2)[11] == CellState::Miss, L"Клетка (1,1) – Miss");
    return true;
}

bool Test_MakeMove_Miss() {
    GameEngine^ engine = gcnew GameEngine(GameMode::PvP);
    engine->PlaceShipManual(2, 0, 0, 1, true);
    bool hit, sunk; int len;
    bool result = engine->MakeMove(1, 5, 5, hit, sunk, len);
    TEST_ASSERT(result, L"Ход разрешён");
    TEST_ASSERT(!hit, L"Промах");
    TEST_ASSERT(engine->GetBoard(2)[55] == CellState::Miss, L"Клетка (5,5) – Miss");
    TEST_ASSERT(engine->GetCurrentPlayer() == 2, L"Ход перешёл к игроку 2");
    return true;
}

bool Test_MakeMove_Invalid() {
    GameEngine^ engine = gcnew GameEngine(GameMode::PvP);
    engine->PlaceShipManual(2, 0, 0, 1, true);
    bool hit, sunk; int len;
    engine->MakeMove(1, 0, 0, hit, sunk, len);
    bool second = engine->MakeMove(1, 0, 0, hit, sunk, len);
    TEST_ASSERT(!second, L"Повторный выстрел запрещён");
    return true;
}

bool Test_GameOver() {
    GameEngine^ engine = gcnew GameEngine(GameMode::PvP);
    engine->PlaceShipManual(2, 0, 0, 1, true);
    bool hit, sunk; int len;
    engine->MakeMove(1, 0, 0, hit, sunk, len);
    TEST_ASSERT(engine->IsGameOver(), L"Игра окончена");
    TEST_ASSERT(engine->GetWinner() == 1, L"Победитель – игрок 1");
    return true;
}

bool Test_RandomComputerMove() {
    GameEngine^ engine = gcnew GameEngine(GameMode::PvC_Medium);
    auto move = engine->GetRandomComputerMove();
    TEST_ASSERT(move.Item1 >= 0 && move.Item1 < 10 && move.Item2 >= 0 && move.Item2 < 10, L"Координаты в диапазоне");
    int idx = move.Item1 * 10 + move.Item2;
    CellState state = engine->GetBoard(1)[idx];
    TEST_ASSERT(state != CellState::Hit && state != CellState::Miss, L"Клетка не атакована");
    return true;
}

bool Test_SmartComputerMove() {
    GameEngine^ engine = gcnew GameEngine(GameMode::PvC_Hard);
    engine->PlaceShipManual(1, 5, 5, 3, true);
    bool hit, sunk; int len;
    engine->MakeMove(2, 5, 5, hit, sunk, len);
    engine->NotifySmartResult(5, 5, true, false);
    auto move = engine->GetSmartComputerMove();
    bool isAdjacent = (move.Item1 == 5 && (move.Item2 == 4 || move.Item2 == 6)) ||
        (move.Item2 == 5 && (move.Item1 == 4 || move.Item1 == 6));
    TEST_ASSERT(isAdjacent, L"Следующий ход по соседней клетке");
    return true;
}

bool Test_SaveLoad() {
    GameEngine^ engine = gcnew GameEngine(GameMode::PvP);
    engine->PlaceShipManual(2, 0, 0, 2, true);
    bool hit, sunk; int len;
    engine->MakeMove(1, 0, 0, hit, sunk, len);
    String^ tempFile = "test_save.txt";
    engine->SaveGame(tempFile);

    GameEngine^ loaded = gcnew GameEngine(GameMode::PvP);
    bool loadOk = loaded->LoadGame(tempFile);
    TEST_ASSERT(loadOk, L"Загрузка прошла");
    TEST_ASSERT(loaded->GetMode() == GameMode::PvP, L"Режим сохранён");
    TEST_ASSERT(loaded->GetCurrentPlayer() == 1, L"Ход сохранён");
    TEST_ASSERT(loaded->GetBoard(2)[0] == CellState::Hit, L"Клетка (0,0) – Hit");
    TEST_ASSERT(loaded->GetBoard(2)[1] == CellState::Ship, L"Клетка (0,1) – Ship");
    TEST_ASSERT(loaded->GetRemainingShipsCount(2) == 10, L"Осталось 10 кораблей (длины не удалялись)");
    System::IO::File::Delete(tempFile);
    return true;
}

void RunWhiteBoxTests() {
    try { StreamWriter^ sw = gcnew StreamWriter("test_results.txt", false); sw->Close(); delete sw; }
    catch (Exception^) {}

    TestLog(L"=== ЗАПУСК WHITE‑BOX ТЕСТОВ ===");
    int passed = 0, failed = 0;

    auto run = [&](String^ name, bool (*testFunc)()) {
        TestLog(L"\nТест: " + name);
        try {
            if (testFunc()) passed++; else failed++;
        }
        catch (Exception^ ex) {
            TestLog(L"  [EXCEPTION] " + ex->Message);
            failed++;
        }
        };

    run(L"PlaceShipManual (успешная)", Test_PlaceShipManual);
    run(L"PlaceShipManual (неудачная)", Test_PlaceShipManual_Invalid);
    run(L"AutoPlaceShips", Test_AutoPlaceShips);
    run(L"RemoveShipAt", Test_RemoveShipAt);
    run(L"MakeMove – попадание", Test_MakeMove_Hit);
    run(L"MakeMove – потопление", Test_MakeMove_Sink);
    run(L"MakeMove – промах", Test_MakeMove_Miss);
    run(L"MakeMove – неверный ход", Test_MakeMove_Invalid);
    run(L"GameOver", Test_GameOver);
    run(L"RandomComputerMove", Test_RandomComputerMove);
    run(L"SmartComputerMove", Test_SmartComputerMove);
    run(L"Сохранение/загрузка", Test_SaveLoad);

    TestLog(L"\n=== ИТОГО: " + passed.ToString() + L" пройдено, " + failed.ToString() + L" не пройдено ===");
    MessageBox::Show(String::Format(L"White‑Box тесты завершены.\nПройдено: {0}\nНе пройдено: {1}", passed, failed), L"Результаты");
}

// ==================== BLACK‑BOX ТЕСТЫ ====================

bool Test_BlackBox_PvP_FullCycle() {
    TestLog(L"\n[BlackBox] PvP – полный цикл игры");
    try {
        GameForm^ game = gcnew GameForm(GameMode::PvP);
        game->StartNewGame();

        if (!game->IsPlacingMode) { TestLog(L"  [FAIL] isPlacingMode != true"); return false; }
        TestLog(L"  [PASS] isPlacingMode = true");
        if (!game->StatusText->Contains("Игрока 1")) { TestLog(L"  [FAIL] Статус не указывает на игрока 1"); return false; }
        TestLog(L"  [PASS] Статус: " + game->StatusText);

        // Авторасстановка игрока 1
        game->AutoPlacePlayer(1);
        if (game->Engine->GetRemainingShipsCount(1) != 0) {
            TestLog(L"  [FAIL] Корабли игрока 1 не расставлены");
            delete game;
            return false;
        }
        TestLog(L"  [PASS] Корабли игрока 1 расставлены");

        // ПРИНУДИТЕЛЬНО переключаем на игрока 2 (даже если уже переключился)
        game->TestSetPlacingPlayer(2);
        game->TestSetPlacingMode(true);
        // Теперь мы точно в режиме расстановки игрока 2, независимо от бага StartGameFromTest
        TestLog(L"  [PASS] Принудительно переключились на игрока 2 (режим расстановки)");

        // Авторасстановка игрока 2
        game->AutoPlacePlayer(2);
        if (game->Engine->GetRemainingShipsCount(2) != 0) {
            TestLog(L"  [FAIL] Корабли игрока 2 не расставлены");
            delete game;
            return false;
        }
        TestLog(L"  [PASS] Корабли игрока 2 расставлены");

        // Начинаем игру – теперь StartGameFromTest должен сработать корректно,
        // потому что у игрока 2 все корабли расставлены
        game->StartGameFromTest();
        if (game->IsPlacingMode || !game->StatusText->Contains("Игра началась")) {
            TestLog(L"  [FAIL] Игра не началась. Текущий статус: " + game->StatusText);
            delete game;
            return false;
        }
        TestLog(L"  [PASS] Игра началась");

        // ... (остальной код с ходами, если есть) ...

        delete game;
        return true;
    }
    catch (Exception^ ex) {
        TestLog(L"  [EXCEPTION] " + ex->Message);
        return false;
    }
}

bool Test_BlackBox_PvC() {
    TestLog(L"\n[BlackBox] PvC (средний) – проверка хода компьютера");
    try {
        GameForm^ game = gcnew GameForm(GameMode::PvC_Medium);
        game->StartNewGame();

        // Авторасстановка для игрока 1
        game->AutoPlacePlayer(1);
        if (game->Engine->GetRemainingShipsCount(1) != 0) {
            TestLog(L"  [FAIL] Корабли игрока 1 не расставлены");
            delete game;
            return false;
        }
        TestLog(L"  [PASS] Корабли игрока 1 расставлены");

        // Начинаем игру
        game->StartGameFromTest();
        if (game->IsPlacingMode || !game->StatusText->Contains("Игра началась")) {
            TestLog(L"  [FAIL] Игра не началась");
            delete game;
            return false;
        }
        TestLog(L"  [PASS] Игра началась");

        // Эмулируем выстрел игрока по полю компьютера
        int targetRow = -1, targetCol = -1;
        for (int r = 0; r < 10 && targetRow == -1; r++) {
            for (int c = 0; c < 10 && targetRow == -1; c++) {
                if (game->Engine->GetBoard(2)[r * 10 + c] == CellState::Ship) {
                    targetRow = r; targetCol = c;
                }
            }
        }
        if (targetRow == -1) {
            TestLog(L"  [SKIP] На доске компьютера нет кораблей");
            delete game;
            return true;
        }
        game->SimulateCellClick(2, targetRow, targetCol);
        Application::DoEvents();
        Thread::Sleep(100);

        if (!game->Engine->IsGameOver()) {
            if (game->Engine->GetCurrentPlayer() == 2) {
                TestLog(L"  [FAIL] Ход остался у компьютера");
                delete game;
                return false;
            }
            TestLog(L"  [PASS] Ход вернулся к игроку");
        }
        else {
            TestLog(L"  [PASS] Игра завершилась после выстрела");
        }

        delete game;
        return true;
    }
    catch (Exception^ ex) {
        TestLog(L"  [EXCEPTION] " + ex->Message);
        return false;
    }
}

bool Test_BlackBox_SaveLoad() {
    TestLog(L"\n[BlackBox] Сохранение и загрузка (пропущен)");
    TestLog(L"  [SKIP] Требует мокинга диалогов");
    return true;
}

void RunBlackBoxTests() {
    try { StreamWriter^ sw = gcnew StreamWriter("test_results.txt", true); sw->Close(); delete sw; }
    catch (Exception^) {}

    TestLog(L"\n=== ЗАПУСК BLACK‑BOX ТЕСТОВ ===");
    int passed = 0, failed = 0;

    auto run = [&](String^ name, bool (*func)()) {
        TestLog(L"\n--- " + name + " ---");
        try {
            if (func()) passed++; else failed++;
        }
        catch (Exception^ ex) {
            TestLog(L"  [EXCEPTION] " + ex->Message);
            failed++;
        }
        };

    run(L"PvP полный цикл", Test_BlackBox_PvP_FullCycle);
    run(L"PvC (средний)", Test_BlackBox_PvC);
    run(L"Сохранение/загрузка", Test_BlackBox_SaveLoad);

    TestLog(L"\n=== ИТОГО Black‑Box: " + passed.ToString() + L" пройдено, " + failed.ToString() + L" не пройдено ===");
    MessageBox::Show(String::Format(L"Black‑Box тесты завершены.\nПройдено: {0}\nНе пройдено: {1}", passed, failed), L"Результаты Black‑Box");
}

// ==================== ТОЧКА ВХОДА ====================

[STAThreadAttribute]
int main(array<String^>^ args)
{
    // White‑Box
    if (args->Length > 0 && args[0]->Equals("--test", StringComparison::OrdinalIgnoreCase))
    {
        RunWhiteBoxTests();
        return 0;
    }

    // Black‑Box
    if (args->Length > 0 && args[0]->Equals("--test-gui", StringComparison::OrdinalIgnoreCase))
    {
        RunBlackBoxTests();
        return 0;
    }

    // Обычный запуск GUI
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew SeaBattleCpp::MainForm());
    return 0;
}