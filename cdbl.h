/*
  CDBL-（C DataBase Link,C数据库链接层）
  
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
#define COLEN	  50	//最大列长度
#define CONNINFOLEN	  32	//最大连接信息长度
#define PORTLEN	  8	//端口描述长度
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
	CDBL_RESULT_NO_MORE_RESULTS,//表明sql查询为空值（就是没有一条满足条件的结果）
	CDBL_RESULT_EMPTY_QUERY, //发送给服务器的字串是空的
	CDBL_RESULT_COMMAND_OK,//成功完成一个不返回数据的命令
	CDBL_RESULT_TUPLES_OK,//成功执行一个返回数据的查询查询（比如 SELECT 或者 SHOW）。
	CDBL_RESULT_COPY_OUT,//（从服务器）Copy Out （拷贝出）数据传输开始
	CDBL_RESULT_COPY_IN,//Copy In （拷贝入）（到服务器）数据传输开始
	CDBL_RESULT_BAD_RESPONSE,//服务器的响应无法理解
	CDBL_RESULT_NONFATAL_ERROR,//发生了一个非致命错误（通知或者警告）
	CDBL_RESULT_FATAL_ERROR,//发生了一个致命错误	
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
	int rowcount; //行总数
	int currow; //当前行
}s_postgresql;

typedef struct _cdbl_context
{
	CDBLType_t eCDBLType;
	char szdbsysName[COLEN];
	CDBLWhether_t eCDBLResult;
	int istrans;//是否处于事务状态
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

/* 数据库连接结构类型CDBL_CONNECT定义 */
typedef struct cdbl_connect
{
	char szUsername[CONNINFOLEN]; /*用户名称*/
    char szPassword[CONNINFOLEN]; /*用户口令*/
    char szDBName[CONNINFOLEN];   /*数据库名称*/
    char szHost[CONNINFOLEN];     /*数据库主机IP地址*/
	char szPort[PORTLEN];	     /*数据库连接端口*/
	s_connect_postgresql pqconn; /*postgresql数据库连接属性*/
} CDBL_CONNECT;

extern void cdbl_get_verinfo(char *ver);


extern int test(void);
extern int test2(void);
extern int test3(void);

extern CDBLDataType_t cdbl_typeget_pg(unsigned int inType);
extern CDBLDataType_t cdbl_typeget_mssql(int inType);


extern CDBLError_t cdbl_init_col(CDBL_CONTEXT * context); //字段信息初始
extern CDBLError_t cdbl_free_col(CDBL_CONTEXT * context); //字段信息释放
extern CDBL_CONTEXT* cdbl_connect(CDBL_CONNECT * conn,CDBLType_t eCDBLType);

extern CDBLError_t cdbl_cmdexec(CDBL_CONTEXT * context,const char szcmd[]);
/*获取行总数*/
extern int cdbl_get_rowconut(CDBL_CONTEXT * context);
/*获取字段名称*/
extern char * cdbl_get_fieldname(CDBL_CONTEXT * context,int column_number);
/*获取字段大小*/
extern int cdbl_get_fieldsize(CDBL_CONTEXT * context,int column_number);
/*获取数值*/
extern char *cdbl_get_value(CDBL_CONTEXT * context,int row_number,int column_number);
/*获取字段数量*/
extern int cdbl_get_fieldscount(CDBL_CONTEXT * context); //字段数量
/*判断字段是否null值，1是，0否*/
extern int cdbl_get_fieldisnull(CDBL_CONTEXT * context,int row_number,int column_number);
/*获取字段类型*/
extern int cdbl_get_coltype(CDBL_CONTEXT * context, int column);
/*获取字段数据大小*/
extern int cdbl_get_coldatalen(CDBL_CONTEXT * context, int column);
/*获取数据集状态*/
extern CDBLRStatus_t cdbl_get_resultstatus(CDBL_CONTEXT * context);

extern CDBLError_t cdbl_get_colid(CDBL_CONTEXT * context,char *colName);//取字段序号	

extern int cdbl_iseof(CDBL_CONTEXT * context);
extern CDBLError_t cdbl_next(CDBL_CONTEXT * context);
extern CDBLError_t cdbl_first(CDBL_CONTEXT * context);

/**
 * Description :获取数据集当前行字段数值（整形）
 * @param      context 上下文
 * @param      colName 字段名称
 * @param      lresult 返回整形数值
 * @return     返回CDBLError_t 类型的错误代码
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
