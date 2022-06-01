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
 * @file lib_mysqludf_preg_position.c
 *
 * @brief Implements the PREG_POSITION mysql udf
 *
 */


/**
 * @page PREG_POSITION PREG_POSITION
 *
 * @brief get position of the of a regular expression capture group in a string
 *
 * @par Function Installation
 *    CREATE FUNCTION preg_position RETURNS INTEGER SONAME 'lib_mysqludf_preg.so';
 *
 * @par Synopsis
 *    PREG_POSITION( pattern , subject [, group] [, occurence] )
 * 
 * @par
 *     @param pattern - is a string that is a perl compatible regular 
 * expression as documented at: http://us.php.net/manual/en/ref.pcre.php 
 * This expression passed to this function should have delimiters and 
 * can contain the standard perl modifiers after the ending delimiter.  
 *
 *     @param subject -is the data to perform the match & position capture on
 *
 *     @param group - the capture group whose position that should be 
 * returned.  This  can be a numeric capture group or a named capture group.
 * Numeric groups should be passed in as integers, while named groups should be
 * strings.  0 should be used to request to position of the entire matching
 * expression.  This parameter defaults to 0.
 *
 *     @param occurence - which match of the regex to perform capture on.
 * This is useful for subjects that have multiple matches of the pattern. This
 * parameter defaults to 1.
 *
 *     @return - integer position of the string that was captured - 
 * if there was a match and the desired  capture group and occurence is valid
 *     @return - NULL if pattern does not match the subject or group is not a 
 * valid capture group or the occurence is larger than the number of 
 * matches for the given pattern and subject.
 *
 * @details
 *    preg_position is a udf that captures the position of matching
 * regular expression from a pcre pattern.  It can be useful when strings
 * need to be split by a pcre pattern or if the location of the pattern
 * within the string is needed for some other reason.
 *
 * @par Examples:
 *
 *
 * SELECT PREG_POSITION('/(.*?)(fox)/' , 'the quick brown fox' ,2 );
 *
 * @b Yields:
 * @verbatim
+-----------------------------------------------------------+
| PREG_POSITION('/(.*?)(fox)/' , 'the quick brown fox' ,2 ) |
+-----------------------------------------------------------+
|                                                        17 | 
+-----------------------------------------------------------+
@endverbatim
 *
 * SELECT PREG_POSITION('/"[^"]+"/' , '"quick","brown","fox" "jumped"',0,4)
 *
 * @b Yields:
 * @verbatim
+-------------------------------------------------------------------+
| PREG_POSITION('/"[^"]+"/' , '"quick","brown","fox" "jumped"',0,4) |
+-------------------------------------------------------------------+
|                                                                23 | 
+-------------------------------------------------------------------+
@endverbatim
 *
 * @note
 *    Remember to add a backslash to escape patterns that use \ notation.
 * The returned position starts at 1 for the first character, as 
 * is standard for MySQL string functions.
 */

#include "ghmysql.h"
#include "preg.h"

/*
 * Public function declarations:
 */
bool preg_position_init(UDF_INIT *initid, UDF_ARGS *args, char *message);
longlong preg_position(UDF_INIT *initid __attribute__((unused)),
                      UDF_ARGS *args,
                      char *is_null __attribute__((unused)),
                      char *error __attribute__((unused)));
void preg_position_deinit( UDF_INIT* initid );


/**
 * @fn bool preg_position_init(UDF_INIT *initid, UDF_ARGS *args, 
 *                               char *message)
 *
 * @brief
 *     Perform the per-query initializations for PREG_POSITION
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
bool preg_position_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    if (args->arg_count < 2)
    {
        strncpy(message,"PREG_POSITION: requires at least 2 arguments", MYSQL_ERRMSG_SIZE);
        return 1;
    }

    // If group isn't an int, then it is a named capture group 
    if( args->arg_type[2] != INT_RESULT )
        args->arg_type[2] != STRING_RESULT ;

    // occurence must be an int
    if( args->arg_count > 3 && args->arg_type[3] != INT_RESULT ) {
        strncpy(message,"PREG_POSITION: optional occurence argument must be an integer", MYSQL_ERRMSG_SIZE);
        return 1;
    }

    // preg_position can return NULL
    initid->maybe_null=1;	

    return ( pregInit( initid , args , message ) ) ;
}


/**
 * @fn longlong preg_position( UDF_INIT *initid, UDF_ARGS *args, char *is_null,
 *                            char *error )
 *
 * @brief
 *     The main routine for the PREG_POSITION udf.
 *
 * @param initid - various info supplied by mysql api - read more at
 * http://dev.mysql.com/doc/refman/5.0/en/adding-udf.html
 *
 * @param args - array of information about arguments from the SQL call
 * See file documentation for the description of the SQL arguments
 *
 * @param is_null - set this if return value is null
 * @param error - to be set if an error occurs
 *
 * @return - the numeric position of the request capture group & occurence
 * @return - NULL - if no match or some other problem
 *
 * @details This function uses the pregSkipToOccurence function to call
 * pcre_ex repeatedly until the requested occurence is found.  It then
 * extracts the string offset for the given capture-group from the 
 * vector of returned strings and returns this as an sql string 
 * position (ie. offset+1);
 */
longlong preg_position( UDF_INIT *initid, UDF_ARGS *args, char *is_null,
                        char *error )
{
    char *ex_subject ;          // part of subject after occurence 
    int groupnum ;              /* numeric group - found or from args */
    char msg[255] ;             /* to store errors from regex compile */
    int occurence ;             /* perform capture on this occurence of match */
    int oveccount ;             /* number of items captures */
    int *ovector;               /* for offsets of captures */
    struct preg_s *ptr ;        /* local holder of initid->ptr */
    int rc ;                    /* number of regex's matched by pattern  */
    pcre *re ;                  /* the compiled pattern */
    char *subject ;             /* args[1] */
    int ret = -1 ;              /* position that will be returned */

    ptr = (struct preg_s *) initid->ptr ;

    *is_null = 1 ;              /* default to NULL return */
    *error = 0 ;                /* default to no error */
    *ptr->return_buffer = '\0'; /* clear return value */

#ifndef GH_1_0_NULL_HANDLING
    if( ghargIsNullConstant( args , 0 ) || ghargIsNullConstant( args , 1 ) 
        || ghargIsNullConstant( args , 2 ) ) 
    {
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
            ghlogprintf( "PREG_POSITION: compile failed: %s\n", msg );
            *error = 1 ;
            return  -1 ;
        }
    }
    
    // create vector to hold offsets for pcre
    ovector = pregCreateOffsetsVector( re,NULL, &oveccount ,msg,sizeof(msg)) ;
    if( !ovector )
    {
        ghlogprintf( "PREG_POSITION: can't create offset vector :%s\n", msg );
        *error = 1 ;
        if( !ptr->constant_pattern ) 
            pcre_free( re ) ;
        return -1 ;
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

        // If groupnum found, get the offset
        if( groupnum >= 0 && groupnum < (oveccount/3) )
        {
            // ovec is in pairs of starting and ending offsets.  (ie. 
            // ovec[0] is start of whole match, ovec[1] is end of whole 
            // match.  ovec[2] is start of 1st capture group
            groupnum *= 2; 

            ret = (long long)ovector[ groupnum ] + (ex_subject - subject) ; 
            ++ret ; // mysql strings indexes ala substr start at 1 not 0
            *is_null = 0 ;
        }
    }

    free( ovector ) ;

    if( !ptr->constant_pattern ) 
        pcre_free( re ) ;

    return ret ;
}

/** 
 * @fn void preg_position_deinit(UDF_INIT *initid)
 *
 *      @brief cleanup after PREG_POSITION 
 *
 *      @param initid - pointer to struct to be cleaned.
 */
void preg_position_deinit(UDF_INIT *initid)
{
    pregDeInit(initid);
}
