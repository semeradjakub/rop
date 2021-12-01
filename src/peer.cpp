#include "peer.h"

Peer::Peer(std::string ip)
{
	WSAStartup(version, &wsaData);
	client = new Client(&peers);
	server = new Server(&peers);
}

Peer::~Peer()
{
	delete client;
	delete server;
	WSACleanup();
}

bool Peer::start()
{
	return (server->start() && client->start());
}

void Peer::SendFile(SOCKET& to, uint8_t flags)
{
	client->sendFile(to);
}

void Peer::DownloadFile(SOCKET& from, std::string fileName, uint8_t flags)
{
	client->downloadFile(from, fileName);
}

void Peer::Connect(std::string ip, short port)
{
	client->Connect(ip, port);
}

void Peer::Disconnect(int peerIndex)
{
	client->Disconnect(peerIndex);
}

void Peer::Disconnect(SOCKET& from)
{
	client->Disconnect(from);
}

std::vector<PeerInfo>& Peer::getPeerList()
{
	return peers;
}

PeerInfo& Peer::getPeer(int index)
{
	return peers.at(index);
}

SOCKET& Peer::getPeerSock(int index)
{
	return peers.at(index).peerSock;
}
