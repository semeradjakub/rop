#pragma once
#include "wx/wx.h"
#include "main.h"

class App : public wxApp
{
public:
	App();
	~App();

private:
	Main* mainFrame = nullptr;
	Peer* localpeer;
public:
	virtual bool OnInit();
};

