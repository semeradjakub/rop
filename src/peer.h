#pragma once
#include "server.h"
#include "client.h"
#include "peerinfo.h"

class Peer
{
public:
	Peer(std::string ip);
	~Peer();
	bool start();

	void SendFile(SOCKET& to, uint8_t flags);
	void DownloadFile(SOCKET& from, std::string fileName, uint8_t flags);
	void Connect(std::string ip, short port);
	void Disconnect(int peerIndex);
	void Disconnect(SOCKET& from);

	std::vector<PeerInfo>& getPeerList();
	PeerInfo& getPeer(int index);
	SOCKET& getPeerSock(int index);

private:
	WSAData wsaData;
	const WORD version = MAKEWORD(3, 0);
	std::vector<PeerInfo> peers;
private:
	Client* client = nullptr;
	Server* server = nullptr;
};
