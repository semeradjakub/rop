#pragma once
#include "wx/wx.h"
#include "peer.h"
#include <thread>

#define MAIN_WINDOW_ID 0

class Main : public wxFrame
{
public:

	Main();
	~Main();

private:

	void onClose(wxCloseEvent& evt);

	DECLARE_EVENT_TABLE()
};
