#include "linkmysql.h"
#include "global.h"

CLinkMySql::CLinkMySql(bool isDefault)
{
	if (isDefault)
	{
		dbInfo.port = 3306;
		strcpy(dbInfo.host, "192.168.0.198");
		strcpy(dbInfo.user, "root");
		strcpy(dbInfo.db,"lthx");
		strcpy(dbInfo.passwd, "123456");//本机密码
	}
	else 
	{
		//读取配置文件信息，稍后实现
	}
	
}

CLinkMySql::~CLinkMySql() {}

void CLinkMySql::init() 
{
	mParamCount = 0;
	mColCount = 0;
	mysql = mysql_init(NULL);
}
bool CLinkMySql::connectMysql() 
{
	init();
	MYSQL* tmpConnect;
	int    tmpRet = 0;
	tmpConnect = mysql_real_connect(mysql, dbInfo.host, dbInfo.user, dbInfo.passwd, dbInfo.db, dbInfo.port, NULL, 0);
	if (tmpConnect == NULL)
	{
		QString tmpStr = QString::fromLocal8Bit(mysql_error(mysql));
		gLog->writeLog(QString::fromLocal8Bit("连接数据库失败:mysql_real_connect()"));
		gLog->writeLog(tmpStr);
		return false;
	}
	tmpRet = mysql_query(mysql, "SET NAMES gbk");       //设置字符集为UTF8
	if (tmpRet != 0)
	{
		gLog->writeLog(QString::fromLocal8Bit("数据库字符集设定失败"));
		return false;
	}
	return true;
}

void CLinkMySql::closeMySql() 
{
	char tmpchar = mysql_stmt_close(mStmt);
	if (tmpchar)
	{
		//打印日志
		gLog->writeLog(QString::fromLocal8Bit("\n数据库关闭失败"));
	}
	mysql_close(mysql);
}

bool CLinkMySql::exec() 
{
	if (mParamCount>0)
	{
		char c = mysql_stmt_bind_param(mStmt, bindP);
		if (c)
		{
			//打印日志
			fprintf(stderr, " %s\n", mysql_stmt_error(mStmt));
			QString tmpStr = QString::fromLocal8Bit(mysql_stmt_error(mStmt));
			gLog->writeLog(tmpStr);
			return false;
		}
	}
	
	if (mysql_stmt_execute(mStmt))
	{
		//打印日志
		fprintf(stderr, " %s\n", mysql_stmt_error(mStmt));
		QString tmpStr = QString::fromLocal8Bit(mysql_stmt_error(mStmt));
		gLog->writeLog(tmpStr);
		return false;
	}

	if (mColCount > 0)
	{
		if (mysql_stmt_bind_result(mStmt, bindC))
		{
			//打印日志
			fprintf(stderr, " %s\n", mysql_stmt_error(mStmt));
			QString tmpStr = QString::fromLocal8Bit(mysql_stmt_error(mStmt));
			gLog->writeLog(tmpStr);
			return false;
		}
		if (mysql_stmt_store_result(mStmt))
		{
			//打印日志
			fprintf(stderr, " %s\n", mysql_stmt_error(mStmt));
			QString tmpStr = QString::fromLocal8Bit(mysql_stmt_error(mStmt));
			gLog->writeLog(tmpStr);
			return false;
		}
	}
	return true;
}

int CLinkMySql::fetch() 
{
	int tmpINT = mysql_stmt_fetch(mStmt);
	return tmpINT;
}

bool CLinkMySql::PrepareSQL(const char* strSQL) 
{
	//先初始化预处理环境
	mStmt = mysql_stmt_init(mysql);
	if (!mStmt)
	{
		//打印日志

	}
	if (mysql_stmt_prepare(mStmt, strSQL, strlen(strSQL))) //预处理环境中 准备sql
	{
		//打印错误日志，稍后实现
		fprintf(stderr, " %s\n", mysql_stmt_error(mStmt));
		QString tmpStr = QString::fromLocal8Bit(mysql_stmt_error(mStmt));
		gLog->writeLog(tmpStr);
		return false;
	}

	mParamCount = mysql_stmt_param_count(mStmt);						//sql绑定参数的数量
	
	MYSQL_RES* p = mysql_stmt_result_metadata(mStmt);
	if (NULL == p)
	{
		mColCount = 0;
	}
	else
	{
		mColCount = mysql_num_fields(p);
	}
	//mColCount = mysql_num_fields(mysql_stmt_result_metadata(mStmt));	//sql绑定列的数量

	//绑定参数动态数组定义
	if (0 != mParamCount)
	{
		bindP = new MYSQL_BIND[mParamCount];
		memset(bindP, 0, sizeof(bindP));
	}
	//绑定列动态数组定义
	if (0 != mColCount)
	{
		bindC = new MYSQL_BIND[mColCount];
		memset(bindC, 0, sizeof(bindC));
		length = new unsigned long[mColCount];
		memset(length, 0, mColCount);
		is_null = new my_bool[mColCount];
		memset(is_null, 0, mColCount);
		error = new my_bool[mColCount];
		memset(error, 0, mColCount);
	}
	return true;
}

void CLinkMySql::bindCol(int colType, unsigned long colSize, void* colAddr, int index) 
{
	bindC[index].buffer_type = (enum_field_types)colType;
	bindC[index].buffer = (char*)colAddr;
	if (0 != colSize)
	{
		bindC[index].buffer_length = colSize;
	}
	bindC[index].length = &length[index];
	bindC[index].is_null = &is_null[index];
	bindC[index].error = &error[index];
}

void CLinkMySql::bindPar(int colType,unsigned long colSize, void* colAddr,int index) 
{
	bindP[index].buffer_type = (enum_field_types)colType;
	bindP[index].buffer = (char*)colAddr;
	if (0 != colSize)
	{
		bindP[index].buffer_length = colSize;
		bindP[index].length = &colSize;
	}
	else 
	{
		bindP[index].length = 0;
	}
	bindP[index].is_null = NULL;
}

bool CLinkMySql::Commit() 
{
	return true;
}