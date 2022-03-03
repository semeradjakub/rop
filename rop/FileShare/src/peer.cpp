#include "peer.h"

Peer::Peer(std::string ip)
{
	WSAStartup(version, &wsaData);
	client = new Client(&peers, &localID, &requests,  &deletedPeers);
	server = new Server(&peers, &localID, &addedPeers);
	srand(time(0));
}

Peer::~Peer()
{
	delete client;
	delete server;
	WSACleanup();
}

bool Peer::start()
{
	return (server->start() && client->start());
}

std::string Peer::generateRandomId(int length)
{
	std::string id = "";
	for (int i = 0; i < length; i++)
		id += (char)('a' + rand() % ('z' - 'a' - 1));
	return id;
}

bool Peer::DownloadFile(PeerInfo& peer, std::string fileName)
{
	std::string requestID = generateRandomId(16);
	requests.push_back(requestID);
	peer.threadManager.workers[requestID] = new NetThreadManager::Worker();
	peer.threadManager.workers[requestID]->thread = client->createRequestThreadClient(peer, "file_download", requestID, fileName, peer.threadManager.workers[requestID]->buffer, nullptr, peer.threadManager.workers[requestID]->finished);
	return true;
}

bool Peer::GetPeerDirectoryContent(PeerInfo& peer, wxListBox* target)
{
	std::string requestID = generateRandomId(16);
	requests.push_back(requestID);
	peer.threadManager.workers[requestID] = new NetThreadManager::Worker();
	peer.threadManager.workers[requestID]->thread = client->createRequestThreadClient(peer, "directory_get", requestID, "", peer.threadManager.workers[requestID]->buffer, target, peer.threadManager.workers[requestID]->finished);
	return true;
}

PeerInfo* Peer::Connect(std::string& ip)
{
	return client->Connect(ip, 55667);
}

void Peer::ConnectWan(std::string& ip)
{
	client->ConnectWan(ip, LISTENPORT);
}

void Peer::Disconnect(std::string& ip)
{
	std::string requestID = generateRandomId(16);
	client->Disconnect(ip, requestID);
}

void Peer::DisconnectFromAll()
{
	client->DisconnectFromAll();
}

PeerInfo* Peer::GetPeerById(std::string& id)
{
	PeerInfo* peer = nullptr;
	for (int i = 0; i < peers.size(); i++)
	{
		try
		{
			if (peers.at(i).id == id)
			{
				peer = &peers.at(i);
				break;
			}
		}
		catch (std::exception e)
		{
			//handle
		}
	}
	return peer;
}

PeerInfo* Peer::GetPeerByIp(std::string& ip)
{
	PeerInfo* peer = nullptr;
	for (int i = 0; i < peers.size(); i++)
	{
		try
		{
			if (inet_ntoa(peers.at(i).peerHint.sin_addr) == ip)
			{
				peer = &peers.at(i);
				break;
			}
		}
		catch (std::exception e)
		{
			//handle
		}
	}
	return peer;
}

void Peer::setID(std::string localID)
{
	this->localID = localID;
}

std::string Peer::getFileSizeByFileName(PeerInfo& peer, std::string fileName)
{
	for (int i = 0; i < peer.files.size(); i++)
	{
		try
		{
			if (peer.files.at(i).fileName == fileName)
			{
				uint64_t size = peer.files.at(i).fileSize;
				uint64_t sizeFinal = 0;
				std::string sufix = "";

				if (size < 1024)
				{
					sizeFinal = size;
					sufix = "B";
				}
				else if (size >= 1024 && size < 1048576)
				{
					sufix = "kB";
					sizeFinal = size / 1024;
				}
				else if (size >= 1048576 && size < 1073741824)
				{
					sufix = "MB";
					sizeFinal = size / 1024 / 1024;
				}
				else if (size >= 1073741824)
				{
					sufix = "GB";
					sizeFinal = size / 1024 / 1024 / 1024;
				}

				return std::to_string(sizeFinal) + sufix;
			}
		}
		catch (std::exception e)
		{
			//handle
			break;
		}
	}

	return "-";
}

std::string Peer::getID()
{
	return localID;
}

void Peer::setDownloadDir(std::string path)
{
	client->setDownloadDir(path);
}

void Peer::setUploadDir(std::string path)
{
	client->setUploadDir(path);
}

std::vector<PeerInfo>& Peer::getPeerList()
{
	return peers;
}