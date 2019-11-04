#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include "Constant_v2.h"
#include <Windows.h>
#include <vector>
#include <string>
#include <thread>
#include <mutex>

#pragma comment(lib,"ws2_32.lib")
#include "Client.h"

class Server
{
public:
	static Server *CreateTCP(u_short port);
	static Server *CreateUDP(u_short port);
	void release();

	int Listen();
	int ClientThread(Client *pClient);
	int StartServer();
private:
	Server();
	~Server();

	Client *FindClientByName(const string &name);

	SOCKET m_socket;
	sockaddr_in m_addr;
	vector<Client*> m_clientList;
	mutex m_listMt;
};