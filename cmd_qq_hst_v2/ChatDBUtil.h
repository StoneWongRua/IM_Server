#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <stdexcept>
#include <unordered_map>
#include <windows.h> //必须在包含mysql.h之前包含windows.h
#include <mysql.h>

using std::string;
using std::vector;
using std::endl;

///数据库连接器类,负责连接数据库,并封装基本的数据库相关操作
class DbConnector {
protected:
	MYSQL* mysql;
public:
	static string host_, user_, passwd_, db_;
public:
	  DbConnector();
	  virtual ~DbConnector();
	//执行sql语句,成功则返回0,失败返回非0
	  int query(const string& sql);
	//返回上一次的错误提示
	  string error();
	//获取结果集,将其保存到二维string数组中
	  vector<vector<string> > GetResult();
};

///消息管理类
class DbMsg {
protected:
	DbConnector db_conn; //数据库连接器对象
	const string tbName;  //要操作的表名
	std::ofstream fsLog;  //日志记录文件流
public:
	  DbMsg(const string& _tableName, const string& _logFileName);
	virtual ~DbMsg();
	//添加消息
	  void push(const string& to, const string& from, const string& msg);
	//获取执行指定sql语句后的结果
	  vector<vector<string> > GetBySql(const string& sql);
	//根据指定sql语句删除相应记录
	  void RemoveBySql(const string& sql);
	  void ServerLog(const string& msg, const string& _logFileName);
	//获取当前日期和时间,用于更新日志
	  static string GetTime();
protected:
	//创建存储聊天记录的表
	  void CreateTable();
};

///离线消息管理类 - 仅用于服务器端
class DbOfflineMsg : public DbMsg {
public:
	DbOfflineMsg(const string& _tableName, const string _logFileName = "")
		: DbMsg(_tableName, _logFileName)
	{}
	//从离线消息表中得到所有发送给user的消息,提取完后从数据库删除
	  vector<vector<string> > pop(const string& user);
};

///聊天记录消息管理类
class DbChatLogMsg : public DbMsg {
public:
	//用户名标识用户,日志文件名为空时不建立前缀用于区分是在客户端还是服务器端
	  DbChatLogMsg(const string& _userName, const string _logFileName = "",
		const string& _prefix = "")
		: DbMsg(_prefix + "chat_log_" + _userName, _logFileName), userName(_userName)
	{}
	//添加聊天记录消息
	void push(const string& userOthers, const string& msg, bool isReceived = 1);
	//添加收到的带时间记录的离线消息
	  void PushOffline(const string& from, const string& msg, const string& _time);
	//获取消息user发来的或发给user的所有消息
	  vector<vector<string> > get(const string& user);
	//在该用户的聊天记录中查找与withUser的包含str的消息记录
	  vector<vector<string> > find(const string& str, const string& withUser = "*");
	//根据传递的用户名删除与对应用户的聊天记录,传入"*"则删除所有记录
	  void remove(const string& user);
	//获取与当前用户有聊天记录的用户列表
	  vector<string> GetUserWithChatLog();
	//获取当前用户的用户名
	  const string& GetUserName();
protected:
	string userName; //该用户的用户名
};



