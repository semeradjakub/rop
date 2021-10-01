#pragma once

#include "wx/wx.h"
#include "main.h"

class App : public wxApp
{
public:
	App();
	~App();

private:
	Main* baseFrame = nullptr;

public:
	virtual bool OnInit();
};

