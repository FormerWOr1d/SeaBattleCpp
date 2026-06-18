#pragma once
#include "GameMode.h"
#include "GameForm.h"

using namespace System;
using namespace System::Windows::Forms;

public ref class GameLauncher abstract sealed {
public:
    static void StartGame(GameMode mode) {
        GameForm^ game = gcnew GameForm(mode);
        game->StartNewGame();
        game->ShowDialog();
    }

    static void LoadGame() {
        OpenFileDialog^ dlg = gcnew OpenFileDialog();
        dlg->Filter = "Text files|*.txt";
        dlg->Title = "Выберите файл сохранения";
        if (dlg->ShowDialog() == System::Windows::Forms::DialogResult::OK) {
            try {
                GameForm^ game = gcnew GameForm(GameMode::PvP);
                if (game->LoadFromFile(dlg->FileName)) {
                    game->ShowDialog();
                }
                else {
                    MessageBox::Show("Не удалось загрузить игру. Файл повреждён или имеет неверный формат.", "Ошибка загрузки");
                    delete game;
                }
            }
            catch (Exception^ ex) {
                MessageBox::Show("Исключение в GameLauncher::LoadGame:\n" + ex->Message + "\n\n" + ex->StackTrace, "Критическая ошибка");
            }
        }
    }
};