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
 * @file lib_mysqludf_preg_rlike.c
 *
 * @brief Implements the PREG_RLIKE mysql udf
 */


/**
 * @page PREG_RLIKE PREG_RLIKE
 *
 * @brief Test if a string matches a perl-compatible regular expression
 *
 * @par Function Installation
 *    CREATE FUNCTION preg_rlike RETURNS INTEGER SONAME 'lib_mysqludf_preg.so';
 *
 * @par Synopsis
 *    PREG_RLIKE( pattern , subject )
 * 
 * @par
 *     @param pattern - is a string that is a perl compatible regular 
 * expression as documented at:
 * http://us.php.net/manual/en/ref.pcre.php This expression passed to
 * this function should have delimiters and can contain the standard
 * perl modifiers after the ending delimiter.
 *
 *     @param subject - is the data to perform the test on.  
 *
 *     @return 1 - a match was found
 *     @return 0 - no match
 *
 * @details
 *    preg_rlike is a udf that tests if whether or not the given 
 * perl compatible regular expression matches the given data. 
 *
 * @par Examples:
 *
 * SELECT PREG_RLIKE('/The quick brown fox/i' , 'the quick brown fox' );
 *
 * @b Yields:
 * @verbatim
   +---------------------------------------------------------------+
   | PREG_RLIKE('/The quick brown fox/i' , 'the quick brown fox' ) |
   +---------------------------------------------------------------+
   |                                                             1 |
   +---------------------------------------------------------------+
@endverbatim
 *
 *  SELECT * from products WHERE PREG_RLIKE( '/organic/i' , products.title )
 *      
 *  Yields:  all of the products with 'organic' in their titles
 */


#include "ghmysql.h"
#include "preg.h"

// Defines
#define OVECCOUNT 30    // offsets vector size - can be constant since it  
                        // it is not used for capturing 


/**
 * Public function declarations:
 */
my_bool preg_rlike_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
longlong preg_rlike(UDF_INIT *initid __attribute__((unused)),
                      UDF_ARGS *args,
                      char *is_null __attribute__((unused)),
                      char *error __attribute__((unused)));
void preg_rlike_deinit( UDF_INIT* initid );


/*
 * Public function definitions:
 */

/**
 * @fn my_bool preg_rlike_init(UDF_INIT *initid, UDF_ARGS *args, 
 *                             char *message)
 *
 * @brief
 *     Perform the per-query initializations
 *
 * @param initid - various info supplied by mysql api - read mode at
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
 * @details This function checks to make sure there are 2 arguments.  It
 * then call pregInit to perform the common initializations.
 */
my_bool preg_rlike_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (args->arg_count != 2)
    {
        strcpy(message,"preg_rlike: needs exactly two arguments");
        return 1;
    }
    initid->maybe_null=0;	

    if( pregInit( initid , args , message ) )
    {
        return 1 ;
    }

    return 0;
}


/**
 * @fn longlong preg_rlike( UDF_INIT *initid ,  UDF_ARGS *args, char *is_null,
 *                          char *error )
 *
 * @brief
 *     The main routine for the PREG_RLIKE udf.
 *
 * @param initid - various info supplied by mysql api - read more at
 * http://dev.mysql.com/doc/refman/5.0/en/adding-udf.html
 *
 * @param args - array of information about arguments from the SQL call
 * See file documentation for the description of the SQL arguments
 *
 * @param is_null - set this is return value is null
 * @param error - to be set if an error occurs
 *
 * @return 0 - if the subject does not match the given pattern
 * @return 1 - if the subject matches the given pattern
 *
 * @details This function calls pcre_ex from the pcre library to execute
 * the compiled pattern (which is either precompiled by ..._init
 * or compiled here for non-constant pattern arguments).  It then
 * does the appropriate thing with the returns :>)
 */
longlong preg_rlike( UDF_INIT *initid ,  UDF_ARGS *args, char *is_null,
                     char *error )
{
    struct preg_s *ptr ;
    char msg [ 255 ] ;
    int ovector[OVECCOUNT];     /* for use by pcre_exex */
    int rc ;
    pcre *re ;                  /* the compiled regex */

    ptr = (struct preg_s *) initid->ptr ;
    if( args->args[1] && args->lengths[1] )
    {
        if( ptr->constant_pattern )
        {
            re = ptr->re ;
        }
        else
        {
            re = pregCompileRegexArg( args , msg , sizeof(msg)) ;
            if( !re )
            {
                fprintf( stderr,"preg: compile failed: %s\n",msg);
                *error = 1 ;
                return 0;
            }
        }

       rc = pcre_exec(re, NULL,  args->args[1] , (int)args->lengths[1],
                       0,0,ovector, OVECCOUNT); 

        if( !ptr->constant_pattern ) 
        {
            pcre_free( re ) ;
        }

        if( rc > 0 )
        {
            return 1 ;
        }
    }

    return 0 ;
}


/** 
 * @fn void preg_rlike_deinit(UDF_INIT *initid)
 *
 *      @brief cleanup after PREG_RLIKE 
 *
 *      @param initid - pointer to struct to be cleaned.
 */
void preg_rlike_deinit(UDF_INIT *initid)
{
    pregDeInit( initid ) ;
}



