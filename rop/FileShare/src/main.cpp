#include "main.h"

BEGIN_EVENT_TABLE(Main, wxFrame)
	EVT_CLOSE(Main::onClose)
END_EVENT_TABLE()

Main::Main() : wxFrame(nullptr, MAIN_WINDOW_ID, "ROP", wxPoint(100, 100), wxSize(550, 550))
{
	
}

Main::~Main()
{
	
}

void Main::onClose(wxCloseEvent& evt)
{
	if (evt.CanVeto())
	{
		if (wxMessageBox("Are you sure you want to quit?", "Quit", wxYES_NO) != wxYES)
		{
			evt.Veto();
			return;
		}
	}

	Destroy();
}
