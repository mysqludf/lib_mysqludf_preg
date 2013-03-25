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
 * @file lib_mysqludf_preg_replace.c
 *
 * @brief Implements the PREG_REPLACE mysql udf
 *
 */


/**
 * @page PREG_REPLACE  PREG_REPLACE
 *
 * @brief performs regular expression search & replace using PCRE.
 *
 * @par Function Installation
 *    CREATE FUNCTION preg_replace RETURNS STRING SONAME 'lib_mysqludf_preg.so';
 *
 * @par Synopsis
 *    PREG_REPLACE( pattern , replacement , subject [ , limit ] )
 * 
 * @par
 *     @param pattern - is a string that is a perl compatible regular 
 * expression as documented at:
 * http://us.php.net/manual/en/ref.pcre.php This expression passed to
 * this function should have delimiters and can contain the standard
 * perl modifiers after the ending delimiter.  
 *
 *     @param replacement - is the string to use as the replacement.  This 
 * string may contain capture group references such as \\1.  You can also use
 * $1 for these in a similar fashion as in PHP.
 *
 *     @param subject -is the data to perform the match & replace on
 *
 *     @param limit - optional number that is the maximum replacements to 
 * perform.  Use -1 (or leave empty) for no limit.

 *     @return - string - 'subject' with the instances of pattern replaced 
 *     @return - string - the same as passed in if there were no matches
 *
 * @details
 *    preg_replace is a udf that performs a regular expression search and 
 * replace on a given piece of data using a PCRE as the replacement
 * pattern.  If limit is not speficied or is -1, preg_replace works
 * on all of the ocurrences of the pattern in the subject data.  Otherwise,
 * preg_replace will only replace the first <limit> occurences.
 *
 * @par Examples:
 *
 * SELECT PREG_REPLACE('/(.*?)(fox)/' , '$1dog' , 'the quick brown fox' );
 *
  * @b Yields:
 * @verbatim
+-----------------------------------------------------------------+
| PREG_REPLACE('/(.*?)(fox)/' , '$1dog' , 'the quick brown fox' ) |
+-----------------------------------------------------------------+
| the quick brown dog                                             | 
+-----------------------------------------------------------------+
@endverbatim
 *
 *
 * SELECT PREG_REPLACE('/\\s\\s/+', ' ' , products.title FROM products;
 *
 * Yields: The product names with all of the extra whitespace removed
 *
 * @note
 *    Remember to add a backslash to escape patterns that use \ notation.
 * Also, using $ notation makes things a little clearer when using 
 * backreferences in the replacement.
 */


#include "ghmysql.h"
#include "preg.h"

/*
 * Public function declarations:
 */
my_bool preg_replace_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
char *preg_replace( UDF_INIT *initid __attribute__((unused)),
                    UDF_ARGS *args, char *result, unsigned long *length,
                    char *is_null __attribute__((unused)),
                    char *error __attribute__((unused)));
void preg_replace_deinit( UDF_INIT* initid );


/**
 * @fn my_bool preg_replace_init(UDF_INIT *initid, UDF_ARGS *args, 
 *                               char *message)
 *
 * @brief
 *     Perform the per-query initializations for PREG_REPLACE
 *
 * @param initid - various info supplied by mysql api - read mode at
 * http://dev.mysql.com/doc/refman/5.0/en/adding-udf.html
 *
 * @param args - array of information about arguments from the SQL call
 * See file documentation for the description of the SQL arguments
 *
 * @param message - for error messages.  Should be <80 but can be up to
 * MYSQL_ERRMSG_SIZE.
 *
 * @return 0 - on success
 * @return 1 - on error
 *
 * @details This function calls pregInit to handle the common init taskes.
 * Then it checks to make sure there are 3 arguments.   The 4th argument
 * must be a number.
 */
my_bool preg_replace_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (args->arg_count < 3)
    {
        strncpy(message,"PREG_REPLACE: requires at least 3 arguments", MYSQL_ERRMSG_SIZE);
        return 1;
    }

    // Require a numeric 4th argument.  (This could possibly be enhanced
    // to allow for numeric strings.  For now, require an int
    if( args->arg_count > 3 && args->arg_type[3] != INT_RESULT )
    {
        strncpy(message,"PREG_REPLACE: 4th argument (limit) must be a number", MYSQL_ERRMSG_SIZE);
        return 1;
    }

    args->arg_type[2] = STRING_RESULT ;  // other 2 are set in common init

    // preg_replace cannot return NULL
    initid->maybe_null=0;	

    // max_length of -1 means no limit ; don't change if that.
    // replacement length might be 0 -- also don't change for that
    if( ((int)initid->max_length) > 0 &&
        ((int)args->lengths[1]) > 0 &&
        ((int)args->lengths[2]) > 0)
    {
        // it's difficult to know the max_length here due to back references
        // and repeating,...  Set it to
        // subject * the length of the replacement * the length of the
        // subject
        initid->max_length=args->lengths[1]*args->lengths[2]*args->lengths[1] ;
    }

    // Keep this after setting of max_length 
    if( pregInit( initid , args , message ) )
    {
        return 1 ;
    }

    return 0;
}

/**
 * @fn char *preg_replace( UDF_INIT *initid , UDF_ARGS *args, char *result, 
 *                         unsigned long *length, char *is_null, char *error )
 *
 * @brief
 *     The main routine that implements the PREG_REPLACE function.
 *
 * @param initid - various info supplied by mysql api - read more at
 * http://dev.mysql.com/doc/refman/5.0/en/adding-udf.html
 *
 * @param args - array of information about arguments from the SQL call
 * See file documentation for the description of the SQL arguments
 *
 * @param result - small place that the modified string can be placed (not used)
 * @param length - put the length of the modified string here.
 * @param is_null - set this if return value is null (not used)
 * @param error - set if an error occurs
 *
 * @return - string - with the replacements applied if there were any
 *
 * @details Most of the difficult work here is done by the pregReplace
 * functions, which was derived from the php extension and is in the
 * from_php.c file.  The function below mostly prepares the arguments
 * for the call to the pregReplace function and it then calls
 * pregMoveToReturnValues to adapt the return from pregReplace to be 
 * compatible with the MySQL UDF api.
 */
char *preg_replace( UDF_INIT *initid , UDF_ARGS *args, char *result, 
                    unsigned long *length, char *is_null, char *error )
{
    int count ;                 /* number of matches */
    char msg[255] ;             /* to store errors from regex compile */
    struct preg_s *ptr ;        /* local holder of initid->ptr */
    pcre *re ;                  /* the compiled pattern */
    char *subject ;             /* args[1] */
    unsigned long subject_len;  /* length of subject */
    char *replacement ;         /* args[2] */
    unsigned long repl_len ;    /* length of replacement */
    char *s  ;                  /* string modified with replacements */
    int s_len ;                 /* length of modified string */
    int limit ;                 /* args[3] */

    ptr = (struct preg_s *) initid->ptr ;

    *is_null = 0 ;
    *error = 0 ;                /* default to no error */

#ifndef GH_1_0_NULL_HANDLING
    if( ghargIsNullConstant( args , 2 ) || ghargIsNullConstant( args , 0 ) ) 
    {
        *is_null = 1 ; 
        return NULL ; 
    }
#endif

    if( ptr->constant_pattern )
    {
        re = ptr->re ;
    }
    else
    {
        re = pregCompileRegexArg( args , msg , sizeof(msg)) ;
        if( !re )
        {
            ghlogprintf( "PREG_REPLACE: compile failed: %s\n", msg );
            *error = 1 ;
            return  NULL ;
        }
    }

    int nullReplacement ; 
    nullReplacement = 0 ; 
    if( ghargIsNullConstant( args , 1 ) ) 
    {
        nullReplacement = 1 ; 
    }

    replacement = ghargdups( args , 1 , &repl_len ) ;
    if( !replacement )
    {
        ghlogprintf( "PREG_REPLACE: out of memory\n" );
        *error = 1 ;
        if( !ptr->constant_pattern ) 
            pcre_free( re ) ;

        return  NULL ;
    }

    subject = ghargdups( args , 2 , &subject_len ) ;
    if( !subject )
    {
        ghlogprintf( "PREG_REPLACE: can't allocate for subject\n", msg );
        *error = 1 ;
        if( !ptr->constant_pattern ) 
            pcre_free( re ) ;
        free( replacement );
        return  NULL ;
    }

    if( args->arg_count > 3 )
    {
        limit = (int)( *(longlong *)args->args[3]) ;
    }
    else
    {
        limit = -1 ;
    }

    memset(&msg, 0, sizeof(msg));

    s = pregReplace( re , NULL , subject, subject_len , replacement , 
                     repl_len , 0 , &s_len , limit , &count , 
                     msg ,  sizeof(msg) ) ;

#ifndef GH_1_0_NULL_HANDLING
    if( nullReplacement && s && subject && strcmp( s , subject ) ) {
        result = NULL  ;
        *is_null = 1 ; 
    }
    else 
#endif
    {
        if (!s && msg[0] != NULL) {
            *error = 1;
            ghlogprintf( "PREG_REPLACE: %s\n", msg );
        }
        
        result = pregMoveToReturnValues( initid ,length,is_null , error,s,s_len  );
    }

    free( subject );
    free( replacement ) ;
        
    if( !ptr->constant_pattern ) 
        pcre_free( re ) ;

    return result ;
}

/** 
 * @fn void preg_replace_deinit(UDF_INIT *initid)
 *
 *      @brief cleanup after PREG_REPLACE 
 *
 *      @param initid - pointer to struct to be cleaned.
 */
void preg_replace_deinit(UDF_INIT *initid)
{
    pregDeInit(initid);
}


