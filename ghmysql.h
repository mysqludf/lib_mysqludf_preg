/*
 * Copyright (C) 2007 Rich Waters <raw@goodhumans.com>
 *
 * This file is part of lib_mysqludf_preg.
 *
 * lib_mysqudf_preg is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * lib_mysqludf_preg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GHMYSQL_H

#define GHMYSQL_H

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

/** @file ghmysql.h
 *  
 * @brief headers for ghmysql functions
 */


// This conflicts with my_config.h.  Therefore is it required that
// STANDARD is defined (in Makefile.am) in order that those mysql
// functions are not included.  
#include "config.h"


// Taken from udf_example.c

#ifdef STANDARD
/* STANDARD is defined, don't use any mysql functions */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __WIN__
typedef unsigned __int64 ulonglong;	/* Microsofts 64 bit types */
typedef __int64 longlong;
#else
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif /*__WIN__*/
#else
#include <my_global.h>
#include <my_sys.h>
#if defined(MYSQL_SERVER)
#include <m_string.h>		/* To get strmov() */
#else
/* when compiled as standalone */
#define strmov(a,b) strcpy(a,b)
#define bzero(a,b) memset(a,0,b)
#define memcpy_fixed(a,b,c) memcpy(a,b,c)
#endif
#endif
#include <mysql.h>
#include <ctype.h>

/*
 * GHMYSQL utility functions:
 */
char *ghargdup( UDF_ARGS *args,int i ) ;
char *ghargdups( UDF_ARGS *args,int i , unsigned long *l) ;
//char *ghstrndup( char *s , int l );
int ghargIsNullConstant(UDF_ARGS *args, int argNum);

#endif
