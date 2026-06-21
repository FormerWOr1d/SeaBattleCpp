#include "MainForm.h"
#include "GameEngine1.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace System::IO;
using namespace SeaBattleCpp;

// ==================== ВСПОМОГАТЕЛЬНЫЕ МАКРОСЫ И ФУНКЦИИ ДЛЯ ТЕСТОВ ====================

void TestLog(String^ message) {
    try {
        // Запись в файл в режиме добавления (append)
        StreamWriter^ sw = gcnew StreamWriter("test_results.txt", true);
        sw->WriteLine(message);
        sw->Close();
        delete sw;
    }
    catch (Exception^) {
        // Если не удалось записать в файл, игнорируем
    }
    // Дублируем в консоль (если она открыта)
    Console::WriteLine(message);
}

#define TEST_ASSERT(condition, message) \
    if (!(condition)) { \
        TestLog(L"  [FAIL] " + message); \
        return false; \
    } else { \
        TestLog(L"  [PASS] " + message); \
    }

// ==================== WHITE‑BOX ТЕСТЫ (ТОЛЬКО LOGIC) ====================

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
    bool result = engine->PlaceShipManual(1, 9, 7, 4, true); // 7+4=11 > 10
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
    // Очищаем список оставшихся длин
    while (engine->GetRemainingShipsCount(1) > 0) {
        engine->RemoveRemainingLengthAt(1, 0);
    }
    // Добавляем одну длину 3
    engine->AddRemainingLength(1, 3);
    // Ставим корабль длиной 3
    engine->PlaceShipManual(1, 0, 0, 3, true);
    // Удаляем корабль
    bool removed = engine->RemoveShipAt(1, 0, 1);
    TEST_ASSERT(removed, L"Корабль удалён");
    TEST_ASSERT(engine->GetBoard(1)[0] == CellState::Empty, L"Клетка (0,0) пуста");
    TEST_ASSERT(engine->GetBoard(1)[1] == CellState::Empty, L"Клетка (0,1) пуста");
    TEST_ASSERT(engine->GetBoard(1)[2] == CellState::Empty, L"Клетка (0,2) пуста");
    // Теперь в списке должно быть две длины 3
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
    // Ставим хотя бы один корабль для игрока 2
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
    // Количество оставшихся длин должно быть 10 (мы не удаляли длину)
    TEST_ASSERT(loaded->GetRemainingShipsCount(2) == 10, L"Осталось 10 кораблей (длины не удалялись)");
    System::IO::File::Delete(tempFile);
    return true;
}

void RunAllTests() {
    // Очищаем лог-файл (создаём заново)
    try {
        StreamWriter^ sw = gcnew StreamWriter("test_results.txt", false);
        sw->Close();
        delete sw;
    }
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

    // Показываем результат в MessageBox
    String^ summary = String::Format(L"Тесты завершены.\nПройдено: {0}\nНе пройдено: {1}\n\nРезультаты сохранены в test_results.txt", passed, failed);
    MessageBox::Show(summary, L"Результаты тестирования", MessageBoxButtons::OK, MessageBoxIcon::Information);
}

// ==================== ТОЧКА ВХОДА ====================

[STAThreadAttribute]
int main(array<String^>^ args)
{
    // Если передан аргумент --test – запускаем тесты
    if (args->Length > 0 && args[0]->Equals("--test", StringComparison::OrdinalIgnoreCase))
    {
        RunAllTests();
        return 0;
    }

    // Обычный запуск GUI
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew SeaBattleCpp::MainForm());
    return 0;
}