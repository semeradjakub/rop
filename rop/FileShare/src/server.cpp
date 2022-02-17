#include "server.h"

Server::Server(std::vector<PeerInfo>* peers, std::string* localID)
{
	this->peers = peers;
	this->localID = localID;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in listenHint;
	listenHint.sin_family = AF_INET;
	listenHint.sin_port = htons(LISTENPORT);
	listenHint.sin_addr.S_un.S_addr = inet_addr(LOCALHOST);
	bind(listenSock, (sockaddr*)&listenHint, sizeof(listenHint));
	listen(listenSock, SOMAXCONN);
}

Server::~Server()
{
	running = false;
	thread.join();
}

/*
 creates server thread
*/
bool Server::start()
{
	running = true;
	u_long nonblocking = 1;
	ioctlsocket(listenSock, FIONBIO, &nonblocking);

	thread = std::thread(&Server::run, this);
	return true;
}

void Server::run()
{
	while (running)
	{
		Sleep(1);
		//if access is allowed, accept incoming connections
		if(newConnectionsAccepted)
			acceptConnections();
	}
}

/*
 Accepts ingress connections and store these connections in peers list
*/
void Server::acceptConnections()
{
	PeerInfo newPeer;
	SOCKET newPeerSock;
	sockaddr_in newPeerHint;
	int peerSize = sizeof(newPeerHint);
	int bytesReceived = 0;
	char data[dataBufferSize];
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	memset(host, 0, NI_MAXHOST);
	memset(service, 0, NI_MAXSERV);

	newPeerSock = accept(listenSock, (sockaddr*)&newPeerHint, &peerSize);

	u_long blocking = 0;
	ioctlsocket(newPeerSock, FIONBIO, &blocking);

	if (newPeerSock != SOCKET_ERROR)
	{
		recv(newPeerSock, data, dataBufferSize, 0);
		if (std::string(data, messageSize) == m_connect)
		{
			send(newPeerSock, m_welcome.c_str(), messageSize, 0);
			bytesReceived = recv(newPeerSock, data, dataBufferSize, 0);
			send(newPeerSock, localID->c_str(), localID->length(), 0);
			if (bytesReceived > 0)
				newPeer.id = std::string(data, bytesReceived);
			newPeer.peerHint = newPeerHint;
			newPeer.peerSock = newPeerSock;
			u_long nonblocking = 1;
			ioctlsocket(newPeer.peerSock, FIONBIO, &nonblocking);
			newPeer.available = true;
			peers->push_back(newPeer);
		}
	}
}