/*
  FreeTDS API
  
  Copyright (c) 2013 Xjj
  
  
*/
#ifndef __CTDSAPI__h
#define __CTDSAPI__h

#include "cdbl.h"
// API header(s)
#include "tds_sysdep_public.h"
#include "sqldb.h"
#include "sybfront.h"
#include "sybdb.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define TDS_MAX_CAPABILITY	22
typedef unsigned char TDS_TINYINT;			/*  8-bit unsigned */
typedef tds_sysdep_int32_type TDS_INT;			/* 32-bit int      */
typedef unsigned tds_sysdep_int16_type TDS_USMALLINT;	/* 16-bit unsigned */
typedef struct tds_dstr {
	char *dstr_s;
	size_t dstr_size;
} DSTR;
typedef struct tds_login
{
	DSTR server_name;
	int port;
	TDS_USMALLINT tds_version;	/* TDS version */
	int block_size;
	DSTR language;			/* e.g. us-english */
	DSTR server_charset;		/* e.g. iso_1 */
	TDS_INT connect_timeout;
	DSTR client_host_name;
	DSTR app_name;
	DSTR user_name;
	DSTR password;
	
	DSTR library;	/* Ct-Library, DB-Library,  TDS-Library or ODBC */
	TDS_TINYINT encryption_level;

	TDS_INT query_timeout;
	unsigned char capabilities[TDS_MAX_CAPABILITY];
	DSTR client_charset;
	DSTR database;
	unsigned int bulk_copy:1;
	unsigned int suppress_language:1;
} TDSLOGIN;
struct tds_dblib_loginrec
{
	TDSLOGIN *tds_login;
};
#ifdef _WIN32
typedef struct tds_dblib_loginrec LOGINREC;
#endif

#ifdef __cplusplus
}
#endif

#endif