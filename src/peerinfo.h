#pragma once
#include <Windows.h>
#include <string>

#define LISTENPORT 12345
#define LOCALHOST "127.0.0.1"
#define NI_MAXHOST 1025
#define NI_MAXSERV 32

struct PeerInfo
{
	SOCKET peerSock;
	sockaddr_in peerHint;
};