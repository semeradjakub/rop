#include "app.h"

wxIMPLEMENT_APP_CONSOLE(App);

App::App()
{
    localpeer = new Peer("127.0.0.1");
}

App::~App()
{
    delete localpeer;
}

bool App::OnInit()
{
    mainFrame = new Main(*localpeer);
    mainFrame->Show();
    localpeer->start();
    return true;
}