#include "main.h"

BEGIN_EVENT_TABLE(Main, wxFrame)
	EVT_CLOSE(Main::onClose)
	
END_EVENT_TABLE()


Main::Main(Peer& localpeer) : wxFrame(nullptr, MAIN_WINDOW_ID, "ROP", wxPoint(100, 100), wxSize(750, 500)), localpeer(localpeer)
{
	//components
	this->SetSizeHints(wxSize(500, 400), wxSize(-1, -1));
	mainSizerV = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* mainSizerH;
	mainSizerH = new wxBoxSizer(wxVERTICAL);
	mainSizerH->SetMinSize(wxSize(0, 0));
	mainSizerV->Add(mainSizerH, 1, wxEXPAND, 5);
	peerPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1, -1), wxTAB_TRAVERSAL);
	peerPanel->SetBackgroundColour(wxColour(255, 255, 255));
	peerPanel->SetMaxSize(wxSize(500, -1));
	peerSizerV = new wxBoxSizer(wxVERTICAL);
	peerSizerV->SetMinSize(wxSize(0, 0));
	wxBoxSizer* peerSizerH;
	peerSizerH = new wxBoxSizer(wxHORIZONTAL);
	peerSizerH->SetMinSize(wxSize(100, -1));
	m_peerListBox = new wxListBox(peerPanel, wxID_ANY, wxPoint(0, 0), wxSize(-1, -1), 0, NULL, 0);
	m_peerListBox->SetMinSize(wxSize(100, 100));
	peerSizerH->Add(m_peerListBox, 1, wxEXPAND, 15);
	peerSizerV->Add(peerSizerH, 1, wxALL | wxEXPAND, 5);
	m_ipTextBox = new wxTextCtrl(peerPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	peerSizerV->Add(m_ipTextBox, 0, wxALL | wxEXPAND, 3);
	m_connectBtn = new wxButton(peerPanel, wxID_ANY, wxT("Connect"), wxPoint(-1, -1), wxSize(-1, -1), 0);
	m_connectBtn->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Arial")));
	peerSizerV->Add(m_connectBtn, 0, wxALL | wxEXPAND, 3);
	m_disconnectBtn = new wxButton(peerPanel, wxID_ANY, wxT("Disconnect"), wxDefaultPosition, wxDefaultSize, 0);
	m_disconnectBtn->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Arial")));
	peerSizerV->Add(m_disconnectBtn, 0, wxALL | wxEXPAND, 3);
	peerPanel->SetSizer(peerSizerV);
	peerPanel->Layout();
	peerSizerV->Fit(peerPanel);
	mainSizerV->Add(peerPanel, 1, wxALL | wxEXPAND, 1);
	filesPanel = new wxPanel(this, wxID_ANY, wxPoint(0, 0), wxSize(150, 150), wxTAB_TRAVERSAL);
	filesPanel->SetBackgroundColour(wxColour(239, 239, 239));
	filesPanel->SetMinSize(wxSize(150, 150));
	wxBoxSizer* filesSizerV;
	filesSizerV = new wxBoxSizer(wxVERTICAL);
	filesSizerV->SetMinSize(wxSize(0, 0));
	wxBoxSizer* filesSizerH;
	filesSizerH = new wxBoxSizer(wxHORIZONTAL);
	m_fileListBox = new wxListBox(filesPanel, wxID_ANY, wxDefaultPosition, wxSize(500, 500), 0, NULL, 0);
	m_fileListBox->SetMinSize(wxSize(500, 500));
	filesSizerH->Add(m_fileListBox, 1, wxALL | wxEXPAND, 5);
	filesSizerV->Add(filesSizerH, 1, wxALL | wxEXPAND, 5);
	filesPanel->SetSizer(filesSizerV);
	filesPanel->Layout();
	mainSizerV->Add(filesPanel, 2, wxEXPAND, 0);
	this->SetSizer(mainSizerV);
	this->Layout();
	this->Centre(wxBOTH);

	// Connect Events
	m_peerListBox->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(Main::onPeerSelect), NULL, this);
	m_connectBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::connectToPeer), NULL, this);
	m_disconnectBtn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::disconnectFromPeer), NULL, this);
	m_fileListBox->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(Main::onPeerFileDClick), NULL, this);


	std::string id;

	//if user pressed ok
	if (setIdDialog.ShowModal() == wxID_OK)
	{
		id = setIdDialog.GetValue().ToStdString();
	}
	
	//if user input is empty or user pressed cancel button
	if(id.length() == 0)
	{
		srand(time(NULL));
		std::string randomID = "";
		for (uint8_t i = 0; i < 12; i++)
			randomID += ((int)'a' + rand() % ((int)'z' - (int)'a'));
		id = randomID;
	}

	localpeer.setID(id);
	std::cout << "ID: " << localpeer.getID() << std::endl;
}


Main::~Main()
{
	//Disconnect events
	m_peerListBox->Disconnect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(Main::onPeerSelect), NULL, this);
	m_connectBtn->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::connectToPeer), NULL, this);
	m_disconnectBtn->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Main::disconnectFromPeer), NULL, this);
	m_fileListBox->Disconnect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(Main::onPeerFileDClick), NULL, this);
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

void Main::showUserSettings(wxCommandEvent& event)
{

}

void Main::showNetworkSettings(wxCommandEvent& event)
{

}

void Main::showGeneralSettings(wxCommandEvent& event)
{

}

void Main::onPeerSelect(wxCommandEvent& event)
{
	int selectedPeer = m_peerListBox->GetSelection();
	std::string id = m_peerListBox->GetString(selectedPeer).ToStdString();
	PeerInfo* peer = localpeer.GetPeerById(id);
	
	if (peer)
	{
		if (localpeer.GetPeerDirectoryContent(*peer))
			for (int i = 0; i < peer->files.size(); i++)
				m_fileListBox->AppendString(wxString(peer->files.at(i).fileName + ":(" + std::to_string(peer->files.at(i).fileSize) + " Bytes)"));
		else
			wxMessageBox("Failed to get files", "Error");
	}
	else
		wxMessageBox("Failed to get files", "Error");
}

void Main::onPeerFileDClick(wxCommandEvent& event)
{
	int selectedPeer = m_peerListBox->GetSelection();
	std::string id = m_peerListBox->GetString(selectedPeer).ToStdString();
	PeerInfo& peer = *localpeer.GetPeerById(id);
	std::string fileName = m_fileListBox->GetString(m_fileListBox->GetSelection()).ToStdString();
	fileName = fileName.substr(0, fileName.rfind(":"));

	if (localpeer.DownloadFile(peer.peerSock, fileName, 0))
		wxMessageBox("File downloaded!", "Message");
	else
		wxMessageBox("Failed to download file!", "Error");
}

void Main::connectToPeer(wxCommandEvent& event)
{
	std::string ip = m_ipTextBox->GetValue().ToStdString();
	PeerInfo* newConnection = localpeer.Connect(ip);

	if (newConnection != nullptr)
		m_peerListBox->AppendString(newConnection->id);
	else
		wxMessageBox("Connection failed!", "Error");
}

void Main::disconnectFromPeer(wxCommandEvent& event)
{
	int selectedPeer = m_peerListBox->GetSelection();
	
	if (selectedPeer != -1)
	{
		std::string id = m_peerListBox->GetString(selectedPeer).ToStdString();
		std::string ip = inet_ntoa(localpeer.GetPeerById(id)->peerHint.sin_addr);
		if (!localpeer.Disconnect(ip))
			wxMessageBox("Failed to disconnect", "Error");

		m_peerListBox->Delete(selectedPeer);
	}
}
