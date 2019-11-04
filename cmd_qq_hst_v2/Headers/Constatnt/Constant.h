#pragma once

#include <vector>
#include "winsock2.h"  
#include <iostream>  
#include<string>
#include <Windows.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")

extern int shutdownf;
extern int goon;
using namespace std;


#define   ChatMsg             0
#define   Server_is_closed    1
#define   UserList            2
#define   OnlineState         3
#define   FileSend            4
#define   FileData            5
#define   AskFileData         6
#define   File_NO             7
#define   File_Over           8
#define   File_Fail           9
#define   LoginFail           10
#define   UserIsOnline        11
#define   OfflineMsg          12
#define   AllUser             13
#define   AddUserList         14
#define   I_am_online         15
#define   Logout              16
#define   Login               17
#define   Register            18
#define   Status			  19
#define   Squezze			  20

#define   TYPE_ChatMsg             "ChatMsg"
#define   TYPE_Server_is_closed    "Server_is_closed"
#define   TYPE_UserList            "UserList"
#define   TYPE_OnlineState         "OnlineState"
#define   TYPE_FileSend            "FileSend"
#define   TYPE_FileData            "FileData"
#define   TYPE_AskFileData         "AskFileData"
#define   TYPE_File_NO             "File_NO"
#define   TYPE_File_Over           "File_Over"
#define   TYPE_File_Fail           "File_Fail"
#define   TYPE_LoginFail           "LoginFail"
#define   TYPE_UserIsOnline        "UserIsOnline"
#define   TYPE_OfflineMsg          "OfflineMsg"
#define   TYPE_AllUser             "AllUser"
#define   TYPE_AddUserList         "AddUserList"
#define   TYPE_I_am_online         "I_am_online"
#define   TYPE_Logout              "Logout"
#define   TYPE_Login               "Login"
#define   TYPE_Register            "Register"
#define   TYPE_Status              "Status"
#define   TYPE_Squezze              "Squezze"

const int BUF_SIZE = 2048;

extern const string TYPE[31];
extern const char seperator[3];

class HstMsg
{
public:
	string userId; //用户名
	string pw; //密码
	string fromUser; //消息来自
	string toUser; //消息去向
	string type; //消息类型
	string data; //消息内容

	explicit HstMsg(string str = "");
	string load(string str);
	string join(string _data = "", string _type = "", string _user = "", string _from = "", string  _to = "", string _pw = "")const;

	static string rightN(const string & str, int n) {
		return str.substr(n);
	}

};