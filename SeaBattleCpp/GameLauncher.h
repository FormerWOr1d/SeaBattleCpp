#pragma once
#include "GameMode.h"
#include "GameForm.h"

namespace SeaBattleCpp {
    using namespace System;
    using namespace System::Windows::Forms;

    public ref class GameLauncher abstract sealed {
    public:
        static void StartGame(GameMode mode) {
            GameForm^ game = gcnew GameForm(mode);
            game->ShowDialog();
            delete game;
        }
    };
}