#pragma once
#include <winsock.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <vector>
#include "Helper.h"
#include "peerinfo.h"

typedef const char* status;
typedef const char* request;

class Client
{
public:
	Client(std::vector<PeerInfo>* peers);
	~Client();
	bool start();
	bool Connect(std::string ip, short port);
	void Disconnect(int peerIndex);
	void Disconnect(SOCKET& from);

	void sendFile(SOCKET& peer);
	bool downloadFile(SOCKET& peer, std::string fileName);

private:
	bool requestFile(SOCKET& peer, std::string fileName);
	bool recvFile(SOCKET& peer, std::string fileRequested);
	bool getDirectoryContent(SOCKET& peer);
	void sendDirectoryContent(SOCKET& peer);
	int64_t getFileSize(std::ifstream& file);

private:
	bool running = false;
	std::thread thread;
	void run();
	std::vector<PeerInfo>* peers;

private:
	bool sharing = false;
	std::string sharedDir = "C:\\Users\\Jakub\\Desktop\\ROP\\rop\\";
	std::string downloadDir = "C:\\Users\\Jakub\\Desktop\\ROP\\rop\\bin\\Win32\\Debug\\";

private:
	static const int fileBufferSize = 1024;
	static const int dataBufferSize = 256;

private:
	static const int requestSize = 4;
	static const int statusSize = 4;

	status s_fileNotAvailable = "\xaa\x55\x20\xaa";
	status s_fileAvailable = "\xbb\x55\x20\xbb";
	status s_readyToReceive = "\xcc\xaa\xf0\x00";
	status s_receivedRequest = "\xcc\xac\xcc\xac";
	status s_receivedData = "\xac\xac\xac\xac";

	request r_unknown = "\xff\x00\x00\xff";
	request r_getDirectoryContent = "\xaa\xbb\xcc\xdd";
	request r_getFileSize = "\x05\x50\xff\x00";
	request r_sendFile = "\x01\x10\x02\x20";
	request r_downloadFile = "\x02\x20\x01\x10";
};
