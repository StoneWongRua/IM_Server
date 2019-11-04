#include "FriendDBUtil.h"




DbUtil::DbUtil()
{
}

DbUtil::~DbUtil()
{
}

int DbUtil::DatabaseInit() {
	/*
	���ã���ʼ�����ݿ�
	��������
	����ֵ��0�ɹ���-1ʧ��
	*/
	//��ʼ�����ݿ�
	mysql_library_init(0, NULL, NULL);
	//��ʼ�����ݽṹ
	if (NULL != mysql_init(&mydata)) {
		cout << "======���ݿ��ʼ���ɹ�====" << endl;
	}
	else {
		cout << "mysql_init() failed" << endl;
		return -1;
	}

	//���������ݿ�֮ǰ�����ö��������ѡ��
	//�������õ�ѡ��ܶ࣬���������ַ����������޷���������
	if (0 == mysql_options(&mydata, MYSQL_SET_CHARSET_NAME, "gbk")) {
		cout << "======���ݿ����óɹ�======" << endl;
	}
	else {
		cout << "mysql_options() failed" << endl;
		return -1;
	}

	//�������ݿ�
	if (NULL != mysql_real_connect(&mydata, NULL, "root", "root", "im", 3306, NULL, CLIENT_MULTI_STATEMENTS))
		//����ĵ�ַ���û��������룬�˿ڿ��Ը����Լ����ص��������
	{
		cout << "======���ݿ����ӳɹ�======" << endl;
	}
	else {
		cout << "mysql_real_connect() failed" << endl;
		cout << mysql_error(&mydata) << endl;
		return -1;
	}
	return 0;
	//sql�ַ���
}


int DbUtil::CreateUser() {
	/*
	���ã������û���
	��������
	����ֵ��0�ɹ���-1ʧ��
	*/
	sqlstr = "CREATE TABLE IF NOT EXISTS `user2` (";
	sqlstr += " `ID` int NOT NULL AUTO_INCREMENT,";
	sqlstr += " `ipAddress` varchar(100) NOT NULL,";
	sqlstr += " `name` varchar(30) NOT NULL, ";
	sqlstr += " `password` varchar(100) NOT NULL,";
	sqlstr += " PRIMARY KEY(`ID`)";
	sqlstr += " ) ENGINE = InnoDB DEFAULT CHARSET = utf8";

	if (mysql_query(&mydata, sqlstr.c_str()) == 0) {
		cout << "======�û������ɹ�======" << endl;
	}
	else {
		cout << "mysql_query() create table failed" << endl;
		mysql_close(&mydata);
		return -1;
	}



	return 0;
}

int DbUtil::CreateFriends() {
	/*
	���ã������û���
	��������
	����ֵ��0�ɹ���-1ʧ��
	*/
	sqlstr = "CREATE TABLE IF NOT EXISTS `friends` (";
	sqlstr += " `ID` int NOT NULL AUTO_INCREMENT,";
	sqlstr += " `user1name` varchar(50) NOT NULL,";
	sqlstr += " `user2name` varchar(50) NOT NULL,";
	sqlstr += " PRIMARY KEY(`ID`)";
	sqlstr += " ) ENGINE = InnoDB DEFAULT CHARSET = utf8";

	if (mysql_query(&mydata, sqlstr.c_str()) == 0) {
		cout << "======���ѶԱ����ɹ�======" << endl;
	}
	else {
		cout << "mysql_query() create table failed" << endl;
		mysql_close(&mydata);
		return -1;
	}
	do {
		MYSQL_RES* pRes = mysql_store_result(&mydata);
		if (pRes != NULL)
			mysql_free_result(pRes);
	} while (!mysql_next_result(&mydata));
	return 0;
}

int DbUtil::GetUsers(User* users) {
	/*
	���ã������ݿ��е��û����뵽����
	�������û�������
	����ֵ��������û�������
	*/
	//�����ݿ��е��û����뵽���أ�����ֵΪ�û�����
	sqlstr = "SELECT *FROM `im`.`user2`;";
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "=====�û����ռ��ɹ�=====" << endl;
		//һ����ȡ�����ݼ�
		result = mysql_store_result(&mydata);
		MYSQL_ROW row = NULL;
		row = mysql_fetch_row(result);
		int count = 0;
		while (NULL != row) {
			users[count].update(atoi(row[0]), row[1], row[2], row[3]);
			row = mysql_fetch_row(result);
			count++;
		}
		do {
			MYSQL_RES* pRes = mysql_store_result(&mydata);
			if (pRes != NULL)
				mysql_free_result(pRes);
		} while (!mysql_next_result(&mydata));
		return count;
	}
	else {
		cout << "�û���֤ select data failed" << endl;
		mysql_close(&mydata);
		return -2;
	}
}

int DbUtil::GetFriends(Friends* friends) {/*
	���ã������ݿ��еĺ��ѶԱ��뵽����
	���������Ѷ�������
	����ֵ������ĺ��ѶԵ�����
	*/

	sqlstr = "SELECT * FROM `im`.`friends`;";

	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "======���ѶԱ��ռ��ɹ�======" << endl;
		//һ����ȡ�����ݼ�
		result = mysql_store_result(&mydata);
		MYSQL_ROW row = NULL;
		row = mysql_fetch_row(result);

		int count = 0;
		while (NULL != row) {
			friends[count].update(atoi(row[0]), row[1], row[2]);
			row = mysql_fetch_row(result);
			count++;
		}
		do {
			MYSQL_RES* pRes = mysql_store_result(&mydata);
			if (pRes != NULL)
				mysql_free_result(pRes);
		} while (!mysql_next_result(&mydata));
		return count;
	}
	else {
		cout << "���Ѷ� select data failed" << endl;
		mysql_close(&mydata);
		return -2;
	}
}

int DbUtil::LogInCheck(string name, string password) {	
	/*	���ã������û������֤	�������û���������	����ֵ�����ɹ��������û�id����û�и��û�������-1	*/	
	//��ѯ���ݿ��е��û���ݣ� ���ɹ��������û�id����û�и��û�������-1	
	sqlstr = "SELECT *FROM `im`.`user2`;";	

	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "=====�û�������֤�ɹ�=====" << endl;
		//һ����ȡ�����ݼ�		
		result = mysql_store_result(&mydata);
		MYSQL_ROW row = NULL;
		row = mysql_fetch_row(result);
		while (NULL != row) {
			if (name == row[2] && password == row[3]) {
				cout << name << "��¼�ɹ���" << endl;
				return atoi(row[0]);
			}
			row = mysql_fetch_row(result);
		}		//cout << "��¼ʧ�ܣ�" << endl;`		
		return -1;
	}
	else {
		cout << "�û���֤ select data failed" << endl;
		mysql_close(&mydata);
		return -2;
	}
}

int DbUtil::InsertUser(string ip, string name, string password) {
	/*
	���ã������û��������ݿ�
	�������û�ip���û���������
	����ֵ���ɹ�Ϊ0��ʧ��Ϊ-1
	*/

	sqlstr = "SELECT *FROM `im`.`user2` where name = '"+name+"';";

	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "=====�û�������֤�ɹ�=====" << endl;

		//һ����ȡ�����ݼ�		
		result = mysql_store_result(&mydata);
		MYSQL_ROW row = NULL;
		row = mysql_fetch_row(result);

		if (NULL != row) {
			cout <<  "�Ѵ��ڣ�" << endl;
			return -4;
			row = mysql_fetch_row(result);
		}

		else
		{
			//����user
			sqlstr = "INSERT INTO `im`.`user2` (`ID`, `ipAddress`, `name`, `password`) ";
			sqlstr += "VALUES (default,inet_aton('";
			sqlstr += ip;
			sqlstr += "'), '" + name + "', '" + password + "');";

			if (0 == mysql_query(&mydata, sqlstr.c_str())) {
				cout << "=====�������û��ɹ�=====" << endl;
				do {
					MYSQL_RES* pRes = mysql_store_result(&mydata);
					if (pRes != NULL)
						mysql_free_result(pRes);
				} while (!mysql_next_result(&mydata));
				return 1;
			}
			else {
				cout << "mysql_query() insert data failed" << endl;
				cout << mysql_error(&mydata) << endl;
				mysql_close(&mydata);
			}
			return 0;
		}
		return -1;
	}
	else {
		cout << "�û���֤ select data failed" << endl;
		mysql_close(&mydata);
		return -2;
	}

}

int DbUtil::InsertFriends(string user1name, string user2name) {
	/*
	���ã����º��ѶԼ������ݿ�
	�������û�1�����û�2��
	����ֵ���ɹ�Ϊ0��ʧ��Ϊ-1
	*/
	sqlstr = "INSERT INTO `im`.`friends` (`user1name`, `user2name`)";
	sqlstr += "VALUES ('" + user1name;
	sqlstr += "','" + user2name + "');";
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "=====�����º��ѶԳɹ�====" << endl;
		do {
			MYSQL_RES* pRes = mysql_store_result(&mydata);
			if (pRes != NULL)
				mysql_free_result(pRes);
		} while (!mysql_next_result(&mydata));
		return 0;
	}
	else {
		cout << "=====�����º��Ѷ�ʧ��====" << endl;
		mysql_close(&mydata);
		return -1;
	}
}



//��ȡ�����,���䱣�浽��άstring������
//vector<vector<string> > DbUtil::GetResult()
//{
//	vector<vector<string> >result;
//	MYSQL_RES* res = mysql_store_result(mysql);
//	if (res == NULL)
//		return result;
//	unsigned num = mysql_num_fields(res); //�����������
//	for (MYSQL_ROW row; (row = mysql_fetch_row(res)) != NULL;) {
//		result.push_back(vector<string>(num));
//		for (size_t cur = result.size() - 1, i = 0; i < num; ++i) {
//			result[cur][i] = row[i];
//		}
//	}
//	mysql_free_result(res); //�ͷŽ������Դ
//	return result;
//}

//��ȡ��ǰ���ں�ʱ��,���ڸ�����־
string DbUtil::GetTime()
{
	char time_buf[64];
	time_t now_time = time(NULL);
	strftime(time_buf, 64, "%Y-%m-%d %H:%M:%S ", localtime(&now_time));
	return time_buf;
}

int DbUtil::test(string user1name) {

	sqlstr = "SELECT * FROM `im`.`friends`WHERE user1name ='" + user1name + "' and user2name = 'hst';";

	return 0;

}

int DbUtil::InsertFriend(string user1name, string user2name) {
	/*
	���ã����º��ѶԼ������ݿ�
	�������û�1�����û�2��
	����ֵ���ɹ�Ϊ0��ʧ��Ϊ-1
	*/
	string sqlstr_c = "SELECT * FROM `im`.`user2` WHERE user2name = '" + user2name + "';";
	string sqlstr = "SELECT * FROM `im`.`friends`WHERE user1name ='" + user1name + "' and user2name = '" + user2name + "';";
//	string sqlstr_cc = "SELECT * FROM `im`.`friends`WHERE user2name ='" + user2name + "' and user1name = '" + user1name + "';";

	//cout << sqlstr;
	

	if (mysql_query(&mydata, sqlstr.c_str()) == 0) {

		cout << "=====�û�������֤�ɹ�=====" << endl;

		//һ����ȡ�����ݼ�  
		result = mysql_store_result(&mydata);
		MYSQL_ROW row = NULL;
		row = mysql_fetch_row(result);

		if (NULL == row) {
			mysql_free_result(result);
			sqlstr = "INSERT INTO `im`.`friends` (`user1name`, `user2name`)";
			sqlstr += "VALUES ('" + user1name;
			sqlstr += "','" + user2name + "');";
			if (0 == mysql_query(&mydata, sqlstr.c_str())) {
				cout << "=====�����º��ѶԳɹ�====" << endl;
				do {
					MYSQL_RES* pRes = mysql_store_result(&mydata);
					if (pRes != NULL)
						mysql_free_result(pRes);
				} while (!mysql_next_result(&mydata));
				return 0;
			}
			else {
				cout << "=====�����º��Ѷ�ʧ��====" << endl;
				mysql_close(&mydata);
				return -1;
			}
		}
		else
		{
			cout << "=====�����Ѿ��Ǻ�����=====";
			return -3;
		}

	}
	else if(mysql_query(&mydata, sqlstr_c.c_str()) != 0){
		cout << "�û�" << user2name <<"������."<< endl;
		cout<< mysql_error(&mydata);
		mysql_close(&mydata);
		return -4;
	}
	else{
		cout << "�û���֤ʧ��" << endl;
		cout << mysql_error(&mydata);
		mysql_close(&mydata);
		return -2;
	}
}
