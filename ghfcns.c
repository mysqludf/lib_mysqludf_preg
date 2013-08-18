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

/** @file ghfcns.c
 *  
 * @brief Provides some utility functions that are independent of mysql. 
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "ghfcns.h"

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
char *ghstrndup( char *s , size_t l )
{
    char *colval = NULL ;
    
    colval = malloc( l + 1 ) ;
    if( !colval )
    {
        fprintf( stderr , "Not enough memory: %zu\n" , l ) ;
        return NULL ;
    }
    memcpy( colval , s , l ) ;
    colval[ l ] = '\0'; 
    
    return colval ;
}


/**
 * @fn void ghlogprintf( fmt, ... )
 *
 * @brief log an error message to stderr in MySQL format
 *
 * @param fmt - a format string as per sprintf()
 * @param ... - varargs as per sprintf()
 *
 * @return void
 *
 * @details - This function writes the specified error message
 * to stderr prefixed by the current time in the same format
 * used by MySQL
 */
void ghlogprintf(char *fmt, ...) {
    va_list vargs;
    char buf[18];
    time_t now;
    struct tm time_val;

    memset(&buf, 0, sizeof(buf));

    now = time(NULL);
    localtime_r(&now, &time_val); 
    strftime(&buf[0], sizeof(buf), "%y%m%d %H:%M:%S ", &time_val);
    fprintf(stderr, "%s", buf);

    va_start(vargs, fmt);
    vfprintf(stderr, fmt, vargs);
    va_end(vargs);
}
