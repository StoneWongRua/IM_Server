//#include <string>
//#include "DbUtil.h"
//using namespace std;
//
//string DbConnector::host_ = "localhost";
//string DbConnector::user_ = "root";
//string DbConnector::passwd_ = "root";
//string DbConnector::db_ = "im";
//
//void main()
//{
//
//	DbOfflineMsg off_msg("offline_msg", "offileMsg.log");
//	//off_msg.push("to", "from", "hey");   //存入一条离线消息
//	auto result = off_msg.pop("to"); //读取发给该用户的全部离线消息
//
//	vector<string> temp_vect;
//
//	for (vector<vector<string>>::iterator ite = result.begin(); ite != result.end(); ite++) {
//		temp_vect = *ite;
//		for (vector<string>::iterator itee = temp_vect.begin(); itee != temp_vect.end(); itee++)
//			std::cout << *itee;
//	}
//	system("pause");
//
//	//DbChatLogMsg clMsg("Bob", "chat_log_Bob.log");
//	//clMsg.push("Miranda-lym", "hello", 0);
//	//clMsg.push("Miranda-lym", "hi!", 1);
//	//clMsg.push("Miranda-lym", "this a test case!", 0);
//	//clMsg.push("Miranda-lym", "soga,I get it!", 1);
//	//clMsg.push("John", "what are you doing!", 1);
//
//	//cin.get();
//
//	//return 0;
//
//	//string str = "insert into test(username,password) values('hst','hst')";
//	//DbConnector* conn = new DbConnector();z
//	//printf("%d", conn->query(str));
//	//system("pause");
//
//}