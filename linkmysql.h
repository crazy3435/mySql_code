#ifndef LINKMYSQL_H
#define LINKMYSQL_H
#include "include.h"
#include <mysql.h>
#include <qstring.h>

//���ݿ���Ϣ
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
//���ݿ������
/*���в�ѯ������ѭ�������ͱ�����ǰ���ַ������������ں��˳��*/
class CLinkMySql 
{
public:
	CLinkMySql(bool isDefault = false);
	~CLinkMySql();

	bool connectMysql();	//����
	void closeMySql();							//�ر�

	bool exec();								//ִ��
	int fetch();								//��ȡ���
	

	bool PrepareSQL(const char* strSQ);							//׼��SQL���
	void bindPar(int colType, unsigned long colSize, void* colAddr, int index);			//�󶨲���
	void bindCol(int colType, unsigned long colSize, void* colAddr, int index);			//����
	bool Commit();												//�ݽ�����
private:

	void init();			//�ڲ����ݳ�ʼ��

	SDataBaseInfo dbInfo;	//���ݿ������ļ���Ϣ

	MYSQL* mysql;
	MYSQL_STMT* mStmt;			//Ԥ������
	int mParamCount;			//�󶨲����ĸ���
	int mColCount;				//�󶨵��еĸ���
	MYSQL_BIND* bindP;			//��̬����󶨲���
	MYSQL_BIND* bindC;			//��̬�������
	unsigned long* length;		//��̬����������ݵĳ��ȵĵ�ַ
	my_bool*       is_null;		//��̬����������ݷ��ؽ�����Ƿ�Ϊnull
	my_bool*       error;		//��̬����������ݴ�����Ϣ
};
#endif // LINKMYSQL_H
