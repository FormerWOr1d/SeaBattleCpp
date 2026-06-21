#include "MainForm.h"
// ... остальные включения ...

void RunApplication()
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew SeaBattleCpp::MainForm());
}

