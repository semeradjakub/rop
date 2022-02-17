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
		Sleep(1);
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
						std::cout << "Received data for existing id(" << requestID << ")\n";
						std::cout << dataReceived << std::endl;
					}
					else
					{
						peers->at(i).threadManager.workers[requestID] = new NetThreadManager::Worker();
						peers->at(i).threadManager.workers[requestID]->thread = createRequestThread(dataReceived, peerSock, requestID, peers->at(i).threadManager.workers[requestID]->buffer);
						std::cout << "Received new request - created new thread(id: " << requestID << ")\n";
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
			send(peers->at(i).peerSock, m_disconnect.c_str(), messageSize, 0);
			closesocket(peers->at(i).peerSock);
			peers->erase(peers->begin() + i);
			return true;
		}
	}
	return false;
}

bool Client::downloadFile(SOCKET& peer, std::string fileName, std::string& requestID, std::vector<std::string>& responseBuffer)
{
	bool status = false;

	if (peer != INVALID_SOCKET)
	{
		if (requestFile(peer, fileName, requestID, responseBuffer))
			status = recvFile(peer, fileName, requestID, responseBuffer);
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
	char recvBuffer[dataBufferSize];
	memset(recvBuffer, 0, dataBufferSize);
	_send(peer, r_downloadFile, requestID);
	recv(peer, recvBuffer, dataBufferSize, 0); //confirmation that the request has been received
	_send(peer, fileName, requestID);
	recv(peer, recvBuffer, requestSize, 0);//file available/not avaliable
	if(std::string(recvBuffer, requestSize) == s_fileAvailable)
		return true;
	return false;
}

bool Client::recvFile(SOCKET& peer, std::string fileRequested, std::string& requestID, std::vector<std::string>& responseBuffer)
{
	int bytesReceived;
	char fileBuffer[fileBufferSize];
	char sizeRecvBuffer[dataBufferSize];
	memset(fileBuffer, 0, fileBufferSize);
	memset(sizeRecvBuffer, 0, dataBufferSize);

	std::ofstream outFile(downloadDir + fileRequested, std::ios::binary | std::ios::trunc);
	_send(peer, r_getFileSize, requestID);
	bytesReceived = recv(peer, sizeRecvBuffer, sizeof(sizeRecvBuffer), 0);
	uint64_t fileSize = std::stoull(std::string(sizeRecvBuffer, bytesReceived));
	_send(peer, s_readyToReceive, requestID);

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
			_send(peer, s_receivedData, requestID);
			totalDownloaded += bytesReceived;
		} while (totalDownloaded < fileSize);

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
		char fileBuffer[fileBufferSize];
		std::string fileName = getResponse(responseBuffer);

		std::ifstream inFile;

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
				inFile.read(fileBuffer, fileBufferSize);

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

bool Client::getDirectoryContent(PeerInfo& peer, std::string requestID, std::vector<std::string>& responseBuffer, wxListBox& target)
{
	try
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
			}
		}
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
	}

	for (int i = 0; i < peer.files.size(); i++)
	{
		target.AppendString(wxString(peer.files.at(i).fileName + ":(" + std::to_string(peer.files.at(i).fileSize) + " Bytes)"));
	}
	std::cout << "finished\n";
	return true;
}

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
				std::string path = entry.path().string();
				std::string fileName = path.substr(path.rfind("\\") + 1, path.size() - path.rfind("\\"));
				std::string fileSize = std::to_string(entry.file_size());
				_send(peer, s_nextMetaFile, requestID);
				getResponse(responseBuffer);
				_send(peer, fileName, requestID);
				getResponse(responseBuffer);
				_send(peer, fileSize, requestID);
				getResponse(responseBuffer);
			}

			_send(peer, s_endMetaFile, requestID);
		}
		else
		{
			_send(peer, s_endMetaFile, requestID);
		}

	}

	std::cout << "finished\n";
}

int64_t Client::getFileSize(std::ifstream& file)
{
	file.seekg(0, std::ios::end);
	int64_t size = file.tellg();
	file.seekg(0, std::ios::beg);
	return size;
}

std::thread Client::createRequestThread(std::string request, SOCKET& peerSock, std::string& requestID, std::vector<std::string>& responseVec)
{

	if (request == r_downloadFile)
		return std::thread(&Client::sendFile, this, std::ref(peerSock), std::ref(requestID), std::ref(responseVec));
	else if (request == r_getDirectoryContent)
		return std::thread(&Client::sendDirectoryContent, this, std::ref(peerSock), std::ref(requestID), std::ref(responseVec));

}

std::thread Client::createRequestThread(PeerInfo& peer, std::string func, std::string& requestID, std::string fileName, std::vector<std::string>& responseVec, wxListBox& targetListBox)
{
	if (func == "dir")
	{
		return std::thread(&Client::getDirectoryContent, this, std::ref(peer), requestID, std::ref(responseVec), std::ref(targetListBox));
	}
	else if (func == "file")
	{
		//download file
	}
}

std::string Client::getResponse(std::vector<std::string>& vec)
{
	while (!vec.size()) { Sleep(1); }
	std::string res = vec.at(0);
	vec.erase(vec.begin());
	return res;
}

