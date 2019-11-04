#pragma once
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
class Event
{
public:
	Event();
	~Event();
	//��������
	int databaseInit();
	int createUser();
	int createFriends();
	int insertUser(string, string, string);
	int insertFriends(string, string);
	int logInCheck(string, string);
	int getUsers(User*);
	int getFriends(Friends*);
	void singleConnection(int idx);
	void chatroomConnection(int idx);

private:

};

Event::Event()
{
}

Event::~Event()
{
}

//ȫ�ֱ���
SOCKET serverSocket;//������׽���
SOCKET clientSocket[100];//�ͻ����׽���
static int concOrder = 0;//�ͻ������ӵĴ���
static char usernames[1024][50];//���ӳɹ����û����û���
static int usersnum;//���ݿ����û�������
User users[100];//�����ݿ��н��յ��û���Ϣ
static int friendssnum;//���ݿ��к��ѶԵ�����
Friends friends[100];//�����ݿ��н��յĺ��Ѷ���Ϣ
char* userIP;