#pragma once
#define IP "127.0.0.1"
#define PORT 12000

/*
每次send和recv的消息的最后一附加位代表消息类型
消息类型 | 编号
-----------
登录验证 | 1
真实消息 | 2
注册验证 | 3
下线信息 | 4
单聊群聊 | 5
添加好友 | 6
选择好友 | 7
*/
#define CHECKNUM 1
#define MESSAGENUM 2
#define SIGHUPNUM 3
#define LOGOUTNUM 4
#define CHOOSENUM 5
#define ADDFRIENDNUM 6
#define CHOOSEFRIENDNUM 7

#define WORNG_USER 100
class Event
{
public:
	Event();
	~Event();
	//函数声明
	int databaseInit();
	int createUser();
	int createFriends();
	int insertUser(string, string, string);
	int insertFriends(string, string);
	int logInCheck(string, string);
	int getUsers(User*);
	int getFriends(Friends*);
	void singleConnection(int idx);
	void chatroomConnection(int idx);

private:

};

Event::Event()
{
}

Event::~Event()
{
}

//全局变量
SOCKET serverSocket;//服务端套接字
SOCKET clientSocket[100];//客户端套接字
static int concOrder = 0;//客户端连接的次序
static char usernames[1024][50];//连接成功的用户的用户名
static int usersnum;//数据库中用户的数量
User users[100];//从数据库中接收的用户信息
static int friendssnum;//数据库中好友对的数量
Friends friends[100];//从数据库中接收的好友对信息
char* userIP;