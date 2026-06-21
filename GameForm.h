#pragma once
#include "GameMode.h"
#include "GameEngine1.h"
using namespace SeaBattleCpp;

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::IO;

public ref class GameForm : public System::Windows::Forms::Form
{
private:
    GameEngine^ engine;
    GameMode mode;
    bool isPlacingMode;
    bool hideShipsDuringDelay;
    int currentShipIndex;
    bool placementHorizontal;
    int placingPlayer;
    const int cellSize = 35;

    TableLayoutPanel^ tablePlayer1;
    TableLayoutPanel^ tablePlayer2;
    Button^ btnBack;
    Button^ btnAutoPlace;
    Button^ btnStartGame;
    Button^ btnSave;
    Button^ btnLoad;
    Label^ lblStatus;
    Label^ lblTurn;

    array<PictureBox^>^ cellsPlayer1;
    array<PictureBox^>^ cellsPlayer2;

    Image^ imgWater;
    Image^ imgShip;
    Image^ imgHit;
    Image^ imgMiss;

    TableLayoutPanel^ tableLayoutPanel1;
    TableLayoutPanel^ tableLayoutPanel2;

    void LoadImages()
    {
        String^ basePath = Application::StartupPath;
        String^ resPath = Path::Combine(basePath, "Resources");
        try {
            imgWater = Image::FromFile(Path::Combine(resPath, "water.png"));
            imgShip = Image::FromFile(Path::Combine(resPath, "ship.png"));
            imgHit = Image::FromFile(Path::Combine(resPath, "hit.png"));
            imgMiss = Image::FromFile(Path::Combine(resPath, "miss.png"));
        }
        catch (...) {
            imgWater = gcnew Bitmap(cellSize, cellSize);
            imgShip = gcnew Bitmap(cellSize, cellSize);
            imgHit = gcnew Bitmap(cellSize, cellSize);
            imgMiss = gcnew Bitmap(cellSize, cellSize);
            Graphics^ g;
            g = Graphics::FromImage(imgWater); g->Clear(Color::LightBlue); delete g;
            g = Graphics::FromImage(imgShip);  g->Clear(Color::DarkGreen); delete g;
            g = Graphics::FromImage(imgHit);   g->Clear(Color::Red);
            g->DrawLine(Pens::White, 2, 2, cellSize - 3, cellSize - 3);
            g->DrawLine(Pens::White, cellSize - 3, 2, 2, cellSize - 3);
            delete g;
            g = Graphics::FromImage(imgMiss);  g->Clear(Color::Gray);
            g->DrawEllipse(Pens::Black, cellSize / 4, cellSize / 4, cellSize / 2, cellSize / 2);
            delete g;
        }
    }

    Image^ GetImageForState(CellState state, bool showShip)
    {
        switch (state) {
        case CellState::Empty: return imgWater;
        case CellState::Ship:  return showShip ? imgShip : imgWater;
        case CellState::Hit:   return imgHit;
        case CellState::Miss:  return imgMiss;
        default: return imgWater;
        }
    }

    int GetIndex(int row, int col) { return row * 10 + col; }

    void CreateCells()
    {
        if (cellsPlayer1 != nullptr) return;
        cellsPlayer1 = gcnew array<PictureBox^>(100);
        cellsPlayer2 = gcnew array<PictureBox^>(100);
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                PictureBox^ pb1 = gcnew PictureBox();
                pb1->Size = System::Drawing::Size(cellSize, cellSize);
                pb1->BorderStyle = BorderStyle::FixedSingle;
                pb1->SizeMode = PictureBoxSizeMode::StretchImage;
                pb1->Tag = gcnew System::ValueTuple<int, int>(i, j);
                pb1->Click += gcnew EventHandler(this, &GameForm::CellPlayer1_Click);
                pb1->MouseClick += gcnew MouseEventHandler(this, &GameForm::CellPlayer1_MouseClick);
                cellsPlayer1[GetIndex(i, j)] = pb1;
                tablePlayer1->Controls->Add(pb1, j, i);

                PictureBox^ pb2 = gcnew PictureBox();
                pb2->Size = System::Drawing::Size(cellSize, cellSize);
                pb2->BorderStyle = BorderStyle::FixedSingle;
                pb2->SizeMode = PictureBoxSizeMode::StretchImage;
                pb2->Tag = gcnew System::ValueTuple<int, int>(i, j);
                pb2->Click += gcnew EventHandler(this, &GameForm::CellPlayer2_Click);
                pb2->MouseClick += gcnew MouseEventHandler(this, &GameForm::CellPlayer2_MouseClick);
                cellsPlayer2[GetIndex(i, j)] = pb2;
                tablePlayer2->Controls->Add(pb2, j, i);
            }
        }
    }

    void CellPlayer1_MouseClick(System::Object^ sender, MouseEventArgs^ e)
    {
        if (!isPlacingMode || placingPlayer != 1) return;
        if (e->Button == System::Windows::Forms::MouseButtons::Right) {
            PictureBox^ pb = safe_cast<PictureBox^>(sender);
            System::ValueTuple<int, int>^ pos = safe_cast<System::ValueTuple<int, int>^>(pb->Tag);
            int row = pos->Item1, col = pos->Item2;
            if (engine->RemoveShipAt(1, row, col)) {
                int count = engine->GetRemainingShipsCount(1);
                currentShipIndex = (count > 0) ? count - 1 : 0;
                UpdateBoards();
                lblStatus->Text = GetPlacementStatus();
            }
        }
    }

    void CellPlayer2_MouseClick(System::Object^ sender, MouseEventArgs^ e)
    {
        if (!isPlacingMode || placingPlayer != 2) return;
        if (e->Button == System::Windows::Forms::MouseButtons::Right) {
            PictureBox^ pb = safe_cast<PictureBox^>(sender);
            System::ValueTuple<int, int>^ pos = safe_cast<System::ValueTuple<int, int>^>(pb->Tag);
            int row = pos->Item1, col = pos->Item2;
            if (engine->RemoveShipAt(2, row, col)) {
                int count = engine->GetRemainingShipsCount(2);
                currentShipIndex = (count > 0) ? count - 1 : 0;
                UpdateBoards();
                lblStatus->Text = GetPlacementStatus();
            }
        }
    }

    void UpdateBoards()
    {
        if (!engine) return;
        if (!cellsPlayer1 || !cellsPlayer2) return;

        int currentPlayer = engine->GetCurrentPlayer();
        bool isPvP = (engine->GetMode() == GameMode::PvP);

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                int idx = GetIndex(i, j);

                // ---- Игрок 1 ----
                CellState state1 = engine->GetBoard(1)[idx];
                bool showShip1 = false;
                if (isPlacingMode) {
                    showShip1 = (state1 == CellState::Ship) && (placingPlayer == 1);
                }
                else if (hideShipsDuringDelay) {
                    showShip1 = false;   // принудительно скрываем все корабли на время задержки
                }
                else {
                    if (isPvP) {
                        // В PvP свои корабли видны только текущему игроку (как было)
                        showShip1 = (state1 == CellState::Ship) && (currentPlayer == 1);
                    }
                    else {
                        showShip1 = (state1 == CellState::Ship); // PvC – всегда видны
                    }
                }
                if (cellsPlayer1 && cellsPlayer1[idx])
                    cellsPlayer1[idx]->Image = GetImageForState(state1, showShip1);

                // ---- Игрок 2 ----
                CellState state2 = engine->GetBoard(2)[idx];
                bool showShip2 = false;
                if (isPlacingMode) {
                    showShip2 = (state2 == CellState::Ship) && (placingPlayer == 2);
                }
                else if (hideShipsDuringDelay) {
                    showShip2 = false;
                }
                else {
                    if (isPvP) {
                        showShip2 = (state2 == CellState::Ship) && (currentPlayer == 2);
                    }
                    else {
                        showShip2 = false; // В PvC поле компьютера скрыто
                    }
                }
                if (cellsPlayer2 && cellsPlayer2[idx])
                    cellsPlayer2[idx]->Image = GetImageForState(state2, showShip2);
            }
        }

        // Обновление текста хода
        if (!isPlacingMode && !engine->IsGameOver()) {
            String^ turn = currentPlayer == 1 ? "Игрок 1" : (isPvP ? "Игрок 2" : "Компьютер");
            lblTurn->Text = "Ход: " + turn;
        }
    }

    String^ GetPlacementStatus()
    {
        int count = engine->GetRemainingShipsCount(placingPlayer);
        if (count == 0) {
            return "Все корабли расставлены!";
        }
        if (currentShipIndex >= count) currentShipIndex = 0;
        int length = engine->GetRemainingLength(placingPlayer, currentShipIndex);
        String^ orient = placementHorizontal ? "горизонталь" : "вертикаль";
        return String::Format("Расставьте корабль длины {0} для Игрока {1}. Ориентация: {2} (ПКМ или R для смены)",
            length, placingPlayer, orient);
    }

    void NewGame()
    {
        engine = gcnew GameEngine(mode);
        engine->NewGame();
        isPlacingMode = true;
        placingPlayer = 1;
        currentShipIndex = 0;
        placementHorizontal = true;
        
        UpdateBoards();
        lblStatus->Text = "Расставьте корабли для Игрока 1 (левое поле)";
        lblTurn->Text = "Режим расстановки (игрок 1)";
        btnStartGame->Enabled = true;
        btnAutoPlace->Enabled = true;
    }

    void CellPlayer1_Click(System::Object^ sender, System::EventArgs^ e)
    {
        if (!engine) return;
        PictureBox^ pb = safe_cast<PictureBox^>(sender);
        System::ValueTuple<int, int>^ pos = safe_cast<System::ValueTuple<int, int>^>(pb->Tag);
        int row = pos->Item1, col = pos->Item2;

        // ---------- Режим расстановки ----------
        if (isPlacingMode) {
            if (placingPlayer == 1) {
                if (engine->GetRemainingShipsCount(1) == 0) {
                    MessageBox::Show("Все корабли расставлены! Нажмите 'Начать игру' для перехода к Игроку 2.", "Информация");
                    return;
                }
                int length = engine->GetRemainingLength(1, currentShipIndex);
                if (engine->PlaceShipManual(1, row, col, length, placementHorizontal)) {
                    engine->RemoveRemainingLengthAt(1, currentShipIndex);
                    if (currentShipIndex >= engine->GetRemainingShipsCount(1))
                        currentShipIndex = engine->GetRemainingShipsCount(1) - 1;
                    if (currentShipIndex < 0) currentShipIndex = 0;
                    UpdateBoards();
                    lblStatus->Text = GetPlacementStatus();
                }
                else {
                    lblStatus->Text = "Нельзя разместить здесь!";
                }
            }
            else {
                MessageBox::Show("Сейчас расставляет Игрок 2 (правое поле).", "Информация");
            }
            return;   // <-- ВАЖНО: выходим, чтобы не выполнять игровую логику
        }

        // Игра окончена
        if (engine->IsGameOver()) return;

        // ---- PvP: ход игрока 2 (он стреляет по левому полю) ----
        if (engine->GetMode() == GameMode::PvP && engine->GetCurrentPlayer() == 2) {
            bool hit, sunk; int sunkLen;
            if (engine->MakeMove(2, row, col, hit, sunk, sunkLen)) {
                hideShipsDuringDelay = true;
                UpdateBoards();
                Application::DoEvents();

                if (engine->IsGameOver()) {
                    hideShipsDuringDelay = false;
                    UpdateBoards();
                    lblStatus->Text = "Победил Игрок 2!";
                    lblTurn->Text = "";
                    String^ statsFileName = "stats_" + DateTime::Now.ToString("yyyy-MM-dd_HH-mm-ss") + ".txt";
                    engine->SaveStatistics(statsFileName);
                    return;
                }

                if (!hit) {
                    lblStatus->Text = "Промах! Ход переходит к Игроку 1.";
                    System::Threading::Thread::Sleep(3000);
                }
                else {
                    lblStatus->Text = "Попадание! Ещё ход.";
                }

                hideShipsDuringDelay = false;
                UpdateBoards();
            }
            else {
                lblStatus->Text = "Неверный ход!";
            }
            return;
        }

        // ---- PvC: ход компьютера (клик по левому полю бесполезен) ----
        if (engine->GetMode() != GameMode::PvP && engine->GetCurrentPlayer() == 2) {
            lblStatus->Text = "Сейчас ход компьютера...";
            return;
        }

        // ---- Игрок 1 (в любом режиме) не должен стрелять по своему полю ----
        if (engine->GetCurrentPlayer() == 1) {
            lblStatus->Text = "Это ваше поле! Стреляйте по полю противника (справа).";
            return;
        }

        // Запасной вариант
        lblStatus->Text = "Неверный ход!";
    }

    void CellPlayer2_Click(System::Object^ sender, System::EventArgs^ e)
    {
        if (!engine) return;
        PictureBox^ pb = safe_cast<PictureBox^>(sender);
        System::ValueTuple<int, int>^ pos = safe_cast<System::ValueTuple<int, int>^>(pb->Tag);
        int row = pos->Item1, col = pos->Item2;

        // ---------- Режим расстановки ----------
        if (isPlacingMode) {
            // Расстановка для игрока 2 (только в PvP)
            if (engine->GetMode() == GameMode::PvP && placingPlayer == 2) {
                if (engine->GetRemainingShipsCount(2) == 0) {
                    MessageBox::Show("Все корабли расставлены! Нажмите 'Начать игру'.", "Информация");
                    return;
                }
                int length = engine->GetRemainingLength(2, currentShipIndex);
                if (engine->PlaceShipManual(2, row, col, length, placementHorizontal)) {
                    engine->RemoveRemainingLengthAt(2, currentShipIndex);
                    if (currentShipIndex >= engine->GetRemainingShipsCount(2))
                        currentShipIndex = engine->GetRemainingShipsCount(2) - 1;
                    if (currentShipIndex < 0) currentShipIndex = 0;
                    UpdateBoards();
                    lblStatus->Text = GetPlacementStatus();
                }
                else {
                    lblStatus->Text = "Нельзя разместить здесь!";
                }
            }
            else {
                // Если не PvP или не ход игрока 2, то сообщаем, что сейчас расставляет игрок 1
                MessageBox::Show("Сейчас расставляет Игрок 1 (левое поле).", "Информация");
            }
            return;
        }

        if (engine->IsGameOver()) return;

        // ---- PvP: ход игрока 1 (он стреляет по правому полю) ----
        if (engine->GetMode() == GameMode::PvP && engine->GetCurrentPlayer() == 1) {
            bool hit, sunk; int sunkLen;
            if (engine->MakeMove(1, row, col, hit, sunk, sunkLen)) {
                // Включаем скрытие кораблей на время задержки
                hideShipsDuringDelay = true;
                UpdateBoards();
                Application::DoEvents();

                if (engine->IsGameOver()) {
                    hideShipsDuringDelay = false;
                    UpdateBoards();
                    lblStatus->Text = "Победил Игрок 1!";
                    lblTurn->Text = "";
                    String^ statsFileName = "stats_" + DateTime::Now.ToString("yyyy-MM-dd_HH-mm-ss") + ".txt";
                    engine->SaveStatistics(statsFileName);
                    return;
                }

                if (!hit) {
                    lblStatus->Text = "Промах! Ход переходит к Игроку 2.";
                    System::Threading::Thread::Sleep(3000);  // задержка 3 секунды
                }
                else {
                    lblStatus->Text = "Попадание! Ещё ход.";
                    // при попадании задержки нет
                }

                hideShipsDuringDelay = false;
                UpdateBoards();
            }
            else {
                lblStatus->Text = "Неверный ход!";
            }
            return;
        }

        // ---- PvC: ход игрока 1 (стреляет по полю компьютера) ----
        if (engine->GetMode() != GameMode::PvP && engine->GetCurrentPlayer() == 1) {
            bool hit, sunk; int sunkLen;
            if (engine->MakeMove(1, row, col, hit, sunk, sunkLen)) {
                UpdateBoards();
                if (engine->IsGameOver()) {
                    lblStatus->Text = "Вы победили!";
                    lblTurn->Text = "";
                    String^ statsFileName = "stats_" + DateTime::Now.ToString("yyyy-MM-dd_HH-mm-ss") + ".txt";
                    engine->SaveStatistics(statsFileName);
                    return;
                }
                if (!hit) {
                    lblStatus->Text = "Промах! Ход компьютера.";
                    DoComputerMove();   // компьютер делает свой ход
                }
                else {
                    lblStatus->Text = "Попадание! Ещё ход.";
                }
                UpdateBoards();
            }
            else {
                lblStatus->Text = "Неверный ход!";
            }
            return;
        }

        // ---- Ход игрока 2 (или компьютера) ----
        if (engine->GetCurrentPlayer() == 2) {
            if (engine->GetMode() == GameMode::PvP) {
                lblStatus->Text = "Сейчас ход Игрока 2. Стреляйте по левому полю!";
            }
            else {
                lblStatus->Text = "Сейчас ход компьютера...";
            }
            return;
        }

        // Запасной вариант
        lblStatus->Text = "Неверный ход!";
    }

    void DoComputerMove()
    {
        if (engine->IsGameOver() || engine->GetCurrentPlayer() == 1) return;

        while (!engine->IsGameOver() && engine->GetCurrentPlayer() == 2) {
            System::Threading::Thread::Sleep(500);

            bool hit, sunk; int sunkLen;
            System::ValueTuple<int, int> move;
            if (engine->GetMode() == GameMode::PvC_Medium)
                move = engine->GetRandomComputerMove();
            else
                move = engine->GetSmartComputerMove();

            if (move.Item1 == -1 && move.Item2 == -1) {
                bool allSunk = true;
                for (int i = 0; i < 10; i++) {
                    for (int j = 0; j < 10; j++) {
                        if (engine->GetBoard(2)[GetIndex(i, j)] == CellState::Ship) {
                            allSunk = false;
                            break;
                        }
                    }
                    if (!allSunk) break;
                }
                if (allSunk) {
                    engine->ForceGameOver(1);
                    lblStatus->Text = "Победа!";
                    lblTurn->Text = "";
                    String^ statsFileName = "stats_" + DateTime::Now.ToString("yyyy-MM-dd_HH-mm-ss") + ".txt";
                    engine->SaveStatistics(statsFileName);
                    UpdateBoards();
                    return;
                }
                else {
                    engine->ForceGameOver(1);
                    lblStatus->Text = "Ошибка! Победа игрока.";
                    lblTurn->Text = "";
                    String^ statsFileName = "stats_" + DateTime::Now.ToString("yyyy-MM-dd_HH-mm-ss") + ".txt";
                    engine->SaveStatistics(statsFileName);
                    UpdateBoards();
                    return;
                }
            }

            bool result = engine->MakeMove(2, move.Item1, move.Item2, hit, sunk, sunkLen);
            if (!result) continue;

            if (engine->GetMode() == GameMode::PvC_Hard)
                engine->NotifySmartResult(move.Item1, move.Item2, hit, sunk);

            UpdateBoards();

            if (engine->IsGameOver()) {
                lblStatus->Text = "Компьютер победил!";
                lblTurn->Text = "";
                String^ statsFileName = "stats_" + DateTime::Now.ToString("yyyy-MM-dd_HH-mm-ss") + ".txt";
                engine->SaveStatistics(statsFileName);
                return;
            }

            if (!hit) {
                lblStatus->Text = "Компьютер промахнулся. Ваш ход.";
                lblTurn->Text = "Ход: Игрок 1";
                UpdateBoards();
                break;
            }
            else {
                lblStatus->Text = "Компьютер попал! Ещё ход.";
                lblTurn->Text = "Ход: Компьютер";
                UpdateBoards();
                
            }
        }
    }

    void btnBack_Click(System::Object^ sender, System::EventArgs^ e) {
        if (MessageBox::Show("Выйти из игры без сохранения?", "Выход", MessageBoxButtons::YesNo) == System::Windows::Forms::DialogResult::Yes)
            this->Close();
    }

    void btnAutoPlace_Click(System::Object^ sender, System::EventArgs^ e) {
        if (isPlacingMode) {
            if (placingPlayer == 1) {
                engine->AutoPlaceShips(1);
                if (engine->GetMode() == GameMode::PvP) {
                    placingPlayer = 2;
                    currentShipIndex = 0;
                    lblStatus->Text = "Теперь расставляет Игрок 2 (правое поле)";
                    lblTurn->Text = "Режим расстановки (игрок 2)";
                    UpdateBoards();
                    return;
                }
                UpdateBoards();
                lblStatus->Text = "Авторасстановка выполнена. Нажмите 'Начать игру'.";
            }
            else if (placingPlayer == 2) {
                engine->AutoPlaceShips(2);
                currentShipIndex = 0;
                UpdateBoards();
                lblStatus->Text = "Авторасстановка для Игрока 2 выполнена. Нажмите 'Начать игру'.";
            }
        }
    }

    void btnStartGame_Click(System::Object^ sender, System::EventArgs^ e) {
        if (isPlacingMode) {
            if (engine->GetMode() == GameMode::PvP) {
                if (placingPlayer == 1) {
                    if (engine->GetRemainingShipsCount(1) != 0) {
                        MessageBox::Show("Игрок 1 ещё не расставил все корабли!", "Ошибка");
                        return;
                    }
                    placingPlayer = 2;
                    currentShipIndex = 0;
                    lblStatus->Text = "Теперь расставляет Игрок 2 (правое поле)";
                    lblTurn->Text = "Режим расстановки (игрок 2)";
                    UpdateBoards();
                    return;
                }
                else {
                    if (engine->GetRemainingShipsCount(2) != 0) {
                        MessageBox::Show("Игрок 2 ещё не расставил все корабли!", "Ошибка");
                        return;
                    }
                    isPlacingMode = false;
                    UpdateBoards();
                    lblStatus->Text = "Игра началась!";
                    lblTurn->Text = "Ход: Игрок 1";
                    return;
                }
            }
            else {
                if (engine->GetRemainingShipsCount(1) != 0) {
                    MessageBox::Show("Расставьте все корабли или используйте авторасстановку.", "Ошибка");
                    return;
                }
                engine->AutoPlaceShips(2);
                isPlacingMode = false;
                UpdateBoards();
                lblStatus->Text = "Игра началась!";
                lblTurn->Text = "Ход: Игрок 1";
            }
        }
    }

    void btnSave_Click(System::Object^ sender, System::EventArgs^ e) {
        SaveFileDialog^ dlg = gcnew SaveFileDialog();
        dlg->Filter = "Text files|*.txt";
        if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            engine->SaveGame(dlg->FileName);
            lblStatus->Text = "Игра сохранена.";
        }
    }

    void btnLoad_Click(System::Object^ sender, System::EventArgs^ e) {
        OpenFileDialog^ dlg = gcnew OpenFileDialog();
        dlg->Filter = "Text files|*.txt";
        if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            if (engine->LoadGame(dlg->FileName)) {
                isPlacingMode = false;
                UpdateBoards();
                lblStatus->Text = "Игра загружена.";
                String^ turn = engine->GetCurrentPlayer() == 1 ? "Игрок 1" : (engine->GetMode() == GameMode::PvP ? "Игрок 2" : "Компьютер");
                lblTurn->Text = "Ход: " + turn;
                if (engine->GetCurrentPlayer() == 2 && engine->GetMode() != GameMode::PvP && !engine->IsGameOver())
                    DoComputerMove();
            }
            else lblStatus->Text = "Ошибка загрузки.";
        }
    }

    void OnMouseRightClick(System::Object^ sender, MouseEventArgs^ e) {
        if (isPlacingMode) {
            placementHorizontal = !placementHorizontal;
            lblStatus->Text = GetPlacementStatus();
        }
    }

    void OnKeyDown(System::Object^ sender, KeyEventArgs^ e) {
        if (isPlacingMode && e->KeyCode == Keys::R) {
            placementHorizontal = !placementHorizontal;
            lblStatus->Text = GetPlacementStatus();
            e->Handled = true;
        }
    }

    void InitializeComponent(void)
    {
        this->tableLayoutPanel1 = gcnew TableLayoutPanel();
        this->tableLayoutPanel2 = gcnew TableLayoutPanel();
        this->btnBack = gcnew Button();
        this->btnAutoPlace = gcnew Button();
        this->btnStartGame = gcnew Button();
        this->btnSave = gcnew Button();
        this->btnLoad = gcnew Button();
        this->SuspendLayout();

        this->tableLayoutPanel1->ColumnCount = 10;
        for (int i = 0; i < 10; i++)
            this->tableLayoutPanel1->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 10));
        this->tableLayoutPanel1->Location = System::Drawing::Point(31, 102);
        this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
        this->tableLayoutPanel1->RowCount = 10;
        for (int i = 0; i < 10; i++)
            this->tableLayoutPanel1->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 10));
        this->tableLayoutPanel1->Size = System::Drawing::Size(350, 350);
        this->tableLayoutPanel1->TabIndex = 0;

        this->tableLayoutPanel2->ColumnCount = 10;
        for (int i = 0; i < 10; i++)
            this->tableLayoutPanel2->ColumnStyles->Add(gcnew ColumnStyle(SizeType::Percent, 10));
        this->tableLayoutPanel2->Location = System::Drawing::Point(438, 102);
        this->tableLayoutPanel2->Name = L"tableLayoutPanel2";
        this->tableLayoutPanel2->RowCount = 10;
        for (int i = 0; i < 10; i++)
            this->tableLayoutPanel2->RowStyles->Add(gcnew RowStyle(SizeType::Percent, 10));
        this->tableLayoutPanel2->Size = System::Drawing::Size(350, 350);
        this->tableLayoutPanel2->TabIndex = 1;

        this->btnBack->Location = System::Drawing::Point(31, 38);
        this->btnBack->Name = L"btnBack";
        this->btnBack->Size = System::Drawing::Size(75, 23);
        this->btnBack->TabIndex = 2;
        this->btnBack->Text = L"В меню";
        this->btnBack->UseVisualStyleBackColor = true;

        this->btnAutoPlace->Location = System::Drawing::Point(125, 38);
        this->btnAutoPlace->Name = L"btnAutoPlace";
        this->btnAutoPlace->Size = System::Drawing::Size(115, 23);
        this->btnAutoPlace->TabIndex = 3;
        this->btnAutoPlace->Text = L"Авторасстановка";
        this->btnAutoPlace->UseVisualStyleBackColor = true;

        this->btnStartGame->Location = System::Drawing::Point(259, 38);
        this->btnStartGame->Name = L"btnStartGame";
        this->btnStartGame->Size = System::Drawing::Size(75, 23);
        this->btnStartGame->TabIndex = 4;
        this->btnStartGame->Text = L"Начать игру";
        this->btnStartGame->UseVisualStyleBackColor = true;

        this->btnSave->Location = System::Drawing::Point(352, 38);
        this->btnSave->Name = L"btnSave";
        this->btnSave->Size = System::Drawing::Size(75, 23);
        this->btnSave->TabIndex = 5;
        this->btnSave->Text = L"Сохранить";
        this->btnSave->UseVisualStyleBackColor = true;

        this->btnLoad->Location = System::Drawing::Point(438, 38);
        this->btnLoad->Name = L"btnLoad";
        this->btnLoad->Size = System::Drawing::Size(75, 23);
        this->btnLoad->TabIndex = 6;
        this->btnLoad->Text = L"Загрузить";
        this->btnLoad->UseVisualStyleBackColor = true;

        this->ClientSize = System::Drawing::Size(821, 512);
        this->Controls->Add(this->btnLoad);
        this->Controls->Add(this->btnSave);
        this->Controls->Add(this->btnStartGame);
        this->Controls->Add(this->btnAutoPlace);
        this->Controls->Add(this->btnBack);
        this->Controls->Add(this->tableLayoutPanel2);
        this->Controls->Add(this->tableLayoutPanel1);
        this->Name = L"GameForm";
        this->ResumeLayout(false);

        this->btnBack->Click += gcnew EventHandler(this, &GameForm::btnBack_Click);
        this->btnAutoPlace->Click += gcnew EventHandler(this, &GameForm::btnAutoPlace_Click);
        this->btnStartGame->Click += gcnew EventHandler(this, &GameForm::btnStartGame_Click);
        this->btnSave->Click += gcnew EventHandler(this, &GameForm::btnSave_Click);
        this->btnLoad->Click += gcnew EventHandler(this, &GameForm::btnLoad_Click);

        this->MouseDown += gcnew MouseEventHandler(this, &GameForm::OnMouseRightClick);
        this->KeyDown += gcnew KeyEventHandler(this, &GameForm::OnKeyDown);
        this->KeyPreview = true;

        lblStatus = gcnew Label();
        lblStatus->Location = System::Drawing::Point(31, 470);
        lblStatus->Size = System::Drawing::Size(350, 30);
        lblStatus->Text = L"Статус";
        this->Controls->Add(lblStatus);

        lblTurn = gcnew Label();
        lblTurn->Location = System::Drawing::Point(438, 470);
        lblTurn->Size = System::Drawing::Size(350, 30);
        lblTurn->Text = L"Ход";
        this->Controls->Add(lblTurn);

        tablePlayer1 = this->tableLayoutPanel1;
        tablePlayer2 = this->tableLayoutPanel2;
    }

    public:
    // Свойства для тестов
    property GameEngine^ Engine {
        GameEngine^ get() { return engine; }
    }
    property bool IsPlacingMode {
        bool get() { return isPlacingMode; }
    }
    property String^ StatusText {
        String^ get() { return lblStatus->Text; }
    }
    property String^ TurnText {
        String^ get() { return lblTurn->Text; }
    }
    void AutoPlacePlayer(int player) {
        if (player == 1) {
            engine->AutoPlaceShips(1);
            lblStatus->Text = "Авторасстановка для игрока 1 выполнена. Нажмите 'Начать игру' для перехода к игроку 2.";
            UpdateBoards();
        }
        else if (player == 2) {
            engine->AutoPlaceShips(2);
            currentShipIndex = 0;
            UpdateBoards();
            lblStatus->Text = "Авторасстановка для игрока 2 выполнена. Нажмите 'Начать игру' для начала игры.";
        }
    }
    void ForcePlacingMode(bool value) {
        isPlacingMode = value;
    }
    void TestSetPlacingMode(bool mode) {
        isPlacingMode = mode;
    }
    void TestSetPlacingPlayer(int player) {
        placingPlayer = player;
    }
    // Метод для начала игры (эквивалент нажатия кнопки "Начать игру")
    void StartGameFromTest() {
        if (!isPlacingMode) return;

        if (engine->GetMode() == GameMode::PvP) {
            if (placingPlayer == 1) {
                if (engine->GetRemainingShipsCount(1) != 0) {
                    return; // не все корабли расставлены
                }
                // Переключаем на игрока 2
                placingPlayer = 2;
                currentShipIndex = 0;
                lblStatus->Text = "Теперь расставляет Игрок 2 (правое поле)";
                lblTurn->Text = "Режим расстановки (игрок 2)";
                UpdateBoards();
                return;
            }
            else { // placingPlayer == 2
                if (engine->GetRemainingShipsCount(2) != 0) {
                    return;
                }
                // Запускаем игру
                isPlacingMode = false;
                UpdateBoards();
                lblStatus->Text = "Игра началась!";
                lblTurn->Text = "Ход: Игрок 1";
                return;
            }
        }
        else { // PvC
            if (engine->GetRemainingShipsCount(1) != 0) {
                return;
            }
            engine->AutoPlaceShips(2);
            isPlacingMode = false;
            UpdateBoards();
            lblStatus->Text = "Игра началась!";
            lblTurn->Text = "Ход: Игрок 1";
        }
    }
    // Эмуляция клика по клетке
    void SimulateCellClick(int player, int row, int col) {
        // player 1 = левое поле (своё), player 2 = правое (поле противника)
        if (player == 1) {
            if (cellsPlayer1 != nullptr && row >= 0 && row < 10 && col >= 0 && col < 10) {
                PictureBox^ pb = cellsPlayer1[row * 10 + col];
                if (pb != nullptr) {
                    CellPlayer1_Click(pb, gcnew EventArgs());
                }
            }
        }
        else if (player == 2) {
            if (cellsPlayer2 != nullptr && row >= 0 && row < 10 && col >= 0 && col < 10) {
                PictureBox^ pb = cellsPlayer2[row * 10 + col];
                if (pb != nullptr) {
                    CellPlayer2_Click(pb, gcnew EventArgs());
                }
            }
        }
    }

    // Эмуляция нажатия кнопок
    void SimulateButtonClick(String^ buttonName) {
        if (buttonName == "AutoPlace") {
            btnAutoPlace->PerformClick();
        }
        else if (buttonName == "StartGame") {
            btnStartGame->PerformClick();
        }
        else if (buttonName == "Save") {
            btnSave->PerformClick();
        }
        else if (buttonName == "Load") {
            btnLoad->PerformClick();
        }
        else if (buttonName == "Back") {
            btnBack->PerformClick();
        }
    }
public:
    GameForm(GameMode gm) : mode(gm), hideShipsDuringDelay(false)
    {
        try {
            InitializeComponent();
            LoadImages();
            CreateCells(); 
        }
        catch (Exception^ ex) {
            MessageBox::Show("Ошибка в GameForm: " + ex->Message, "Критическая ошибка");
        }
    }

    void StartNewGame()
    {
        NewGame();
    }

    bool LoadFromFile(String^ filename)
    {
        try {
            if (!engine) {
                engine = gcnew GameEngine(mode);
            }
            if (engine->LoadGame(filename)) {
                isPlacingMode = false;
                UpdateBoards();
                lblStatus->Text = "Игра загружена.";
                String^ turn = engine->GetCurrentPlayer() == 1 ? "Игрок 1" : (engine->GetMode() == GameMode::PvP ? "Игрок 2" : "Компьютер");
                lblTurn->Text = "Ход: " + turn;
                this->Refresh();
                if (engine->GetCurrentPlayer() == 2 && engine->GetMode() != GameMode::PvP && !engine->IsGameOver())
                    DoComputerMove();
                return true;
            }
            return false;
        }
        catch (Exception^ ex) {
            MessageBox::Show("Ошибка в LoadFromFile:\n" + ex->Message + "\n\n" + ex->StackTrace, "Ошибка");
            return false;
        }
    }
};