#pragma once
#include <wx/listbox.h>
#include "server.h"
#include "client.h"
#include "peerinfo.h"
#include "globals.h"

/*
 Interface for GUI(main class)
*/
class Peer
{
public:
	Peer(std::string ip);
	~Peer();
	bool start();

	bool DownloadFile(PeerInfo& peer, std::string fileName);
	bool GetPeerDirectoryContent(PeerInfo& peer, wxListBox* target);
	PeerInfo* Connect(std::string& ip);
	bool Disconnect(std::string& ip);
	PeerInfo* GetPeerById(std::string& id);
	void setID(std::string localID);
	std::string getID();

	std::string generateRandomId(int length);

	std::vector<PeerInfo>& getPeerList();

private:
	WSAData wsaData;
	const WORD version = MAKEWORD(3, 0);
	std::vector<PeerInfo> peers;

private:
	std::string localID = "";
	uint8_t defaultIdLength = 12;

private:
	Client* client = nullptr;
	Server* server = nullptr;
};
