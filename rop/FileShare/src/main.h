#pragma once
#include "wx/wx.h"
#include "peer.h"
#include <thread>
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/bmpbuttn.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/listbox.h>
#include <wx/treectrl.h>
#include <wx/dataview.h>
#include <wx/frame.h>

#define MAIN_WINDOW_ID 0

class Main : public wxFrame
{
public:

	Main(Peer& localpeer);
	Peer& localpeer;

	Main(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxPoint(-1, -1), const wxSize& size = wxSize(860, 416), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
	~Main();

private:
	void onClose(wxCloseEvent& evt);

private:
	wxBoxSizer* mainSizerV;

	bool running = false;
	std::thread thread;
	void main();

	int downloadReq = 0;
	std::vector<std::string> fileNamesToDownload;

protected:
	//GUI compoments
	wxPanel* peerPanel;
	wxBoxSizer* peerSizerV;
	wxListBox* m_peerListBox;
	wxPanel* filesPanel;
	wxListBox* m_fileListBox;
	wxStaticText* m_downloadedFileName;
	wxStaticText* m_downloadQueueLabel;
	wxGauge* m_gaugeDownloadProgress;
	wxMenuBar* m_mainMenuBar;
	wxMenu* m_menuConnections;
	wxMenu* m_menuSettings;

	//event functions
	void onPeerFileDClick(wxCommandEvent& event);
	void onPeerSelect(wxCommandEvent& event);

	void connectToPeer(wxCommandEvent& event);
	void connectToPool(wxCommandEvent& event);
	void disconnectFromAll(wxCommandEvent& event);
	void disconnectFromPeer(wxCommandEvent& event);

	void setDownloadDir(wxCommandEvent& event);
	void setUploadDir(wxCommandEvent& event);


	//Dialogs
	wxTextEntryDialog setIdDialog = wxTextEntryDialog(this, "Set your username", "Username Change", "");

	DECLARE_EVENT_TABLE()

};
