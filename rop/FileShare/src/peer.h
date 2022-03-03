#pragma once
#include <wx/listbox.h>
#include <charconv>
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
	void ConnectWan(std::string& ip);
	void Disconnect(std::string& ip);
	void DisconnectFromAll();
	PeerInfo* GetPeerById(std::string& id);
	PeerInfo* GetPeerByIp(std::string& ip);
	void setID(std::string localID);
	std::string getFileSizeByFileName(PeerInfo& peer, std::string fileName);
	std::string getID();
	void setDownloadDir(std::string path);
	void setUploadDir(std::string path);
	std::string getDownloadDir() { return client->getDownloadDir(); }
	std::string getUploadDir() { return client->getUploadDir(); }

	std::string generateRandomId(int length);

	std::vector<PeerInfo>& getPeerList();

private:
	WSAData wsaData;
	const WORD version = MAKEWORD(3, 0);
	std::vector<PeerInfo> peers;
	std::vector<std::string> requests;

public:
	//for GUI
	std::queue<std::string> addedPeers;
	std::queue<std::string> deletedPeers;
private:
	std::string localID = "";
	uint8_t defaultIdLength = 12;

private:
	Client* client = nullptr;
	Server* server = nullptr;
};
