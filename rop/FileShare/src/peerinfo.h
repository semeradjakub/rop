#pragma once
#include <Windows.h>
#include <string>
#include <map>
#include <queue>
#include "fileinfo.h"

typedef void(__thiscall* function)(SOCKET& peer, std::string& requestID);

struct Worker
{

	Worker(function handler, SOCKET& socket, std::string& requestID, void* caller)
	{
		if (handler != nullptr)
		{
			std::cout << "thread spawn\n";
			thread = std::thread(&handler, &caller, socket, requestID); //problem creating thread
		}
	}

	~Worker()
	{
		thread.join();
		std::cout << "thread destroyed\n";
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
