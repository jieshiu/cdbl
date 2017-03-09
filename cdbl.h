/*
  CDBL-��C DataBase Link,C���ݿ����Ӳ㣩
  
  Copyright (c) 2013 Xjj
  
  
*/

#ifndef cdbl__h
#define cdbl__h

#include "leak_detector_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

# ifdef _DEBUG
#define  cdbl_malloc(size) 	    	   	xmalloc (size, __FILE__, __LINE__)
#define  cdbl_calloc(elements, size)  	xcalloc (elements, size, __FILE__, __LINE__)
#define  cdbl_free(mem_ref) 		  	xfree(mem_ref)
#define  cdbl_realloc(mem_address,newsize) 		  	xrealloc(mem_address,newsize,__FILE__, __LINE__)
#else
#define  cdbl_malloc(size) 	    	   	malloc (size)
#define  cdbl_calloc(elements, size)  	calloc (elements, size)
#define  cdbl_free(mem_ref) 		  	free(mem_ref)
#define  cdbl_realloc(mem_address,newsize) 		  	realloc(mem_address,newsize)
#endif

#define STRSQLMAX 5000	//SQL max len
#define TABLECOLS 50	//max table columns
#define COLEN	  50	//����г���
#define CONNINFOLEN	  32	//���������Ϣ����
#define PORTLEN	  8	//�˿���������
#define CDBL_VER	"V1.0.1(Bata)"

typedef struct colinfo{
		char name[COLEN];
		unsigned int index;
		unsigned int type;
		long len;
		struct colinfo *next;
} COLINFO;
	

typedef
enum eCDBLType
{
	//! DBMS client is not specified
	CDBL_Client_NotSpecified,
	//! ODBC
	CDBL_ODBC_Client,
	//! Oracle
	CDBL_Oracle_Client,
	//! Microsoft SQL Server
	CDBL_SQLServer_Client,
	//! InterBase or Firebird
	CDBL_InterBase_Client,
	//! SQLBase
	CDBL_SQLBase_Client,
	//! IBM DB2
	CDBL_DB2_Client,
	//! Informix
	CDBL_Informix_Client,
	//! Sybase ASE 
	CDBL_Sybase_Client,
	//! MySQL
	CDBL_MySQL_Client,
	//! PostgreSQL
	CDBL_PostgreSQL_Client,
	//! SQLite
	CDBL_SQLite_Client	
} CDBLType_t;

typedef
enum eCDBLError
{
	//! no error occurred
	CDBL_No_Error,
	//! user-generated error
	CDBL_UserGenerated_Error,
	//! the Library-generated error
	CDBL_Library_Error,
	//! DBMS API error occured
	CDBL_DBMS_API_Error,
	//! 
	CDBL_PARM_Error,
	CDBL_NO_FIND_COL_Error,
	CDBL_RESULTS_NOCOLS_Error,
	CDBL_NO_INIT_BUFF_Error,
	CDBL_NO_MORE_RESULTS_Error,
	CDBL_NO_MORE_ROWS_Error,
	CDBL_OTHER_Error,
	CDBL_COLTYPE_NOT_MATCHING_Error,
	CDBL_NO_VALUE_Error
} CDBLError_t;

typedef
enum eCDBLWhether
{
	CDBL_NO,
	CDBL_YES
} CDBLWhether_t;

typedef
enum eCDBLRStatus
{
	CDBL_RESULT_NO_MORE_RESULTS,//����sql��ѯΪ��ֵ������û��һ�����������Ľ����
	CDBL_RESULT_EMPTY_QUERY, //���͸����������ִ��ǿյ�
	CDBL_RESULT_COMMAND_OK,//�ɹ����һ�����������ݵ�����
	CDBL_RESULT_TUPLES_OK,//�ɹ�ִ��һ���������ݵĲ�ѯ��ѯ������ SELECT ���� SHOW����
	CDBL_RESULT_COPY_OUT,//���ӷ�������Copy Out �������������ݴ��俪ʼ
	CDBL_RESULT_COPY_IN,//Copy In �������룩���������������ݴ��俪ʼ
	CDBL_RESULT_BAD_RESPONSE,//����������Ӧ�޷����
	CDBL_RESULT_NONFATAL_ERROR,//������һ������������֪ͨ���߾��棩
	CDBL_RESULT_FATAL_ERROR,//������һ����������	
} CDBLRStatus_t;

typedef
enum eCDBLDataType
{
	CDBL_TYPE_BOOLEAN,
	CDBL_TYPE_STRING,
	CDBL_TYPE_BITSTRING,
	CDBL_TYPE_INTEGER,
	CDBL_TYPE_NUMERIC,
	CDBL_TYPE_DATETIME,
	CDBL_TYPE_TIMESPAN,
	CDBL_TYPE_GEOMETRIC,
	CDBL_TYPE_NETWORK,
	CDBL_TYPE_UNKNOWN,
	CDBL_TYPE_GENERIC,
	CDBL_TYPE_USER,
} CDBLDataType_t;

typedef struct ssqlserver
{
	void* dblogin;  //LOGINREC *
	void* dbproc; //DBPROCESS *
}s_sqlserver;

typedef struct spostgresql
{
	void* pgconn;//PGconn *
	void* pgresult;	//PGresult *
	int rowcount; //������
	int currow; //��ǰ��
}s_postgresql;

typedef struct _cdbl_context
{
	CDBLType_t eCDBLType;
	char szdbsysName[COLEN];
	CDBLWhether_t eCDBLResult;
	int istrans;//�Ƿ�������״̬
	int EOFTAG;
	COLINFO *COLUMN;
	COLINFO *COL;
	s_sqlserver sqlsvrsession;
    s_postgresql pqsession;
} CDBL_CONTEXT;

typedef struct sconnect_postgresql
{	
	char *pgoptions;
	char *pgtty;   
}s_connect_postgresql;

/* ���ݿ����ӽṹ����CDBL_CONNECT���� */
typedef struct cdbl_connect
{
	char szUsername[CONNINFOLEN]; /*�û�����*/
    char szPassword[CONNINFOLEN]; /*�û�����*/
    char szDBName[CONNINFOLEN];   /*���ݿ�����*/
    char szHost[CONNINFOLEN];     /*���ݿ�����IP��ַ*/
	char szPort[PORTLEN];	     /*���ݿ����Ӷ˿�*/
	s_connect_postgresql pqconn; /*postgresql���ݿ���������*/
} CDBL_CONNECT;

extern void cdbl_get_verinfo(char *ver);


extern int test(void);
extern int test2(void);
extern int test3(void);

extern CDBLDataType_t cdbl_typeget_pg(unsigned int inType);
extern CDBLDataType_t cdbl_typeget_mssql(int inType);


extern CDBLError_t cdbl_init_col(CDBL_CONTEXT * context); //�ֶ���Ϣ��ʼ
extern CDBLError_t cdbl_free_col(CDBL_CONTEXT * context); //�ֶ���Ϣ�ͷ�
extern CDBL_CONTEXT* cdbl_connect(CDBL_CONNECT * conn,CDBLType_t eCDBLType);

extern CDBLError_t cdbl_cmdexec(CDBL_CONTEXT * context,const char szcmd[]);
/*��ȡ������*/
extern int cdbl_get_rowconut(CDBL_CONTEXT * context);
/*��ȡ�ֶ�����*/
extern char * cdbl_get_fieldname(CDBL_CONTEXT * context,int column_number);
/*��ȡ�ֶδ�С*/
extern int cdbl_get_fieldsize(CDBL_CONTEXT * context,int column_number);
/*��ȡ��ֵ*/
extern char *cdbl_get_value(CDBL_CONTEXT * context,int row_number,int column_number);
/*��ȡ�ֶ�����*/
extern int cdbl_get_fieldscount(CDBL_CONTEXT * context); //�ֶ�����
/*�ж��ֶ��Ƿ�nullֵ��1�ǣ�0��*/
extern int cdbl_get_fieldisnull(CDBL_CONTEXT * context,int row_number,int column_number);
/*��ȡ�ֶ�����*/
extern int cdbl_get_coltype(CDBL_CONTEXT * context, int column);
/*��ȡ�ֶ����ݴ�С*/
extern int cdbl_get_coldatalen(CDBL_CONTEXT * context, int column);
/*��ȡ���ݼ�״̬*/
extern CDBLRStatus_t cdbl_get_resultstatus(CDBL_CONTEXT * context);

extern CDBLError_t cdbl_get_colid(CDBL_CONTEXT * context,char *colName);//ȡ�ֶ����	

extern int cdbl_iseof(CDBL_CONTEXT * context);
extern CDBLError_t cdbl_next(CDBL_CONTEXT * context);
extern CDBLError_t cdbl_first(CDBL_CONTEXT * context);

/**
 * Description :��ȡ���ݼ���ǰ���ֶ���ֵ�����Σ�
 * @param      context ������
 * @param      colName �ֶ�����
 * @param      lresult ����������ֵ
 * @return     ����CDBLError_t ���͵Ĵ������
 * @exception 
 */
extern CDBLError_t cdbl_get_itemvalue_l(CDBL_CONTEXT * context,char *colName,long *lresult);//int ,short,long
extern CDBLError_t cdbl_get_itemvalue_d(CDBL_CONTEXT * context,char *colName,double *pdresult);//float,double,real,money,smallmoney
extern CDBLError_t cdbl_get_itemvalue_s(CDBL_CONTEXT * context,char *colName,char *cpresult,int buflen); //char,text,varchar
extern CDBLError_t cdbl_get_itemdate(CDBL_CONTEXT * context,char *colName,char *strdate,int buflen);//datetime
extern CDBLError_t cdbl_get_itemdate_extra(CDBL_CONTEXT * context,char *colName,char *strdate,int buflen);//datetime
extern CDBLError_t cdbl_close(CDBL_CONTEXT * context);

#ifdef __cplusplus
}
#endif

#endif
