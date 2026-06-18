#pragma once
#include "GameMode.h"

namespace SeaBattleCpp {
    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;

    public ref class ChooseModeForm : public Form {
    public:
        property GameMode SelectedMode { GameMode get() { return selected; } }

        ChooseModeForm() { InitializeComponent(); }
    private:
        RadioButton^ rbPvP;
        RadioButton^ rbPvCMedium;
        RadioButton^ rbPvCHard;
        Button^ btnOK;
        Button^ btnCancel;
        GameMode selected;

        void InitializeComponent() {
            this->Text = L"Выбор режима";
            this->ClientSize = Drawing::Size(300, 250);
            this->StartPosition = FormStartPosition::CenterParent;
            this->FormBorderStyle = FormBorderStyle::FixedDialog;
            this->MaximizeBox = false;
            this->MinimizeBox = false;

            rbPvP = gcnew RadioButton();
            rbPvP->Text = L"Человек vs Человек";
            rbPvP->Location = Point(20, 30);
            rbPvP->Size = Drawing::Size(200, 30);
            rbPvP->Checked = true;
            this->Controls->Add(rbPvP);

            rbPvCMedium = gcnew RadioButton();
            rbPvCMedium->Text = L"Человек vs Компьютер (средний)";
            rbPvCMedium->Location = Point(20, 70);
            rbPvCMedium->Size = Drawing::Size(250, 30);
            this->Controls->Add(rbPvCMedium);

            rbPvCHard = gcnew RadioButton();
            rbPvCHard->Text = L"Человек vs Компьютер (сложный)";
            rbPvCHard->Location = Point(20, 110);
            rbPvCHard->Size = Drawing::Size(250, 30);
            this->Controls->Add(rbPvCHard);

            btnOK = gcnew Button();
            btnOK->Text = L"ИГРАТЬ";
            btnOK->Location = Point(40, 170);
            btnOK->Size = Drawing::Size(90, 30);
            btnOK->Click += gcnew EventHandler(this, &ChooseModeForm::btnOK_Click);
            this->Controls->Add(btnOK);

            btnCancel = gcnew Button();
            btnCancel->Text = L"ОТМЕНА";
            btnCancel->Location = Point(150, 170);
            btnCancel->Size = Drawing::Size(90, 30);
            btnCancel->Click += gcnew EventHandler(this, &ChooseModeForm::btnCancel_Click);
            this->Controls->Add(btnCancel);
        }

        void btnOK_Click(Object^ sender, EventArgs^ e) {
            if (rbPvP->Checked) selected = GameMode::PvP;
            else if (rbPvCMedium->Checked) selected = GameMode::PvC_Medium;
            else if (rbPvCHard->Checked) selected = GameMode::PvC_Hard;
            this->DialogResult = DialogResult::OK;
            Close();
        }

        void btnCancel_Click(Object^ sender, EventArgs^ e) {
            this->DialogResult = DialogResult::Cancel;
            Close();
        }
    };
}