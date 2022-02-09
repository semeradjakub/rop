#pragma once
#include <Windows.h>
#include <string>
#include <map>
#include <queue>
#include "fileinfo.h"

typedef void (*function)(SOCKET &peer);

struct Worker
{

	Worker(function handler, SOCKET& socket)
	{
		if (handler != nullptr)
		{
			std::cout << "thread spawn\n";
		}
	}

	~Worker()
	{

	}

	std::queue<std::string> receiveBuffer;
	std::thread thread;
};

struct PeerInfo
{
	bool available = true;
	SOCKET peerSock;
	sockaddr_in peerHint;
	std::string id = "";
	std::vector<FileInfo> files;

	std::map<std::string, Worker*> workers;
};
