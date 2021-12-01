#include "client.h"

Client::Client(std::vector<PeerInfo>* peers)
{
	this->peers = peers;
}

//deletes main client thread
Client::~Client()
{
	running = false;
	thread.join();
}

//creates main client thread
bool Client::start()
{
	running = true;
	thread = std::thread(&Client::run, this);
	return true;
}

//client loop
void Client::run()
{
	/*std::string fileName = "";
	std::cout << "CLIENT\nEnter download directory: ";
	std::cin >> downloadDir;

	while (!Connect(LOCALHOST, 11111));
	Sleep(20);

	while (running)
	{
		std::cin >> fileName;
		downloadFile(peers->at(0).peerSock, fileName);
	}

	char recvBuffer[dataBufferSize];
	int bytesRecv = 0;

	std::cout << "SERVER\nEnter shared directory: ";
	std::cin >> sharedDir;

	while (running) 
	{
		if (peers->size() > 0)
		{
			bytesRecv = recv(peers->at(0).peerSock, recvBuffer, dataBufferSize, 0);
			if (bytesRecv > 0)
			{
				if (std::string(recvBuffer, bytesRecv) == r_downloadFile)
				{
					send(peers->at(0).peerSock, s_receivedRequest, statusSize, 0);
					sendFile(peers->at(0).peerSock);
				}
			}
		}
	}*/
}

bool Client::Connect(std::string ip, short port)
{
	PeerInfo peer;
	peer.peerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	peer.peerHint.sin_family = AF_INET;
	peer.peerHint.sin_port = htons(port);
	peer.peerHint.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

	if (connect(peer.peerSock, (sockaddr*)&peer.peerHint, sizeof(peer.peerHint)) != SOCKET_ERROR)
	{
		u_long nonblocking = 1;
		ioctlsocket(peer.peerSock, FIONBIO, &nonblocking);
		peers->push_back(peer);
		return true;
	}

	return false;
}

void Client::Disconnect(int peerIndex)
{
	closesocket(peers->at(peerIndex).peerSock);
	peers->erase(peers->begin() + peerIndex);
}

void Client::Disconnect(SOCKET& from)
{
	for (int i = 0; i < peers->size(); i++)
	{
		if (peers->at(i).peerSock == from)
		{
			closesocket(peers->at(i).peerSock);
			peers->erase(peers->begin() + i);
			return;
		}
	}
}

void Client::sendFile(SOCKET& peer)
{
	u_long blocking = 0;
	ioctlsocket(peer, FIONBIO, &blocking);

	DWORD timeout = 3;
	setsockopt(peer, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)); //sets timeout
	setsockopt(peer, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

	char dataBuffer[dataBufferSize];
	char fileBuffer[fileBufferSize];
	int bytesReceived = recv(peer, dataBuffer, dataBufferSize, 0);
	std::string fileName = std::string(dataBuffer, bytesReceived);
	memset(fileBuffer, 0, fileBufferSize);
	std::ifstream inFile;

	inFile.open(sharedDir + fileName, std::ios::binary);

	if (inFile.is_open())
	{
		std::string fileSize = std::to_string(getFileSize(inFile));
		
		send(peer, s_fileAvailable, requestSize, 0);
		recv(peer, dataBuffer, dataBufferSize, 0); //get file size
		send(peer, fileSize.c_str(), fileSize.length(), 0);
		recv(peer, dataBuffer, dataBufferSize, 0); //ready to receive

		do
		{
			inFile.read(fileBuffer, fileBufferSize);

			if (inFile.gcount() > 0)
			{
				int status = send(peer, fileBuffer, inFile.gcount(), 0);
				if (status == -1 || status == 0)
				{
					std::cout << "Can't send the data(" << WSAGetLastError() << ")\n";
					break;
				}
				recv(peer, dataBuffer, dataBufferSize, 0); //confirmation from peer that the data was received
			}
		} while (inFile.gcount() > 0);
	}
	else
	{
		std::cout << "Failed to open file '" << fileName << "'\n";
		send(peer, s_fileNotAvailable, statusSize, 0);
	}

	inFile.close();

	timeout = 0;
	setsockopt(peer, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)); //sets timeout
	setsockopt(peer, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

	u_long nonblocking = 1;
	ioctlsocket(peer, FIONBIO, &nonblocking);
}

bool Client::downloadFile(SOCKET& peer, std::string fileName)
{
	u_long blocking = 0;
	ioctlsocket(peer, FIONBIO, &blocking);

	DWORD timeout = 3;
	setsockopt(peer, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)); //sets timeout
	setsockopt(peer, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

	bool status = false;
	if (requestFile(peer, fileName))
		status = recvFile(peer, fileName);

	timeout = 0;
	setsockopt(peer, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)); //sets timeout
	setsockopt(peer, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

	u_long nonblocking = 1;
	ioctlsocket(peer, FIONBIO, &nonblocking);

	return status;
}

bool Client::requestFile(SOCKET& peer, std::string fileName)
{
	char recvBuffer[dataBufferSize];
	memset(recvBuffer, 0, dataBufferSize);
	send(peer, r_downloadFile, requestSize, 0);
	recv(peer, recvBuffer, dataBufferSize, 0); //confirmation that the request was received
	send(peer, fileName.c_str(), fileName.length(), 0);
	recv(peer, recvBuffer, requestSize, 0);//file available/not avaliable
	if(std::string(recvBuffer, requestSize) == s_fileAvailable)
		return true;
	return false;
}

bool Client::recvFile(SOCKET& peer, std::string fileRequested)
{
	int bytesReceived;
	char fileBuffer[fileBufferSize];
	char sizeRecvBuffer[dataBufferSize];
	memset(fileBuffer, 0, fileBufferSize);
	memset(sizeRecvBuffer, 0, dataBufferSize);

	std::ofstream outFile(downloadDir + fileRequested, std::ios::binary | std::ios::trunc);
	send(peer, r_getFileSize, requestSize, 0);
	bytesReceived = recv(peer, sizeRecvBuffer, sizeof(sizeRecvBuffer), 0);
	uint64_t fileSize = std::stoull(std::string(sizeRecvBuffer, bytesReceived));
	send(peer, s_readyToReceive, statusSize, 0);

	if (outFile.is_open())
	{
		bool errorOccured = false;
		long totalDownloaded = 0;
		do {
			memset(fileBuffer, 0, fileBufferSize);
			bytesReceived = recv(peer, fileBuffer, fileBufferSize, 0);
			if (bytesReceived == 0 || bytesReceived == -1)
			{
				std::cout << "Error receiving the data!(" << WSAGetLastError() << ")\n" << std::endl;
				errorOccured = true;
				break;
			}
			outFile.write(fileBuffer, bytesReceived);
			send(peer, s_receivedData, statusSize, 0);
			totalDownloaded += bytesReceived;
		} while (totalDownloaded < fileSize);

		outFile.close();

		if(!errorOccured)
			return true;
	}
	return false;
}

bool Client::getDirectoryContent(SOCKET& peer)
{
	send(peer, r_getDirectoryContent, requestSize, 0);
	return false;
}

void Client::sendDirectoryContent(SOCKET& peer)
{
	send(peer, s_receivedRequest, statusSize, 0);
}

int64_t Client::getFileSize(std::ifstream& file)
{
	file.seekg(0, std::ios::end);
	int64_t size = file.tellg();
	file.seekg(0, std::ios::beg);
	return size;
}