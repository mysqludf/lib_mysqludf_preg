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

#ifndef PREG_H
#define PREG_H


/** @file preg.h
 *  
 * @brief headers for preg functions
 */


// Include the libpcre headers
#include <pcre.h>
#include "from_php.h"

/*
 * PCRE Structures:
 */
struct preg_s {
    pcre *re ;                  /* the compiled regex */
    int constant_pattern ;      /* is the pattern argument constant? */
    char *return_buffer ;       /* alloc'd memory for returning strings */
    unsigned long return_buffer_size ;
};

/*
 * lib_mysqludf_preg utility functions:
 */

// from_php.c
//pcre *compileRegex( char *regex , int regex_len , char *msg , int msglen ) ;

/*
char *pregReplace(pcre *re , pcre_extra *extra , 
                  char *subject, int subject_len, char *replace, 
                  int replace_len , 
                  int is_callable_replace, int *result_len, int limit, 
                  int *replace_count, char *msg , int msglen );
*/
// preg.c
void destroyPtrInfo( struct preg_s *ghptr );
int initPtrInfo( struct preg_s *ghptr , UDF_ARGS *args,char*msg );
my_bool pregInit(UDF_INIT *initid, UDF_ARGS *args, char *message);
pcre *pregCompileRegexArg( UDF_ARGS *args , char *msg , int msglen ) ;
int pregCopyToReturnBuffer( struct preg_s *ptr , char *s  , int l );
void pregDeInit(UDF_INIT *initid) ;

int *pregCreateOffsetsVector( pcre *re , pcre_extra *extra , int *count ,
                              char *msg , int msglen );

char *pregMoveToReturnValues( UDF_INIT *initid ,
                              unsigned long *length , 
                              char *is_null , char *error ,
                              char *s , int s_len  )  ;
int pregGetGroupNum( pcre *re ,  UDF_ARGS *args , int argnum );

char *pregSkipToOccurence( pcre *re , char *subject , int subject_len , 
                           int *ovector  , int oveccount , int occurence, 
                           int *rc);
void pregSetLimits(pcre_extra *extra);
const char *pregExecErrorString(int errno);



#endif
