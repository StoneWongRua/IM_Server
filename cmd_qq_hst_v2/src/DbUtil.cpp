/*
*@Discription 消息记录等数据库操作
*@Author tong
*/


#define _CRT_SECURE_NO_WARNINGS

#include "Sha256.h"
#include"ChatDBUtil.h"

DbConnector::DbConnector()
{
	mysql = mysql_init(NULL);
	if (mysql_real_connect(mysql, host_.c_str(), user_.c_str(),
		passwd_.c_str(), db_.c_str(), 0, 0, 0) == 0) {
		string err = "Failed to connect database server: " + error();
		throw std::logic_error(err);
	}
	query("set names 'GBK'");//设置字符集,防止中文出现乱码
}

DbConnector::~DbConnector()
{
	mysql_close(mysql);
}

//执行sql语句,成功则返回0,失败返回非0
int DbConnector::query(const string& sql)
{
	return mysql_query(mysql, sql.c_str());
}

//返回上一次的错误提示
string DbConnector::error()
{
	return mysql_error(mysql);
}

//获取结果集,将其保存到二维string数组中
vector<vector<string> > DbConnector::GetResult()
{
	vector<vector<string> >result;
	MYSQL_RES* res = mysql_store_result(mysql);
	if (res == NULL)
		return result;
	unsigned num = mysql_num_fields(res); //结果集的列数
	for (MYSQL_ROW row; (row = mysql_fetch_row(res)) != NULL;) {
		result.push_back(vector<string>(num));
		for (size_t cur = result.size() - 1, i = 0; i < num; ++i) {
			result[cur][i] = row[i];
		}
	}
	mysql_free_result(res); //释放结果集资源
	return result;
}


DbMsg::~DbMsg()
{
	if (fsLog) {
		fsLog.close();
	}
}

//添加消息
void DbMsg::push(const string& to, const string& from, const string& msg)
{
	string sql = "insert into " + tbName + "(toUser,fromUser,msg) values('"
		+ to + "','" + from + "','" + msg + "')";
	if (fsLog) {
		fsLog << GetTime() << ">>>execute: " + sql << endl;
	}
	if (db_conn.query(sql) != 0 && fsLog) { //执行sql语句失败返回非0
		fsLog << "Failed to insert: " << db_conn.error() << endl;
	}
}

//获取执行指定sql语句后的结果
vector<vector<string> > DbMsg::GetBySql(const string& sql)
{
	vector<vector<string> >result;
	if (fsLog) {
		fsLog << GetTime() << ">>>execute: " + sql << endl;
	}
	if (db_conn.query(sql) == 0) {   //执行成功则把结果输出
		result = db_conn.GetResult();
	}
	else if (fsLog) {
		fsLog << "Failed to search: " << db_conn.error() << endl;
	}
	return result;
}

//根据指定sql语句删除相应记录
void DbMsg::RemoveBySql(const string& sql)
{
	if (fsLog) {
		fsLog << GetTime() << ">>>execute: " << sql << endl;
	}
	if (db_conn.query(sql) != 0 && fsLog) {
		fsLog << "Failed to delete: " << db_conn.error() << endl;
	}
}

//获取当前日期和时间,用于更新日志
string DbMsg::GetTime()
{
	char time_buf[64];
	time_t now_time = time(NULL);
	strftime(time_buf, 64, "%Y-%m-%d %H:%M:%S ", localtime(&now_time));
	return time_buf;
}

//创建存储聊天记录的表
void DbMsg::CreateTable()
{
	db_conn.query("show tables like '" + tbName + "'");
	auto res = db_conn.GetResult();
	if (res.size() > 0 && res[0][0] == tbName) {
		return;
	}
	string sql = "CREATE TABLE `" + tbName + "` ("
		"`id` tinyint(4) NOT NULL AUTO_INCREMENT,"
		"`toUser` varchar(20) NOT NULL,"
		"`fromUser` varchar(20) NOT NULL,"
		"`time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,"
		"`msg` text NOT NULL,"
		"PRIMARY KEY (`id`)"
		") ENGINE=InnoDB DEFAULT CHARSET=utf8;";
	if (fsLog) {
		fsLog << GetTime() << ">>>execute: " << sql << endl;
	}
	if (db_conn.query(sql) != 0 && fsLog) {
		fsLog << "Failed to create: " << db_conn.error() << endl;
	}
}

DbMsg::DbMsg(const string& _tableName, const string& _logFileName)
	:tbName(_tableName)
{
	if (_logFileName != "") {
		fsLog.open(_logFileName, std::ios::app); //append,指可在原log文件后追加
	} //若日志名为空字符串，即不想要日志文件，那么就不会调用open()，那么fsLog这个对象就是失效的（相当于false）
	CreateTable();
}


//从离线消息表中得到所有发送给user的消息,提取完后从数据库删除
vector<vector<string> > DbOfflineMsg::pop(const string& user)
{
	string sql = "select from User,time,msg from " + tbName
		+ " where toUser='" + user + "'";

	vector<vector<string> >result = GetBySql(sql);

	//删除发给user的所有消息
	if (result.size() > 0) {
		sql = "delete from " + tbName + " where toUser='" + user + "'";
		RemoveBySql(sql);
	}


	return result;
}

//添加聊天记录消息
void DbChatLogMsg::push(const string& userOthers, const string& msg, bool isReceived)
{
	if (isReceived) { //收到消息
		DbMsg::push(userName, userOthers, msg);
	}
	else { //发送消息
		DbMsg::push(userOthers, userName, msg);
	}
}

//添加收到的带时间记录的离线消息
void DbChatLogMsg::PushOffline(const string& from, const string& msg,
	const string& _time)
{
	string sql = "insert into " + tbName + "(toUser,fromUser,time,msg) values('"
		+ userName + "','" + from + "','" + _time + "','" + msg + "')";
	if (fsLog) {
		fsLog << GetTime() << ">>>execute: " + sql << endl;
	}
	if (db_conn.query(sql) != 0 && fsLog) { //执行sql语句失败返回非0
		fsLog << "Failed to insert: " << db_conn.error() << endl;
	}
}



//获取消息user发来的或发给user的所有消息
vector<vector<string> > DbChatLogMsg::get(const string& user)
{
	string sql = "select fromUser,time,msg from " + tbName +
		" where toUser='" + user + "' or fromUser='" + user + "'";
	return GetBySql(sql);
}

//在该用户的聊天记录中查找与withUser的包含str的消息记录
vector<vector<string> > DbChatLogMsg::find(const string& str, const string& withUser)
{
	string sql = "select fromUser,toUser,time,msg from " + tbName + " where ";
	if (withUser != "*") {
		sql += "(toUser='" + withUser + "' or fromUser='" + withUser + "') and";
	}
	sql += " msg like '%" + str + "%'";
	return GetBySql(sql);
}

//根据传递的用户名删除与用户user的聊天记录,传入"*"则删除所有记录
void DbChatLogMsg::remove(const string& user)
{
	string sql = "delete from " + tbName;
	if (user != "*") {
		sql += " where toUser='" + user + "' or fromUser='" + user + "'";
	}
	RemoveBySql(sql);
}

//获取与当前用户有聊天记录的用户列表
vector<string> DbChatLogMsg::GetUserWithChatLog()
{
	string sql = "select distinct toUser from " + tbName;
	auto users = GetBySql(sql);
	std::unordered_map<string, bool> userList;
	for (auto &elem : users) {
		userList[elem[0]];
	}
	sql = "select distinct from User from " + tbName;
	users = GetBySql(sql);
	for (auto &elem : users) {
		userList[elem[0]];
	}
	userList.erase(userList.find(userName)); //删除用户列表中的自己
	//将结果保存到vector<string>中返回
	vector<string> res;
	res.reserve(userList.size());
	for (auto &it : userList) {
		res.push_back(it.first);
	}
	return res;
}

//获取当前用户的用户名
const string& DbChatLogMsg::GetUserName()
{
	return userName;
}

using std::cout;
//可直接输出查询结果
std::ostream& operator<<(std::ostream& out, const vector<vector<string> >& res)
{
	if (res.size() == 0) {
		cout << "暂无内容可显示!" << endl;
		return out;
	}
	for (auto &it : res) { //显示提取出的离线消息内容
		for (auto &elem : it) {
			cout << elem << " ";
		}
		cout << endl;
	}
	return out;
}


