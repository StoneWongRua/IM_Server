#define _CRT_SECURE_NO_WARNINGS
#pragma once

/*
这里定义了与数据库沟通的一些函数
*/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <stdexcept>
#include <unordered_map>
#include <windows.h> //必须在包含mysql.h之前包含windows.h
#include <mysql.h>

#include"ObjectConstant.h"

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

using namespace std;



class DbUtil
{
public:
	DbUtil();
	~DbUtil();
	int DatabaseInit();
	int CreateUser();
	int CreateFriends();
	int GetUsers(User* users);
	int GetFriends(Friends* friends);
	int LogInCheck(string name, string password);
	int InsertUser(string ip, string name, string password);
	int InsertFriends(string user1name, string user2name);
	int InsertFriend(string user1name, string user2name);
	int test(string user1name);

	void ChatroomConnection(int idx);
	void SingleConnection(int idx);
	void AddFriend(int idx);
	static string GetTime();


private:
	MYSQL mydata;
	string sqlstr;
	MYSQL_RES* result = NULL;


};




