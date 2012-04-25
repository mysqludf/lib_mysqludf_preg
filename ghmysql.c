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


/** @file ghmysql.c
 *  
 * @brief Provides some functions that are shared by the various
 *        gh udf functions.   
 *
 */
#include "ghmysql.h"


/**
 * @fn char *ghstrndup( char *s , int l )
 *
 * @brief safe strndup
 *
 * @param s - bytes to copy
 * @param l - number of bytes to copy
 *
 * @return ptr -  to newly allocate memory 
 * @return NULL - if memory not available
 *
 * @details - This function copies the given number of bytes from the given
 * buffer into a newly allocated memory area.  It then adds a '\\0' to the
 * end.  This function shouldn't be necessary, but since many call rely on
 * null-termination instead of lengths, this creates things that can be
 * passed to those functions safely.
 */
char *ghstrndup( char *s , int l )
{
    char *colval = NULL ;

    colval = malloc( l + 1 ) ;
    if( !colval )
    {
        fprintf( stderr , "Not enough memory: %d\n" , l ) ;
        return NULL ;
    }
    memcpy( colval , s , l ) ;
    colval[ l ] = '\0'; 

    return colval ;
}


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
