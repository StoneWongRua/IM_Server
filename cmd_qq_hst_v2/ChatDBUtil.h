#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <stdexcept>
#include <unordered_map>
#include <windows.h> //�����ڰ���mysql.h֮ǰ����windows.h
#include <mysql.h>

using std::string;
using std::vector;
using std::endl;

///���ݿ���������,�����������ݿ�,����װ���������ݿ���ز���
class DbConnector {
protected:
	MYSQL* mysql;
public:
	static string host_, user_, passwd_, db_;
public:
	  DbConnector();
	  virtual ~DbConnector();
	//ִ��sql���,�ɹ��򷵻�0,ʧ�ܷ��ط�0
	  int query(const string& sql);
	//������һ�εĴ�����ʾ
	  string error();
	//��ȡ�����,���䱣�浽��άstring������
	  vector<vector<string> > GetResult();
};

///��Ϣ������
class DbMsg {
protected:
	DbConnector db_conn; //���ݿ�����������
	const string tbName;  //Ҫ�����ı���
	std::ofstream fsLog;  //��־��¼�ļ���
public:
	  DbMsg(const string& _tableName, const string& _logFileName);
	virtual ~DbMsg();
	//�����Ϣ
	  void push(const string& to, const string& from, const string& msg);
	//��ȡִ��ָ��sql����Ľ��
	  vector<vector<string> > GetBySql(const string& sql);
	//����ָ��sql���ɾ����Ӧ��¼
	  void RemoveBySql(const string& sql);
	  void ServerLog(const string& msg, const string& _logFileName);
	//��ȡ��ǰ���ں�ʱ��,���ڸ�����־
	  static string GetTime();
protected:
	//�����洢�����¼�ı�
	  void CreateTable();
};

///������Ϣ������ - �����ڷ�������
class DbOfflineMsg : public DbMsg {
public:
	DbOfflineMsg(const string& _tableName, const string _logFileName = "")
		: DbMsg(_tableName, _logFileName)
	{}
	//��������Ϣ���еõ����з��͸�user����Ϣ,��ȡ�������ݿ�ɾ��
	  vector<vector<string> > pop(const string& user);
};

///�����¼��Ϣ������
class DbChatLogMsg : public DbMsg {
public:
	//�û�����ʶ�û�,��־�ļ���Ϊ��ʱ������ǰ׺�����������ڿͻ��˻��Ƿ�������
	  DbChatLogMsg(const string& _userName, const string _logFileName = "",
		const string& _prefix = "")
		: DbMsg(_prefix + "chat_log_" + _userName, _logFileName), userName(_userName)
	{}
	//��������¼��Ϣ
	void push(const string& userOthers, const string& msg, bool isReceived = 1);
	//����յ��Ĵ�ʱ���¼��������Ϣ
	  void PushOffline(const string& from, const string& msg, const string& _time);
	//��ȡ��Ϣuser�����Ļ򷢸�user��������Ϣ
	  vector<vector<string> > get(const string& user);
	//�ڸ��û��������¼�в�����withUser�İ���str����Ϣ��¼
	  vector<vector<string> > find(const string& str, const string& withUser = "*");
	//���ݴ��ݵ��û���ɾ�����Ӧ�û��������¼,����"*"��ɾ�����м�¼
	  void remove(const string& user);
	//��ȡ�뵱ǰ�û��������¼���û��б�
	  vector<string> GetUserWithChatLog();
	//��ȡ��ǰ�û����û���
	  const string& GetUserName();
protected:
	string userName; //���û����û���
};



