#define DOCTEST_CONFIG_NO_MULTITHREADING
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

#include "MainForm.h"
#include "GameEngine1.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace SeaBattleCpp;

// ---------- Тесты ----------
TEST_CASE("GameEngine: создание игры в режиме PvP") {
    GameEngine engine(GameMode::PvP);
    CHECK((int)engine.GetMode() == (int)GameMode::PvP);
    CHECK(engine.IsGameOver() == false);
    CHECK(engine.GetCurrentPlayer() == 1);
}

TEST_CASE("GameEngine: расстановка кораблей") {
    GameEngine engine(GameMode::PvP);
    CHECK(engine.GetRemainingShipsCount(1) == 10);
    bool placed = engine.PlaceShipManual(1, 0, 0, 4, true);
    CHECK(placed == true);
    CHECK(engine.GetRemainingShipsCount(1) == 9);
}

TEST_CASE("GameEngine: удаление корабля") {
    GameEngine engine(GameMode::PvP);
    engine.PlaceShipManual(1, 0, 0, 4, true);
    CHECK(engine.GetRemainingShipsCount(1) == 9);
    bool removed = engine.RemoveShipAt(1, 0, 0);
    CHECK(removed == true);
    CHECK(engine.GetRemainingShipsCount(1) == 10);
}

// ---------- Основная функция ----------
[STAThreadAttribute]
int main(array<String^>^ args)
{
    bool runTests = false;
    for each (String ^ arg in args) {
        if (arg->Equals("--test", StringComparison::OrdinalIgnoreCase)) {
            runTests = true;
            break;
        }
    }

    if (runTests) {
        doctest::Context context;
        int result = context.run();
        return result;
    }

    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew SeaBattleCpp::MainForm());
    return 0;
}