# cdbl
CDBL is C library for accessing SQL databases (SQL Server, Postgre, MySQL, Oracle).

# Quick Example
```c	
static const char *szDBsql[] = {
  "select tablename from pg_tables", 
  "select tablename from pg_tables where schemaname=?"
};

enum DBsql_key
{
	select_pgtable,
	select_tablename,	
	SQL_END
};

int main(int argc, char **argv)
{
	CDBL_CONTEXT *cs=NULL;	
	CDBL_CONNECT connectinfo;	
	CDBLError_t ret;
	char stmp[128]={""};	
	/* connect db. */
	strcpy(connectinfo.szHost,"127.0.0.1");
	strcpy(connectinfo.szUsername,"postgres");
	strcpy(connectinfo.szPassword,"***");
	strcpy(connectinfo.szPort,"5432");
	strcpy(connectinfo.szDBName,"pgdb");	
	cs=cdbl_connect(&connectinfo,CDBL_PostgreSQL_Client);	
	if(cs == NULL)
	{				
		return 0;
	}		
	ret=cdbl_cmdexec(cs,szMSTsql[select_pgtable]);
	if(ret != CDBL_No_Error)
	{		
		ret=cdbl_close(cs);
		return 0;
	}
	while(!cdbl_iseof(cs))
	{				
			memset(stmp,0,128);
			cdbl_get_itemvalue_s(cs,"tablename",stmp,128);		
			cdbl_next(cs);
	}	
	ret=cdbl_close(cs);
	return 0;
}
	
