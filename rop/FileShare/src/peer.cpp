#include "peer.h"

Peer::Peer()
{
}

Peer::~Peer()
{
}

void Peer::run()
{
	runServer();
	runClient();
}

