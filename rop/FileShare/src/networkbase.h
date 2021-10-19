#pragma once
#include <winsock.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include "peerinfo.h"

#define LISTENPORT 55555
#define LOCALHOST "127.0.0.1"
#define NI_MAXHOST 1025
#define NI_MAXSERV 32

class NetworkBase
{
public:
	NetworkBase();
	~NetworkBase();
protected:
	static std::thread clientThread;
	static std::thread serverThread;
	static inline WSAData wsaData;
	static inline WORD version = MAKEWORD(3, 0);
	static std::vector<PeerInfo> peers;
protected:
	void addPeer(PeerInfo& peer);
	void removePeer(int index);
};