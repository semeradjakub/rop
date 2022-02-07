#include "peer.h"

Peer::Peer(std::string ip)
{
	WSAStartup(version, &wsaData);
	client = new Client(&peers, &localID);
	server = new Server(&peers, &localID);
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

bool Peer::DownloadFile(SOCKET& from, std::string fileName, uint8_t flags)
{
	return client->downloadFile(from, fileName);
}

bool Peer::GetPeerDirectoryContent(PeerInfo& peer)
{
	return client->getDirectoryContent(peer);
}

PeerInfo* Peer::Connect(std::string& ip)
{
	return client->Connect(ip, 55667);
}

bool Peer::Disconnect(std::string& ip)
{
	return client->Disconnect(ip);
}

PeerInfo* Peer::GetPeerById(std::string& id)
{
	PeerInfo* peer = nullptr;
	for (int i = 0; i < peers.size(); i++)
		if (peers.at(i).id == id)
		{
			peer = &peers.at(i);
			break;
		}
	return peer;
}

void Peer::setID(std::string localID)
{
	this->localID = localID;
}

std::string Peer::getID()
{
	return localID;
}

std::vector<PeerInfo>& Peer::getPeerList()
{
	return peers;
}