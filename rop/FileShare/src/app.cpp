#include "app.h"

wxIMPLEMENT_APP_CONSOLE(App);

App::App()
{

}

App::~App()
{

}

bool App::OnInit()
{
    mainFrame = new Main(localpeer);
    mainFrame->Show();
    localpeer.Run();
    return true;
}