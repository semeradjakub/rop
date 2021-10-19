#pragma once
#include "networkbase.h"

class Client : public NetworkBase
{
protected:
	Client();
	~Client();
	void runClient();
	void connect(std::string ip, short port);
	void disconnect(PeerInfo& peer);
};
