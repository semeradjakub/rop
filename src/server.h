#pragma once
#include "networkbase.h"

class Server : public NetworkBase
{
public:
	Server();
	~Server();
protected:
	void runServer();
	void handleConnections();
	void acceptConnections();
	void terminate(int peerIndex);
protected:
	SOCKET listenSock = 0;
};
