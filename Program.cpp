#include "MainForm.h"
#include "GameEngine1.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace SeaBattleCpp;

[STAThreadAttribute]
int main(array<String^>^ args)
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew SeaBattleCpp::MainForm());
    return 0;
}