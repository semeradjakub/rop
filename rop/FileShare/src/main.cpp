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
	filesSizerH->Add(m_fileListBox, 1, wxALL | wxEXPAND, 3);
	filesSizerV->Add(filesSizerH, 1, wxALL | wxEXPAND, 5);
	m_downloadedFileName = new wxStaticText(filesPanel, wxID_ANY, wxT("downloading: "), wxDefaultPosition, wxDefaultSize, 0);
	m_downloadedFileName->Wrap(-1);
	filesSizerV->Add(m_downloadedFileName, 0, wxALL, 3);
	m_gaugeDownloadProgress = new wxGauge(filesPanel, wxID_ANY, 10000, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
	m_gaugeDownloadProgress->SetValue(0);
	filesSizerV->Add(m_gaugeDownloadProgress, 0, wxALL | wxEXPAND, 3);
	filesPanel->SetSizer(filesSizerV);
	filesPanel->Layout();
	mainSizerV->Add(filesPanel, 2, wxEXPAND, 0);
	this->SetSizer(mainSizerV);
	this->Layout();
	m_mainMenuBar = new wxMenuBar(0);
	m_menuConnections = new wxMenu();
	wxMenuItem* m_menuItemConnectServ;
	m_menuItemConnectServ = new wxMenuItem(m_menuConnections, wxID_ANY, wxString(wxT("Connect via server")), wxEmptyString, wxITEM_NORMAL);
	m_menuConnections->Append(m_menuItemConnectServ);
	wxMenuItem* m_menuItemConnectDir;
	m_menuItemConnectDir = new wxMenuItem(m_menuConnections, wxID_ANY, wxString(wxT("Connect directly(only LAN)")), wxEmptyString, wxITEM_NORMAL);
	m_menuConnections->Append(m_menuItemConnectDir);
	wxMenuItem* m_menuItemDisconnectSel;
	m_menuItemDisconnectSel = new wxMenuItem(m_menuConnections, wxID_ANY, wxString(wxT("Disconnect(selected peer)")), wxEmptyString, wxITEM_NORMAL);
	m_menuConnections->Append(m_menuItemDisconnectSel);
	wxMenuItem* m_menuItemDisconnectAll;
	m_menuItemDisconnectAll = new wxMenuItem(m_menuConnections, wxID_ANY, wxString(wxT("Disconnect(all)")), wxEmptyString, wxITEM_NORMAL);
	m_menuConnections->Append(m_menuItemDisconnectAll);
	m_mainMenuBar->Append(m_menuConnections, wxT("CONNECTIONS"));
	m_menuSettings = new wxMenu();
	wxMenuItem* m_menuItemChangeDownload;
	m_menuItemChangeDownload = new wxMenuItem(m_menuSettings, wxID_ANY, wxString(wxT("set download directory")), wxEmptyString, wxITEM_NORMAL);
	m_menuSettings->Append(m_menuItemChangeDownload);
	wxMenuItem* m_menuItemChangeUpload;
	m_menuItemChangeUpload = new wxMenuItem(m_menuSettings, wxID_ANY, wxString(wxT("set shared directory")), wxEmptyString, wxITEM_NORMAL);
	m_menuSettings->Append(m_menuItemChangeUpload);
	m_mainMenuBar->Append(m_menuSettings, wxT("SETTINGS"));
	this->SetMenuBar(m_mainMenuBar);
	this->Centre(wxBOTH);

	// Connect Events
	m_peerListBox->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(Main::onPeerSelect), NULL, this);
	m_fileListBox->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(Main::onPeerFileDClick), NULL, this);
	m_menuSettings->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::setDownloadDir), this, m_menuItemChangeDownload->GetId());
	m_menuSettings->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::setUploadDir), this, m_menuItemChangeUpload->GetId());
	m_menuConnections->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::connectToPeer), this, m_menuItemConnectDir->GetId());
	m_menuConnections->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::disconnectFromPeer), this, m_menuItemDisconnectSel->GetId());
	m_menuConnections->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::disconnectFromAll), this, m_menuItemDisconnectAll->GetId());
	m_menuConnections->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Main::connectToPool), this, m_menuItemConnectServ->GetId());

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

	running = true;
	thread = std::thread(&Main::main, this);
}

Main::~Main()
{
	//Disconnect events
	m_peerListBox->Disconnect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(Main::onPeerSelect), NULL, this);
	m_fileListBox->Disconnect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(Main::onPeerFileDClick), NULL, this);
	running = false;
	thread.join();
}

void Main::main()
{
	while (running)
	{
		if (localpeer.addedPeers.size())
		{
			m_peerListBox->AppendString(wxString(localpeer.addedPeers.front()));
			localpeer.addedPeers.pop();
		}

		if (localpeer.deletedPeers.size())
		{
			int pos = m_peerListBox->FindString(wxString(localpeer.deletedPeers.front()), true);
			if (pos == m_peerListBox->GetSelection() && pos != -1)
				m_peerListBox->Deselect(pos);
			if(pos != -1)
				m_peerListBox->Delete(pos);
			localpeer.deletedPeers.pop();
		}
	}
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

void Main::onPeerSelect(wxCommandEvent& event)
{
	int selectedPeer = m_peerListBox->GetSelection();
	std::string id = m_peerListBox->GetString(selectedPeer).ToStdString();
	PeerInfo* peer = localpeer.GetPeerById(id);

	if (peer)
	{
		localpeer.GetPeerDirectoryContent(*peer, m_fileListBox);
	}
}

void Main::onPeerFileDClick(wxCommandEvent& event)
{
	int selectedPeer = m_peerListBox->GetSelection();
	std::string id = m_peerListBox->GetString(selectedPeer).ToStdString();
	PeerInfo* peer = localpeer.GetPeerById(id);
	if (peer)
	{
		std::string fileName = m_fileListBox->GetString(m_fileListBox->GetSelection()).ToStdString();
		std::string msg = "Download file " + fileName + " of size " + localpeer.getFileSizeByFileName(*peer, fileName) + "?";

			if (wxMessageBox(msg, "File download", wxYES_NO) == wxYES)
			{
				localpeer.DownloadFile(*peer, fileName);
			}
	}
}

void Main::connectToPeer(wxCommandEvent& event)
{
	wxTextEntryDialog ted(nullptr, "Enter IP address: ", "Connect");

	if (ted.ShowModal() == wxID_OK)
	{
		std::string ip = ted.GetValue().ToStdString();
		PeerInfo* newConnection = localpeer.GetPeerByIp(ip);
		if (newConnection == nullptr)
		{
			newConnection = localpeer.Connect(ip);

			if (newConnection != nullptr)
				localpeer.addedPeers.push(newConnection->id);
			else
				wxMessageBox("Connection failed!", "Error");
		}
		else
		{
			wxMessageBox("Already connected!", "Error");
		}
	}
}

void Main::connectToPool(wxCommandEvent& event)
{
	wxTextEntryDialog ted(nullptr, "Enter IP address: ", "Connect");
	if (ted.ShowModal() == wxID_OK)
	{
		std::string ip = ted.GetValue().ToStdString();
		localpeer.ConnectWan(ip);
	}
}

void Main::disconnectFromAll(wxCommandEvent& event)
{
	localpeer.DisconnectFromAll();
}

void Main::disconnectFromPeer(wxCommandEvent& event)
{
	int selectedPeer = m_peerListBox->GetSelection();
	
	if (selectedPeer != -1)
	{
		std::string id = m_peerListBox->GetString(selectedPeer).ToStdString();
		std::string ip = inet_ntoa(localpeer.GetPeerById(id)->peerHint.sin_addr);
		localpeer.deletedPeers.push(id);
		localpeer.Disconnect(ip);
	}
	else
	{
		wxMessageBox("No peer selected!", "Error");
	}
}

void Main::setDownloadDir(wxCommandEvent& event)
{
	wxDirDialog downloadDirDialog(nullptr, "select target download directory", "", wxDD_DIR_MUST_EXIST | wxDD_DEFAULT_STYLE);

	if (downloadDirDialog.ShowModal() == wxID_OK)
	{
		localpeer.setDownloadDir(downloadDirDialog.GetPath().ToStdString());
	}
}

void Main::setUploadDir(wxCommandEvent& event)
{
	wxDirDialog uploadDirDialog(nullptr, "select shared directory", "", wxDD_DIR_MUST_EXIST | wxDD_DEFAULT_STYLE);

	if (uploadDirDialog.ShowModal() == wxID_OK)
	{
		localpeer.setUploadDir(uploadDirDialog.GetPath().ToStdString());
	}
}
