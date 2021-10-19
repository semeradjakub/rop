#include "client.h"

Client::Client()
{
}

Client::~Client()
{
}

void Client::connectTo(std::string ip, short port)
{
	PeerInfo peer;
	peer.peerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	u_long mode = 1;
	ioctlsocket(peer.peerSock, FIONBIO, &mode);
	peer.peerAddr.sin_family = AF_INET;
	peer.peerAddr.sin_port = htons(port);
	peer.peerAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	
	connect(peer.peerSock, (sockaddr*)&peer.peerAddr, sizeof(peer.peerAddr));

	peers.push_back(peer);
}

void Client::disconnect(int peerIndex)
{
	closesocket(peers.at(peerIndex).peerSock);
	removePeer(peerIndex);
}