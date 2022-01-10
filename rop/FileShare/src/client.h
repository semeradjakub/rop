#pragma once
#include <winsock.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include "Helper.h"
#include "peerinfo.h"
#include "fileinfo.h"

typedef const char* status;
typedef const char* request;


//DO REQUEST HANDLING
class Client
{
public:
	Client(std::vector<PeerInfo>* peers); //working
	~Client(); //working
	bool start(); //working
	bool Connect(std::string ip, short port); //working
	void Disconnect(int peerIndex); //working
	void Disconnect(SOCKET& from); //working

	void sendFile(SOCKET& peer); //working
	bool downloadFile(SOCKET& peer, std::string fileName); //working

private:
	bool requestFile(SOCKET& peer, std::string fileName); //working
	bool recvFile(SOCKET& peer, std::string fileRequested); //working
	bool getDirectoryContent(PeerInfo& peer);
	void sendDirectoryContent(SOCKET& peer);
	int64_t getFileSize(std::ifstream& file); //working

private:
	void handleRequests();

private:
	bool running = false;
	std::thread thread;
	void run();
	std::vector<PeerInfo>* peers;

private:
	void setSocketMode(SOCKET& sock, u_long& mode);
	u_long mode_blocking = 0;
	u_long mode_nonblocking = 1;

private:
	bool sharing = false;
	std::string sharedDir = "C:\\Users\\Jakub\\Desktop\\ROP\\upload\\";
	std::string downloadDir = "C:\\Users\\Jakub\\Desktop\\ROP\\download\\";

private:
	static const int fileBufferSize = 1024;
	static const int dataBufferSize = 256;

private:
	static const int requestSize = 4;
	static const int statusSize = 4;

	status s_fileNotAvailable = "\xbb\x55\x20\xaa";
	status s_fileAvailable = "\xbb\x55\x20\xbb";
	status s_readyToReceive = "\xbb\xaa\xf0\x97";
	status s_nextMetaFile = "\xbb\xaa\x52\x30";
	status s_endMetaFile = "\xbb\x78\x55\x30";
	status s_receivedRequest = "\xbb\x55\x62\x33";
	status s_receivedData = "\xbb\x88\x51\x32";
	status s_requestHandled = "\xbb\xaa\xbb\x11";

	status s_genericError = "\xbb\x55\xb4\x0a";

	request r_unknown = "\xaa\x05\x0a\xff";
	request r_getDirectoryContent = "\xaa\xbb\xcc\xdd";
	request r_getFileSize = "\xaa\x50\xff\x00";
	request r_sendFile = "\xaa\x10\x02\x20";
	request r_downloadFile = "\xaa\x20\x01\x10";
};
