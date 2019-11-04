#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<iostream>
#include<string>
#include <windows.h>
using namespace std;

#include "FriendDBUtil.h"
#include "GlobalConstant.h"
#include "Sha256.h"
#include "Logger.h"

#pragma comment(lib, "ws2_32.lib")


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


int  prepare();
void service(int idx);


DbUtil* dbUtil = new DbUtil();

void DbUtil::SingleConnection(int idx) {//���ĺ���


	//��ѯ���û��ĺ���,����friendList�����friendStream�ַ�����
	int count = 0;
	string friendList[100];
	string friendStream;

	cout << "friendssnum:" << friendssnum << endl;

	for (int i = 0; i < friendssnum; i++) {
		if (usernames[idx] == friends[i].user1name) {
			cout << i << ":" << friends[i].user2name << endl;
			friendList[count++] = friends[i].user2name;
			friendStream += friends[i].user2name;
			friendStream += ";";
		}
	}

	//��friendStream����ʶ����תΪchar*���͸��ͻ���
	char* buff = (char*)friendStream.data();
	int len = strlen(buff);
	buff[len] = CHOOSEFRIENDNUM;
	buff[len + 1] = 0;
	int s = send(clientSocket[idx], buff, len + 1, NULL);
	//���տͻ��˵�ѡ����Ϣ
	int r = recv(clientSocket[idx], buff, 1023, NULL);


	//cout << "�û�ѡ����" << friends[buff[0]].user2name << endl;

	cout << "�û�ѡ����" << buff[0] << endl;
	ServerLog chooseLog_2("�û�ѡ����" + buff[0], "Choose.log");
	string choosenFriend = friendList[buff[0] - '0'];


	//�������Ӧ���ѵ��������
	memset(buff, 0, sizeof(buff));

	while (1) {
		int r = recv(clientSocket[idx], buff, 1023, NULL);

		if (r > 0 && buff[r - 1] == MESSAGENUM) {
			buff[r - 1] = 0;
			printf(">>�û�%s:%s\n", usernames[idx], buff);


			//������¼���������ߣ����û���������Ϣ
			for (int i = 0; i < concOrder; i++) {
				if (usernames[i] == choosenFriend || i == idx) {

					string logname = "chat_log_";
					DbChatLogMsg clMsg(usernames[idx], logname.append(usernames[idx]).append(".log"));

					clMsg.push(choosenFriend, buff, 0);

					//����Ϣǰ�����û�name
					char temp[50];
					strcpy(temp, usernames[idx]);
					strcat(temp, ":");
					strcat(temp, buff);
					strcpy(buff, temp);



					int len = strlen(buff);
					buff[len] = MESSAGENUM;
					buff[len + 1] = 0;


					int s = send(clientSocket[i], buff, len + 1, NULL);
				}
				else if (usernames[i] != choosenFriend)
				{
					cout << "!+";
					DbOfflineMsg off_msg("offline_msg", "offileMsg.log");
					off_msg.push("to", "from", "hey");   //����һ��������Ϣ
					auto result = off_msg.pop("to"); //��ȡ�������û���ȫ��������Ϣ
				}
			}
		}
		//����Ҳ���Խ����û�������������Ӻ��ѵ�
		else if (r > 0 && buff[r - 1] == ADDFRIENDNUM) {
			buff[r - 1] = 0;
			string addFriendName = buff;
			int i = dbUtil->InsertFriends(usernames[idx], addFriendName);
			if (i == 0) {
				friendssnum = dbUtil->GetFriends(friends);
				cout << "�������ѹ�ϵ�ɹ���" << usernames[idx] << "��" << addFriendName << endl;
				ServerLog friendrLog("�û�" + string(usernames[idx]) + "��" + string(addFriendName) + "�ɹ��������ѹ�ϵ��", "Friend.log");
			}
			else {
				cout << "�������ѹ�ϵʧ�ܣ�" << endl;
				ServerLog friendrLog_1("�û�" + string(usernames[idx]) + "��" + string(addFriendName) + "�������ѹ�ϵʧ�ܡ�", "Friend.log");

			}
		}
	}

}

void DbUtil::ChatroomConnection(int idx) {//Ⱥ�ĺ�����
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	while (1) {
		int r = recv(clientSocket[idx], buff, 1023, NULL);
		if (r > 0 && buff[r - 1] == MESSAGENUM) {
			buff[r - 1] = 0;
			printf(">>�û�%s:%s\n", usernames[idx], buff);

			DbChatLogMsg clMsg(usernames[idx], "chat_log_Bob.log");
			clMsg.push("group", buff, 0);

			//����Ϣǰ�����û�name
			char temp[50];
			strcpy(temp, usernames[idx]);
			strcat(temp, ":");
			strcat(temp, buff);
			strcpy(buff, temp);
			int len = strlen(buff);
			buff[len] = MESSAGENUM;
			buff[len + 1] = 0;
			//�㲥��Ϣ
			for (int i = 0; i < concOrder; i++) {
				//if (i == idx) continue;
				int s = send(clientSocket[i], buff, len + 1, NULL);
			}
			buff[0] = 0;
		}
		else if (r > 0 && buff[r - 1] == ADDFRIENDNUM) {
			buff[r - 1] = 0;
			string addFriendName = buff;
			int i = dbUtil->InsertFriends(usernames[idx], addFriendName);
			if (i == 0) {
				cout << "�������ѹ�ϵ�ɹ���" << usernames[idx] << "��" << addFriendName << endl;
				ServerLog friendrLog_2("�û�" + string(usernames[idx]) + "��" + string(addFriendName) + "�ɹ��������ѹ�ϵ��", "Friend.log");

			}
			else {
				cout << "�������ѹ�ϵʧ�ܣ�" << endl;
				ServerLog friendrLog_3("�û�" + string(usernames[idx]) + "��" + string(addFriendName) + "�������ѹ�ϵʧ�ܡ�", "Friend.log");

			}
		}
	}
}


void DbUtil::AddFriend(int idx) {
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	while (1) {
		int r = recv(clientSocket[idx], buff, 1023, NULL);
		if (r > 0 && buff[r - 1] == ADDFRIENDNUM) {
			cout << "2";
			buff[r - 1] = 0;
			string addFriendName = buff;
			cout << addFriendName;

			int i = dbUtil->InsertFriend(usernames[idx], addFriendName);

			if (i == 0) {
				buff[0] = 0;
				cout << "�������ѹ�ϵ�ɹ���" << usernames[idx] << "��" << addFriendName << endl;
				ServerLog friendrLog_2("�û�" + string(usernames[idx]) + "��" + string(addFriendName) + "�ɹ��������ѹ�ϵ��", "Friend.log");

			}
			else if (i == -2) {
				buff[0] == -2;
				cout << "���ݿ��ѯʧ�ܡ��������ѹ�ϵʧ��23333��" << endl;
				ServerLog friendrLog_3("�û�" + string(usernames[idx]) + "��" + string(addFriendName) + "�������ѹ�ϵʧ�ܡ�", "Friend.log");

			}
			else if (i == -3) {
				buff[0] == -3;
				cout << "�Ѿ��Ǻ��ѹ�ϵ��" << usernames[idx] << "��" << addFriendName << endl;
			}
			else if (i = -4)
			{
				buff[0] == -4;
				cout << "�������û�" << addFriendName << endl;
			}
		}
	}
}


int prepare() {
	SetConsoleTitle("����ϵͳ�����");
	/*
	���ã�ǰ��׼�����������ݿ⣬����socket��
	�������ޣ�
	����ֵ��-1��ʾ����ֱ���˳���
	*/
	memset(users, 0, sizeof(users));
	//��ʼ�����ݿ�,����
	
	dbUtil->DatabaseInit();
	dbUtil->CreateUser();
	dbUtil->CreateFriends();
	usersnum = dbUtil->GetUsers(users);
	friendssnum = dbUtil->GetFriends(friends);

	// 1.����汾Э��
	WSADATA wsData;
	WSAStartup(MAKEWORD(2, 2), &wsData);
	if ((LOBYTE(wsData.wVersion) != 2) || (HIBYTE(wsData.wVersion) != 2)) {
		printf("------����汾Э��ʧ��------\n");
		ServerLog initServerLog_1("����汾Э��ʧ��", 
			"InitServer.log");
		return -1;
	}
	printf("------����汾Э��ɹ�------\n");
	ServerLog initServerLog_2("����汾Э��ɹ�", 
		"InitServer.log");


	// 2.����socket
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == SOCKET_ERROR) {
		printf("����socketʧ��\n");
		ServerLog initServerLog_3("����socketʧ��", 
			"InitServer.log");
		WSACleanup();
		return -2;
	}
	printf("------����socket�ɹ�--------\n");
	ServerLog initServerLog_4("����socket�ɹ�", 
		"InitServer.log");


	// 3.����ip,port
	SOCKADDR_IN addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(IP);
	addr.sin_port = htons(PORT);


	// 4.��ip,port��socket
	int b = bind(serverSocket, (sockaddr*)& addr, sizeof addr);
	if (b == -1) {
		printf("��ip��portʧ��\n");
		ServerLog serverLog("��ip��portʧ��", 
			"InitServer.log");
		closesocket(serverSocket);
		WSACleanup();
		return -2;
	}
	printf("------��ip���˿ڳɹ�------\n");
	ServerLog initServerLog_5("��ip���˿ڳɹ�", 
		"InitServer.log");


	// 5. ����
	int l = listen(serverSocket, 10);
	if (l == -1) {
		printf("����ʧ��\n");
		ServerLog initServerLog_6("����ʧ��", 
			"InitServer.log");
		closesocket(serverSocket);
		WSACleanup();
		return -2;
	}

	printf("���ڼ���......\n");
	ServerLog initServerLog_7("��ʼ����\n", 
		"InitServer.log");
	return 0;
}

void service(int idx) {

	/*
	���ã�ÿ��һ���ͻ������ӣ������µ��̷߳���
	�������ͻ����������
	����ֵ����
	*/

	int flag = 1;
	while (flag) {
		char checkBuff[1024];
		int r = recv(clientSocket[idx], checkBuff, 1023, NULL);
		if (r > 0) {

			int type = checkBuff[r - 1];//������¼����ע��
			checkBuff[r - 1] = 0;
			char name[1024];
			char password[1024];

			//��checkBuff�е����ݷֱ����name�� password��
			char* p = password;
			int temp = 0;
			for (int i = 0; checkBuff[i]; i++) {
				if (temp)* (p++) = checkBuff[i];
				if (checkBuff[i] != ';') {
					name[i] = checkBuff[i];
				}
				else {
					name[i] = 0;
					temp = 1;
				}
			}
			*p = 0;


			if (type == CHECKNUM) {//��¼

				char sha256[256];
				Sha256* sha_hst = nullptr;
				sha_hst->StrSHA256(password, sizeof(password) - 1, sha256);  // sizeof()������ĩβ����ֹ��'\0'�� -1

				//��֤�û���������
				int userid = dbUtil->LogInCheck(name, sha256);
				if (userid == -1) {
					cout << usernames[idx] << "��¼ʧ�ܣ�" << endl;
					ServerLog loginLog("�û�" +string(usernames[idx]) + "��¼ʧ�ܣ�", 
						"login.log");
					checkBuff[0] = WORNG_USER;
				}
				else {
					flag = 0;
					cout << usernames[idx] << "��¼�ɹ���" << endl;
					ServerLog loginLog_2("�û�" + string(usernames[idx]) + "��¼�ɹ���",
						"login.log");
				}
				checkBuff[1] = CHECKNUM;
				checkBuff[2] = 0;
				send(clientSocket[idx], checkBuff, strlen(checkBuff), NULL);
			}
			else if (type == SIGHUPNUM) {//ע��
				int _register = dbUtil->InsertUser(userIP, name, password);
				if (_register == -4) {
					checkBuff[0] = WORNG_USER;
					cout << usernames[idx] << "�Ѵ��ڣ�" << endl;
					ServerLog registerLog("�û�" + string(usernames[idx]) + "ע��ʧ�ܣ�", "Register.log");
				}
				else if(_register == 1)
				{
					//���±����û���˳��ı���ע���û�������״̬
					usersnum = dbUtil->GetUsers(users);
					flag = 0;
					cout << usernames[idx] << "ע��ɹ���" << endl;
					ServerLog registerLog_1("�û�" + string(usernames[idx]) + "ע��ɹ���", "Register.log");
				}
				else
				{
					cout << "�û���֤ select data failed" << endl;
					ServerLog registerLog("�û�" + string(usernames[idx]) + "ע��ʧ�ܣ�", "Register.log");
				}

				checkBuff[1] = SIGHUPNUM;
				checkBuff[2] = 0;
				send(clientSocket[idx], checkBuff, strlen(checkBuff), NULL);
			}

			//���û������¼��Ű�
			strcpy(usernames[idx], name);
		}
	}
	//�û�ѡ���ǵ��Ļ���Ⱥ��	
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	int r = recv(clientSocket[idx], buff, 1023, NULL);
	if (r > 0 && buff[r - 1] == CHOOSENUM) {
		if (buff[0] == '1') {
			//�������߳�, ����Ⱥ�Ľ���
			// 7. ���������տͻ��˷�������Ϣ
			cout << "�û�" << usernames[idx] << "ѡ����Ⱥ��" << endl;
			ServerLog chooseLog("�û�" + string(usernames[idx]) + "ѡ����Ⱥ�ģ�",
				"Choose.log");
			dbUtil->ChatroomConnection(idx);
		}

		else if (buff[0] == '2') {
			cout << "�û�" << usernames[idx] << "ѡ���˵���" << endl;
			ServerLog chooseLog_1("�û�" + string(usernames[idx]) + "ѡ���˵��ģ�", 
				"Choose.log");
			dbUtil->SingleConnection(idx);
		}
		else if (buff[0] == '3') {
			dbUtil->AddFriend(idx);
		}
}

}



int main() {
	if (prepare() < 0) return 0;

	while (1) {

		SOCKADDR_IN cAddr = { 0 };
		int len = sizeof(cAddr);
		clientSocket[concOrder] = accept(serverSocket, (sockaddr*)& cAddr, &len);
		if (clientSocket[concOrder] == -1) {
			printf("�ͻ�������ʧ��\n");
			ServerLog serverLog("�ͻ�������ʧ�ܡ�", "Server.log");

			closesocket(serverSocket);
			WSACleanup();
			return -2;
		}

		userIP = inet_ntoa(cAddr.sin_addr);
		printf("�û�%d�����˷�������%s!\n", concOrder, userIP);
		ServerLog serverLog_1("�û�" + string(userIP) + "�����˷�����", "Server.log");
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)service, (char*)concOrder, NULL, NULL);
		concOrder++;

	}

	return 0;
}