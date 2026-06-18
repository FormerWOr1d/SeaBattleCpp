#pragma once
#include "GameMode.h"

namespace SeaBattleCpp {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    public ref class ChooseModeForm : public System::Windows::Forms::Form
    {
    public:
        property GameMode SelectedMode { GameMode get() { return selected; } }

        ChooseModeForm(void)
        {
            InitializeComponent();
        }

    protected:
        ~ChooseModeForm()
        {
            if (components)
                delete components;
        }

    private:
        System::Windows::Forms::Label^ label1;
        System::Windows::Forms::RadioButton^ rbPvP;
        System::Windows::Forms::RadioButton^ rbPvCMedium;
        System::Windows::Forms::RadioButton^ rbPvCHard;
        System::Windows::Forms::PictureBox^ pictureBox1;
        System::Windows::Forms::Button^ btnOK;
        System::Windows::Forms::Button^ btnCancel;
        GameMode selected;
        System::ComponentModel::Container^ components;

        void InitializeComponent(void)
        {
            System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(ChooseModeForm::typeid));
            this->label1 = (gcnew System::Windows::Forms::Label());
            this->rbPvP = (gcnew System::Windows::Forms::RadioButton());
            this->rbPvCMedium = (gcnew System::Windows::Forms::RadioButton());
            this->rbPvCHard = (gcnew System::Windows::Forms::RadioButton());
            this->btnOK = (gcnew System::Windows::Forms::Button());
            this->btnCancel = (gcnew System::Windows::Forms::Button());
            this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->BeginInit();
            this->SuspendLayout();
            // 
            // label1
            // 
            this->label1->AutoSize = true;
            this->label1->BackColor = System::Drawing::Color::Navy;
            this->label1->Font = (gcnew System::Drawing::Font(L"Franklin Gothic Heavy", 20.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->label1->ForeColor = System::Drawing::Color::Red;
            this->label1->Location = System::Drawing::Point(100, 32);
            this->label1->Name = L"label1";
            this->label1->Size = System::Drawing::Size(316, 34);
            this->label1->TabIndex = 0;
            this->label1->Text = L"ВЫБОР РЕЖИМА ИГРЫ";
            // 
            // rbPvP
            // 
            this->rbPvP->AutoSize = true;
            this->rbPvP->Checked = true;
            this->rbPvP->Font = (gcnew System::Drawing::Font(L"Franklin Gothic Heavy", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(204)));
            this->rbPvP->ForeColor = System::Drawing::Color::White;
            this->rbPvP->Location = System::Drawing::Point(30, 220);
            this->rbPvP->Name = L"rbPvP";
            this->rbPvP->Size = System::Drawing::Size(216, 28);
            this->rbPvP->TabIndex = 1;
            this->rbPvP->TabStop = true;
            this->rbPvP->Text = L"Человек VS Человек";
            this->rbPvP->UseVisualStyleBackColor = true;
            // 
            // rbPvCMedium
            // 
            this->rbPvCMedium->AutoSize = true;
            this->rbPvCMedium->Font = (gcnew System::Drawing::Font(L"Franklin Gothic Heavy", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(204)));
            this->rbPvCMedium->ForeColor = System::Drawing::Color::White;
            this->rbPvCMedium->Location = System::Drawing::Point(30, 254);
            this->rbPvCMedium->Name = L"rbPvCMedium";
            this->rbPvCMedium->Size = System::Drawing::Size(337, 28);
            this->rbPvCMedium->TabIndex = 2;
            this->rbPvCMedium->Text = L"Человек VS Компьютер (средний)";
            this->rbPvCMedium->UseVisualStyleBackColor = true;
            // 
            // rbPvCHard
            // 
            this->rbPvCHard->AutoSize = true;
            this->rbPvCHard->Font = (gcnew System::Drawing::Font(L"Franklin Gothic Heavy", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(204)));
            this->rbPvCHard->ForeColor = System::Drawing::Color::White;
            this->rbPvCHard->Location = System::Drawing::Point(30, 288);
            this->rbPvCHard->Name = L"rbPvCHard";
            this->rbPvCHard->Size = System::Drawing::Size(345, 28);
            this->rbPvCHard->TabIndex = 3;
            this->rbPvCHard->Text = L"Человек VS Компьютер (сложный)";
            this->rbPvCHard->UseVisualStyleBackColor = true;
            // 
            // btnOK
            // 
            this->btnOK->BackColor = System::Drawing::Color::MidnightBlue;
            this->btnOK->Font = (gcnew System::Drawing::Font(L"Franklin Gothic Heavy", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(204)));
            this->btnOK->ForeColor = System::Drawing::Color::Red;
            this->btnOK->Location = System::Drawing::Point(30, 326);
            this->btnOK->Name = L"btnOK";
            this->btnOK->Size = System::Drawing::Size(100, 30);
            this->btnOK->TabIndex = 5;
            this->btnOK->Text = L"ИГРАТЬ";
            this->btnOK->UseVisualStyleBackColor = false;
            this->btnOK->Click += gcnew System::EventHandler(this, &ChooseModeForm::btnOK_Click);
            // 
            // btnCancel
            // 
            this->btnCancel->BackColor = System::Drawing::Color::MidnightBlue;
            this->btnCancel->Font = (gcnew System::Drawing::Font(L"Franklin Gothic Heavy", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(204)));
            this->btnCancel->ForeColor = System::Drawing::Color::Red;
            this->btnCancel->Location = System::Drawing::Point(156, 326);
            this->btnCancel->Name = L"btnCancel";
            this->btnCancel->Size = System::Drawing::Size(100, 30);
            this->btnCancel->TabIndex = 6;
            this->btnCancel->Text = L"НАЗАД";
            this->btnCancel->UseVisualStyleBackColor = false;
            this->btnCancel->Click += gcnew System::EventHandler(this, &ChooseModeForm::btnCancel_Click);
            // 
            // pictureBox1
            // 
            this->pictureBox1->Dock = System::Windows::Forms::DockStyle::Fill;
            this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBox1.Image")));
            this->pictureBox1->Location = System::Drawing::Point(0, 0);
            this->pictureBox1->Name = L"pictureBox1";
            this->pictureBox1->Size = System::Drawing::Size(484, 361);
            this->pictureBox1->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
            this->pictureBox1->TabIndex = 7;
            this->pictureBox1->TabStop = false;
            // 
            // ChooseModeForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->BackColor = System::Drawing::Color::MidnightBlue;
            this->ClientSize = System::Drawing::Size(484, 361);
            this->Controls->Add(this->btnCancel);
            this->Controls->Add(this->btnOK);
            this->Controls->Add(this->rbPvCHard);
            this->Controls->Add(this->rbPvCMedium);
            this->Controls->Add(this->rbPvP);
            this->Controls->Add(this->label1);
            this->Controls->Add(this->pictureBox1);
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
            this->Name = L"ChooseModeForm";
            this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
            this->Text = L"Выбор режима Игры";
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBox1))->EndInit();
            this->ResumeLayout(false);
            this->PerformLayout();
            this->btnOK->Click += gcnew System::EventHandler(this, &ChooseModeForm::btnOK_Click);
            this->btnCancel->Click += gcnew System::EventHandler(this, &ChooseModeForm::btnCancel_Click);
        }

        System::Void btnOK_Click(System::Object^ sender, System::EventArgs^ e) {
            if (rbPvP->Checked)
                selected = GameMode::PvP;
            else if (rbPvCMedium->Checked)
                selected = GameMode::PvC_Medium;
            else if (rbPvCHard->Checked)
                selected = GameMode::PvC_Hard;
            else {
                MessageBox::Show(L"Выберите режим.", L"Внимание");
                return;
            }
            this->DialogResult = System::Windows::Forms::DialogResult::OK;
            Close();
        }

        System::Void btnCancel_Click(System::Object^ sender, System::EventArgs^ e) {
            this->DialogResult = System::Windows::Forms::DialogResult::Cancel;
            Close();
        }
    };
}