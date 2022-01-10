#pragma once
#include <Windows.h>
#include <string>
#include "fileinfo.h"

#define LISTENPORT 12346
#define LOCALHOST "127.0.0.1"
#define NI_MAXHOST 1025
#define NI_MAXSERV 32

struct PeerInfo
{
	SOCKET peerSock;
	sockaddr_in peerHint;

	std::vector<FileInfo> files;
};