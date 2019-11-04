#include "FriendDBUtil.h"




DbUtil::DbUtil()
{
}

DbUtil::~DbUtil()
{
}

int DbUtil::DatabaseInit() {
	/*
	作用：初始化数据库
	参数：无
	返回值：0成功，-1失败
	*/
	//初始化数据库
	mysql_library_init(0, NULL, NULL);
	//初始化数据结构
	if (NULL != mysql_init(&mydata)) {
		cout << "======数据库初始化成功====" << endl;
	}
	else {
		cout << "mysql_init() failed" << endl;
		return -1;
	}

	//在连接数据库之前，设置额外的连接选项
	//可以设置的选项很多，这里设置字符集，否则无法处理中文
	if (0 == mysql_options(&mydata, MYSQL_SET_CHARSET_NAME, "gbk")) {
		cout << "======数据库设置成功======" << endl;
	}
	else {
		cout << "mysql_options() failed" << endl;
		return -1;
	}

	//连接数据库
	if (NULL != mysql_real_connect(&mydata, NULL, "root", "root", "im", 3306, NULL, CLIENT_MULTI_STATEMENTS))
		//这里的地址，用户名，密码，端口可以根据自己本地的情况更改
	{
		cout << "======数据库连接成功======" << endl;
	}
	else {
		cout << "mysql_real_connect() failed" << endl;
		cout << mysql_error(&mydata) << endl;
		return -1;
	}
	return 0;
	//sql字符串
}


int DbUtil::CreateUser() {
	/*
	作用：创建用户表
	参数：无
	返回值：0成功，-1失败
	*/
	sqlstr = "CREATE TABLE IF NOT EXISTS `user2` (";
	sqlstr += " `ID` int NOT NULL AUTO_INCREMENT,";
	sqlstr += " `ipAddress` varchar(100) NOT NULL,";
	sqlstr += " `name` varchar(30) NOT NULL, ";
	sqlstr += " `password` varchar(100) NOT NULL,";
	sqlstr += " PRIMARY KEY(`ID`)";
	sqlstr += " ) ENGINE = InnoDB DEFAULT CHARSET = utf8";

	if (mysql_query(&mydata, sqlstr.c_str()) == 0) {
		cout << "======用户表创建成功======" << endl;
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
	作用：创建用户表
	参数：无
	返回值：0成功，-1失败
	*/
	sqlstr = "CREATE TABLE IF NOT EXISTS `friends` (";
	sqlstr += " `ID` int NOT NULL AUTO_INCREMENT,";
	sqlstr += " `user1name` varchar(50) NOT NULL,";
	sqlstr += " `user2name` varchar(50) NOT NULL,";
	sqlstr += " PRIMARY KEY(`ID`)";
	sqlstr += " ) ENGINE = InnoDB DEFAULT CHARSET = utf8";

	if (mysql_query(&mydata, sqlstr.c_str()) == 0) {
		cout << "======好友对表创建成功======" << endl;
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
	作用：将数据库中的用户表导入到本地
	参数：用户类数组
	返回值：导入的用户的数量
	*/
	//将数据库中的用户表导入到本地，返回值为用户数量
	sqlstr = "SELECT *FROM `im`.`user2`;";
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "=====用户表收集成功=====" << endl;
		//一次性取得数据集
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
		cout << "用户验证 select data failed" << endl;
		mysql_close(&mydata);
		return -2;
	}
}

int DbUtil::GetFriends(Friends* friends) {/*
	作用：将数据库中的好友对表导入到本地
	参数：好友对类数组
	返回值：导入的好友对的数量
	*/

	sqlstr = "SELECT * FROM `im`.`friends`;";

	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "======好友对表收集成功======" << endl;
		//一次性取得数据集
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
		cout << "好友对 select data failed" << endl;
		mysql_close(&mydata);
		return -2;
	}
}

int DbUtil::LogInCheck(string name, string password) {	
	/*	作用：进行用户身份验证	参数：用户名与密码	返回值：若成功，返回用户id，若没有该用户，返回-1	*/	
	//查询数据库中的用户身份， 若成功，返回用户id，若没有该用户，返回-1	
	sqlstr = "SELECT *FROM `im`.`user2`;";	

	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "=====用户数据验证成功=====" << endl;
		//一次性取得数据集		
		result = mysql_store_result(&mydata);
		MYSQL_ROW row = NULL;
		row = mysql_fetch_row(result);
		while (NULL != row) {
			if (name == row[2] && password == row[3]) {
				cout << name << "登录成功！" << endl;
				return atoi(row[0]);
			}
			row = mysql_fetch_row(result);
		}		//cout << "登录失败！" << endl;`		
		return -1;
	}
	else {
		cout << "用户验证 select data failed" << endl;
		mysql_close(&mydata);
		return -2;
	}
}

int DbUtil::InsertUser(string ip, string name, string password) {
	/*
	作用：将新用户加入数据库
	参数：用户ip，用户名，密码
	返回值：成功为0，失败为-1
	*/

	sqlstr = "SELECT *FROM `im`.`user2` where name = '"+name+"';";

	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "=====用户数据验证成功=====" << endl;

		//一次性取得数据集		
		result = mysql_store_result(&mydata);
		MYSQL_ROW row = NULL;
		row = mysql_fetch_row(result);

		if (NULL != row) {
			cout <<  "已存在！" << endl;
			return -4;
			row = mysql_fetch_row(result);
		}

		else
		{
			//插入user
			sqlstr = "INSERT INTO `im`.`user2` (`ID`, `ipAddress`, `name`, `password`) ";
			sqlstr += "VALUES (default,inet_aton('";
			sqlstr += ip;
			sqlstr += "'), '" + name + "', '" + password + "');";

			if (0 == mysql_query(&mydata, sqlstr.c_str())) {
				cout << "=====创建新用户成功=====" << endl;
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
		cout << "用户验证 select data failed" << endl;
		mysql_close(&mydata);
		return -2;
	}

}

int DbUtil::InsertFriends(string user1name, string user2name) {
	/*
	作用：将新好友对加入数据库
	参数：用户1名，用户2名
	返回值：成功为0，失败为-1
	*/
	sqlstr = "INSERT INTO `im`.`friends` (`user1name`, `user2name`)";
	sqlstr += "VALUES ('" + user1name;
	sqlstr += "','" + user2name + "');";
	if (0 == mysql_query(&mydata, sqlstr.c_str())) {
		cout << "=====创建新好友对成功====" << endl;
		do {
			MYSQL_RES* pRes = mysql_store_result(&mydata);
			if (pRes != NULL)
				mysql_free_result(pRes);
		} while (!mysql_next_result(&mydata));
		return 0;
	}
	else {
		cout << "=====创建新好友对失败====" << endl;
		mysql_close(&mydata);
		return -1;
	}
}



//获取结果集,将其保存到二维string数组中
//vector<vector<string> > DbUtil::GetResult()
//{
//	vector<vector<string> >result;
//	MYSQL_RES* res = mysql_store_result(mysql);
//	if (res == NULL)
//		return result;
//	unsigned num = mysql_num_fields(res); //结果集的列数
//	for (MYSQL_ROW row; (row = mysql_fetch_row(res)) != NULL;) {
//		result.push_back(vector<string>(num));
//		for (size_t cur = result.size() - 1, i = 0; i < num; ++i) {
//			result[cur][i] = row[i];
//		}
//	}
//	mysql_free_result(res); //释放结果集资源
//	return result;
//}

//获取当前日期和时间,用于更新日志
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
	作用：将新好友对加入数据库
	参数：用户1名，用户2名
	返回值：成功为0，失败为-1
	*/
	string sqlstr_c = "SELECT * FROM `im`.`user2` WHERE user2name = '" + user2name + "';";
	string sqlstr = "SELECT * FROM `im`.`friends`WHERE user1name ='" + user1name + "' and user2name = '" + user2name + "';";
//	string sqlstr_cc = "SELECT * FROM `im`.`friends`WHERE user2name ='" + user2name + "' and user1name = '" + user1name + "';";

	//cout << sqlstr;
	

	if (mysql_query(&mydata, sqlstr.c_str()) == 0) {

		cout << "=====用户数据验证成功=====" << endl;

		//一次性取得数据集  
		result = mysql_store_result(&mydata);
		MYSQL_ROW row = NULL;
		row = mysql_fetch_row(result);

		if (NULL == row) {
			mysql_free_result(result);
			sqlstr = "INSERT INTO `im`.`friends` (`user1name`, `user2name`)";
			sqlstr += "VALUES ('" + user1name;
			sqlstr += "','" + user2name + "');";
			if (0 == mysql_query(&mydata, sqlstr.c_str())) {
				cout << "=====创建新好友对成功====" << endl;
				do {
					MYSQL_RES* pRes = mysql_store_result(&mydata);
					if (pRes != NULL)
						mysql_free_result(pRes);
				} while (!mysql_next_result(&mydata));
				return 0;
			}
			else {
				cout << "=====创建新好友对失败====" << endl;
				mysql_close(&mydata);
				return -1;
			}
		}
		else
		{
			cout << "=====你们已经是好友了=====";
			return -3;
		}

	}
	else if(mysql_query(&mydata, sqlstr_c.c_str()) != 0){
		cout << "用户" << user2name <<"不存在."<< endl;
		cout<< mysql_error(&mydata);
		mysql_close(&mydata);
		return -4;
	}
	else{
		cout << "用户验证失败" << endl;
		cout << mysql_error(&mydata);
		mysql_close(&mydata);
		return -2;
	}
}
