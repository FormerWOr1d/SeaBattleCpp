#pragma once
#include "GameEngine.h"
#include <msclr\marshal_cppstd.h>

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
    // ---------- Игровой движок ----------
    GameEngine^ engine;
    GameMode mode;
    bool isPlacingMode;
    int currentShipIndex;
    bool placementHorizontal;
    int placingPlayer;
    const int cellSize = 35;

    // ---------- Элементы управления (убедитесь, что имена совпадают с конструктором) ----------
    TableLayoutPanel^ tablePlayer1;
    TableLayoutPanel^ tablePlayer2;
    Button^ btnBack;
    Button^ btnAutoPlace;
    Button^ btnStartGame;
    Button^ btnSave;
    Button^ btnLoad;
    Label^ lblStatus;
    Label^ lblTurn;

    // ---------- Массивы клеток ----------
    array<array<PictureBox^, 10>, 10>^ cellsPlayer1;
    array<array<PictureBox^, 10>, 10>^ cellsPlayer2;

    // ---------- Картинки ----------
    Image^ imgWater;
    Image^ imgShip;
    Image^ imgHit;
private: System::Windows::Forms::TableLayoutPanel^ tableLayoutPanel1;
private: System::Windows::Forms::TableLayoutPanel^ tableLayoutPanel2;
private: System::Windows::Forms::Button^ button1;
private: System::Windows::Forms::Button^ button2;
private: System::Windows::Forms::Button^ button3;
private: System::Windows::Forms::Button^ button4;
private: System::Windows::Forms::Button^ button5;
       Image^ imgMiss;

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
            // Цветные заливки-заглушки
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

    void CreateCells()
    {
        if (cellsPlayer1 != nullptr) return; // уже созданы
        cellsPlayer1 = gcnew array<array<PictureBox^, 10>, 10>(10);
        cellsPlayer2 = gcnew array<array<PictureBox^, 10>, 10>(10);
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                PictureBox^ pb1 = gcnew PictureBox();
                pb1->Size = Drawing::Size(cellSize, cellSize);
                pb1->BorderStyle = BorderStyle::FixedSingle;
                pb1->SizeMode = PictureBoxSizeMode::StretchImage;
                pb1->Tag = gcnew Tuple<int, int>(i, j);
                pb1->Click += gcnew EventHandler(this, &GameForm::CellPlayer1_Click);
                cellsPlayer1[i][j] = pb1;
                tablePlayer1->Controls->Add(pb1, j, i);

                PictureBox^ pb2 = gcnew PictureBox();
                pb2->Size = Drawing::Size(cellSize, cellSize);
                pb2->BorderStyle = BorderStyle::FixedSingle;
                pb2->SizeMode = PictureBoxSizeMode::StretchImage;
                pb2->Tag = gcnew Tuple<int, int>(i, j);
                pb2->Click += gcnew EventHandler(this, &GameForm::CellPlayer2_Click);
                cellsPlayer2[i][j] = pb2;
                tablePlayer2->Controls->Add(pb2, j, i);
            }
        }
    }

    void UpdateBoards()
    {
        if (!engine) return;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                CellState state1 = engine->GetBoard(1)[i][j];
                bool showShip1 = (state1 == CellState::Ship) && (!isPlacingMode || (isPlacingMode && placingPlayer == 1));
                cellsPlayer1[i][j]->Image = GetImageForState(state1, showShip1);

                CellState state2 = engine->GetBoard(2)[i][j];
                bool showShip2 = (state2 == CellState::Ship) && (!isPlacingMode || (isPlacingMode && placingPlayer == 2));
                cellsPlayer2[i][j]->Image = GetImageForState(state2, showShip2);
            }
        }
        if (!isPlacingMode && !engine->IsGameOver()) {
            String^ turn = engine->GetCurrentPlayer() == 1 ? "Игрок 1" : (engine->GetMode() == GameMode::PvP ? "Игрок 2" : "Компьютер");
            lblTurn->Text = "Ход: " + turn;
        }
    }

    String^ GetPlacementStatus()
    {
        int length = engine->GetShipLengths()[currentShipIndex];
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
        CreateCells();
        UpdateBoards();
        lblStatus->Text = "Расставьте корабли для Игрока 1 (левое поле)";
        lblTurn->Text = "Режим расстановки (игрок 1)";
        btnStartGame->Enabled = true;
        btnAutoPlace->Enabled = true;
    }

    // ---------- Обработчики ----------
    void CellPlayer1_Click(System::Object^ sender, System::EventArgs^ e)
    {
        PictureBox^ pb = safe_cast<PictureBox^>(sender);
        Tuple<int, int>^ pos = safe_cast<Tuple<int, int>^>(pb->Tag);
        int row = pos->Item1, col = pos->Item2;

        if (isPlacingMode) {
            if (placingPlayer == 1) {
                if (currentShipIndex >= engine->GetShipLengths().size()) {
                    MessageBox::Show("Все корабли расставлены! Переключитесь на Игрока 2 через 'Начать игру'.", "Информация");
                    return;
                }
                int length = engine->GetShipLengths()[currentShipIndex];
                if (engine->PlaceShipManual(1, row, col, length, placementHorizontal)) {
                    currentShipIndex++;
                    UpdateBoards();
                    if (currentShipIndex >= engine->GetShipLengths().size())
                        lblStatus->Text = "Все корабли Игрока 1 расставлены! Нажмите 'Начать игру' для перехода к Игроку 2.";
                    else
                        lblStatus->Text = GetPlacementStatus();
                }
                else lblStatus->Text = "Нельзя разместить здесь!";
            }
            else {
                MessageBox::Show("Сейчас расставляет Игрок 2 (правое поле).", "Информация");
            }
        }
        else if (!engine->IsGameOver() && engine->GetCurrentPlayer() == 2 && engine->GetMode() == GameMode::PvP) {
            bool hit, sunk; int sunkLen;
            if (engine->MakeMove(2, row, col, hit, sunk, sunkLen)) {
                UpdateBoards();
                if (engine->IsGameOver()) { lblStatus->Text = "Победил Игрок 2"; lblTurn->Text = ""; return; }
                if (!hit) UpdateBoards();
                else lblStatus->Text = "Попадание! Ещё ход.";
            }
            else lblStatus->Text = "Неверный ход!";
        }
    }

    void CellPlayer2_Click(System::Object^ sender, System::EventArgs^ e)
    {
        PictureBox^ pb = safe_cast<PictureBox^>(sender);
        Tuple<int, int>^ pos = safe_cast<Tuple<int, int>^>(pb->Tag);
        int row = pos->Item1, col = pos->Item2;

        if (isPlacingMode) {
            if (engine->GetMode() == GameMode::PvP && placingPlayer == 2) {
                if (currentShipIndex >= engine->GetShipLengths().size()) {
                    MessageBox::Show("Все корабли расставлены! Нажмите 'Начать игру'.", "Информация");
                    return;
                }
                int length = engine->GetShipLengths()[currentShipIndex];
                if (engine->PlaceShipManual(2, row, col, length, placementHorizontal)) {
                    currentShipIndex++;
                    UpdateBoards();
                    if (currentShipIndex >= engine->GetShipLengths().size())
                        lblStatus->Text = "Все корабли Игрока 2 расставлены! Нажмите 'Начать игру'.";
                    else
                        lblStatus->Text = GetPlacementStatus();
                }
                else lblStatus->Text = "Нельзя разместить здесь!";
            }
            else {
                MessageBox::Show("Сейчас расставляет Игрок 1 (левое поле).", "Информация");
            }
            return;
        }

        if (engine->IsGameOver()) return;
        if (engine->GetCurrentPlayer() == 1) {
            bool hit, sunk; int sunkLen;
            if (engine->MakeMove(1, row, col, hit, sunk, sunkLen)) {
                UpdateBoards();
                if (engine->IsGameOver()) { lblStatus->Text = "Победил Игрок 1"; lblTurn->Text = ""; return; }
                if (!hit) {
                    if (engine->GetMode() != GameMode::PvP && engine->GetCurrentPlayer() == 2)
                        DoComputerMove();
                    else
                        UpdateBoards();
                }
                else lblStatus->Text = "Попадание! Ещё ход.";
            }
            else lblStatus->Text = "Неверный ход!";
        }
    }

    void DoComputerMove()
    {
        if (engine->IsGameOver() || engine->GetCurrentPlayer() == 1) return;

        while (!engine->IsGameOver() && engine->GetCurrentPlayer() == 2) {
            lblStatus->Text = "Компьютер думает...";
            UpdateBoards();
            System::Threading::Thread::Sleep(400);

            bool hit, sunk; int sunkLen;
            std::pair<int, int> move;
            if (engine->GetMode() == GameMode::PvC_Medium)
                move = engine->GetRandomComputerMove();
            else
                move = engine->GetSmartComputerMove();

            if (move.first == -1) {
                bool allSunk = true;
                for (int i = 0; i < 10; i++) for (int j = 0; j < 10; j++)
                    if (engine->GetBoard(2)[i][j] == CellState::Ship) allSunk = false;
                if (allSunk) {
                    engine->ForceGameOver(1);
                    lblStatus->Text = "Победа игрока! Все корабли уничтожены.";
                    lblTurn->Text = "";
                    UpdateBoards();
                    return;
                }
                else {
                    engine->ForceGameOver(1);
                    lblStatus->Text = "Ошибка: компьютер не может ходить. Победа игрока.";
                    lblTurn->Text = "";
                    UpdateBoards();
                    return;
                }
            }

            bool result = engine->MakeMove(2, move.first, move.second, hit, sunk, sunkLen);
            if (!result) continue;

            if (engine->GetMode() == GameMode::PvC_Hard)
                engine->NotifySmartResult(move.first, move.second, hit, sunk);

            UpdateBoards();

            if (engine->IsGameOver()) {
                lblStatus->Text = "Компьютер победил!";
                lblTurn->Text = "";
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
            }
        }
    }

    // ---------- Обработчики кнопок ----------
    void btnBack_Click(System::Object^ sender, System::EventArgs^ e) {
        if (MessageBox::Show("Выйти из игры без сохранения?", "Выход", MessageBoxButtons::YesNo) == DialogResult::Yes)
            this->Close();
    }

    void btnAutoPlace_Click(System::Object^ sender, System::EventArgs^ e) {
        if (isPlacingMode) {
            if (placingPlayer == 1) {
                engine->AutoPlaceShips(1);
                currentShipIndex = engine->GetShipLengths().size();
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
                currentShipIndex = engine->GetShipLengths().size();
                UpdateBoards();
                lblStatus->Text = "Авторасстановка для Игрока 2 выполнена. Нажмите 'Начать игру'.";
            }
        }
    }

    void btnStartGame_Click(System::Object^ sender, System::EventArgs^ e) {
        if (isPlacingMode) {
            if (engine->GetMode() == GameMode::PvP) {
                if (placingPlayer == 1) {
                    if (currentShipIndex < engine->GetShipLengths().size()) {
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
                    if (currentShipIndex < engine->GetShipLengths().size()) {
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
                if (currentShipIndex < engine->GetShipLengths().size()) {
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
        if (dlg->ShowDialog() == DialogResult::OK) {
            engine->SaveGame(dlg->FileName);
            lblStatus->Text = "Игра сохранена.";
        }
    }

    void btnLoad_Click(System::Object^ sender, System::EventArgs^ e) {
        OpenFileDialog^ dlg = gcnew OpenFileDialog();
        dlg->Filter = "Text files|*.txt";
        if (dlg->ShowDialog() == DialogResult::OK) {
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
        if (isPlacingMode && e->Button == System::Windows::Forms::MouseButtons::Right) {
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

#pragma region Windows Form Designer generated code
    /// <summary>
    /// Обязательный метод для поддержки конструктора.
    /// </summary>
    void InitializeComponent(void)
    {
        this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
        this->tableLayoutPanel2 = (gcnew System::Windows::Forms::TableLayoutPanel());
        this->button1 = (gcnew System::Windows::Forms::Button());
        this->button2 = (gcnew System::Windows::Forms::Button());
        this->button3 = (gcnew System::Windows::Forms::Button());
        this->button4 = (gcnew System::Windows::Forms::Button());
        this->button5 = (gcnew System::Windows::Forms::Button());
        this->SuspendLayout();
        // 
        // tableLayoutPanel1
        // 
        this->tableLayoutPanel1->ColumnCount = 10;
        this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel1->Location = System::Drawing::Point(31, 102);
        this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
        this->tableLayoutPanel1->RowCount = 10;
        this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel1->Size = System::Drawing::Size(350, 350);
        this->tableLayoutPanel1->TabIndex = 0;
        // 
        // tableLayoutPanel2
        // 
        this->tableLayoutPanel2->ColumnCount = 10;
        this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel2->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent,
            10)));
        this->tableLayoutPanel2->Location = System::Drawing::Point(438, 102);
        this->tableLayoutPanel2->Name = L"tableLayoutPanel2";
        this->tableLayoutPanel2->RowCount = 10;
        this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel2->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 10)));
        this->tableLayoutPanel2->Size = System::Drawing::Size(350, 350);
        this->tableLayoutPanel2->TabIndex = 1;
        // 
        // button1
        // 
        this->button1->Location = System::Drawing::Point(31, 38);
        this->button1->Name = L"button1";
        this->button1->Size = System::Drawing::Size(75, 23);
        this->button1->TabIndex = 2;
        this->button1->Text = L"В меню";
        this->button1->UseVisualStyleBackColor = true;
        // 
        // button2
        // 
        this->button2->Location = System::Drawing::Point(125, 38);
        this->button2->Name = L"button2";
        this->button2->Size = System::Drawing::Size(115, 23);
        this->button2->TabIndex = 3;
        this->button2->Text = L"Авторасстановка";
        this->button2->UseVisualStyleBackColor = true;
        // 
        // button3
        // 
        this->button3->Location = System::Drawing::Point(259, 38);
        this->button3->Name = L"button3";
        this->button3->Size = System::Drawing::Size(75, 23);
        this->button3->TabIndex = 4;
        this->button3->Text = L"Начать игру";
        this->button3->UseVisualStyleBackColor = true;
        // 
        // button4
        // 
        this->button4->Location = System::Drawing::Point(352, 38);
        this->button4->Name = L"button4";
        this->button4->Size = System::Drawing::Size(75, 23);
        this->button4->TabIndex = 5;
        this->button4->Text = L"Сохранить";
        this->button4->UseVisualStyleBackColor = true;
        // 
        // button5
        // 
        this->button5->Location = System::Drawing::Point(438, 38);
        this->button5->Name = L"button5";
        this->button5->Size = System::Drawing::Size(75, 23);
        this->button5->TabIndex = 6;
        this->button5->Text = L"Загрузить";
        this->button5->UseVisualStyleBackColor = true;
        // 
        // GameForm
        // 
        this->ClientSize = System::Drawing::Size(821, 512);
        this->Controls->Add(this->button5);
        this->Controls->Add(this->button4);
        this->Controls->Add(this->button3);
        this->Controls->Add(this->button2);
        this->Controls->Add(this->button1);
        this->Controls->Add(this->tableLayoutPanel2);
        this->Controls->Add(this->tableLayoutPanel1);
        this->Name = L"GameForm";
        this->ResumeLayout(false);

    }
#pragma endregion

public:
    GameForm(GameMode gm) : mode(gm)
    {
        try {
            InitializeComponent();
            LoadImages();
            NewGame();
        }
        catch (Exception^ ex) {
            MessageBox::Show("Ошибка в GameForm: " + ex->Message, "Критическая ошибка");
        }
    }