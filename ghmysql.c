/*
 * Copyright (C) 2007-2013 Rich Waters <raw@goodhumans.net>
 *
 * This file is part of lib_mysqludf_preg.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


/** @file ghmysql.c
 *  
 * @brief Provides some functions that are shared by the various
 *        gh udf functions.   
 *
 */
#include "ghmysql.h"
#include "ghfcns.h"
#include <time.h>
#include <stdarg.h>


/**
 * @fn char *ghargdup( UDF_ARGS *args,int i ) 
 *
 * @brief extract argument as a newly allocated null-terminated memory area
 *
 * @param args - args from the UDF function call
 * @param i - offset of argument to extract.
 *
 * @return pointer -  to newly allocate memory if successful
 * @return NULL -  if arg is NULL or not enough memory
 * 
 */
char *ghargdup( UDF_ARGS *args,int i ) 
{      
    int l ;
    char *s ;
    char *colval = NULL ;

    if( (l=args->lengths[i] ) && (s=args->args[i]) )
    {
        colval = ghstrndup( s , l ) ;
    }

    return colval ;
}        

/**
 * @fn char *ghargdups( UDF_ARGS *args,int i , unsigned long *l)
 *
 * @brief extract argument as a newly allocated null-terminated memory area,
 * and set the length to the size of that area
 *
 * @param args - args from the UDF function call
 * @param i - offset of argument to extract.
 * @param l - store the length of the argument here
 *
 * @return pointer -  to newly allocate memory if successful
 *
 * @details This is useful for guaranteeing not to have NULL as an argument
 */
char *ghargdups( UDF_ARGS *args , int i , unsigned long *l)
{
    if( !(*l=args->lengths[ i ]))
    {   
        return strdup( "" ) ;
    }
    else if( !args->args[i]) {
        *l = 0 ; 
        return strdup( "" ) ;
    }
    else {
        return ghargdup( args , i ) ;
    }
}


/**
 * @fn char *ghargIsNull( UDF_ARGS *args,int argNum)
 *
 * @brief is the given argument NULL
 *
 * @param args - args from the UDF function call
 * @param i - the argument to test
 *
 * @return int - 1 if the argument is NULL, 0 otherwise
 *
 * @details - this only returns true if the arg is specified as NULL
 * when in the mysql statement.  This does not return true if the
 * NULL results from evaluating a column.  The purpose of this function
 * in so that init functions can differentiate NULL constants from 
 * NULL meaning that there are no constant arguments.  This is important
 * because there is better error handling in the API for init functions
 * than mains.
 */
int ghargIsNullConstant(UDF_ARGS *args, int argNum)
{
    if( args->arg_count > argNum && !args->args[argNum] && 
        !strncmp( args->attributes[argNum], "NULL" , 4 ) )  {
        return 1 ;
    }

    return 0 ;
}

