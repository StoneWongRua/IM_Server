#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <stdexcept>
#include <unordered_map>
#include <windows.h> //必须在包含mysql.h之前包含windows.h
#include <mysql.h>

#include"ChatDBUtil.h"

using std::string;
using std::vector;
using std::endl;

class ServerLog
{
public:
	ServerLog();

	ServerLog(const string& _actionName, const string& _logFileName)
		:actionName(_actionName) {
		if (_logFileName != "") {
			fsLog.open(_logFileName, std::ios::app); //append,指可在原log文件后追加
		}

		string sql = "_sql_";
		if (fsLog) {
			fsLog << DbMsg::GetTime() << ">>>execute: " << actionName << endl;
		}

	}

protected:
private:
	const string actionName;  //操作
	std::ofstream fsLog;  //日志记录文件流
	string userName; //该用户的用户名
};

ServerLog::ServerLog() {
}