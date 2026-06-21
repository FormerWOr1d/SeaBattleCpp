#pragma once
#include "GameMode.h"
#include "ChooseModeForm.h"
#include "GameLauncher.h"

namespace SeaBattleCpp {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class MainForm : public System::Windows::Forms::Form
	{
	public:
		MainForm(void)
		{
			InitializeComponent();
		}

	protected:
		~MainForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		System::Windows::Forms::Label^ label1;
		System::Windows::Forms::Button^ btnNewGame;
		System::Windows::Forms::Button^ btnExit;
		System::Windows::Forms::Button^ btnAbout;
		System::Windows::Forms::Button^ btnHelp;

		System::Windows::Forms::PictureBox^ pictureBoxBackground;
		System::ComponentModel::Container^ components;

		
		
	protected:


#pragma region Windows Form Designer generated code
		
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MainForm::typeid));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->btnNewGame = (gcnew System::Windows::Forms::Button());
			this->btnExit = (gcnew System::Windows::Forms::Button());
			this->btnAbout = (gcnew System::Windows::Forms::Button());
			this->btnHelp = (gcnew System::Windows::Forms::Button());
			this->pictureBoxBackground = (gcnew System::Windows::Forms::PictureBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxBackground))->BeginInit();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Bernard MT Condensed", 48, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->ForeColor = System::Drawing::Color::Red;
			this->label1->Location = System::Drawing::Point(35, 37);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(460, 76);
			this->label1->TabIndex = 0;
			this->label1->Text = L"THUNDER BATTLE";
			// 
			// btnNewGame
			// 
			this->btnNewGame->BackColor = System::Drawing::Color::Red;
			this->btnNewGame->FlatAppearance->BorderColor = System::Drawing::Color::Gold;
			this->btnNewGame->FlatAppearance->BorderSize = 2;
			this->btnNewGame->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->btnNewGame->Font = (gcnew System::Drawing::Font(L"Arial", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->btnNewGame->ForeColor = System::Drawing::Color::White;
			this->btnNewGame->Location = System::Drawing::Point(48, 243);
			this->btnNewGame->Name = L"btnNewGame";
			this->btnNewGame->Size = System::Drawing::Size(312, 50);
			this->btnNewGame->TabIndex = 2;
			this->btnNewGame->Text = L"НОВАЯ ИГРА";
			this->btnNewGame->UseVisualStyleBackColor = false;
			this->btnNewGame->Click += gcnew System::EventHandler(this, &MainForm::btnNewGame_Click);
			this->btnNewGame->MouseEnter += gcnew System::EventHandler(this, &MainForm::btn_MouseEnter);
			this->btnNewGame->MouseLeave += gcnew System::EventHandler(this, &MainForm::btn_MouseLeave);
			// 
			// btnExit
			// 
			this->btnExit->BackColor = System::Drawing::Color::Red;
			this->btnExit->FlatAppearance->BorderColor = System::Drawing::Color::Gold;
			this->btnExit->FlatAppearance->BorderSize = 2;
			this->btnExit->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->btnExit->Font = (gcnew System::Drawing::Font(L"Arial", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->btnExit->ForeColor = System::Drawing::Color::White;
			this->btnExit->Location = System::Drawing::Point(48, 467);
			this->btnExit->Name = L"btnExit";
			this->btnExit->Size = System::Drawing::Size(312, 50);
			this->btnExit->TabIndex = 9;
			this->btnExit->Text = L"ВЫХОД";
			this->btnExit->UseVisualStyleBackColor = false;
			this->btnExit->Click += gcnew System::EventHandler(this, &MainForm::btnExit_Click);
			this->btnExit->MouseEnter += gcnew System::EventHandler(this, &MainForm::btn_MouseEnter);
			this->btnExit->MouseLeave += gcnew System::EventHandler(this, &MainForm::btn_MouseLeave);
			// 
			// btnAbout
			// 
			this->btnAbout->BackColor = System::Drawing::Color::Red;
			this->btnAbout->FlatAppearance->BorderColor = System::Drawing::Color::Gold;
			this->btnAbout->FlatAppearance->BorderSize = 2;
			this->btnAbout->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->btnAbout->Font = (gcnew System::Drawing::Font(L"Arial", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->btnAbout->ForeColor = System::Drawing::Color::White;
			this->btnAbout->Location = System::Drawing::Point(48, 318);
			this->btnAbout->Name = L"btnAbout";
			this->btnAbout->Size = System::Drawing::Size(312, 50);
			this->btnAbout->TabIndex = 8;
			this->btnAbout->Text = L"О ПРОГРАММЕ";
			this->btnAbout->UseVisualStyleBackColor = false;
			this->btnAbout->Click += gcnew System::EventHandler(this, &MainForm::btnAbout_Click);
			this->btnAbout->MouseEnter += gcnew System::EventHandler(this, &MainForm::btn_MouseEnter);
			this->btnAbout->MouseLeave += gcnew System::EventHandler(this, &MainForm::btn_MouseLeave);
			// 
			// btnHelp
			// 
			this->btnHelp->BackColor = System::Drawing::Color::Red;
			this->btnHelp->FlatAppearance->BorderColor = System::Drawing::Color::Gold;
			this->btnHelp->FlatAppearance->BorderSize = 2;
			this->btnHelp->FlatStyle = System::Windows::Forms::FlatStyle::Popup;
			this->btnHelp->Font = (gcnew System::Drawing::Font(L"Arial", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->btnHelp->ForeColor = System::Drawing::Color::White;
			this->btnHelp->Location = System::Drawing::Point(48, 390);
			this->btnHelp->Name = L"btnHelp";
			this->btnHelp->Size = System::Drawing::Size(312, 50);
			this->btnHelp->TabIndex = 7;
			this->btnHelp->Text = L"РУКОВОДСТВО";
			this->btnHelp->UseVisualStyleBackColor = false;
			this->btnHelp->Click += gcnew System::EventHandler(this, &MainForm::btnHelp_Click);
			this->btnHelp->MouseEnter += gcnew System::EventHandler(this, &MainForm::btn_MouseEnter);
			this->btnHelp->MouseLeave += gcnew System::EventHandler(this, &MainForm::btn_MouseLeave);
			// 
			// pictureBoxBackground
			// 
			this->pictureBoxBackground->Dock = System::Windows::Forms::DockStyle::Fill;
			this->pictureBoxBackground->Image = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"pictureBoxBackground.Image")));
			this->pictureBoxBackground->Location = System::Drawing::Point(0, 0);
			this->pictureBoxBackground->Name = L"pictureBoxBackground";
			this->pictureBoxBackground->Size = System::Drawing::Size(900, 600);
			this->pictureBoxBackground->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->pictureBoxBackground->TabIndex = 10;
			this->pictureBoxBackground->TabStop = false;
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::MidnightBlue;
			this->ClientSize = System::Drawing::Size(900, 600);
			this->Controls->Add(this->btnExit);
			this->Controls->Add(this->btnAbout);
			this->Controls->Add(this->btnHelp);
			this->Controls->Add(this->btnNewGame);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->pictureBoxBackground);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = L"MainForm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"Морской Бой";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->pictureBoxBackground))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		private: System::Void btnNewGame_Click(System::Object^ sender, System::EventArgs^ e) {
			ChooseModeForm^ choose = gcnew ChooseModeForm();
			if (choose->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
				GameLauncher::StartGame(choose->SelectedMode);
			}
			delete choose;
		}

		System::Void btnLoadGame_Click(System::Object^ sender, System::EventArgs^ e) {
			GameLauncher::LoadGame();
		}

		System::Void btnHelp_Click(System::Object^ sender, System::EventArgs^ e) {
			MessageBox::Show(
				L"ПРАВИЛА ИГРЫ «МОРСКОЙ БОЙ»\n\n"
				L"1. Поле 10×10 клеток.\n"
				L"2. Расстановка кораблей (4,3,2,1 палуб).\n"
				L"3. По очереди стреляйте по клеткам противника.\n"
				L"4. Попадание — дополнительный ход.\n"
				L"5. Режимы: PvP, PvC (средний), PvC (сложный).\n"
				L"6. Статистика сохраняется в stats.txt.\n"
				L"7. Сохранение и загрузка игры через меню.",
				L"Руководство пользователя",
				MessageBoxButtons::OK,
				MessageBoxIcon::Information
			);
		}

		System::Void btnAbout_Click(System::Object^ sender, System::EventArgs^ e) {
			MessageBox::Show(
				L"МОРСКОЙ БОЙ\nВерсия 1.0\nГруппа: ИСТб-25-1\nАвторы: Аникушкин Данил, Анашкин Данил\nИрНИТУ 2026",
				L"О программе",
				MessageBoxButtons::OK,
				MessageBoxIcon::Information
			);
		}

		System::Void btnExit_Click(System::Object^ sender, System::EventArgs^ e) {
			if (MessageBox::Show(L"Вы действительно хотите выйти?", L"Выход", MessageBoxButtons::YesNo) == System::Windows::Forms::DialogResult::Yes) {
				Application::Exit();
			}
		}
	private: System::Void btn_MouseEnter(System::Object^ sender, System::EventArgs^ e) {
		Button^ btn = safe_cast<Button^>(sender);
		btn->Size = System::Drawing::Size(320, 55);
		btn->Font = gcnew System::Drawing::Font(L"Arial", 26, System::Drawing::FontStyle::Bold);
		btn->BackColor = System::Drawing::Color::White;
		btn->ForeColor = System::Drawing::Color::Black;
	}

	private: System::Void btn_MouseLeave(System::Object^ sender, System::EventArgs^ e) {
		Button^ btn = safe_cast<Button^>(sender);
		btn->Size = System::Drawing::Size(312, 50);
		btn->Font = gcnew System::Drawing::Font(L"Arial", 24, System::Drawing::FontStyle::Bold);
		btn->BackColor = System::Drawing::Color::Red;
		btn->ForeColor = System::Drawing::Color::White;
		
	}
};
}
