/*
  CDBL-（C Data Base Link,C数据库链接层）
  Copyright (c) 2013 Xjj
*/

/* cdbl */
/* cdbl in C. */

#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 


#include "cdbl.h"
#include "c_tdsAPI.h"
#include "c_pgAPI.h"
//#include "../tsbase/logger.h"

#ifdef _WIN32
#ifdef WIN64 //考虑64位编译情况。
#pragma comment(lib,"..\\..\\cdbl\\sqlserver\\win64\\dblib_dll.lib") 
#pragma comment(lib,"..\\..\\cdbl\\postgresql\\win64\\libpq.lib")     
#else
#pragma comment(lib,"..\\..\\cdbl\\sqlserver\\win32\\dblib_dll.lib") 
#pragma comment(lib,"..\\..\\cdbl\\postgresql\\win32\\libpq.lib") 
#endif
#else
#include <ctype.h>
#include "w_to_l_types.h"
static char* strupr(char *str)   
{   
    char *p = str; 
	if(str == NULL)
		return NULL;      
    while(*str){   
        *str = toupper(*str);   
        str++;   
    }   
    return p;   
}
#endif

int msg_handler(DBPROCESS *dbproc, DBINT msgno, int msgstate, int severity,
    char *msgtext, char *srvname, char *procname, int line);
int err_handler(DBPROCESS * dbproc, int severity, int dberr, int oserr,
    char *dberrstr, char *oserrstr);
/**
 * Description :获取cdbl库版本
 * @param      ver 返回版本信息
 * @return     返回void
 * @exception 
 */
void cdbl_get_verinfo(char *ver)
{
#ifndef _WIN32
	       strcpy(ver,CDBL_VER);
#else
	       strcpy_s(ver,strlen(CDBL_VER),CDBL_VER);
#endif	
}

CDBLError_t cdbl_init_col(CDBL_CONTEXT * context)
{
	COLINFO *p,*q;
	CDBLRStatus_t tmp;
#ifdef _WIN32
	char* sztmp=NULL;
	char sztmp2[128];
	size_t len=0;
#endif
	int i=0,cols=0;
	if(context == NULL)
		return CDBL_PARM_Error;
	cols=cdbl_get_fieldscount(context);
	if(cols<1)
		return CDBL_RESULTS_NOCOLS_Error;
	
	p=q=NULL;
	if(context->COLUMN!=NULL) q=context->COLUMN;
	while(q)
	{
		context->COLUMN=context->COLUMN->next;
		p=context->COLUMN;
		cdbl_free(q);
		q=p;
	}
	context->COLUMN=NULL;
	context->COL=NULL;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   
			while(++i<=cols)
			{
				p=(COLINFO*)cdbl_malloc(sizeof(COLINFO));
		#ifndef _WIN32
				   sprintf(p->name,"%s",strupr((char*)cdbl_get_fieldname(context,i)));
		#else
				   sztmp=cdbl_get_fieldname(context,i);
				   //len=strlen(sztmp)+1;
				   len=strlen(sztmp);
				   memset(sztmp2,0x00,128);
				   strcpy(sztmp2,sztmp);	
				   //_strupr_s(sztmp2,len);
				   _strupr_s(sztmp2,128);
				   //sprintf_s(p->name,50,"%s",sztmp);
				   sprintf_s(p->name,50,"%s",sztmp2);
		#endif		
				p->index=i;
				p->type=cdbl_get_coltype(context,i);
				p->len=cdbl_get_coldatalen(context,i);
				p->next=NULL;
				if(context->COLUMN==NULL)
					context->COLUMN=q=p;
				else
				{
					q->next=p;
					q=q->next;
				}
			}
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{	
			while(i < cols)
			{
				p=(COLINFO*)cdbl_malloc(sizeof(COLINFO));
		#ifndef _WIN32
				   sprintf(p->name,"%s",strupr((char*)cdbl_get_fieldname(context,i)));
		#else
				   sztmp=cdbl_get_fieldname(context,i);
				   len=strlen(sztmp)+1;
				   _strupr_s(sztmp,len);
				   sprintf_s(p->name,50,"%s",sztmp);
		#endif		
				p->index=i;
				p->type=cdbl_get_coltype(context,i);
				p->len=cdbl_get_coldatalen(context,i);
				p->next=NULL;
				if(context->COLUMN==NULL)
					context->COLUMN=q=p;
				else
				{
					q->next=p;
					q=q->next;
				}
				i++;
			}
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;
	default:
		{			
		}
	}	
	tmp=cdbl_get_resultstatus(context);	
	if(tmp != CDBL_RESULT_TUPLES_OK)
	{				
		context->eCDBLResult=CDBL_YES;		
		cdbl_next(context);		
	}
	return CDBL_No_Error;
}

CDBLError_t cdbl_free_col(CDBL_CONTEXT * context)
{
	COLINFO *p,*q;
    p=q=NULL;
	if(context->COLUMN != NULL)
		q=context->COLUMN;
	while(q)
	{
		context->COLUMN=context->COLUMN->next;
		p=context->COLUMN;
		cdbl_free(q);
		q=p;
	} 
    context->COLUMN=NULL;
	context->COL=NULL;
	return CDBL_No_Error;
}

CDBL_CONTEXT* cdbl_connect(CDBL_CONNECT * conninfo,CDBLType_t eCDBLType)
{
	CDBL_CONTEXT *context = (CDBL_CONTEXT *)cdbl_malloc(sizeof(CDBL_CONTEXT));
	if(context == NULL)
		return NULL;
	context->sqlsvrsession.dblogin=NULL;
	context->sqlsvrsession.dbproc=NULL;
	context->pqsession.pgresult=NULL;
	context->pqsession.pgconn=NULL;
	context->pqsession.rowcount=0;
	context->pqsession.currow=0;
	context->COLUMN=NULL;
	context->COL=NULL;
	context->EOFTAG=TRUE;
	
	switch(eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{
		   LOGINREC *loginrec = NULL; 
		   DBPROCESS *dbprocess=NULL;		      
		   context->eCDBLType=eCDBLType;
#ifndef _WIN32
	       strcpy(context->szdbsysName,"CDBL_SQLServer");
#else
	       strcpy_s(context->szdbsysName,COLEN,"CDBL_SQLServer");
#endif
		   //初始化db-library 
		   dbinit(); 
		   //
		   loginrec = dblogin(); 		   
		   DBSETLUSER(loginrec, conninfo->szUsername);        
		   DBSETLPWD(loginrec, conninfo->szPassword); 
		   dbprocess = dbopen(loginrec, conninfo->szHost); 		   
		   if(dbprocess == FAIL || dbprocess == NULL)
		   { 
				  //LOG_ERROR("Conect MS SQL SERVER fail\n");	
				  dberrhandle(err_handler);
				  dbmsghandle(msg_handler);
				  cdbl_free(context);
				  return NULL; 
		   }
		   else
		   { 				  
				  context->sqlsvrsession.dblogin=loginrec;
				  context->sqlsvrsession.dbproc=dbprocess;				  
		   } 
		   if(dbuse(dbprocess, conninfo->szDBName) == FAIL)
		   { 
				  //LOG_ERROR("Open database fail!\n"); 
				  dberrhandle(err_handler);
				  dbmsghandle(msg_handler);
				  //操作结束关闭对象
				  dbclose(dbprocess); 
				  context->sqlsvrsession.dblogin=NULL;
				  context->sqlsvrsession.dbproc=NULL;	
				  cdbl_free(context);
				  return NULL;
		   }		   
		   return context;
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{
            PGconn           *conn;     
			int ret=0;
			context->eCDBLType=CDBL_PostgreSQL_Client;
#ifndef _WIN32
	        strcpy(context->szdbsysName,"CDBL_PostgreSQL");
#else
	        strcpy_s(context->szdbsysName,COLEN,"CDBL_PostgreSQL");
#endif           
            conninfo->pqconn.pgoptions   =   NULL;  //special   options   to   start   up   the   backend server 
			conninfo->pqconn.pgtty   =   NULL; //debugging   tty   for   the   backend   server          
			conn   =   PQsetdbLogin(conninfo->szHost,conninfo->szPort,conninfo->pqconn.pgoptions,conninfo->pqconn.pgtty,conninfo->szDBName,conninfo->szUsername,conninfo->szPassword);   
			if   (PQstatus(conn)   ==   CONNECTION_BAD)   
			{
				//LOG_ERROR("Connection to database '%s' failed.\n",conninfo->szDBName);   
				//LOG_ERROR("%s\n",PQerrorMessage(conn));   
				PQfinish(conn);  
				context->pqsession.pgresult=NULL;
				context->pqsession.pgconn=NULL;				  
				cdbl_free(context);
				return NULL;
			}   		 
			context->pqsession.pgconn=conn;
			ret = PQclientEncoding(conn);
			ret = PQsetClientEncoding(conn, "UTF8");
			ret = PQclientEncoding(conn);
			return context;
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;
	default:
		{			
			cdbl_free(context);
		}
	}
	return NULL;
}

CDBLError_t cdbl_cmdexec(CDBL_CONTEXT * context,const char szcmd[])
{
	if(context == NULL)
		return CDBL_PARM_Error;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{		   		   
		   //执行查询
		   int res=0;
		   CDBLRStatus_t tmp;
		   if(context->eCDBLResult == CDBL_YES)
		   {
				dbcancel((DBPROCESS*)context->sqlsvrsession.dbproc);	
				context->eCDBLResult = CDBL_NO;
				context->EOFTAG=TRUE;
				//cdbl_free_col(context);
		   }
		   dbcmd((DBPROCESS*)context->sqlsvrsession.dbproc, szcmd); 
		   res=dbsqlexec((DBPROCESS*)context->sqlsvrsession.dbproc);
		   if( res == FAIL)
		   { 
				//LOG_ERROR("SQL exec error!\n");   
				dberrhandle(err_handler);
				dbmsghandle(msg_handler);
				return CDBL_DBMS_API_Error;
		   }
		   tmp=cdbl_get_resultstatus(context);			  
		   cdbl_init_col(context);
		   return CDBL_No_Error;
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{			
            /* 开始一个事务块 */
			PGresult   *res=NULL;
			CDBLRStatus_t tmp;
			if(context->pqsession.pgresult != NULL)
			{
				if(context->eCDBLResult == CDBL_YES)
				{
					PQclear((PGresult*)context->pqsession.pgresult);
					context->eCDBLResult = CDBL_NO;
					context->EOFTAG=TRUE;
				}
			}
			res = PQexec((PGconn*)context->pqsession.pgconn, szcmd);
			if(res != NULL)
			{
				context->pqsession.pgresult=res;
			}
			else
			{
				//LOG_ERROR("PostgreSQL command failed: %s", PQerrorMessage((PGconn*)context->pqsession.pgconn));
				PQclear((PGresult*)context->pqsession.pgresult);
				context->eCDBLResult=CDBL_NO;
				context->EOFTAG=TRUE;		
				return CDBL_DBMS_API_Error;
			}
			/*
			context->pqsession.rowcount=PQntuples(res);
			if(context->pqsession.rowcount <=0 )
			{
				PQclear((PGresult*)context->pqsession.pgresult);
				context->eCDBLResult=CDBL_NO;
				context->EOFTAG=TRUE;				
				return CDBL_NO_MORE_ROWS_Error;
			}
			context->pqsession.currow=0;*/
			tmp=cdbl_get_resultstatus(context);		

			if(tmp == CDBL_RESULT_COMMAND_OK || tmp == CDBL_RESULT_TUPLES_OK)
			{
				context->pqsession.rowcount=PQntuples(res);
				if(context->pqsession.rowcount <=0 )
				{
					PQclear((PGresult*)context->pqsession.pgresult);
					context->eCDBLResult=CDBL_NO;
					context->EOFTAG=TRUE;				
					return CDBL_NO_MORE_ROWS_Error;
				}
				context->pqsession.currow=0;
				cdbl_init_col(context);
				return CDBL_No_Error;		    
			}
			else
			{
				//LOG_ERROR("PostgreSQL command failed: %s", PQerrorMessage((PGconn*)context->pqsession.pgconn));
				return CDBL_DBMS_API_Error;
			}
			return CDBL_OTHER_Error;
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;
	default:
		{			
		}
	}
	return CDBL_DBMS_API_Error;
}

/*获取行总数*/
int cdbl_get_rowconut(CDBL_CONTEXT * context)
{
	if(context == NULL)
		return 0;
	if(context->eCDBLResult == CDBL_NO)
		return 0;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   
			return DBLASTROW((DBPROCESS*)context->sqlsvrsession.dbproc);
			//return dbcount((DBPROCESS*)context->sqlsvrsession.dbproc);
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{	
			if(context->pqsession.pgresult == NULL)
				return 0;
			return PQntuples((PGresult*)context->pqsession.pgresult);
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;
	default:
		{			
		}
	}
	return 0;
}

/*获取字段名称*/
char * cdbl_get_fieldname(CDBL_CONTEXT * context,int column_number)
{
	if(context == NULL)
		return NULL;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   
			return dbcolname((DBPROCESS*)context->sqlsvrsession.dbproc,column_number);
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{		         
			if(context->pqsession.pgresult == NULL)
				return NULL;
			return PQfname((PGresult*)context->pqsession.pgresult,column_number);
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;
	default:
		{
			
		}
	}
	return NULL;
}

/*获取字段大小*/
int cdbl_get_fieldsize(CDBL_CONTEXT * context,int column_number)
{
	if(context == NULL)
		return 0;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   
			return dbcollen((DBPROCESS*)context->sqlsvrsession.dbproc,column_number);
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{		  
			if(context->pqsession.pgresult == NULL)
				return 0;
			return PQfsize((PGresult*)context->pqsession.pgresult,column_number);
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;
	default:
		{
			
		}
	}
	return 0;
}

/*获取数值*/
char *cdbl_get_value(CDBL_CONTEXT * context,int row_number,int column_number)
{
	if(context == NULL)
		return NULL;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   
			if(dbgetrow((DBPROCESS*)context->sqlsvrsession.dbproc, row_number) == REG_ROW)
			{				
				//int len = dbdatlen((DBPROCESS*)context->sqlsvrsession.dbproc, column_number); //获取数据长度
				return (char*)dbdata((DBPROCESS*)context->sqlsvrsession.dbproc, column_number);
			}			
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{		  
			if(context->pqsession.pgresult == NULL)
				return NULL;
			return PQgetvalue((PGresult*)context->pqsession.pgresult,row_number,column_number);			
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;
	default:
		{
			
		}
	}
	return NULL;
}

/*获取当前结果集字段数量*/
int cdbl_get_fieldscount(CDBL_CONTEXT * context)//字段数量
{
	if(context == NULL)
		return 0;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	 			
			return dbnumcols((DBPROCESS*)context->sqlsvrsession.dbproc);			
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{	
			if(context->pqsession.pgresult == NULL)
				return 0;
			return PQnfields((PGresult*)context->pqsession.pgresult);			
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;
	default:
		{
			
		}
	}
	return 0;
}


/*判断字段是否null值，1是，0否,-1错误*/
int cdbl_get_fieldisnull(CDBL_CONTEXT * context,int row_number,int column_number)
{
	if(context == NULL)
		return -1;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{		      
			if(context->pqsession.pgresult == NULL)
				return -1;
			return PQgetisnull((PGresult*)context->pqsession.pgresult,row_number,column_number);			
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;
	default:
		{
			
		}
	}
	return -1;
}

/*获取字段类型*/
int cdbl_get_coltype(CDBL_CONTEXT * context, int column)
{
	if(context == NULL)
		return -1;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   
			return dbcoltype((DBPROCESS*)context->sqlsvrsession.dbproc,column);
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{		         
			if(context->pqsession.pgresult == NULL)
				return -1;
			return cdbl_typeget_pg(PQftype((PGresult*)context->pqsession.pgresult,column));
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;
	default:
		{
			
		}
	}
	return -1;
}

/*获取字段数据大小*/
int cdbl_get_coldatalen(CDBL_CONTEXT * context, int column)
{
	if(context == NULL)
		return -1;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   
			return dbdatlen((DBPROCESS*)context->sqlsvrsession.dbproc,column);
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{		         
			if(context->pqsession.pgresult == NULL)
				return -1;
			return PQfsize((PGresult*)context->pqsession.pgresult,column);
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;
	default:
		{
			
		}
	}
	return -1;
}

/*获取数据集状态*/
CDBLRStatus_t cdbl_get_resultstatus(CDBL_CONTEXT * context)
{
	if(context == NULL)
		return CDBL_RESULT_FATAL_ERROR;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   
		   int res=dbresults((DBPROCESS*)context->sqlsvrsession.dbproc);
		   if(res == SUCCEED)
		   {			   
			   context->eCDBLResult=CDBL_YES;	
			   context->EOFTAG=FALSE;
			   return CDBL_RESULT_TUPLES_OK;
		   }
		   else if(res == NO_MORE_RESULTS)
		   {			   
			   context->eCDBLResult=CDBL_NO;
			   context->EOFTAG=TRUE;
			   return CDBL_RESULT_NO_MORE_RESULTS;
		   }
		   else
		   {			  				   
			   return CDBL_RESULT_FATAL_ERROR;
		   }
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{		         
			ExecStatusType status;
			if(context->pqsession.pgresult == NULL)
			{
				return CDBL_RESULT_FATAL_ERROR;
			}
			
			status=PQresultStatus((PGresult*)context->pqsession.pgresult);
			if(status == PGRES_EMPTY_QUERY)
			{
				context->eCDBLResult=CDBL_NO;
				context->EOFTAG=TRUE;
				return CDBL_RESULT_EMPTY_QUERY;
			}
			else if(status == PGRES_COMMAND_OK)
			{
				context->eCDBLResult=CDBL_NO;
				context->EOFTAG=TRUE;
				return CDBL_RESULT_COMMAND_OK;
			}
			else if(status ==  PGRES_TUPLES_OK)
			{
				context->eCDBLResult=CDBL_YES;
				context->EOFTAG=FALSE;
				return CDBL_RESULT_TUPLES_OK;
			}
			else if(status == PGRES_COPY_OUT)
				return CDBL_RESULT_COPY_OUT;
			else if(status == PGRES_COPY_IN)
				return CDBL_RESULT_COPY_IN;
			else if(status == PGRES_BAD_RESPONSE)
			{
				context->eCDBLResult=CDBL_NO;
				context->EOFTAG=TRUE;
				return CDBL_RESULT_BAD_RESPONSE;
			}
			else if(status ==  PGRES_NONFATAL_ERROR)
			{
				context->eCDBLResult=CDBL_NO;
				context->EOFTAG=TRUE;
				return CDBL_RESULT_NONFATAL_ERROR;
			}
			else if(status ==  PGRES_FATAL_ERROR)
			{
				context->eCDBLResult=CDBL_NO;
				context->EOFTAG=TRUE;
				return CDBL_RESULT_FATAL_ERROR;
			}
			
			context->eCDBLResult=CDBL_NO;
			context->EOFTAG=TRUE;
			return CDBL_RESULT_FATAL_ERROR;			
			
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;	
	}
	return CDBL_RESULT_FATAL_ERROR;
}
//取字段序号	
CDBLError_t cdbl_get_colid(CDBL_CONTEXT * context,char *colName)
{
	char curcol[COLEN];
#ifndef _WIN32
	       sprintf((char *)curcol,"%s",colName);
		   for(context->COL=context->COLUMN;context->COL;context->COL=context->COL->next)
			if(strcmp(strupr(curcol),context->COL->name)==0)
				break;
#else	       
	       sprintf_s((char *)curcol,COLEN,"%s",colName);
		   _strupr_s(curcol,COLEN);
		   for(context->COL=context->COLUMN;context->COL;context->COL=context->COL->next)
			if(strcmp(curcol,context->COL->name)==0)
				break;
#endif	
	
	if(context->COL) return 1;
	return CDBL_NO_FIND_COL_Error;
}
int cdbl_iseof(CDBL_CONTEXT * context)
{
	return context->EOFTAG;
}

CDBLError_t cdbl_next(CDBL_CONTEXT * context)
{
	if(context == NULL)
		return CDBL_PARM_Error;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   
			RETCODE row_code; 
			if ( context->eCDBLResult == CDBL_NO ) 
				return CDBL_NO_INIT_BUFF_Error;
			if(dbresults((DBPROCESS*)context->sqlsvrsession.dbproc) != NO_MORE_RESULTS)
			{
				row_code=dbnextrow((DBPROCESS*)context->sqlsvrsession.dbproc);
				switch(row_code)
				{	
				case NO_MORE_ROWS://没有结果行可读
					{
						context->eCDBLResult = CDBL_NO;
						context->EOFTAG=TRUE;
						return CDBL_NO_MORE_ROWS_Error;
					}
					break;
				case REG_ROW://常规列
					return CDBL_No_Error;
					break;
				default:
					if(row_code>0)
					{
						//COMPUTE_COLS; 计算列？
						return CDBL_No_Error;
					}
				}
			}
			return CDBL_NO_MORE_RESULTS_Error;
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{		    		
			if ( context->eCDBLResult == CDBL_NO ) 
				return CDBL_NO_INIT_BUFF_Error;
			if(context->pqsession.rowcount > 0)
			{		
				if(context->pqsession.currow < context->pqsession.rowcount)
				{					
					context->pqsession.currow++;					
				}	
				if(context->pqsession.currow >= context->pqsession.rowcount)
				{
					//没有结果行可读
					context->eCDBLResult = CDBL_NO;
					context->EOFTAG=TRUE;
					return CDBL_NO_MORE_ROWS_Error;
				}
				return CDBL_No_Error;							
			}
			return CDBL_NO_MORE_RESULTS_Error;
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;
	default:
		{
			
		}
	}
	return CDBL_OTHER_Error;
}

CDBLError_t cdbl_first(CDBL_CONTEXT * context)
{	
	if(context == NULL)
		return CDBL_PARM_Error;
	if(context->eCDBLResult == CDBL_NO)
		return CDBL_PARM_Error;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   
			int res;				
			//res=dbsetrow((DBPROCESS*)context->sqlsvrsession.dbproc,1);		
			res=dbgetrow((DBPROCESS*)context->sqlsvrsession.dbproc,1);
			if((res != NO_MORE_ROWS) && (res != FAIL))
			{
				context->EOFTAG=FALSE;	
				return CDBL_No_Error;
			}			
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{						
			if(context->pqsession.rowcount > 0 )
			{
				context->pqsession.currow=0;
				context->EOFTAG=FALSE;	
				return CDBL_No_Error;
			}			
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;
	default:
		{			
		}
	}
	return CDBL_OTHER_Error;
}

//int ,short,long
CDBLError_t cdbl_get_itemvalue_l(CDBL_CONTEXT * context,char *colName,long *lresult)
{
	CDBLError_t reterr;
	if(context == NULL)
		return CDBL_PARM_Error;
	if(context->eCDBLResult == CDBL_NO)
		return CDBL_PARM_Error;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   			
			int lv=0;
			unsigned char bit[16];
			char smint[16];
			CDBLError_t ret;
			memset(bit,0,16);
			memset(smint,0,16);
			ret=cdbl_get_colid(context,colName);
			if (ret == CDBL_NO_FIND_COL_Error)
				return CDBL_NO_FIND_COL_Error;
			switch(context->COL->type)
			{
			case SQLINT1:
				{
					if(dbconvert((DBPROCESS*)context->sqlsvrsession.dbproc,SQLINT1,dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index),
						(DBINT)-1,SQLCHAR,(LPBYTE)bit,(DBINT)-1)==FAIL)
						reterr = CDBL_COLTYPE_NOT_MATCHING_Error;
						//ERR_COLTYPE_NOT_MATCHING		
					//LOG_DEBUG("cdbl_get_itemvalue_l:%s",bit);
					*lresult=atol((char *)bit);
					return reterr;
				}
			case SQLINT2:
				{
					if(dbconvert((DBPROCESS*)context->sqlsvrsession.dbproc,SQLINT2,dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index),
						(DBINT)-1,SQLCHAR,(LPBYTE)smint,(DBINT)-1)==FAIL)
						reterr=CDBL_COLTYPE_NOT_MATCHING_Error;
						//ERR_COLTYPE_NOT_MATCHING
					//LOG_DEBUG("cdbl_get_itemvalue_l:%s",smint);
					*lresult=atol((char *)smint);
					return reterr;					
				}
			case SQLINT4:
			case SQLINTN:
				{					
					BYTE* lp=dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index);
					lv=*((int*)lp);
					*lresult=(long)lv;
					return CDBL_No_Error;					
				}
			case SQLBIT:
				{
					if(dbconvert((DBPROCESS*)context->sqlsvrsession.dbproc,SQLBIT,dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index),
						(DBINT)-1,SQLCHAR,(LPBYTE)bit,(DBINT)-1)==FAIL)
						reterr=CDBL_COLTYPE_NOT_MATCHING_Error;
						//ERR_COLTYPE_NOT_MATCHING	
					//LOG_DEBUG("cdbl_get_itemvalue_l:%s",bit);
					*lresult=atol((char *)bit);
					return reterr;
				}
			default:
				return CDBL_COLTYPE_NOT_MATCHING_Error;
			}
			return CDBL_No_Error;
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{		         			
			CDBLError_t ret;			
			char *iptr=NULL; 	
			ret=cdbl_get_colid(context,colName);
			if (ret == CDBL_NO_FIND_COL_Error)
				return CDBL_NO_FIND_COL_Error;			
			switch (context->COL->type) {
				case CDBL_TYPE_BOOLEAN:					
				case CDBL_TYPE_INTEGER:			
					{
						//*lresult = ntohl(*((UINT32 *) iptr));
						iptr=PQgetvalue((PGresult*)(context->pqsession.pgresult), context->pqsession.currow, context->COL->index);
						if(iptr==NULL)
							return CDBL_NO_VALUE_Error;
						*lresult = atol(iptr);
					}
					break;				
				default:
					return CDBL_COLTYPE_NOT_MATCHING_Error;		  
			  }			
			return CDBL_No_Error;
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;	
	}
	return CDBL_OTHER_Error;
}
//float,double,real,money,smallmoney
CDBLError_t cdbl_get_itemvalue_d(CDBL_CONTEXT * context,char *colName,double *pdresult)
{
	if(context == NULL)
		return CDBL_PARM_Error;
	if(context->eCDBLResult == CDBL_NO)
		return CDBL_PARM_Error;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   			
			CDBLError_t ret;	
			CDBLRStatus_t rst;
			ret=cdbl_get_colid(context,colName);
			if (ret == CDBL_NO_FIND_COL_Error)
				return CDBL_NO_FIND_COL_Error;
			rst=cdbl_get_resultstatus(context);	
			if(rst != CDBL_RESULT_NO_MORE_RESULTS)
			{		
				switch(context->COL->type)
				{
				case SQLFLT4:
					if(dbconvert((DBPROCESS*)context->sqlsvrsession.dbproc,SQLFLT4,dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index),
						(DBINT)-1,SQLFLT4,(LPBYTE)pdresult,(DBINT)-1)==FAIL)
						return CDBL_COLTYPE_NOT_MATCHING_Error;
					break;
				case SQLFLTN:
					{
						#ifndef _WIN32
						if(dbconvert((DBPROCESS*)context->sqlsvrsession.dbproc,SQLFLTN,dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index),
							(DBINT)-1,SQLFLT8,(LPBYTE)pdresult,(DBINT)-1)==FAIL)
							return CDBL_COLTYPE_NOT_MATCHING_Error;
						#else
						if(dbconvert((DBPROCESS*)context->sqlsvrsession.dbproc,SQLFLTN,dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index),
							(DBINT)-1,SQLFLTN,(LPBYTE)pdresult,(DBINT)-1)==FAIL)
							return CDBL_COLTYPE_NOT_MATCHING_Error;
						#endif
					}
					break;
				case SQLFLT8:
					if(dbconvert((DBPROCESS*)context->sqlsvrsession.dbproc,SQLFLT8,dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index),
						(DBINT)-1,SQLFLT8,(LPBYTE)pdresult,(DBINT)-1)==FAIL)
						return CDBL_COLTYPE_NOT_MATCHING_Error;
					break;
				case SQLDECIMAL:
					{
						#ifndef _WIN32
						if(dbconvert((DBPROCESS*)context->sqlsvrsession.dbproc,SQLDECIMAL,dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index),
							(DBINT)-1,SQLFLT8,(LPBYTE)pdresult,(DBINT)-1)==FAIL)
							return CDBL_COLTYPE_NOT_MATCHING_Error;
						#else
						if(dbconvert((DBPROCESS*)context->sqlsvrsession.dbproc,SQLDECIMAL,dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index),
							(DBINT)-1,SQLDECIMAL,(LPBYTE)pdresult,(DBINT)-1)==FAIL)
							return CDBL_COLTYPE_NOT_MATCHING_Error;
						#endif
					}
					break;
				case SQLNUMERIC:
					{
						#ifndef _WIN32
						if(dbconvert((DBPROCESS*)context->sqlsvrsession.dbproc,SQLNUMERIC,dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index),
							(DBINT)-1,SQLFLT8,(LPBYTE)pdresult,(DBINT)-1)==FAIL)
							return CDBL_COLTYPE_NOT_MATCHING_Error;
						#else
						if(dbconvert((DBPROCESS*)context->sqlsvrsession.dbproc,SQLNUMERIC,dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index),
							(DBINT)-1,SQLNUMERIC,(LPBYTE)pdresult,(DBINT)-1)==FAIL)
							return CDBL_COLTYPE_NOT_MATCHING_Error;
						#endif
					}
					break;
				case SQLMONEY4:
					if(dbconvert((DBPROCESS*)context->sqlsvrsession.dbproc,SQLMONEY4,dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index),
						(DBINT)-1,SQLFLT8,(LPBYTE)pdresult,(DBINT)-1)==FAIL)
						return CDBL_COLTYPE_NOT_MATCHING_Error;
					break;
				case SQLMONEY:
					if(dbconvert((DBPROCESS*)context->sqlsvrsession.dbproc,SQLMONEY,dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index),
						(DBINT)-1,SQLFLT8,(LPBYTE)pdresult,(DBINT)-1)==FAIL)
						return CDBL_COLTYPE_NOT_MATCHING_Error;
					break;
				default:
					return CDBL_COLTYPE_NOT_MATCHING_Error;
				}
			}			
			return CDBL_No_Error;

		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{		         
			CDBLError_t ret;				
			char *dptr=NULL; 			
			ret=cdbl_get_colid(context,colName);
			if (ret == CDBL_NO_FIND_COL_Error)
				return CDBL_NO_FIND_COL_Error;
			
			switch (context->COL->type) {				
				case CDBL_TYPE_NUMERIC:		
					{
						dptr=PQgetvalue((PGresult*)(context->pqsession.pgresult), context->pqsession.currow, context->COL->index);
						if(dptr==NULL)
							return CDBL_NO_VALUE_Error;
						*pdresult = atof(dptr);
					}
					break;				
				default:
					return CDBL_COLTYPE_NOT_MATCHING_Error;		  
			  }			
			return CDBL_No_Error;
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;	
	}
	return CDBL_OTHER_Error;
}
//char,text,varchar
CDBLError_t cdbl_get_itemvalue_s(CDBL_CONTEXT * context,char *colName,char *cpresult,int buflen)
{
	if(context == NULL)
		return CDBL_PARM_Error;
	if(context->eCDBLResult == CDBL_NO)
		return CDBL_PARM_Error;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   			
			CDBLError_t ret;			
			char *sptr=NULL; 
			int colvlen=0;
			ret=cdbl_get_colid(context,colName);
			if (ret == CDBL_NO_FIND_COL_Error)
				return CDBL_NO_FIND_COL_Error;			
			switch(context->COL->type)
			{
			case SQLCHAR:
			case SQLVARCHAR:
			case SQLTEXT:
				{
					sptr=(char *)dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index);
					colvlen=dbdatlen((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index);
					if(sptr==NULL || colvlen <= 0)
						return CDBL_NO_VALUE_Error;
					if(colvlen > buflen)
						return CDBL_PARM_Error;
					memset(cpresult,0x00,buflen);
#ifndef _WIN32
					strncpy(cpresult,sptr,colvlen);
#else	       
					strncpy_s(cpresult,buflen,sptr,colvlen);
#endif
				}
				break;
			default:
				return CDBL_COLTYPE_NOT_MATCHING_Error;
			}
			return CDBL_No_Error;
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{		         
			CDBLError_t ret;	
			char *sptr=NULL; 
			int colvlen=0;
			ret=cdbl_get_colid(context,colName);
			if (ret == CDBL_NO_FIND_COL_Error)
				return CDBL_NO_FIND_COL_Error;			
			switch (context->COL->type) {				
				case CDBL_TYPE_STRING:						
				case CDBL_TYPE_BITSTRING:
					{
						sptr=PQgetvalue((PGresult*)(context->pqsession.pgresult), context->pqsession.currow, context->COL->index);
						if(sptr==NULL)
							return CDBL_NO_VALUE_Error;
						colvlen=PQfsize((PGresult*)(context->pqsession.pgresult),context->COL->index);
						if(colvlen <= 0 && sptr)
						{
							colvlen=strlen(sptr);
						}
						if(colvlen <= 0)
							return CDBL_NO_VALUE_Error;
						if(colvlen > buflen)
							return CDBL_PARM_Error;
						memset(cpresult,0x00,buflen);
#ifndef _WIN32
						strncpy(cpresult,sptr,colvlen);
#else	       
						strncpy_s(cpresult,buflen,sptr,colvlen);
#endif
					}
					break;
				default:
					return CDBL_COLTYPE_NOT_MATCHING_Error;		  
			  }			
			return CDBL_No_Error;
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;	
	}
	return CDBL_OTHER_Error;
}
//datetime
CDBLError_t cdbl_get_itemdate(CDBL_CONTEXT * context,char *colName,char *strdate,int buflen)
{
	if(context == NULL)
		return CDBL_PARM_Error;
	if(context->eCDBLResult == CDBL_NO)
		return CDBL_PARM_Error;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   		
			CDBLError_t ret;				
			DBDATEREC dc;
			int rt;
			ret=cdbl_get_colid(context,colName);
			if (ret == CDBL_NO_FIND_COL_Error)
				return CDBL_NO_FIND_COL_Error;						
			switch(context->COL->type)
			{
			case SQLDATETIM4:
			case SQLDATETIME:
				{
					rt=dbdatecrack((DBPROCESS*)context->sqlsvrsession.dbproc,&dc,(DBDATETIME *)dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index));
					if (rt==1)
					{
						char tmp_date[20];		
#ifndef _WIN32
						sprintf(tmp_date,"%04d-%02d-%02d %02d:%02d:%02d",
							dc.dateyear,dc.datemonth+1,dc.datedmonth,dc.datehour,dc.dateminute,dc.datesecond);					
						sprintf(strdate,"%s",tmp_date);
#else	       
						sprintf_s(tmp_date,20,"%04d-%02d-%02d %02d:%02d:%02d",
							dc.dateyear,dc.datemonth+1,dc.datedmonth,dc.datehour,dc.dateminute,dc.datesecond);					
						sprintf_s(strdate,buflen,"%s",tmp_date);
#endif
						
						return CDBL_No_Error;
					}
					return CDBL_COLTYPE_NOT_MATCHING_Error;
				}
			default:
				return CDBL_COLTYPE_NOT_MATCHING_Error;
			}
		   return CDBL_No_Error;
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{		         
			CDBLError_t ret;	
			char *tptr=NULL; 
			int colvlen=0;			
			ret=cdbl_get_colid(context,colName);
			if (ret == CDBL_NO_FIND_COL_Error)
				return CDBL_NO_FIND_COL_Error;			
			switch (context->COL->type) {				
				case CDBL_TYPE_STRING:						
				case CDBL_TYPE_BITSTRING:
					{
						tptr=PQgetvalue((PGresult*)(context->pqsession.pgresult), context->pqsession.currow, context->COL->index);
						if(tptr==NULL)
							return CDBL_NO_VALUE_Error;
						colvlen=PQfsize((PGresult*)(context->pqsession.pgresult),context->COL->index);						
						if(colvlen <= 0 && tptr)
						{
							colvlen=strlen(tptr);
						}
						if(colvlen <= 0)
							return CDBL_NO_VALUE_Error;						
						if(colvlen > buflen)
							return CDBL_PARM_Error;
						memset(strdate,0x00,buflen);
#ifndef _WIN32
						strncpy(strdate,tptr,colvlen);
#else	       
						strncpy_s(strdate,buflen,tptr,colvlen);
#endif
					}
					break;
				default:
					return CDBL_COLTYPE_NOT_MATCHING_Error;		  
			  }			
			return CDBL_No_Error;
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;	
	}
	return CDBL_OTHER_Error;
}

//datetime
CDBLError_t cdbl_get_itemdate_extra(CDBL_CONTEXT * context,char *colName,char *strdate,int buflen)
{
	if(context == NULL)
		return CDBL_PARM_Error;
	if(context->eCDBLResult == CDBL_NO)
		return CDBL_PARM_Error;
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   		
			CDBLError_t ret;				
			DBDATEREC dc;
			int rt;
			ret=cdbl_get_colid(context,colName);
			if (ret == CDBL_NO_FIND_COL_Error)
				return CDBL_NO_FIND_COL_Error;						
			switch(context->COL->type)
			{
			case SQLDATETIM4:
			case SQLDATETIME:
				{
					rt=dbdatecrack((DBPROCESS*)context->sqlsvrsession.dbproc,&dc,(DBDATETIME *)dbdata((DBPROCESS*)context->sqlsvrsession.dbproc,context->COL->index));
					if (rt==1)
					{
						char tmp_date[20];		
#ifndef _WIN32
						sprintf(tmp_date,"%04d%02d%02d%02d:%02d:%02d",
							dc.dateyear,dc.datemonth+1,dc.datedmonth,dc.datehour,dc.dateminute,dc.datesecond);					
						sprintf(strdate,"%s",tmp_date);
#else	       
						sprintf_s(tmp_date,20,"%04d%02d%02d%02d%02d%02d",
							dc.dateyear,dc.datemonth+1,dc.datedmonth,dc.datehour,dc.dateminute,dc.datesecond);					
						sprintf_s(strdate,buflen,"%s",tmp_date);
#endif
						
						return CDBL_No_Error;
					}
					return CDBL_COLTYPE_NOT_MATCHING_Error;
				}
			default:
				return CDBL_COLTYPE_NOT_MATCHING_Error;
			}
		   return CDBL_No_Error;
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{		         
			CDBLError_t ret;	
			char *tptr=NULL; 
			int colvlen=0;		
			ret=cdbl_get_colid(context,colName);
			if (ret == CDBL_NO_FIND_COL_Error)
				return CDBL_NO_FIND_COL_Error;
			tptr=PQgetvalue((PGresult*)(context->pqsession.pgresult), context->pqsession.currow, context->COL->index);
			switch (context->COL->type) {				
				case CDBL_TYPE_STRING:						
				case CDBL_TYPE_BITSTRING:
					{
						tptr=PQgetvalue((PGresult*)(context->pqsession.pgresult), context->pqsession.currow, context->COL->index);
						if(tptr==NULL)
							return CDBL_NO_VALUE_Error;
						colvlen=PQfsize((PGresult*)(context->pqsession.pgresult),context->COL->index);						
						if(colvlen <= 0 && tptr)
						{
							colvlen=strlen(tptr);
						}
						if(colvlen <= 0)
							return CDBL_NO_VALUE_Error;						
						if(colvlen > buflen)
							return CDBL_PARM_Error;
						memset(strdate,0x00,buflen);
#ifndef _WIN32
						strncpy(strdate,tptr,colvlen);
#else	       
						strncpy_s(strdate,buflen,tptr,colvlen);
#endif
					}
					break;
				default:
					return CDBL_COLTYPE_NOT_MATCHING_Error;		  
			  }			
			return CDBL_No_Error;
		}
		break;
	case CDBL_SQLite_Client:
		{
		}
		break;	
	}
	return CDBL_OTHER_Error;
}

CDBLError_t cdbl_close(CDBL_CONTEXT * context)
{
	if(context == NULL)
		return CDBL_PARM_Error;	
	switch(context->eCDBLType)
	{
	case CDBL_Client_NotSpecified:
		{
		}
		break;
	case CDBL_ODBC_Client:
		{
		}
		break;
	case CDBL_Oracle_Client:
		{
		}
		break;
	case CDBL_SQLServer_Client:
		{	   		
			if(context->eCDBLResult == CDBL_YES)
			{
				dbcancel((DBPROCESS*)context->sqlsvrsession.dbproc);
				context->eCDBLResult = CDBL_NO;
				cdbl_free_col(context);
			}
			dbclose((DBPROCESS*)context->sqlsvrsession.dbproc); 
			return CDBL_No_Error;
		}
		break;
	case CDBL_InterBase_Client:
		{
		}
		break;
	case CDBL_SQLBase_Client:
		{
		}
		break;
	case CDBL_DB2_Client:
		{
		}
		break;
	case CDBL_Informix_Client:
		{
		}
		break;
	case CDBL_Sybase_Client:
		{
		}
		break;
	case CDBL_MySQL_Client:
		{
		}
		break;
	case CDBL_PostgreSQL_Client:
		{		         
			if(context->eCDBLResult == CDBL_YES)
			{
				PQclear((PGresult*)context->pqsession.pgresult);	
				context->eCDBLResult=CDBL_NO;				
				cdbl_free_col(context);
			}
			/* 关闭数据库连接并清理 */
			PQfinish((PGconn*)context->pqsession.pgconn); 
		}
		break;
	case CDBL_SQLite_Client:
		{			
		}
		break;	
	}
	return CDBL_OTHER_Error;
}

CDBLDataType_t cdbl_typeget_mssql(int inType)
{
	CDBLDataType_t	result;

	switch (inType)
	{
		case SQLINT1:
		case SQLINT2:
		case SQLINT4:
		case SQLINTN:
		case SQLBIT:
			result = CDBL_TYPE_INTEGER;
			break;
		case SQLFLT4:
		case SQLFLTN:
		case SQLFLT8:
		case SQLDECIMAL:
		case SQLNUMERIC:
		case SQLMONEY4:
		case SQLMONEY:
			result = CDBL_TYPE_NUMERIC;
			break;
		case SQLCHAR:
		case SQLVARCHAR:
		case SQLTEXT:
			result = CDBL_TYPE_STRING;
			break;
		case SQLDATETIM4:
		case SQLDATETIME:
			result = CDBL_TYPE_DATETIME;
			break;	
		default:
			result = CDBL_TYPE_USER;
			break;
	}
	return result;
}

///////////
CDBLDataType_t cdbl_typeget_pg(unsigned int inType)
{
	CDBLDataType_t	result;

	switch (inType)
	{
		case (BOOLOID):
			result = CDBL_TYPE_BOOLEAN;
			break;

		case (CHAROID):
		case (NAMEOID):
		case (BPCHAROID):
		case (VARCHAROID):
		case (TEXTOID):
			result = CDBL_TYPE_STRING;
			break;

		case (BITOID):
		case (VARBITOID):
			result = CDBL_TYPE_BITSTRING;
			break;

		case (OIDOID):
		case (REGPROCOID):
		case (REGPROCEDUREOID):
		case (REGOPEROID):
		case (REGOPERATOROID):
		case (REGCLASSOID):
		case (REGTYPEOID):
		case (INT2OID):
		case (INT4OID):
		case (INT8OID):
			result = CDBL_TYPE_INTEGER;
			break;

		case (FLOAT4OID):
		case (FLOAT8OID):
		case (NUMERICOID):
		case (CASHOID):
			result = CDBL_TYPE_NUMERIC;
			break;

		case (DATEOID):
		case (TIMEOID):
		case (TIMETZOID):
		case (ABSTIMEOID):
		case (TIMESTAMPOID):
		case (TIMESTAMPTZOID):
			result = CDBL_TYPE_DATETIME;
			break;

		case (RELTIMEOID):
		case (TINTERVALOID):
		case (INTERVALOID):
			result = CDBL_TYPE_TIMESPAN;
			break;

		case (POINTOID):
		case (LSEGOID):
		case (PATHOID):
		case (BOXOID):
		case (POLYGONOID):
		case (LINEOID):
		case (CIRCLEOID):
			result = CDBL_TYPE_GEOMETRIC;
			break;

		case (INETOID):
		case (CIDROID):
			result = CDBL_TYPE_NETWORK;
			break;

		case (UNKNOWNOID):
		case (InvalidOid):
			result = CDBL_TYPE_UNKNOWN;
			break;

		case (RECORDOID):
		case (CSTRINGOID):
		case (ANYOID):
		case (ANYARRAYOID):
		case (VOIDOID):
		case (TRIGGEROID):
		case (LANGUAGE_HANDLEROID):
		case (INTERNALOID):
		case (OPAQUEOID):
		case (ANYELEMENTOID):
			result = CDBL_TYPE_GENERIC;
			break;

		default:
			result = CDBL_TYPE_USER;
			break;
	}
	return result;
}	/* cdbl_typeget_pg() */

int test2(void)
{
	CDBL_CONTEXT *cs=NULL;
	CDBL_CONNECT connectinfo;
	CDBLError_t ret;
	long id=0;
	char pname[128]={""};
	char tmp[128]={""};
#ifndef _WIN32
	strcpy(connectinfo.szHost,"sql2008");
	strcpy(connectinfo.szUsername,"sa");
	strcpy(connectinfo.szPassword,"123");
	strcpy(connectinfo.szPort,"1433");
	strcpy(connectinfo.szDBName,"PmecTestDB");	
#else	       
	strcpy_s(connectinfo.szHost,CONNINFOLEN,"192.168.148.131:1433");
	strcpy_s(connectinfo.szUsername,CONNINFOLEN,"sa");
	strcpy_s(connectinfo.szPassword,CONNINFOLEN,"123");
	strcpy_s(connectinfo.szPort,PORTLEN,"1433");
	strcpy_s(connectinfo.szDBName,CONNINFOLEN,"PmecTestDB");	
#endif	
	cs=cdbl_connect(&connectinfo,CDBL_SQLServer_Client);
	if(cs == NULL)
	{
		printf("CDBL_Connect fail!\n"); 
		return -1;
	}
	ret=cdbl_cmdexec(cs,"select * from Area");
	if(ret != CDBL_No_Error)
	{
		printf("CDBL_CmdExec fail!\n"); 
		return -1;
	}
	while(!cdbl_iseof(cs))
	{
		
		cdbl_get_itemvalue_l(cs,"Id",&id);
		cdbl_get_itemvalue_s(cs,"AreaName",pname,128);
#ifndef _WIN32
		sprintf(tmp,"[%03d]%s\n",(int)id,pname);
#else
		sprintf_s(tmp,128,"[%03d]%s\n",(int)id,pname);
#endif
		printf(tmp); 
		cdbl_next(cs);
	}	
	ret=cdbl_close(cs);
	return 0;
}

int test3(void)
{
	CDBL_CONTEXT *cs=NULL;
	CDBL_CONNECT connectinfo;
	CDBLError_t ret;
	long id=0;
	char pname[128]={""};
	char tmp[128]={""};
#ifndef _WIN32
	strcpy(connectinfo.szHost,"sql2008");
	strcpy(connectinfo.szUsername,"sa");
	strcpy(connectinfo.szPassword,"123");
	strcpy(connectinfo.szPort,"1433");
	strcpy(connectinfo.szDBName,"PmecTestDB");	
#else	       
	strcpy_s(connectinfo.szHost,CONNINFOLEN,"192.168.148.131");
	strcpy_s(connectinfo.szUsername,CONNINFOLEN,"postgres");
	strcpy_s(connectinfo.szPassword,CONNINFOLEN,"123");
	strcpy_s(connectinfo.szPort,PORTLEN,"5432");
	strcpy_s(connectinfo.szDBName,CONNINFOLEN,"pmbidb");	
#endif	
	cs=cdbl_connect(&connectinfo,CDBL_PostgreSQL_Client);
	if(cs == NULL)
	{
		printf("CDBL_Connect fail!\n"); 
		return -1;
	}
	ret=cdbl_cmdexec(cs,"select * from pmbi_aeracode");
	if(ret != CDBL_No_Error)
	{
		printf("CDBL_CmdExec fail!\n"); 
		return -1;
	}
	while(!cdbl_iseof(cs))
	{
		
		cdbl_get_itemvalue_l(cs,"code_id",&id);
		cdbl_get_itemvalue_s(cs,"cities_name",pname,128);
#ifndef _WIN32
		sprintf(tmp,"[%03d]%s\n",(int)id,pname);
#else
		sprintf_s(tmp,128,"[%03d]%s\n",(int)id,pname);
#endif
		printf(tmp); 
		cdbl_next(cs);
	}	
	ret=cdbl_close(cs);
	return 0;
}

int msg_handler(DBPROCESS *dbproc, DBINT msgno, int msgstate, int severity,
    char *msgtext, char *srvname, char *procname, int line)
{                                    
    enum {changed_database = 5701, changed_language = 5703 };       
    if (msgno == changed_database || msgno == changed_language)
        return 0;    
    if (msgno > 0) 
	{
        fprintf(stderr, "Msg %ld, Level %d, State %d\n",(long) msgno, severity, msgstate);    
        if (strlen(srvname) > 0)
            fprintf(stderr, "Server '%s', ", srvname);
        if (strlen(procname) > 0)
            fprintf(stderr, "Procedure '%s', ", procname);
        if (line > 0)
            fprintf(stderr, "Line %d", line);    
        fprintf(stderr, "\n\t");
    }
    //LOG_ERROR("%s\n", msgtext);    
    if (severity > 10) 
	{                        
        fprintf(stderr, "error: severity %d > 10, exiting\n",severity);
        exit(severity);
    }
    
    return 0;                            
}
    
int err_handler(DBPROCESS * dbproc, int severity, int dberr, int oserr,
    char *dberrstr, char *oserrstr)
{                                    
    if (dberr) 
	{                            
        fprintf(stderr, "Msg %d, Level %d\n",dberr, severity);
        //LOG_ERROR("%s\n\n", dberrstr);
    }
    else 
	{
        fprintf(stderr, "DB-LIBRARY error:\n\t");
        //LOG_ERROR("%s\n", dberrstr);
    }    
    return INT_CANCEL;                        
}