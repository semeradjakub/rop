#include "main.h"

BEGIN_EVENT_TABLE(Main, wxFrame)
	EVT_CLOSE(Main::onClose)
	
END_EVENT_TABLE()

Main::Main(Peer& localpeer) : wxFrame(nullptr, MAIN_WINDOW_ID, "ROP", wxPoint(100, 100), wxSize(550, 550)), localpeer(localpeer)
{
	this->SetMinSize(wxSize(500, 400));
	this->SetSize(this->GetMinSize());

	wxInitAllImageHandlers();

	mainSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* subSizer1;
	subSizer1 = new wxBoxSizer(wxVERTICAL);


	mainSizer->Add(subSizer1, 1, wxEXPAND, 5);

	menuPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	menuPanel->SetBackgroundColour(wxColour(255, 255, 255));

	wxBoxSizer* subSizer2;
	subSizer2 = new wxBoxSizer(wxVERTICAL);

	menubtnUser = new wxBitmapButton(menuPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(40, 40), wxBU_AUTODRAW | 0);

	menubtnUser->SetBitmap(wxBitmap(wxT("res/avatar.png"), wxBITMAP_TYPE_ANY));
	subSizer2->Add(menubtnUser, 0, wxALL, 0);

	menubtnNetwork = new wxBitmapButton(menuPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(40, 40), wxBU_AUTODRAW | 0);

	menubtnNetwork->SetBitmap(wxBitmap(wxT("res/network-hub.png"), wxBITMAP_TYPE_PNG));
	subSizer2->Add(menubtnNetwork, 0, wxALL, 0);

	menubtnSettings = new wxBitmapButton(menuPanel, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize(40, 40), wxBU_AUTODRAW | 0);

	menubtnSettings->SetBitmap(wxBitmap(wxT("res/setting.png"), wxBITMAP_TYPE_PNG));
	subSizer2->Add(menubtnSettings, 0, wxALL, 0);

	menuPanel->SetSizer(subSizer2);
	menuPanel->Layout();
	subSizer2->Fit(menuPanel);
	mainSizer->Add(menuPanel, 0, wxEXPAND | wxRIGHT, 1);

	optionPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	optionPanel->SetBackgroundColour(wxColour(255, 255, 255));

	mainSizer->Add(optionPanel, 1, wxEXPAND | wxRIGHT, 1);

	contentPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	contentPanel->SetBackgroundColour(wxColour(239, 239, 239));

	wxBoxSizer* subSizer3;
	subSizer3 = new wxBoxSizer(wxVERTICAL);


	contentPanel->SetSizer(subSizer3);
	contentPanel->Layout();
	subSizer3->Fit(contentPanel);
	mainSizer->Add(contentPanel, 5, wxEXPAND, 0);

	
	this->SetSizer(mainSizer);
	this->Layout();

	this->Centre(wxBOTH);
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