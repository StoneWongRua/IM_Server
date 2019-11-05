#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<iostream>
#include<string>
#include <windows.h>
using namespace std;

#include "FriendDBUtil.h"
#include "GlobalConstant.h"
#include "Sha256.h"
#include "Logger.h"
#include "HstSocket.h"

#pragma comment(lib, "ws2_32.lib")


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




DbUtil* dbUtil = new DbUtil();
HstSocket* hstSocket = new HstSocket();

void DbUtil::SingleConnection(int idx) {//单聊函数


	//查询该用户的好友,加入friendList链表和friendStream字符串流
	int count = 0;
	string friendList[100];
	string friendStream;

	cout << "friendssnum:" << friendssnum << endl;

	for (int i = 0; i < friendssnum; i++) {
		if (usernames[idx] == friends[i].user1name) {
			cout << i << ":" << friends[i].user2name << endl;
			friendList[count++] = friends[i].user2name;
			friendStream += friends[i].user2name;
			friendStream += ";";
		}
	}

	//将friendStream加上识别码转为char*发送给客户端
	char* buff = (char*)friendStream.data();
	int len = strlen(buff);
	buff[len] = CHOOSEFRIENDNUM;
	buff[len + 1] = 0;
	int s = send(clientSocket[idx], buff, len + 1, NULL);
	//接收客户端的选择消息
	int r = recv(clientSocket[idx], buff, 1023, NULL);


	//cout << "用户选择了" << friends[buff[0]].user2name << endl;

	cout << "用户选择了" << buff[0] << endl;
	ServerLog chooseLog_2("用户选择了" + buff[0], "Choose.log");
	string choosenFriend = friendList[buff[0] - '0'];


	//进入与对应好友的聊天界面
	memset(buff, 0, sizeof(buff));

	while (1) {
		int r = recv(clientSocket[idx], buff, 1023, NULL);

		if (r > 0 && buff[r - 1] == MESSAGENUM) {
			buff[r - 1] = 0;
			printf(">>用户%s:%s\n", usernames[idx], buff);


			//遍历登录过（正在线）的用户，发送消息
			for (int i = 0; i < concOrder; i++) {
				if (usernames[i] == choosenFriend || i == idx) {

					string logname = "chat_log_";
					DbChatLogMsg clMsg(usernames[idx], logname.append(usernames[idx]).append(".log"));

					clMsg.push(choosenFriend, buff, 0);

					//在消息前加上用户name
					char temp[50];
					strcpy(temp, usernames[idx]);
					strcat(temp, ":");
					strcat(temp, buff);
					strcpy(buff, temp);



					int len = strlen(buff);
					buff[len] = MESSAGENUM;
					buff[len + 1] = 0;


					int s = send(clientSocket[i], buff, len + 1, NULL);
				}
				else if (usernames[i] != choosenFriend)
				{
					char temp[50];
					//strcpy(temp, usernames[idx]);
					strcat(temp, "用户不在线，对其发送离线信息：");
					strcat(temp, buff);
					strcpy(buff, temp);



					int len = strlen(buff);
					buff[len] = MESSAGENUM;
					buff[len + 1] = 0;


					int s = send(clientSocket[i], buff, len + 1, NULL);

					cout << "用户不在线，对其发送离线信息：";
					DbOfflineMsg off_msg("offline_msg", "offileMsg.log");
					off_msg.push("to", "from", "hey");   //存入一条离线消息
					auto result = off_msg.pop("to"); //读取发给该用户的全部离线消息
				}
			}
		}
		//这里也可以接受用户的其他请求，如加好友等
		else if (r > 0 && buff[r - 1] == ADDFRIENDNUM) {
			buff[r - 1] = 0;
			string addFriendName = buff;
			int i = dbUtil->InsertFriends(usernames[idx], addFriendName);
			if (i == 0) {
				friendssnum = dbUtil->GetFriends(friends);
				cout << "建立好友关系成功！" << usernames[idx] << "和" << addFriendName << endl;
				ServerLog friendrLog("用户" + string(usernames[idx]) + "和" + string(addFriendName) + "成功建立好友关系。", "Friend.log");
			}
			else {
				cout << "建立好友关系失败！" << endl;
				ServerLog friendrLog_1("用户" + string(usernames[idx]) + "和" + string(addFriendName) + "建立好友关系失败。", "Friend.log");

			}
		}
	}

}

void DbUtil::ChatroomConnection(int idx) {//群聊函数；
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	while (1) {
		int r = recv(clientSocket[idx], buff, 1023, NULL);
		if (r > 0 && buff[r - 1] == MESSAGENUM) {
			buff[r - 1] = 0;
			printf(">>用户%s:%s\n", usernames[idx], buff);

			DbChatLogMsg clMsg(usernames[idx], "chat_log_Bob.log");
			clMsg.push("group", buff, 0);

			//在消息前加上用户name
			char temp[50];
			strcpy(temp, usernames[idx]);
			strcat(temp, ":");
			strcat(temp, buff);
			strcpy(buff, temp);
			int len = strlen(buff);
			buff[len] = MESSAGENUM;
			buff[len + 1] = 0;

			//广播信息
			for (int i = 0; i < concOrder; i++) {
				if (i == idx) continue;
				int s = send(clientSocket[i], buff, len + 1, NULL);
			}
			buff[0] = 0;
		}
		else if (r > 0 && buff[r - 1] == ADDFRIENDNUM) {
			buff[r - 1] = 0;
			string addFriendName = buff;
			int i = dbUtil->InsertFriends(usernames[idx], addFriendName);
			if (i == 0) {
				cout << "建立好友关系成功！" << usernames[idx] << "和" << addFriendName << endl;
				ServerLog friendrLog_2("用户" + string(usernames[idx]) + "和" + string(addFriendName) + "成功建立好友关系。", "Friend.log");

			}
			else {
				cout << "建立好友关系失败！" << endl;
				ServerLog friendrLog_3("用户" + string(usernames[idx]) + "和" + string(addFriendName) + "建立好友关系失败。", "Friend.log");

			}
		}
	}
}


void DbUtil::AddFriend(int idx,char* addFriendName) {

	char buff[1024];




	while (1) {

			int i = dbUtil->InsertFriend(usernames[idx], addFriendName);
			memset(buff, 0, 1024);
			if (i == 0) {

				buff[0] = 0;
				send(clientSocket[idx], buff, strlen(buff), NULL);
				cout << "建立好友关系成功！" << usernames[idx] << "和" << addFriendName << endl;
				ServerLog friendrLog_2("用户" + string(usernames[idx]) + "和" + string(addFriendName) + "成功建立好友关系。", "Friend.log");
				
			}
			else if (i == -2) {
				buff[0] == -2;
				send(clientSocket[idx], buff, strlen(buff), NULL);
				cout << "数据库查询失败。建立好友关系失败23333！" << endl;
				ServerLog friendrLog_3("用户" + string(usernames[idx]) + "和" + string(addFriendName) + "建立好友关系失败。", "Friend.log");
				
			}
			else if (i == -3) {
				buff[0] == -3;
				send(clientSocket[idx], buff, strlen(buff), NULL);
				cout  << usernames[idx] << "和" << addFriendName << "已经是好友关系！" << endl;
				
			}
			else if (i = -4)
			{
				buff[0] == -4;
				send(clientSocket[idx], buff, strlen(buff), NULL);
				cout << "不存在用户" << addFriendName << endl;
				
			}
		}
	}


int HstSocket::prepare() {
	SetConsoleTitle("聊天系统服务端");
	/*
	作用：前期准备，连接数据库，创建socket等
	参数：无；
	返回值：-1表示出错，直接退出；
	*/
	memset(users, 0, sizeof(users));
	//初始化数据库,建表
	
	dbUtil->DatabaseInit();
	dbUtil->CreateUser();
	dbUtil->CreateFriends();
	usersnum = dbUtil->GetUsers(users);
	friendssnum = dbUtil->GetFriends(friends);

	// 1.请求版本协议
	WSADATA wsData;
	WSAStartup(MAKEWORD(2, 2), &wsData);
	if ((LOBYTE(wsData.wVersion) != 2) || (HIBYTE(wsData.wVersion) != 2)) {
		printf("------请求版本协议失败------\n");
		ServerLog initServerLog_1("请求版本协议失败", 
			"InitServer.log");
		return -1;
	}
	printf("------请求版本协议成功------\n");
	ServerLog initServerLog_2("请求版本协议成功", 
		"InitServer.log");


	// 2.创建socket
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == SOCKET_ERROR) {
		printf("创建socket失败\n");
		ServerLog initServerLog_3("创建socket失败", 
			"InitServer.log");
		WSACleanup();
		return -2;
	}
	printf("------创建socket成功--------\n");
	ServerLog initServerLog_4("创建socket成功", 
		"InitServer.log");


	// 3.创建ip,port
	SOCKADDR_IN addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(IP);
	addr.sin_port = htons(PORT);


	// 4.绑定ip,port给socket
	int b = bind(serverSocket, (sockaddr*)& addr, sizeof addr);
	if (b == -1) {
		printf("绑定ip，port失败\n");
		ServerLog serverLog("绑定ip，port失败", 
			"InitServer.log");
		closesocket(serverSocket);
		WSACleanup();
		return -2;
	}
	printf("------绑定ip，端口成功------\n");
	ServerLog initServerLog_5("绑定ip，端口成功", 
		"InitServer.log");


	// 5. 监听
	int l = listen(serverSocket, 10);
	if (l == -1) {
		printf("监听失败\n");
		ServerLog initServerLog_6("监听失败", 
			"InitServer.log");
		closesocket(serverSocket);
		WSACleanup();
		return -2;
	}

	printf("正在监听......\n");
	ServerLog initServerLog_7("开始监听\n", 
		"InitServer.log");
	return 0;
}

void HstSocket::service(int idx) {

	/*
	作用：每有一个客户端连接，建立新的线程服务
	参数：客户端连接序号
	返回值：无
	*/

	int flag = 1;
	while (flag) {
		char checkBuff[1024];
		int r = recv(clientSocket[idx], checkBuff, 1023, NULL);
		if (r > 0) {

			int type = checkBuff[r - 1];//决定登录还是注册
			checkBuff[r - 1] = 0;
			char name[1024];
			char password[1024];

			//将checkBuff中的内容分别存入name， password中
			char* p = password;
			int temp = 0;
			for (int i = 0; checkBuff[i]; i++) {
				if (temp)* (p++) = checkBuff[i];
				if (checkBuff[i] != ';') {
					name[i] = checkBuff[i];
				}
				else {
					name[i] = 0;
					temp = 1;
				}
			}
			*p = 0;


			if (type == CHECKNUM) {//登录

				char sha256[256];
				Sha256* sha_hst = nullptr;
				sha_hst->StrSHA256(password, sizeof(password) - 1, sha256);  // sizeof()包含了末尾的终止符'\0'故 -1

				//验证用户名和密码
				int userid = dbUtil->LogInCheck(name, sha256);
				if (userid == -1) {
					cout << usernames[idx] << "登录失败！" << endl;
					ServerLog loginLog("用户" +string(usernames[idx]) + "登录失败！", 
						"login.log");
					checkBuff[0] = WORNG_USER;
				}
				else {
					flag = 0;
					cout << usernames[idx] << "登录成功！" << endl;
					ServerLog loginLog_2("用户" + string(usernames[idx]) + "登录成功！",
						"login.log");
				}
				checkBuff[1] = CHECKNUM;
				checkBuff[2] = 0;
				send(clientSocket[idx], checkBuff, strlen(checkBuff), NULL);
			}
			else if (type == SIGHUPNUM) {//注册
				int _register = dbUtil->InsertUser(userIP, name, password);
				if (_register == -4) {
					checkBuff[0] = WORNG_USER;
					cout << usernames[idx] << "已存在！" << endl;
					ServerLog registerLog("用户" + string(usernames[idx]) + "注册失败！", "Register.log");
				}
				else if(_register == 1)
				{
					//更新本地用户类顺便改变新注册用户的在线状态
					usersnum = dbUtil->GetUsers(users);
					flag = 0;
					cout << usernames[idx] << "注册成功！" << endl;
					ServerLog registerLog_1("用户" + string(usernames[idx]) + "注册成功！", "Register.log");
				}
				else
				{
					cout << "用户验证 select data failed" << endl;
					ServerLog registerLog("用户" + string(usernames[idx]) + "注册失败！", "Register.log");
				}

				checkBuff[1] = SIGHUPNUM;
				checkBuff[2] = 0;
				send(clientSocket[idx], checkBuff, strlen(checkBuff), NULL);
			}

			//将用户名与登录序号绑定
			strcpy(usernames[idx], name);
		}
	}
	//用户选择是单聊还是群聊	
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	int r = recv(clientSocket[idx], buff, 1023, NULL);
	if (r > 0 && buff[r - 1] == CHOOSENUM) {
		if (buff[0] == '1') {
			//创造新线程, 进入群聊界面
			cout << "用户" << usernames[idx] << "选择了群聊" << endl;
			ServerLog chooseLog("用户" + string(usernames[idx]) + "选择了群聊！",
				"Choose.log");
			dbUtil->ChatroomConnection(idx);
		}

		else if (buff[0] == '2') {
			cout << "用户" << usernames[idx] << "选择了单聊" << endl;
			ServerLog chooseLog_1("用户" + string(usernames[idx]) + "选择了单聊！", 
				"Choose.log");
			dbUtil->SingleConnection(idx);
		}

}


}



int main() {
	if (hstSocket->prepare() < 0)
	{
		return 0;
	}

	while (1) {

		SOCKADDR_IN cAddr = { 0 };
		int len = sizeof(cAddr);
		clientSocket[concOrder] = accept(serverSocket, (sockaddr*)& cAddr, &len);

		if (clientSocket[concOrder] == -1) {
			printf("客户端连接失败\n");
			ServerLog serverLog("客户端连接失败。", "Server.log");

			closesocket(serverSocket);
			WSACleanup();
			return -2;
		}

		userIP = inet_ntoa(cAddr.sin_addr);
		printf("用户%d连接了服务器：%s!\n", concOrder, userIP);
		ServerLog serverLog_1("用户" + string(userIP) + "连接了服务器", "Server.log");
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)hstSocket->service, (char*)concOrder, NULL, NULL);
		concOrder++;

	}

	return 0;
}
