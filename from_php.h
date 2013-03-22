/*
 *  from_php.h
 *  eMailGanizer
 *
 *  Created by Rich Waters on 9/3/10.
 *  Copyright 2010 GoodHumans. All rights reserved.
 *
 */

char *pregReplace(pcre *re , pcre_extra *extra , 
                  const char *subject, int subject_len, const char *replace, 
                  int replace_len , 
                  int is_callable_replace, int *result_len, int limit, 
                  int *replace_count, char *msg , int msglen );

pcre *compileRegex( const char *regex , int regex_len , char *msg , int msglen ) ;
