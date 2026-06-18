#pragma once
#include "ChooseModeForm.h"
#include "GameLauncher.h"

namespace SeaBattleCpp {
    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;

    public ref class MainForm : public Form {
    public:
        MainForm() { InitializeComponent(); }
    private:
        Button^ btnNewGame;
        Button^ btnExit;
        Label^ label1;

        void InitializeComponent() {
            this->Text = L"Морской бой";
            this->ClientSize = Drawing::Size(400, 300);
            this->StartPosition = FormStartPosition::CenterScreen;
            this->BackColor = Color::Navy;

            label1 = gcnew Label();
            label1->Text = L"МОРСКОЙ БОЙ";
            label1->Font = gcnew Font(L"Arial", 24, FontStyle::Bold);
            label1->ForeColor = Color::Gold;
            label1->Location = Point(80, 40);
            label1->Size = Drawing::Size(240, 50);
            this->Controls->Add(label1);

            btnNewGame = gcnew Button();
            btnNewGame->Text = L"НОВАЯ ИГРА";
            btnNewGame->Location = Point(100, 120);
            btnNewGame->Size = Drawing::Size(200, 40);
            btnNewGame->Click += gcnew EventHandler(this, &MainForm::btnNewGame_Click);
            this->Controls->Add(btnNewGame);

            btnExit = gcnew Button();
            btnExit->Text = L"ВЫХОД";
            btnExit->Location = Point(100, 200);
            btnExit->Size = Drawing::Size(200, 40);
            btnExit->Click += gcnew EventHandler(this, &MainForm::btnExit_Click);
            this->Controls->Add(btnExit);
        }

        void btnNewGame_Click(Object^ sender, EventArgs^ e) {
            ChooseModeForm^ choose = gcnew ChooseModeForm();
            if (choose->ShowDialog() == DialogResult::OK) {
                GameLauncher::StartGame(choose->SelectedMode);
            }
            delete choose;
        }

        void btnExit_Click(Object^ sender, EventArgs^ e) {
            Application::Exit();
        }
    };
}