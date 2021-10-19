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
    mainFrame = new Main();
    mainFrame->Show();
    localpeer.run();
    return true;
}