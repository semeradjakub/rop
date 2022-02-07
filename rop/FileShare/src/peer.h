#pragma once
#include "server.h"
#include "client.h"
#include "peerinfo.h"
#include "globals.h"

/*
* Interface for GUI(main class)
*/
class Peer
{
public:
	Peer(std::string ip);
	~Peer();
	bool start();

	bool DownloadFile(SOCKET& from, std::string fileName, uint8_t flags);
	bool GetPeerDirectoryContent(PeerInfo& peer);
	PeerInfo* Connect(std::string& ip);
	bool Disconnect(std::string& ip);
	PeerInfo* GetPeerById(std::string& id);
	void setID(std::string localID);
	std::string getID();

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
