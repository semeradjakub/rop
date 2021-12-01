#include "server.h"

Server::Server(std::vector<PeerInfo>* peers)
{
	this->peers = peers;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in listenHint;
	listenHint.sin_family = AF_INET;
	listenHint.sin_port = htons(LISTENPORT);
	listenHint.sin_addr.S_un.S_addr = inet_addr(LOCALHOST);
	bind(listenSock, (sockaddr*)&listenHint, sizeof(listenHint));
	listen(listenSock, SOMAXCONN);
}

//deletes main server thread
Server::~Server()
{
	running = false;
	thread.join();
}

//creates main server thread
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
		acceptConnections();
	}
}

void Server::acceptConnections()
{
	PeerInfo newPeer;
	SOCKET newPeerSock;
	sockaddr_in newPeerHint;
	int peerSize = sizeof(newPeerHint);
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	memset(host, 0, NI_MAXHOST);
	memset(service, 0, NI_MAXSERV);

	newPeerSock = accept(listenSock, (sockaddr*)&newPeerHint, &peerSize);

	if (newPeerSock != SOCKET_ERROR)
	{
		std::cout << inet_ntoa(newPeerHint.sin_addr) << ":" << ntohs(newPeerHint.sin_port) << "\n";
		newPeer.peerHint = newPeerHint;
		newPeer.peerSock = newPeerSock;
		peers->push_back(newPeer);
	}
}

