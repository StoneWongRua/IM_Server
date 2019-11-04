#include "Client.h"

#include "ChatDBUtil.h"


string DbConnector::host_ = "localhost";
string DbConnector::user_ = "root";
string DbConnector::passwd_ = "root";
string DbConnector::db_ = "chat";

Client::Client(SOCKET socket, sockaddr_in addr, string name)
	: m_socket(socket), m_addr(addr), m_name(name), m_connenter(nullptr)
{
}

int Client::SetName(const string &name, const string &password)
{
	m_name = name;
	string str = "insert into test(username,password) values('"+ name + "'123,'" + password + "')";
	DbConnector* conn = new DbConnector();
	return conn->query(str);

}

