#pragma once
#include <Windows.h>
#include <string>
#include <map>
#include <queue>
#include "fileinfo.h"

struct NetThreadManager
{
	struct Worker
	{
		//receive buffer
		std::vector<std::string> buffer;
		//running thread
		std::thread thread;
	};
	
	std::map<std::string, Worker*> workers;
};

struct PeerInfo
{
	bool available = true;
	SOCKET peerSock;
	sockaddr_in peerHint;
	std::string id = "";
	std::vector<FileInfo> files;

	NetThreadManager threadManager;
};
