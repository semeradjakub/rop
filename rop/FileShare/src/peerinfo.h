#pragma once
#include <Windows.h>
#include <string>

struct PeerInfo
{
	SOCKET peerSock;
	sockaddr_in peerAddr;
	std::string id;
};