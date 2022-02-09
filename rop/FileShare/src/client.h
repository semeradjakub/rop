#pragma once
#include <winsock.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <filesystem>
#include "peerinfo.h"
#include "fileinfo.h"
#include "globals.h"

typedef const char* status;
typedef const char* request;
typedef const char* message;

class Client
{
public:
	Client(std::vector<PeerInfo>* peers, std::string* localID); 
	~Client(); 
	bool start(); 

	PeerInfo* Connect(std::string& ip, short port);
	bool Disconnect(std::string& ip); 
	bool downloadFile(SOCKET& peer, std::string fileName); 
	bool getDirectoryContent(PeerInfo& peer);

	
private:
	bool requestFile(SOCKET& peer, std::string fileName); 
	bool recvFile(SOCKET& peer, std::string fileRequested);
	void sendFile(SOCKET& peer);
	void sendDirectoryContent(SOCKET& peer);

private:
	//core
	void run();
	std::thread thread;
	std::vector<PeerInfo>* peers;

private:
	//utility
	void setSocketMode(SOCKET& sock, u_long& mode);
	int64_t getFileSize(std::ifstream& file);
	void* mapRequestHandlerFunction(std::string request);
	u_long mode_blocking = 0;
	u_long mode_nonblocking = 1;

private:
	//control
	bool running = false;
	bool sharing = false;
	bool fileSharingAllowed = true;
	std::string* localID = nullptr;
	std::string sharedDir = "C:\\Users\\Jakub\\Desktop\\ROP\\upload\\";
	std::string downloadDir = "C:\\Users\\Jakub\\Desktop\\ROP\\download\\";

private:
	static const int fileBufferSize = 1024;
	static const int dataBufferSize = 256;
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

	message m_disconnect = "\x68\x62\x79\x65";
	message m_welcome = "\x68\x65\x6C\x6F";
	message m_connect = "\x68\x63\x6F\x6E";

	request r_unknown = "\x72\x75\x6E\x6B";
	request r_getDirectoryContent = "\x72\x67\x64\x63";
	request r_getFileSize = "\x72\x67\x66\x73";
	request r_sendFile = "\x72\x73\x65\x66";
	request r_downloadFile = "\x72\x64\x77\x66";

};
