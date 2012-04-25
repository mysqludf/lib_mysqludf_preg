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
my_bool lib_mysqludf_preg_info_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
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
 * @fn my_bool lib_mysqludf_preg_info_init(UDF_INIT *initid, UDF_ARGS *args, 
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
my_bool lib_mysqludf_preg_info_init(UDF_INIT *initid, UDF_ARGS *args, 
                                    char *message)
{
    if (args->arg_count != 0)
    {
        strcpy(message,"lib_mysqludf_preg_info: does not accept arguments" ) ;
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



