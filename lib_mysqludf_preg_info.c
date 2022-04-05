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


/**
 * @file lib_mysqludf_preg_info.c
 *
 * @brief Implements the LIB_MYSQLUDF_INFO mysql udf
 */


/**
 * @page LIB_MYSQLUDF_PREG_INFO LIB_MYSQLUDF_PREG_INFO
 *
 * @brief Return version information for lib_mysqludf_preg package
 *
 * @par Function Installation
 *    CREATE FUNCTION lib_mysqludf_preg_info RETURNS STRING SONAME 'lib_mysqludf_preg.so' ;
 *
 * @par Synopsis
 *    LIB_MYSQLUDF_PREG_INFO()
 * 
 *     @return string - version information for the lib_mysqludf_preg package
 *
 * @par Examples:
 *    SELECT LIB_MYSQLUDF_PREG_INFO();
 *
  * @b Yields:
 * @verbatim
+--------------------------+
| LIB_MYSQLUDF_PREG_INFO() |
+--------------------------+
| lib_mysqludf_preg 0.6.1  | 
+--------------------------+
  @endverbatim
 */


#include "ghmysql.h"
//#include "preg.h"


/**
 * Public function declarations:
 */
bool lib_mysqludf_preg_info_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
char *lib_mysqludf_preg_info(UDF_INIT *initid __attribute__((unused)),
                             UDF_ARGS *args,
                             char *result, unsigned long *length,
                             char *is_null __attribute__((unused)),
                             char *error __attribute__((unused)));
void lib_mysqludf_info_deinit( UDF_INIT* initid );


/*
 * Public function definitions:
 */


/**
 * @fn bool lib_mysqludf_preg_info_init(UDF_INIT *initid, UDF_ARGS *args, 
 *                                         char *message)
 *
 * @brief
 *     Perform the per-query initializations
 *
 * @param initid - various info supplied by mysql api - read more at
 * http://dev.mysql.com/doc/refman/5.0/en/adding-udf.html
 *
 * @param args - array of information about arguments from the SQL call
 * See file documentation for the description of the SQL arguments
 *
 * @param message - for error messages.  Should be <80 but can be 255.
 *
 * @return 0 - on success
 * @return 1 - on error
 *
 * @details This function checks to make sure there are no arguments.
 */
bool lib_mysqludf_preg_info_init(UDF_INIT *initid, UDF_ARGS *args, 
                                    char *message)
{
    if (args->arg_count != 0)
    {
        strncpy(message, "lib_mysqludf_preg_info: does not accept arguments", MYSQL_ERRMSG_SIZE) ;
        return 1;
    }

    return 0;
}


/**
 * @fn char *lib_mysqludf_preg_info( UDF_INIT *initid , UDF_ARGS *args,
 *                                  char *result, unsigned long *length,
 *                                  char *is_null , char *error )
 *
 * @brief
 *     Get information about the installed lib_mysqludf_preg library
 *
 * @param initid - various info supplied by mysql api - read more at
 * http://dev.mysql.com/doc/refman/5.0/en/adding-udf.html
 *
 * @param args - array of information about arguments from the SQL call
 * See file documentation for the description of the SQL arguments
 *
 * @param result - a place the captured string can be placed (255 len max)
 * @param length - put the length of the captured item here.
 * @param is_null - set this if return value is null
 * @param error - to be set if an error occurs
 *
 * @return string - The information requested.
 *
 * @note: PACKAGE_STRING comes from autotools
 */
char *lib_mysqludf_preg_info( UDF_INIT *initid , UDF_ARGS *args,
                              char *result, unsigned long *length,
                              char *is_null , char *error )
{
    strcpy( result , PACKAGE_STRING );
    *length = strlen( result ) ;
    *is_null = 0 ; 
    *error = 0 ;
    return result ;
}


/** 
 * @fn void lib_mysqludf_preg_info_deinit(UDF_INIT *initid)
 *
 *      @brief cleanup after LIB_MYSQLUDF_PREG_INFO call
 *
 *      @param initid - pointer to struct to be cleaned.
 */
void lib_mysqludf_preg_info_deinit(UDF_INIT *initid)
{
}



