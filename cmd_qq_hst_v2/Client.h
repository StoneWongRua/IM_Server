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

using namespace std;

class Client
{
public:
	Client(SOCKET socket, sockaddr_in addr, string name = "");

	int SetName(const string &name, const string &password);
	string GetName() { return m_name; }
	SOCKET GetSocket() { return m_socket; }
	sockaddr_in GetAddr() { return m_addr; }
	Client *GetConnenter() { return m_connenter; }
	void SetConnenter(Client *connenter) { m_connenter = connenter; }

private:
	SOCKET m_socket;
	string m_name;
	sockaddr_in m_addr;
	Client *m_connenter;
};