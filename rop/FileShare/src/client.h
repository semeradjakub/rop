#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <winsock.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <filesystem>
#include <wx/listbox.h>
#include "peerinfo.h"
#include "fileinfo.h"
#include "globals.h"

class Client
{
public:
	Client(std::vector<PeerInfo>* peers, std::string* localID, std::vector<std::string>* requests); 
	~Client(); 
	bool start(); 

public:
	PeerInfo* Connect(std::string& ip, short port);
	void Disconnect(std::string& ip, std::string requestID);
	void downloadFile(PeerInfo& peer, std::string fileName, std::string requestID, std::vector<std::string>& responseBuffer, bool& finished);
	void getDirectoryContent(PeerInfo& peer, std::string requestID, std::vector<std::string>& responseBuffer, wxListBox* target, bool& finished);

	void setDownloadDir(std::string path);
	void setUploadDir(std::string path);
	std::string getDownloadDir() { return downloadDir; }
	std::string getUploadDir() { return sharedDir;  }
	
private:
	bool requestFile(SOCKET& peer, std::string fileName, std::string& requestID, std::vector<std::string>& responseBuffer);
	bool recvFile(SOCKET& peer, std::string fileRequested, std::string& requestID, std::vector<std::string>& responseBuffer);
	void sendFile(SOCKET& peer, std::string& requestID, std::vector<std::string>& responseBuffer, bool& finished);
	void sendDirectoryContent(SOCKET& peer, std::string& requestID, std::vector<std::string>& responseBuffer, bool& finished);
	void clientDisconnect(SOCKET& peer, std::string& requestID, std::vector<std::string>& responseBuffer, bool& finished);

private:
	//core
	void run();
	std::thread thread;
	std::vector<PeerInfo>* peers;

private:
	//utility
	u_long mode_blocking = 0;
	u_long mode_nonblocking = 1;
	void setSocketMode(SOCKET& sock, u_long& mode);
	int64_t getFileSize(std::ifstream& file);
	std::thread createRequestThreadServer(std::string request, SOCKET& peerSock, std::string& requestID, std::vector<std::string>& responseVec, bool& finished);
	void _send(SOCKET& s, std::string buf, std::string& requestID);
	std::vector<std::string>* requests = nullptr; //for thread/Worker object termination
	std::string getResponse(std::vector<std::string>& vec);

public:
	std::thread createRequestThreadClient(PeerInfo& peer, std::string func, std::string& requestID, std::string fileName, std::vector<std::string>& responseVec, wxListBox* targetListBox, bool& finished);

private:
	//control
	SOCKET defaultSocket = 0;
	bool running = false;
	bool fileSharingAllowed = true;
	std::string* localID = nullptr;
	std::string sharedDir = "";
	std::string downloadDir = "";

private:
	static const int dataBufferSize = 1024;
	static const int requestSize = 4;
	static const int statusSize = 4;
	static const int messageSize = 4;

private:
	//communication
	status s_fileNotAvailable = "\x73\x66\x6E\x61";
	status s_fileAvailable = "\x73\x66\x69\x61";
	status s_idChange = "\x73\x64\x65\x61";
	status s_readyToReceive = "\x73\x72\x74\x72";
	status s_nextMetaFile = "\x73\x6E\x6D\x66";
	status s_endMetaFile = "\x73\x65\x6D\x66";
	status s_receivedRequest = "\x73\x72\x65\x72";
	status s_receivedData = "\x73\x72\x65\x64";
	status s_requestHandled = "\x73\x72\x65\x68";
	status s_genericError = "\x73\x67\x65\x72";

	message m_welcome = "\x68\x65\x6C\x6F";
	message m_error = "\x68\x64\x6E\x6F";
	message m_connect = "\x68\x63\x6F\x6E";

	request r_unknown = "\x72\x75\x6E\x6B";
	request r_getDirectoryContent = "\x72\x67\x64\x63";
	request r_getFileSize = "\x72\x67\x66\x73";
	request r_sendFile = "\x72\x73\x65\x66";
	request r_downloadFile = "\x72\x64\x77\x66";
	request r_disconnect = "\x72\x62\x79\x65";
};
