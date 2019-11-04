#ifndef __MYSQLDEF_H__
#define __MYSQLDEF_H__
#include <string>
//���ݿ�����������ֵ
#define MaxMysqlMsgCount    50

//���ݿ����Ӷ˿�
#define MysqlConnPort       3306

using namespace std;

//���ݿ��������
enum MysqlOper
{
	MYSQL_OPER_INVALID = 0,
	MYSQL_OPER_START = 0,  //��Чֵ��ʼֵ
	MYSQL_OPER_INSERT = 1,  //��
	MYSQL_OPER_DELETE = 2,  //ɾ
	MYSQL_OPER_SELECT = 3,  //��
	MYSQL_OPER_UPDATE = 4,  //��
	MYSQL_OPER_ALTER = 5,  //���ı�����
	MYSQL_OPER_CALL_PROC = 6,  //���ô洢����

	MYSQL_OPER_END             //��Чֵ��ֵֹ
};

//���ݿ���нṹ��
typedef struct
{
	string strMysql;
	MysqlOper mysqlOper;
	int AssistId; //Э�鸨��id
	unsigned __int64 SocketId;
	string strRecord;
} MysqlMsgData, *PtrMysqlMsgData;



#endif