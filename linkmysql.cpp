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
		strcpy(dbInfo.passwd, "123456");//��������
	}
	else 
	{
		//��ȡ�����ļ���Ϣ���Ժ�ʵ��
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
		gLog->writeLog(QString::fromLocal8Bit("�������ݿ�ʧ��:mysql_real_connect()"));
		gLog->writeLog(tmpStr);
		return false;
	}
	tmpRet = mysql_query(mysql, "SET NAMES gbk");       //�����ַ���ΪUTF8
	if (tmpRet != 0)
	{
		gLog->writeLog(QString::fromLocal8Bit("���ݿ��ַ����趨ʧ��"));
		return false;
	}
	return true;
}

void CLinkMySql::closeMySql() 
{
	char tmpchar = mysql_stmt_close(mStmt);
	if (tmpchar)
	{
		//��ӡ��־
		gLog->writeLog(QString::fromLocal8Bit("\n���ݿ�ر�ʧ��"));
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
			//��ӡ��־
			fprintf(stderr, " %s\n", mysql_stmt_error(mStmt));
			QString tmpStr = QString::fromLocal8Bit(mysql_stmt_error(mStmt));
			gLog->writeLog(tmpStr);
			return false;
		}
	}
	
	if (mysql_stmt_execute(mStmt))
	{
		//��ӡ��־
		fprintf(stderr, " %s\n", mysql_stmt_error(mStmt));
		QString tmpStr = QString::fromLocal8Bit(mysql_stmt_error(mStmt));
		gLog->writeLog(tmpStr);
		return false;
	}

	if (mColCount > 0)
	{
		if (mysql_stmt_bind_result(mStmt, bindC))
		{
			//��ӡ��־
			fprintf(stderr, " %s\n", mysql_stmt_error(mStmt));
			QString tmpStr = QString::fromLocal8Bit(mysql_stmt_error(mStmt));
			gLog->writeLog(tmpStr);
			return false;
		}
		if (mysql_stmt_store_result(mStmt))
		{
			//��ӡ��־
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
	//�ȳ�ʼ��Ԥ������
	mStmt = mysql_stmt_init(mysql);
	if (!mStmt)
	{
		//��ӡ��־

	}
	if (mysql_stmt_prepare(mStmt, strSQL, strlen(strSQL))) //Ԥ�������� ׼��sql
	{
		//��ӡ������־���Ժ�ʵ��
		fprintf(stderr, " %s\n", mysql_stmt_error(mStmt));
		QString tmpStr = QString::fromLocal8Bit(mysql_stmt_error(mStmt));
		gLog->writeLog(tmpStr);
		return false;
	}

	mParamCount = mysql_stmt_param_count(mStmt);						//sql�󶨲���������
	
	MYSQL_RES* p = mysql_stmt_result_metadata(mStmt);
	if (NULL == p)
	{
		mColCount = 0;
	}
	else
	{
		mColCount = mysql_num_fields(p);
	}
	//mColCount = mysql_num_fields(mysql_stmt_result_metadata(mStmt));	//sql���е�����

	//�󶨲�����̬���鶨��
	if (0 != mParamCount)
	{
		bindP = new MYSQL_BIND[mParamCount];
		memset(bindP, 0, sizeof(bindP));
	}
	//���ж�̬���鶨��
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