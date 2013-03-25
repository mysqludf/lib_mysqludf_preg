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
 * @file lib_mysqludf_preg_capture.c
 *
 * @brief Implements the PREG_CAPTURE mysql udf
 *
 */


/**
 * @page PREG_CAPTURE  PREG_CAPTURE
 *
 * @brief capture a parenthesized subexpression from a PCRE pattern
 *
 * @par Function Installation
 *    CREATE FUNCTION preg_capture RETURNS STRING SONAME 'lib_mysqludf_preg.so';
 *
 * @par Synopsis
 *    PREG_CAPTURE( pattern , subject [, group] [, occurence] )
 * 
 * @par
 *     @param pattern - is a string that is a perl compatible regular 
 * expression as documented at:
 * http://us.php.net/manual/en/ref.pcre.php This expression passed to
 * this function should have delimiters and can contain the standard
 * perl modifiers after the ending delimiter.  
 *
 *     @param subject -is the data to perform the match & capture on
 *
 *     @param group - is the capture group that should be 
 * returned.  This  can be a numeric capture group or a named capture group.
 * Numeric groups should be passed in as integers while named groups should be
 * strings.  If not speficied, this defaults to 0, which will capture the 
 * entire matching regular expression.
 *
 *     @param occurence - which match of the regex to perform capture on.
 * This is useful for subjects that have multiple matches of the pattern. If
 * not speficied, this defaults to 1, which will capture the requested group,
 * from the first matching occurence of the pattern.
 *
 *     @return - string that was captured - if there was a match and the desired
 * capture group is valid
 *     @return - string that is the entire portion of subject which matches the
 * pattern - if 0 is passed in as the group and pattern matches subject
 *     @return - NULL - if pattern does not match the subject or group is not a 
 * valid capture group for the given pattern and subject.
 *
 * @details
 *    preg_capture is a udf that captures parenthesized sub-expressions
 * from a pcre pattern.
 *
 * @par Examples:
 *
 *
 * SELECT PREG_CAPTURE('/(.*?)(fox)/' , 'the quick brown fox' ,2 );
 *
 * @b Yields:
 * @verbatim
+----------------------------------------------------------+
| PREG_CAPTURE('/(.*?)(fox)/' , 'the quick brown fox' ,2 ) |
+----------------------------------------------------------+
| fox                                                      | 
+----------------------------------------------------------+
@endverbatim
 *
 * SELECT PREG_CAPTURE( '/"([^"]+)"/' , 'the "quick" brown fox "jumped" over the "lazy" dog' , 1,2 );
 *
 * @verbatim
+--------------------------------------------------------------------------------------------+
| PREG_CAPTURE( '/"([^"]+)"/' , 'the "quick" brown fox "jumped" over the "lazy" dog' , 1,2 ) |
+--------------------------------------------------------------------------------------------+
| jumped                                                                                     | 
+--------------------------------------------------------------------------------------------+
@endverbatim
 *
 * SELECT PREG_CAPTURE( '/b[^\\s]+/' , 'the quick brown fox jumped over' ) 
 *
  * @b Yields:
 *
 * @verbatim
+------------------------------------------------------------------+
| PREG_CAPTURE( '/b[^\\s]+/' , 'the quick brown fox jumped over' ) |
+------------------------------------------------------------------+
| brown                                                            | 
+------------------------------------------------------------------+
@endverbatim
 *
 * @note
 *    Remember to add a backslash to escape patterns that use \ notation
 */


#include "ghmysql.h"
#include "preg.h"

/*
 * Public function declarations:
 */
my_bool preg_capture_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
char *preg_capture( UDF_INIT *initid __attribute__((unused)),
                    UDF_ARGS *args, char *result, unsigned long *length,
                    char *is_null __attribute__((unused)),
                    char *error __attribute__((unused)));
void preg_capture_deinit( UDF_INIT* initid );


/**
 * @fn my_bool preg_capture_init(UDF_INIT *initid, UDF_ARGS *args, 
 *                               char *message)
 *
 * @brief
 *     Perform the per-query initializations for PREG_CAPTURE
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
 * @details This function calls pregInit to handle the common init taskes.
 * It also checks to make sure there are at least arguments, and checks
 * the type of the 'group'  and 'occurence' arguments.
 */
my_bool preg_capture_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (args->arg_count < 2)
    {
        strncpy(message,"PREG_CAPTURE: requires at least 2 arguments", MYSQL_ERRMSG_SIZE);
        return 1;
    }

    if( args->arg_count > 3 && args->arg_type[3] != INT_RESULT ) {
        strncpy(message,"PREG_CAPTURE: optional occurence argument must be an integer", MYSQL_ERRMSG_SIZE);
        return 1;
    }

    // If occurence isn't an int, then it is a named capture group 
    if( args->arg_count > 2 &&  args->arg_type[2] != INT_RESULT )
        args->arg_type[2] != STRING_RESULT ;

    // preg_capture can return NULL
    initid->maybe_null=1;	

    // Default value of max_length should be sufficient

    return ( pregInit( initid , args , message ) ) ;
}


/**
 * @fn char *preg_capture(UDF_INIT *initid , UDF_ARGS *args, char *result, 
 *                        unsigned long *length, char *is_null , char *error )
 *
 * @brief
 *     The main routine for the PREG_CAPTURE udf.
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
 * @return - string - if the numeric or named group can be captured
 * @return - NULL - if no match or some other problem
 *
 * @details This function uses the pregSkipToOccurence function to call
 * pcre_ex repeatedly until the requested occurence is found.
 * It then uses pcre_get_substring (or copy) to retrieve any 
 * capture groups from the pattern.  It then returns the appropriate 
 * string or NULL.
 */
char *preg_capture(UDF_INIT *initid , UDF_ARGS *args, char *result, 
                   unsigned long *length, char *is_null , char *error )
{
    char *ex_subject ;          /* part subject starting after occurence*/
    int groupnum ;              /* numeric group - found or from args */
    int l ;                     /* length of captured info */
    char msg[255] ;             /* to store errors from regex compile */
    int occurence ;             /* occurence of the pattern to capture from */
    int oveccount ;             /* number of items captures */
    int *ovector;               /* for offsets of captures */
    struct preg_s *ptr ;        /* local holder of initid->ptr */
    int rc ;                    /* number of regex's matched by pattern  */
    pcre *re ;                  /* the compiled pattern */
    const char *res2 ;          /* for pcre_get_substring to alloc */
    char *subject ;             /* args[1] */

    ptr = (struct preg_s *) initid->ptr ;

    *is_null = 1 ;              /* default to NULL return */
    *error = 0 ;                /* default to no error */
    *ptr->return_buffer = '\0'; /* clear return value */
    *length = 0 ;               /* just to be safe  */

#ifndef GH_1_0_NULL_HANDLING
    if( ghargIsNullConstant( args , 0 ) || ghargIsNullConstant( args , 1 ) 
        || ghargIsNullConstant( args , 2 ) ) 
    {
        //fprintf( stderr , "null subject. Returning NULL\n" ) ; 
        *is_null = 1 ; 
        return NULL ; 
    }
#endif

    // compile the regex if necessary
    if( ptr->constant_pattern )
        re = ptr->re ;
    else
    {
        re = pregCompileRegexArg( args , msg , sizeof(msg)) ;
        if( !re )
        {
            ghlogprintf( "PREG_CAPTURE: compile failed: %s\n", msg );
            *error = 1 ;
            return  NULL ;
        }
    }

    // create vector to hold offsets for pcre
    ovector = pregCreateOffsetsVector( re,NULL, &oveccount ,msg,sizeof(msg)) ;
    if( !ovector )
    {
        ghlogprintf( "PREG_CAPTURE: can't create offset vector :%s\n", msg );
        *error = 1 ;
        if( !ptr->constant_pattern ) 
            pcre_free( re ) ;
        return NULL ;
    }

    if( args->arg_count > 3 ) 
        occurence = (int)(*(longlong *)args->args[3]) ;
    else 
        occurence = 1 ;

    subject = ghargdup( args , 1 ) ;

    if( subject )
    {
        ex_subject = pregSkipToOccurence( re , subject , args->lengths[1] , 
                                          ovector , oveccount , occurence,&rc);
        groupnum = -1 ;
        if( rc > 0 )
            groupnum = pregGetGroupNum( re , args , 2 ) ;

        // If groupnum found, get the substring and prepare for return
        if( groupnum >= 0 && groupnum < (oveccount/3) )
        {
            l = pcre_get_substring( ex_subject, ovector,rc,groupnum, &res2 ) ;

            result = pregMoveToReturnValues( initid,length,is_null , error, 
                                             (char *)res2 , l  );
        }
        free( subject ) ;
    }

    free( ovector ) ;

    if( !ptr->constant_pattern ) 
        pcre_free( re ) ;

    return result ;
}

/** 
 * @fn void preg_capture_deinit(UDF_INIT *initid)
 *
 *      @brief cleanup after PREG_CAPTURE 
 *
 *      @param initid - pointer to struct to be cleaned.
 */
void preg_capture_deinit(UDF_INIT *initid)
{
    pregDeInit(initid);
}
