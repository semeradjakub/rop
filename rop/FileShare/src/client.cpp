#include "client.h"

Client::Client(std::vector<PeerInfo>* peers, std::string* localID)
{
	this->peers = peers;
	this->localID = localID;
}

Client::~Client()
{
	running = false;
	thread.join();
}

/*
* creates client thread -> run() function
*/
bool Client::start()
{
	running = true;
	thread = std::thread(&Client::run, this);
	return true;
}

void Client::run()
{
	int bytesReceived = 0;
	char receiveBuffer[dataBufferSize];
	std::string receivedStr;
	
	while (running)
	{
		int peersSize = peers->size();
		for (int i = 0; i < peersSize; i++)
		{
			SOCKET& peerSock = peers->at(i).peerSock;
			bytesReceived = recv(peerSock, receiveBuffer, dataBufferSize, 0);

			if (bytesReceived > 0)
			{
				receivedStr = std::string(receiveBuffer, bytesReceived);

				if (receivedStr == r_getDirectoryContent)
					sendDirectoryContent(peerSock);
				if (receivedStr == r_downloadFile)
					sendFile(peerSock);
				if (receivedStr == m_disconnect)
				{
					peers->erase(peers->begin() + i);
					peersSize--;
				}

			}
		}
	}
}


/*
* sets socket into blocking / non-blocking mode
*/
void Client::setSocketMode(SOCKET& sock, u_long& mode)
{
	if(sock != INVALID_SOCKET)
		ioctlsocket(sock, FIONBIO, &mode);
}

PeerInfo* Client::Connect(std::string& ip, short port)
{
	PeerInfo peer;
	int bytesReceived;
	char data[dataBufferSize];
	peer.peerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	peer.peerHint.sin_family = AF_INET;
	peer.peerHint.sin_port = htons(port);
	peer.peerHint.sin_addr.S_un.S_addr = inet_addr(ip.c_str());

	setSocketMode(peer.peerSock, mode_blocking);

	if (connect(peer.peerSock, (sockaddr*)&peer.peerHint, sizeof(peer.peerHint)) != SOCKET_ERROR)
	{
		send(peer.peerSock, m_connect, dataBufferSize, 0);
		recv(peer.peerSock, data, dataBufferSize, 0);
		if (std::string(data, messageSize) == m_welcome)
		{
			send(peer.peerSock, localID->c_str(), localID->length(), 0);
			bytesReceived = recv(peer.peerSock, data, dataBufferSize, 0);
			if (bytesReceived > 0)
				peer.id = std::string(data, bytesReceived);
			peer.available = true;
			setSocketMode(peer.peerSock, mode_nonblocking);
			peers->push_back(peer);
			return &peers->at(peers->size() - 1);
		}
	}

	return nullptr;
}

bool Client::Disconnect(std::string& ip)
{
	for (int i = 0; i < peers->size(); i++)
	{
		if (inet_ntoa(peers->at(i).peerHint.sin_addr) == ip)
		{
			send(peers->at(i).peerSock, m_disconnect, messageSize, 0);
			closesocket(peers->at(i).peerSock);
			peers->erase(peers->begin() + i);
			return true;
		}
	}
	return false;
}

void Client::sendFile(SOCKET& peer)
{
	if (peer != INVALID_SOCKET)
	{
		send(peer, s_receivedRequest, statusSize, 0);

		setSocketMode(peer, mode_blocking);

		DWORD timeout = 3;
		setsockopt(peer, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
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
			recv(peer, dataBuffer, dataBufferSize, 0);
			send(peer, fileSize.c_str(), fileSize.length(), 0);
			recv(peer, dataBuffer, dataBufferSize, 0);

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
					recv(peer, dataBuffer, dataBufferSize, 0); //confirmation from peer that the data has been received
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

		setSocketMode(peer, mode_nonblocking);
	}
}

bool Client::downloadFile(SOCKET& peer, std::string fileName)
{
	bool status = false;

	if (peer != INVALID_SOCKET)
	{
		setSocketMode(peer, mode_blocking);

		DWORD timeout = 3;
		setsockopt(peer, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)); //sets timeout
		setsockopt(peer, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

		if (requestFile(peer, fileName))
			status = recvFile(peer, fileName);

		timeout = 0;
		setsockopt(peer, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)); //sets timeout
		setsockopt(peer, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

		setSocketMode(peer, mode_nonblocking);
	}

	return status;
}

bool Client::requestFile(SOCKET& peer, std::string fileName)
{
	char recvBuffer[dataBufferSize];
	memset(recvBuffer, 0, dataBufferSize);
	send(peer, r_downloadFile, requestSize, 0);
	recv(peer, recvBuffer, dataBufferSize, 0); //confirmation that the request has been received
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

bool Client::getDirectoryContent(PeerInfo& peer)
{
	peer.files.clear();
	setSocketMode(peer.peerSock, mode_blocking);

	char dataBuffer[dataBufferSize];
	int bytesReceived = 0;

	send(peer.peerSock, r_getDirectoryContent, requestSize, 0);
	bytesReceived = recv(peer.peerSock, dataBuffer, dataBufferSize, 0);

	if (std::string(dataBuffer, bytesReceived) == s_receivedRequest)
	{
		send(peer.peerSock, s_readyToReceive, statusSize, 0);
		FileInfo file;
		bytesReceived = recv(peer.peerSock, dataBuffer, dataBufferSize, 0);
		while (std::string(dataBuffer, statusSize) == s_nextMetaFile)
		{
			send(peer.peerSock, s_receivedData, statusSize, 0);
			bytesReceived = recv(peer.peerSock, dataBuffer, dataBufferSize, 0);
			file.fileName = std::string(dataBuffer, bytesReceived);
			send(peer.peerSock, s_receivedData, statusSize, 0);
			bytesReceived = recv(peer.peerSock, dataBuffer, dataBufferSize, 0);
			file.fileSize = std::stoull(std::string(dataBuffer, bytesReceived));
			peer.files.push_back(file);
			send(peer.peerSock, s_receivedData, statusSize, 0);
			recv(peer.peerSock, dataBuffer, dataBufferSize, 0);
		}

		setSocketMode(peer.peerSock, mode_nonblocking);

		return true;
	}

	setSocketMode(peer.peerSock, mode_nonblocking);

	return false;
}

void Client::sendDirectoryContent(SOCKET& peer)
{
	if (peer != INVALID_SOCKET)
	{
		setSocketMode(peer, mode_blocking);

		char dataBuffer[dataBufferSize];
		int bytesReceived = 0;

		send(peer, s_receivedRequest, statusSize, 0);
		bytesReceived = recv(peer, dataBuffer, dataBufferSize, 0);

		if (std::string(dataBuffer, bytesReceived) == s_readyToReceive)
		{
			for (auto& entry : std::filesystem::directory_iterator(sharedDir))
			{
				std::string path = entry.path().string();
				std::string filename = path.substr(path.rfind("\\") + 1, path.size() - path.rfind("\\"));
				std::string fileSize = std::to_string(entry.file_size());
				send(peer, s_nextMetaFile, statusSize, 0);
				recv(peer, dataBuffer, dataBufferSize, 0);
				send(peer, filename.c_str(), filename.length(), 0);
				recv(peer, dataBuffer, dataBufferSize, 0);
				send(peer, fileSize.c_str(), fileSize.length(), 0);
				recv(peer, dataBuffer, dataBufferSize, 0);
			}

			send(peer, s_endMetaFile, statusSize, 0);
		}
		else
		{
			send(peer, s_endMetaFile, statusSize, 0);
		}

		setSocketMode(peer, mode_nonblocking);
	}
}

int64_t Client::getFileSize(std::ifstream& file)
{
	file.seekg(0, std::ios::end);
	int64_t size = file.tellg();
	file.seekg(0, std::ios::beg);
	return size;
}

