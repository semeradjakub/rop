#pragma once
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#pragma comment(lib, "ws2_32.lib")
#include "peerinfo.h"

#define LISTENPORT 42069
#define LOCALHOST "127.0.0.1"
#define NI_MAXHOST 1025
#define NI_MAXSERV 32

class NetworkBase
{
public:
	NetworkBase();
	~NetworkBase();
protected:
	static std::vector<PeerInfo> peers;
	static std::thread clientThread;
	static std::thread serverThread;
	static inline WSAData wsaData;
	static inline WORD version = MAKEWORD(3, 0);
protected:
	virtual void send();
	virtual void receive();
};