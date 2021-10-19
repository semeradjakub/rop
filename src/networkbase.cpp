#include "networkbase.h"

std::vector<PeerInfo> NetworkBase::peers;
std::thread NetworkBase::clientThread;
std::thread NetworkBase::serverThread;

NetworkBase::NetworkBase()
{
	WSAStartup(version, &wsaData);
}

NetworkBase::~NetworkBase()
{
	if (serverThread.joinable())
		serverThread.join();
	if (clientThread.joinable())
		clientThread.join();
	WSACleanup();
}

void NetworkBase::addPeer(PeerInfo& peer)
{
	peers.push_back(peer);
}

void NetworkBase::removePeer(int index)
{
	peers.erase(peers.begin() + index);
}

