#pragma once
#include <winsock.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include "peerinfo.h"
#include "globals.h"

class Server
{
public:
	Server(std::vector<PeerInfo>* peers, std::string* localID);
	~Server();
	bool start();
	void acceptConnections();
private:
	//core
	void run();
	std::thread thread;
	SOCKET listenSock = 0;
	std::vector<PeerInfo>* peers;
	std::string* localID = nullptr;

private:
	//control
	bool running = false;
	bool newConnectionsAccepted = true; // = true(peers are going to be able to connect)

private:
	//communication
	static const int messageSize = 4;
	static const int dataBufferSize = 256;

	message m_disconnect = "\x68\x62\x79\x65";
	message m_welcome = "\x68\x65\x6C\x6F";
	message m_connect = "\x68\x63\x6F\x6E";
};
