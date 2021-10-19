#pragma once
#include "wx/wx.h"
#include "main.h"
//memory leaks, send/recv file, gui
class App : public wxApp
{
public:
	App();
	~App();

private:
	Main* mainFrame = nullptr;
	Peer localpeer;
public:
	virtual bool OnInit();
};

