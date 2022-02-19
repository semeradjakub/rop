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
* creates client thread
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
	std::string receivedBuffer;
	std::string userID, requestID, dataReceived;
	
	while (running)
	{
		//Sleep(1);
		int peersSize = peers->size();
		
		for (int i = 0; i < peersSize; i++)
		{
			SOCKET& peerSock = peers->at(i).peerSock;
			bytesReceived = recv(peerSock, receiveBuffer, dataBufferSize, 0);

			if (bytesReceived > 0)
			{
				receivedBuffer = std::string(receiveBuffer, bytesReceived);
				//separate packet identificator and data - FORMAT: {[id]:[number]}-packetData
				userID = receivedBuffer.substr(receivedBuffer.find("{") + 1, receivedBuffer.find(":") - receivedBuffer.find("{") - 1);
				requestID = receivedBuffer.substr(receivedBuffer.find(":") + 1, receivedBuffer.find("}") - receivedBuffer.find(":") - 1);
				dataReceived = receivedBuffer.substr(receivedBuffer.find("-") + 1, receivedBuffer.size() - receivedBuffer.find("-"));

				if (peers->at(i).id == userID)
				{
					if (peers->at(i).threadManager.workers.find(requestID) != peers->at(i).threadManager.workers.end())
					{
						peers->at(i).threadManager.workers[requestID]->buffer.push_back(dataReceived);
						//std::cout << "Received data for existing id(" << requestID << ")\n";
						//std::cout << dataReceived << std::endl;
					}
					else
					{
						peers->at(i).threadManager.workers[requestID] = new NetThreadManager::Worker();
						peers->at(i).threadManager.workers[requestID]->thread = createRequestThreadServer(dataReceived, peerSock, requestID, peers->at(i).threadManager.workers[requestID]->buffer);
						//std::cout << "Received new request - created new thread(id: " << requestID << ")\n";
					}
				}
			}	
		}
	}
}


/*
 sets socket into blocking / non-blocking mode
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
		send(peer.peerSock, m_connect.c_str(), messageSize, 0);
		recv(peer.peerSock, data, dataBufferSize, 0);
		if (std::string(data, messageSize) == m_welcome)
		{
			send(peer.peerSock, localID->c_str(), localID->length(), 0);
			bytesReceived = recv(peer.peerSock, data, dataBufferSize, 0);
			if (bytesReceived > 0)
				peer.id = std::string(data, bytesReceived);
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
			send(peers->at(i).peerSock, m_disconnect.c_str(), messageSize, 0);
			closesocket(peers->at(i).peerSock);
			peers->erase(peers->begin() + i);
			return true;
		}
	}
	return false;
}

bool Client::downloadFile(PeerInfo& peer, std::string fileName, std::string requestID, std::vector<std::string>& responseBuffer)
{
	bool status = false;
	if (peer.peerSock != INVALID_SOCKET)
	{
		if (requestFile(peer.peerSock, fileName, requestID, responseBuffer))
			status = recvFile(peer.peerSock, fileName, requestID, responseBuffer);
	}

	return status;
}

/*
	adds header {id:number}-originalData
*/
void Client::_send(SOCKET& s, std::string buf, std::string& requestID)
{
	buf.insert(0, "{" + *localID + ":" + requestID + "}-");
	send(s, buf.c_str(), buf.length(), 0);
}

bool Client::requestFile(SOCKET& peer, std::string fileName, std::string& requestID, std::vector<std::string>& responseBuffer)
{
	_send(peer, r_downloadFile, requestID);
	getResponse(responseBuffer); //confirmation
	_send(peer, fileName, requestID);
	if(getResponse(responseBuffer) == s_fileAvailable)
		return true;
	return false;
}

bool Client::recvFile(SOCKET& peer, std::string fileRequested, std::string& requestID, std::vector<std::string>& responseBuffer)
{
	int bytesReceived;
	std::string fileData;

	std::ofstream outFile(downloadDir + fileRequested, std::ios::binary | std::ios::trunc);
	_send(peer, r_getFileSize, requestID);
	uint64_t fileSize = std::stoull(getResponse(responseBuffer));
	_send(peer, s_readyToReceive, requestID);

	if (outFile.is_open())
	{
		bool errorOccured = false;
		long totalDownloaded = 0;
		if (fileSize > 0)
		{
			do {
				std::string fileData = getResponse(responseBuffer);
				bytesReceived = fileData.length();
				if (bytesReceived == 0 || bytesReceived == -1)
				{
					std::cout << "Error receiving the data!(" << WSAGetLastError() << ")\n" << std::endl;
					errorOccured = true;
					break;
				}
				outFile.write(fileData.c_str(), bytesReceived);
				_send(peer, s_receivedData, requestID);
				totalDownloaded += bytesReceived;
			} while (totalDownloaded < fileSize);
		}

		outFile.close();

		if(!errorOccured)
			return true;
	}

	return false;
}

void Client::sendFile(SOCKET& peer, std::string& requestID, std::vector<std::string>& responseBuffer)
{
	if (peer != INVALID_SOCKET)
	{
		_send(peer, s_receivedRequest, requestID);

		char dataBuffer[dataBufferSize];
		char fileBuffer[dataBufferSize];
		std::string fileName = getResponse(responseBuffer);

		std::ifstream inFile;
		int prefixLength = localID->length() + requestID.length() + 4 + 1;

		inFile.open(sharedDir + fileName, std::ios::binary);

		if (inFile.is_open())
		{
			std::string fileSize = std::to_string(getFileSize(inFile));

			_send(peer, s_fileAvailable, requestID);
			getResponse(responseBuffer);
			_send(peer, fileSize, requestID);
			getResponse(responseBuffer);

			do
			{
				inFile.read(fileBuffer, dataBufferSize - prefixLength);

				if (inFile.gcount() > 0)
				{
					std::string fileData = std::string(fileBuffer, inFile.gcount());
					_send(peer, fileData, requestID);
					getResponse(responseBuffer); //confirmation from peer that the data has been received
				}
			} while (inFile.gcount() > 0);
		}
		else
		{
			std::cout << "Failed to open file '" << fileName << "'\n";
			_send(peer, s_fileNotAvailable, requestID);
		}

		inFile.close();
	}
}

//working
bool Client::getDirectoryContent(PeerInfo& peer, std::string requestID, std::vector<std::string>& responseBuffer, wxListBox* target)
{
	peer.files.clear();
	char dataBuffer[dataBufferSize];
	int bytesReceived = 0;

	_send(peer.peerSock, r_getDirectoryContent, requestID);
		std::string req = getResponse(responseBuffer);

	if (req == s_receivedRequest)
	{
		_send(peer.peerSock, s_readyToReceive, requestID);
		FileInfo file;
		while (getResponse(responseBuffer) == s_nextMetaFile)
		{
			_send(peer.peerSock, s_receivedData, requestID);
			file.fileName = getResponse(responseBuffer);
			_send(peer.peerSock, s_receivedData, requestID);
			file.fileSize = std::stoull(getResponse(responseBuffer));
			peer.files.push_back(file);
			_send(peer.peerSock, s_receivedData, requestID);
			
			target->AppendString(wxString(file.fileName + ":(" + std::to_string(file.fileSize) + " Bytes)"));
		}
	}

	return true;
}

//working
void Client::sendDirectoryContent(SOCKET& peer, std::string& requestID, std::vector<std::string>& responseBuffer)
{
	if (peer != INVALID_SOCKET)
	{
		char dataBuffer[dataBufferSize];
		int bytesReceived = 0;

		_send(peer, s_receivedRequest, requestID);

		if (getResponse(responseBuffer) == s_readyToReceive)
		{
			for (auto& entry : std::filesystem::directory_iterator(sharedDir))
			{ 
				if (entry.is_regular_file())
				{
					std::string path = entry.path().string();
					std::string subDirfileName = path.substr(path.rfind("\\") + 1, path.size() - path.rfind("\\"));
					std::string fileSize = std::to_string(entry.file_size());
					_send(peer, s_nextMetaFile, requestID);
					getResponse(responseBuffer);
					_send(peer, subDirfileName, requestID);
					getResponse(responseBuffer);
					_send(peer, fileSize, requestID);
					getResponse(responseBuffer);
				}
			}

			_send(peer, s_endMetaFile, requestID);
		}
		else
		{
			_send(peer, s_endMetaFile, requestID);
		}

	}
}

int64_t Client::getFileSize(std::ifstream& file)
{
	file.seekg(0, std::ios::end);
	int64_t size = file.tellg();
	file.seekg(0, std::ios::beg);
	return size;
}

std::thread Client::createRequestThreadServer(std::string request, SOCKET& peerSock, std::string& requestID, std::vector<std::string>& responseVec)
{

	if (request == r_downloadFile)
		return std::thread(&Client::sendFile, this, std::ref(peerSock), std::ref(requestID), std::ref(responseVec));
	else if (request == r_getDirectoryContent)
		return std::thread(&Client::sendDirectoryContent, this, std::ref(peerSock), std::ref(requestID), std::ref(responseVec));

}

std::thread Client::createRequestThreadClient(PeerInfo& peer, std::string func, std::string& requestID, std::string fileName, std::vector<std::string>& responseVec, wxListBox* targetListBox)
{
	if (func == "directory_get")
	{
		return std::thread(&Client::getDirectoryContent, this, std::ref(peer), requestID, std::ref(responseVec), targetListBox);
	}
	else if (func == "file_download")
	{
		return std::thread(&Client::downloadFile, this, std::ref(peer), fileName, requestID, std::ref(responseVec));
	}
}

std::string Client::getResponse(std::vector<std::string>& vec)
{
	while (!vec.size()) { /*Sleep(1);*/ }
	std::string res = vec.at(0);
	vec.erase(vec.begin());
	return res;
}
