#define _CRT_SECURE_NO_WARNINGS
#pragma once

/*
���ﶨ���������ݿ⹵ͨ��һЩ����
*/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <stdexcept>
#include <unordered_map>
#include <windows.h> //�����ڰ���mysql.h֮ǰ����windows.h
#include <mysql.h>

#include"ObjectConstant.h"

#define IP "127.0.0.1"
#define PORT 12000

/*
ÿ��send��recv����Ϣ�����һ����λ������Ϣ����
��Ϣ���� | ���
-----------
��¼��֤ | 1
��ʵ��Ϣ | 2
ע����֤ | 3
������Ϣ | 4
����Ⱥ�� | 5
��Ӻ��� | 6
ѡ����� | 7
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




