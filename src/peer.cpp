#include "peer.h"

Peer::Peer()
{
}

Peer::~Peer()
{
}

void Peer::Run()
{
	runServer();
	clientThread = std::thread(&Peer::handlePeers, this);
}

void Peer::Send(SOCKET& to, std::string msg)
{
	send(to, msg.c_str(), msg.length() + 1, 0);
}

void Peer::Connect(std::string ip, short port)
{
	connectTo(ip, port);
}

void Peer::Terminate(int peerIndex)
{
	terminate(peerIndex);
}

std::vector<PeerInfo>& Peer::getPeerList()
{
	return peers;
}

PeerInfo& Peer::getPeer(int index)
{
	return peers.at(index);
}

SOCKET& Peer::getPeerSock(int peerindex)
{
	return peers.at(peerindex).peerSock;
}

void Peer::handlePeers()
{
	while (true)
	{
		for (int i = 0; i < peers.size(); i++)
		{
			memset(recvBuffer, 0, 4096);
			int bytes = recv(peers.at(i).peerSock, recvBuffer, 4096, 0);
			if (bytes > 0 && recvBuffer[0] != -1)
			{
				std::cout << "Peer #" << i << " : " << std::string(recvBuffer, 0, bytes);
				Send(peers.at(i).peerSock, std::string(recvBuffer, 0, bytes));
			}
		}

		Sleep(10);
	}
}
