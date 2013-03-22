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



#endif
