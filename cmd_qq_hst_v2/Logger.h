#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <stdexcept>
#include <unordered_map>
#include <windows.h> //�����ڰ���mysql.h֮ǰ����windows.h
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
			fsLog.open(_logFileName, std::ios::app); //append,ָ����ԭlog�ļ���׷��
		}

		string sql = "_sql_";
		if (fsLog) {
			fsLog << DbMsg::GetTime() << ">>>execute: " << actionName << endl;
		}

	}

protected:
private:
	const string actionName;  //����
	std::ofstream fsLog;  //��־��¼�ļ���
	string userName; //���û����û���
};

ServerLog::ServerLog() {
}