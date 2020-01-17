#ifndef LINKMYSQL_H
#define LINKMYSQL_H
#include "include.h"
#include <mysql.h>
#include <qstring.h>

//数据库信息
struct SDataBaseInfo 
{
	char host[NAMELENGTH];
	char user[NAMELENGTH];
	char passwd[NAMELENGTH];
	char db[NAMELENGTH];
	unsigned int port;
	SDataBaseInfo() 
	{
		memset(host, 0, NAMELENGTH);
		memset(user, 0, NAMELENGTH);
		memset(passwd, 0, NAMELENGTH);
		memset(db, 0, NAMELENGTH);
		port = 0;
	}
};
//数据库操作类
/*进行查询操作遵循基本类型变量在前，字符等其他变量在后的顺序*/
class CLinkMySql 
{
public:
	CLinkMySql(bool isDefault = false);
	~CLinkMySql();

	bool connectMysql();	//连接
	void closeMySql();							//关闭

	bool exec();								//执行
	int fetch();								//获取结果
	

	bool PrepareSQL(const char* strSQ);							//准备SQL语句
	void bindPar(int colType, unsigned long colSize, void* colAddr, int index);			//绑定参数
	void bindCol(int colType, unsigned long colSize, void* colAddr, int index);			//绑定列
	bool Commit();												//递交事务
private:

	void init();			//内部数据初始化

	SDataBaseInfo dbInfo;	//数据库配置文件信息

	MYSQL* mysql;
	MYSQL_STMT* mStmt;			//预处理句柄
	int mParamCount;			//绑定参数的个数
	int mColCount;				//绑定的列的个数
	MYSQL_BIND* bindP;			//动态数组绑定参数
	MYSQL_BIND* bindC;			//动态数组绑定列
	unsigned long* length;		//动态数组绑定列数据的长度的地址
	my_bool*       is_null;		//动态数组绑定列数据返回结果集是否为null
	my_bool*       error;		//动态数组绑定列数据错误信息
};
#endif // LINKMYSQL_H
