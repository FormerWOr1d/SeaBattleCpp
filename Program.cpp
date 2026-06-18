#define DOCTEST_CONFIG_NO_MULTITHREADING
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

#include "MainForm.h"
#include "GameEngine1.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace SeaBattleCpp;

// ==================== WHITE BOX TESTS (20 тестов) ====================

TEST_SUITE("WhiteBox") {

    TEST_CASE("WhiteBox.Plus") {
        GameEngine engine(GameMode::PvP);
        bool placed = engine.PlaceShipManual(1, 0, 0, 4, true);
        CHECK(placed == true);
    }

    TEST_CASE("WhiteBox.Minus") {
        GameEngine engine(GameMode::PvP);
        engine.PlaceShipManual(1, 0, 0, 4, true);
        bool removed = engine.RemoveShipAt(1, 0, 0);
        CHECK(removed == true);
    }

    TEST_CASE("WhiteBox.Ymn") {
        GameEngine engine(GameMode::PvP);
        engine.AutoPlaceShips(1);
        bool hasShip = false;
        for (int i = 0; i < 100; ++i) {
            if ((int)engine.GetBoard(1)[i] == (int)CellState::Ship) {
                hasShip = true;
                break;
            }
        }
        CHECK(hasShip == true);
    }

    TEST_CASE("WhiteBox.Del") {
        GameEngine engine(GameMode::PvP);
        engine.PlaceShipManual(1, 0, 0, 4, true);
        bool removed = engine.RemoveShipAt(1, 0, 0);
        CHECK(removed == true);
        // Проверяем, что клетки стали пустыми
        bool allEmpty = true;
        for (int i = 0; i < 4; ++i) {
            if ((int)engine.GetBoard(1)[0 * 10 + i] != (int)CellState::Empty) {
                allEmpty = false;
                break;
            }
        }
        CHECK(allEmpty == true);
    }

    TEST_CASE("WhiteBox.Del_NaNul") {
        GameEngine engine(GameMode::PvP);
        bool removed = engine.RemoveShipAt(1, 5, 5);
        CHECK(removed == false);
    }

    TEST_CASE("WhiteBox.Root") {
        GameEngine engine(GameMode::PvP);
        engine.PlaceShipManual(1, 0, 0, 4, true);
        CHECK((int)engine.GetBoard(1)[0 * 10 + 0] == (int)CellState::Ship);
        CHECK((int)engine.GetBoard(1)[0 * 10 + 3] == (int)CellState::Ship);
    }

    TEST_CASE("WhiteBox.Power") {
        GameEngine engine(GameMode::PvP);
        engine.PlaceShipManual(1, 0, 0, 4, true);
        engine.RemoveShipAt(1, 0, 0);
        CHECK((int)engine.GetBoard(1)[0 * 10 + 0] == (int)CellState::Empty);
        CHECK((int)engine.GetBoard(1)[0 * 10 + 3] == (int)CellState::Empty);
    }

    TEST_CASE("WhiteBox.Skb") {
        GameEngine engine(GameMode::PvP);
        CHECK(engine.IsGameOver() == false);
        CHECK(engine.GetWinner() == 0);
    }

    TEST_CASE("WhiteBox.Nothing") {
        GameEngine engine(GameMode::PvP);
        // Проверяем, что доски созданы и имеют правильный размер
        array<CellState>^ board = engine.GetBoard(1);
        CHECK(board != nullptr);
        CHECK(board->Length == 100);
    }

    TEST_CASE("WhiteBox.DoubleUnaryMinus") {
        GameEngine engine(GameMode::PvP);
        engine.PlaceShipManual(1, 0, 0, 4, true);
        engine.PlaceShipManual(1, 0, 5, 3, true);
        // Проверяем, что оба корабля на доске
        CHECK((int)engine.GetBoard(1)[0 * 10 + 0] == (int)CellState::Ship);
        CHECK((int)engine.GetBoard(1)[0 * 10 + 5] == (int)CellState::Ship);
        // Удаляем первый
        engine.RemoveShipAt(1, 0, 0);
        CHECK((int)engine.GetBoard(1)[0 * 10 + 0] == (int)CellState::Empty);
        // Второй должен остаться
        CHECK((int)engine.GetBoard(1)[0 * 10 + 5] == (int)CellState::Ship);
    }

    TEST_CASE("WhiteBox.LongChain") {
        GameEngine engine(GameMode::PvP);
        // Проверяем, что можно расставить несколько кораблей
        bool placed1 = engine.PlaceShipManual(1, 0, 0, 4, true);
        bool placed2 = engine.PlaceShipManual(1, 0, 5, 3, true);
        bool placed3 = engine.PlaceShipManual(1, 2, 0, 3, true);
        CHECK(placed1 == true);
        CHECK(placed2 == true);
        CHECK(placed3 == true);
    }

    TEST_CASE("WhiteBox.Fraction") {
        GameEngine engine(GameMode::PvP);
        // Проверяем, что авторасстановка не падает
        engine.AutoPlaceShips(1);
        CHECK(true);
    }

    TEST_CASE("WhiteBox.SigedBeforSkobka") {
        GameEngine engine(GameMode::PvP);
        engine.PlaceShipManual(1, 0, 0, 4, true);
        bool removed = engine.RemoveShipAt(1, 0, 0);
        CHECK(removed == true);
    }

    TEST_CASE("WhiteBox.ComboSym") {
        GameEngine engine(GameMode::PvP);
        engine.PlaceShipManual(1, 0, 0, 4, true);
        engine.RemoveShipAt(1, 0, 0);
        CHECK(true);
    }

    TEST_CASE("WhiteBox.TestFormata") {
        GameEngine engine(GameMode::PvP);
        array<CellState>^ board = engine.GetBoard(1);
        CHECK(board->Length == 100);
    }

    TEST_CASE("WhiteBox.NegativeRoot") {
        GameEngine engine(GameMode::PvP);
        bool placed = engine.PlaceShipManual(1, 9, 8, 3, true);
        CHECK(placed == false);
    }

    TEST_CASE("WhiteBox.PlusPlus") {
        GameEngine engine(GameMode::PvP);
        engine.AutoPlaceShips(1);
        // Удаляем первый попавшийся корабль
        bool found = false;
        for (int i = 0; i < 10 && !found; ++i) {
            for (int j = 0; j < 10 && !found; ++j) {
                if ((int)engine.GetBoard(1)[i * 10 + j] == (int)CellState::Ship) {
                    bool removed = engine.RemoveShipAt(1, i, j);
                    CHECK(removed == true);
                    found = true;
                }
            }
        }
        CHECK(found == true);
    }

    TEST_CASE("WhiteBox.MinusMinus") {
        GameEngine engine(GameMode::PvP);
        engine.PlaceShipManual(1, 0, 0, 4, true);
        bool placed = engine.PlaceShipManual(1, 0, 0, 3, true);
        CHECK(placed == false);
    }

    TEST_CASE("WhiteBox.Multiply") {
        GameEngine engine(GameMode::PvP);
        engine.AutoPlaceShips(1);
        // Проверяем, что можно удалить корабль после авторасстановки
        bool found = false;
        for (int i = 0; i < 10 && !found; ++i) {
            for (int j = 0; j < 10 && !found; ++j) {
                if ((int)engine.GetBoard(1)[i * 10 + j] == (int)CellState::Ship) {
                    engine.RemoveShipAt(1, i, j);
                    found = true;
                }
            }
        }
        CHECK(found == true);
    }

    TEST_CASE("WhiteBox.Divide") {
        GameEngine engine(GameMode::PvP);
        engine.PlaceShipManual(1, 0, 0, 4, true);
        engine.PlaceShipManual(1, 0, 5, 3, true);
        // Удаляем оба
        engine.RemoveShipAt(1, 0, 0);
        engine.RemoveShipAt(1, 0, 5);
        // Проверяем, что доска пуста в этих местах
        CHECK((int)engine.GetBoard(1)[0 * 10 + 0] == (int)CellState::Empty);
        CHECK((int)engine.GetBoard(1)[0 * 10 + 5] == (int)CellState::Empty);
    }

} // TEST_SUITE("WhiteBox")

// ==================== BLACK BOX TESTS (20 тестов) ====================

TEST_SUITE("BlackBox") {

    TEST_CASE("BlackBox.MissingBracket") {
        GameEngine engine(GameMode::PvP);
        CHECK((int)engine.GetMode() == (int)GameMode::PvP);
    }

    TEST_CASE("BlackBox.UnaryMinus") {
        GameEngine engine(GameMode::PvP);
        engine.AutoPlaceShips(2);
        // Находим пустую клетку
        int row = -1, col = -1;
        for (int i = 0; i < 10 && row == -1; ++i) {
            for (int j = 0; j < 10 && row == -1; ++j) {
                if ((int)engine.GetBoard(2)[i * 10 + j] == (int)CellState::Empty) {
                    row = i; col = j;
                }
            }
        }
        REQUIRE(row != -1);
        bool hit, sunk; int sunkLen;
        bool result = engine.MakeMove(1, row, col, hit, sunk, sunkLen);
        CHECK(result == true);
        CHECK(hit == false);
        CHECK(engine.GetCurrentPlayer() == 2);
    }

    TEST_CASE("BlackBox.Float") {
        GameEngine engine(GameMode::PvP);
        engine.AutoPlaceShips(2);
        int row = -1, col = -1;
        for (int i = 0; i < 10 && row == -1; ++i) {
            for (int j = 0; j < 10 && row == -1; ++j) {
                if ((int)engine.GetBoard(2)[i * 10 + j] == (int)CellState::Ship) {
                    row = i; col = j;
                }
            }
        }
        REQUIRE(row != -1);
        bool hit, sunk; int sunkLen;
        bool result = engine.MakeMove(1, row, col, hit, sunk, sunkLen);
        CHECK(result == true);
        CHECK(hit == true);
        CHECK(engine.GetCurrentPlayer() == 1);
    }

    TEST_CASE("BlackBox.NestedParentheses") {
        GameEngine engine(GameMode::PvP);
        engine.AutoPlaceShips(2);
        // Уничтожаем все корабли
        for (int i = 0; i < 10; ++i) {
            for (int j = 0; j < 10; ++j) {
                if ((int)engine.GetBoard(2)[i * 10 + j] == (int)CellState::Ship) {
                    bool hit, sunk; int sunkLen;
                    engine.MakeMove(1, i, j, hit, sunk, sunkLen);
                }
            }
        }
        CHECK(engine.IsGameOver() == true);
        CHECK(engine.GetWinner() == 1);
    }

    TEST_CASE("BlackBox.RootAndPower") {
        GameEngine engine(GameMode::PvC_Medium);
        engine.AutoPlaceShips(2);
        auto move = engine.GetRandomComputerMove();
        CHECK(move.Item1 >= 0);
        CHECK(move.Item1 < 10);
        CHECK(move.Item2 >= 0);
        CHECK(move.Item2 < 10);
    }

    TEST_CASE("BlackBox.EmptyExpression") {
        GameEngine engine(GameMode::PvC_Hard);
        engine.AutoPlaceShips(2);
        auto move = engine.GetSmartComputerMove();
        CHECK(move.Item1 >= 0);
        CHECK(move.Item1 < 10);
        CHECK(move.Item2 >= 0);
        CHECK(move.Item2 < 10);
    }

    TEST_CASE("BlackBox.LargeNumbers") {
        GameEngine engine(GameMode::PvP);
        engine.AutoPlaceShips(1);
        engine.AutoPlaceShips(2);
        String^ tempFile = "test_save_large.txt";
        engine.SaveGame(tempFile);
        GameEngine loadedEngine(GameMode::PvP);
        bool loadOk = loadedEngine.LoadGame(tempFile);
        CHECK(loadOk == true);
        System::IO::File::Delete(tempFile);
    }

    TEST_CASE("BlackBox.DotWithoutDigitAfter") {
        GameEngine engine(GameMode::PvP);
        engine.AutoPlaceShips(2);
        bool hit, sunk; int sunkLen;
        bool foundEmpty = false;
        for (int i = 0; i < 10 && !foundEmpty; ++i) {
            for (int j = 0; j < 10 && !foundEmpty; ++j) {
                if ((int)engine.GetBoard(2)[i * 10 + j] == (int)CellState::Empty) {
                    engine.MakeMove(1, i, j, hit, sunk, sunkLen);
                    foundEmpty = true;
                }
            }
        }
        CHECK(engine.IsGameOver() == false);
    }

    TEST_CASE("BlackBox.LeadingDot") {
        GameEngine engineMed(GameMode::PvC_Medium);
        CHECK((int)engineMed.GetMode() == (int)GameMode::PvC_Medium);
    }

    TEST_CASE("BlackBox.ZeroResult") {
        GameEngine engine(GameMode::PvP);
        engine.AutoPlaceShips(2);
        int row = -1, col = -1;
        for (int i = 0; i < 10 && row == -1; ++i) {
            for (int j = 0; j < 10 && row == -1; ++j) {
                if ((int)engine.GetBoard(2)[i * 10 + j] == (int)CellState::Ship) {
                    row = i; col = j;
                }
            }
        }
        REQUIRE(row != -1);
        bool hit, sunk; int sunkLen;
        engine.MakeMove(1, row, col, hit, sunk, sunkLen);
        bool result = engine.MakeMove(1, row, col, hit, sunk, sunkLen);
        CHECK(result == false);
    }

    TEST_CASE("BlackBox.NegativePower") {
        GameEngine engine(GameMode::PvP);
        engine.PlaceShipManual(1, 0, 0, 4, true);
        bool removed = engine.RemoveShipAt(1, 0, 0);
        CHECK(removed == true);
    }

    TEST_CASE("BlackBox.ZeroPowerZero") {
        GameEngine engine(GameMode::PvP);
        bool placed = engine.PlaceShipManual(1, 5, 5, 2, true);
        CHECK(placed == true);
        CHECK((int)engine.GetBoard(1)[5 * 10 + 5] == (int)CellState::Ship);
    }

    TEST_CASE("BlackBox.RootOfZero") {
        GameEngine engine(GameMode::PvP);
        engine.AutoPlaceShips(2);
        bool hasShip = false;
        for (int i = 0; i < 100; ++i) {
            if ((int)engine.GetBoard(2)[i] == (int)CellState::Ship) {
                hasShip = true;
                break;
            }
        }
        CHECK(hasShip == true);
    }

    TEST_CASE("BlackBox.ManyNestedParentheses") {
        GameEngine engine(GameMode::PvP);
        engine.AutoPlaceShips(2);
        int row = -1, col = -1;
        for (int i = 0; i < 10 && row == -1; ++i) {
            for (int j = 0; j < 10 && row == -1; ++j) {
                if ((int)engine.GetBoard(2)[i * 10 + j] == (int)CellState::Empty) {
                    row = i; col = j;
                }
            }
        }
        REQUIRE(row != -1);
        bool hit, sunk; int sunkLen;
        engine.MakeMove(1, row, col, hit, sunk, sunkLen);
        CHECK(engine.GetCurrentPlayer() == 2);
    }

    TEST_CASE("BlackBox.EmptyParentheses") {
        GameEngine engine(GameMode::PvP);
        engine.AutoPlaceShips(1);
        engine.AutoPlaceShips(2);
        String^ tempFile = "test_save_empty.txt";
        engine.SaveGame(tempFile);
        GameEngine loadedEngine(GameMode::PvP);
        loadedEngine.LoadGame(tempFile);
        CHECK((int)loadedEngine.GetMode() == (int)engine.GetMode());
        System::IO::File::Delete(tempFile);
    }

    TEST_CASE("BlackBox.OnlyOperator") {
        GameEngine engine(GameMode::PvP);
        bool removed = engine.RemoveShipAt(1, 10, 10);
        CHECK(removed == false);
    }

    TEST_CASE("BlackBox.Overflow") {
        GameEngine engine(GameMode::PvP);
        bool placed = engine.PlaceShipManual(1, 9, 8, 3, true);
        CHECK(placed == false);
    }

    TEST_CASE("BlackBox.RootOfRoot") {
        GameEngine engine(GameMode::PvP);
        engine.AutoPlaceShips(2);
        int row = -1, col = -1;
        for (int i = 0; i < 10 && row == -1; ++i) {
            for (int j = 0; j < 10 && row == -1; ++j) {
                if ((int)engine.GetBoard(2)[i * 10 + j] == (int)CellState::Ship) {
                    row = i; col = j;
                }
            }
        }
        REQUIRE(row != -1);
        bool hit, sunk; int sunkLen;
        engine.MakeMove(1, row, col, hit, sunk, sunkLen);
        CHECK((int)engine.GetBoard(2)[row * 10 + col] == (int)CellState::Hit);
    }

    TEST_CASE("BlackBox.NegativeRoot") {
        GameEngine engine(GameMode::PvP);
        engine.PlaceShipManual(2, 0, 0, 2, true);
        bool hit1, sunk1; int len1;
        engine.MakeMove(1, 0, 0, hit1, sunk1, len1);
        bool hit2, sunk2; int len2;
        engine.MakeMove(1, 0, 1, hit2, sunk2, len2);
        CHECK(sunk2 == true);
        // Проверяем, что вокруг корабля появились Miss
        CHECK((int)engine.GetBoard(2)[0 * 10 + 2] == (int)CellState::Miss);
    }

} // TEST_SUITE("BlackBox")

// ---------- Основная функция ----------
[STAThreadAttribute]
int main(array<String^>^ args)
{
    bool runTests = false;
    for each(String ^ arg in args) {
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