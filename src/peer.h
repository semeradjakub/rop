#pragma once
#include "server.h"
#include "client.h"
#include "peerinfo.h"

class Peer : public Server, Client
{
public:
	Peer();
	~Peer();
	void Run();
	void Send(SOCKET& to, std::string msg);
	void Connect(std::string ip, short port);
	void handlePeers();
	void Terminate(int peerIndex);

	std::vector<PeerInfo>& getPeerList();
	PeerInfo& getPeer(int index);
	SOCKET& getPeerSock(int index);
};
