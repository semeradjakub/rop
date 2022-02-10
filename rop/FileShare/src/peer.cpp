#include "peer.h"

Peer::Peer(std::string ip)
{
	WSAStartup(version, &wsaData);
	client = new Client(&peers, &localID);
	server = new Server(&peers, &localID);
	srand(time(0));
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

std::string Peer::generateRandomId(int length)
{
	std::string id = "";
	for (int i = 0; i < length; i++)
		id += (char)('a' + rand() % ('z' - 'a' - 1));
	return id;
}

bool Peer::DownloadFile(SOCKET& from, std::string fileName, uint8_t flags)
{
	std::string requestID = generateRandomId(16);
	client->downloadFile(from, fileName, requestID);
	return true;
}

bool Peer::GetPeerDirectoryContent(PeerInfo& peer)
{
	std::string requestID = generateRandomId(16);
	client->getDirectoryContent(peer, requestID);
	return true;
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