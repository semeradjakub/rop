#pragma once
#include <Windows.h>
#include <string>
#include "fileinfo.h"

struct PeerInfo
{
	bool available = false;
	SOCKET peerSock;
	sockaddr_in peerHint;
	std::string id = "";
	std::vector<FileInfo> files;
};
