#include "App.h"

wxIMPLEMENT_APP_CONSOLE(App);

App::App()
{

}

App::~App()
{

}

bool App::OnInit()
{
    baseFrame = new Main();
    baseFrame->Show();
    FreeConsole();
    return true;
}
