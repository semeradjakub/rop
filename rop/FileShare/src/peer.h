#pragma once
#include "server.h"
#include "client.h"
#include "peerinfo.h"


class Peer : public Server, Client
{
public:
	Peer();
	~Peer();
	void run();
};
