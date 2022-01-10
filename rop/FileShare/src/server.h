#pragma once
#include <winsock.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include "peerinfo.h"

class Server
{
public:
	Server(std::vector<PeerInfo>* peers);
	~Server();
	bool start();
	void acceptConnections();
private:
	bool running = false;
	std::thread thread;
	void run();
	std::vector<PeerInfo>* peers;
	SOCKET listenSock = 0;
};
