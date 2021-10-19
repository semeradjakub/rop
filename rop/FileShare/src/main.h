#pragma once
#include "wx/wx.h"
#include "peer.h"
#include <thread>

#define MAIN_WINDOW_ID 0

class Main : public wxFrame
{
public:
	Main(Peer& localpeer);
	~Main();
	Peer& localpeer;
private:
	void onClose(wxCloseEvent& evt);

private:
	wxBoxSizer* mainSizer;

protected:
	wxPanel* menuPanel;
	wxBitmapButton* menubtnUser;
	wxBitmapButton* menubtnNetwork;
	wxBitmapButton* menubtnSettings;
	wxPanel* optionPanel;
	wxPanel* contentPanel;

	DECLARE_EVENT_TABLE()
};
