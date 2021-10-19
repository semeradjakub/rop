#include "client.h"

Client::Client()
{
}

Client::~Client()
{
}

void Client::runClient()
{

}

void Client::connect(std::string ip, short port)
{
	PeerInfo peer;
	peer.peerAddr.sin_family = AF_INET;
	peer.peerAddr.sin_port = htons(port);
	peer.peerAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
}

void Client::disconnect(PeerInfo& peer)
{
	closesocket(peer.peerSock);
}
