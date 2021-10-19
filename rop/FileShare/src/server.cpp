#include "server.h"

Server::Server()
{
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(LISTENPORT);
	serverAddr.sin_addr.S_un.S_addr = inet_addr(LOCALHOST);
	bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr));
	listen(listenSock, SOMAXCONN);
}

Server::~Server()
{
}

void Server::runServer()
{
	serverThread = std::thread(&Server::handleConnections, this);
}

void Server::handleConnections()
{
	while (true)
	{
		acceptConnections();
		Sleep(10);
	}
}

void Server::acceptConnections()
{
	PeerInfo newPeer;
	SOCKET newPeerSock;
	sockaddr_in newPeerAddr;
	int peerSize = sizeof(newPeerAddr);
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	memset(host, 0, NI_MAXHOST);
	memset(service, 0, NI_MAXSERV);

	newPeerSock = accept(listenSock, (sockaddr*)&newPeerAddr, &peerSize);
	std::cout << inet_ntoa(newPeerAddr.sin_addr) << ":" << ntohs(newPeerAddr.sin_port) << "\n";
	newPeer.peerAddr = newPeerAddr;
	newPeer.peerSock = newPeerSock;
	peers.push_back(newPeer);
}

void Server::terminate(PeerInfo& peer)
{
	closesocket(peer.peerSock);
}
