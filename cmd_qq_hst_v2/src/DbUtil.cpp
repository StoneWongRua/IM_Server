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
	query("set names 'GBK'");//�����ַ���,��ֹ���ĳ�������
}

DbConnector::~DbConnector()
{
	mysql_close(mysql);
}

//ִ��sql���,�ɹ��򷵻�0,ʧ�ܷ��ط�0
int DbConnector::query(const string& sql)
{
	return mysql_query(mysql, sql.c_str());
}

//������һ�εĴ�����ʾ
string DbConnector::error()
{
	return mysql_error(mysql);
}

//��ȡ�����,���䱣�浽��άstring������
vector<vector<string> > DbConnector::GetResult()
{
	vector<vector<string> >result;
	MYSQL_RES* res = mysql_store_result(mysql);
	if (res == NULL)
		return result;
	unsigned num = mysql_num_fields(res); //�����������
	for (MYSQL_ROW row; (row = mysql_fetch_row(res)) != NULL;) {
		result.push_back(vector<string>(num));
		for (size_t cur = result.size() - 1, i = 0; i < num; ++i) {
			result[cur][i] = row[i];
		}
	}
	mysql_free_result(res); //�ͷŽ������Դ
	return result;
}


DbMsg::~DbMsg()
{
	if (fsLog) {
		fsLog.close();
	}
}

//�����Ϣ
void DbMsg::push(const string& to, const string& from, const string& msg)
{
	string sql = "insert into " + tbName + "(toUser,fromUser,msg) values('"
		+ to + "','" + from + "','" + msg + "')";
	if (fsLog) {
		fsLog << GetTime() << ">>>execute: " + sql << endl;
	}
	if (db_conn.query(sql) != 0 && fsLog) { //ִ��sql���ʧ�ܷ��ط�0
		fsLog << "Failed to insert: " << db_conn.error() << endl;
	}
}

//��ȡִ��ָ��sql����Ľ��
vector<vector<string> > DbMsg::GetBySql(const string& sql)
{
	vector<vector<string> >result;
	if (fsLog) {
		fsLog << GetTime() << ">>>execute: " + sql << endl;
	}
	if (db_conn.query(sql) == 0) {   //ִ�гɹ���ѽ�����
		result = db_conn.GetResult();
	}
	else if (fsLog) {
		fsLog << "Failed to search: " << db_conn.error() << endl;
	}
	return result;
}

//����ָ��sql���ɾ����Ӧ��¼
void DbMsg::RemoveBySql(const string& sql)
{
	if (fsLog) {
		fsLog << GetTime() << ">>>execute: " << sql << endl;
	}
	if (db_conn.query(sql) != 0 && fsLog) {
		fsLog << "Failed to delete: " << db_conn.error() << endl;
	}
}

//��ȡ��ǰ���ں�ʱ��,���ڸ�����־
string DbMsg::GetTime()
{
	char time_buf[64];
	time_t now_time = time(NULL);
	strftime(time_buf, 64, "%Y-%m-%d %H:%M:%S ", localtime(&now_time));
	return time_buf;
}

//�����洢�����¼�ı�
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
		fsLog.open(_logFileName, std::ios::app); //append,ָ����ԭlog�ļ���׷��
	} //����־��Ϊ���ַ�����������Ҫ��־�ļ�����ô�Ͳ������open()����ôfsLog����������ʧЧ�ģ��൱��false��
	CreateTable();
}


//��������Ϣ���еõ����з��͸�user����Ϣ,��ȡ�������ݿ�ɾ��
vector<vector<string> > DbOfflineMsg::pop(const string& user)
{
	string sql = "select from User,time,msg from " + tbName
		+ " where toUser='" + user + "'";

	vector<vector<string> >result = GetBySql(sql);

	//ɾ������user��������Ϣ
	if (result.size() > 0) {
		sql = "delete from " + tbName + " where toUser='" + user + "'";
		RemoveBySql(sql);
	}


	return result;
}

//��������¼��Ϣ
void DbChatLogMsg::push(const string& userOthers, const string& msg, bool isReceived)
{
	if (isReceived) { //�յ���Ϣ
		DbMsg::push(userName, userOthers, msg);
	}
	else { //������Ϣ
		DbMsg::push(userOthers, userName, msg);
	}
}

//����յ��Ĵ�ʱ���¼��������Ϣ
void DbChatLogMsg::PushOffline(const string& from, const string& msg,
	const string& _time)
{
	string sql = "insert into " + tbName + "(toUser,fromUser,time,msg) values('"
		+ userName + "','" + from + "','" + _time + "','" + msg + "')";
	if (fsLog) {
		fsLog << GetTime() << ">>>execute: " + sql << endl;
	}
	if (db_conn.query(sql) != 0 && fsLog) { //ִ��sql���ʧ�ܷ��ط�0
		fsLog << "Failed to insert: " << db_conn.error() << endl;
	}
}



//��ȡ��Ϣuser�����Ļ򷢸�user��������Ϣ
vector<vector<string> > DbChatLogMsg::get(const string& user)
{
	string sql = "select fromUser,time,msg from " + tbName +
		" where toUser='" + user + "' or fromUser='" + user + "'";
	return GetBySql(sql);
}

//�ڸ��û��������¼�в�����withUser�İ���str����Ϣ��¼
vector<vector<string> > DbChatLogMsg::find(const string& str, const string& withUser)
{
	string sql = "select fromUser,toUser,time,msg from " + tbName + " where ";
	if (withUser != "*") {
		sql += "(toUser='" + withUser + "' or fromUser='" + withUser + "') and";
	}
	sql += " msg like '%" + str + "%'";
	return GetBySql(sql);
}

//���ݴ��ݵ��û���ɾ�����û�user�������¼,����"*"��ɾ�����м�¼
void DbChatLogMsg::remove(const string& user)
{
	string sql = "delete from " + tbName;
	if (user != "*") {
		sql += " where toUser='" + user + "' or fromUser='" + user + "'";
	}
	RemoveBySql(sql);
}

//��ȡ�뵱ǰ�û��������¼���û��б�
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
	userList.erase(userList.find(userName)); //ɾ���û��б��е��Լ�
	//��������浽vector<string>�з���
	vector<string> res;
	res.reserve(userList.size());
	for (auto &it : userList) {
		res.push_back(it.first);
	}
	return res;
}

//��ȡ��ǰ�û����û���
const string& DbChatLogMsg::GetUserName()
{
	return userName;
}

using std::cout;
//��ֱ�������ѯ���
std::ostream& operator<<(std::ostream& out, const vector<vector<string> >& res)
{
	if (res.size() == 0) {
		cout << "�������ݿ���ʾ!" << endl;
		return out;
	}
	for (auto &it : res) { //��ʾ��ȡ����������Ϣ����
		for (auto &elem : it) {
			cout << elem << " ";
		}
		cout << endl;
	}
	return out;
}


