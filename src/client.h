#pragma once
#include "networkbase.h"

class Client : public NetworkBase
{
protected:
	Client();
	~Client();
	void connectTo(std::string ip, short port);
	void disconnect(int peerIndex);
protected:
	char recvBuffer[4096]{ 0 };
};
