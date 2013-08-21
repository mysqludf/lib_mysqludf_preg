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


/** @file pregUtils.c
 *  
 * @brief Provides some functions for interfacing with pcre that are independent
 *        of mysql. 
 *
 * @notes Be sure to #define GH_PREG_NO_MYSQL if used
 *        without mysql
 */

#include <pthread.h>

#include "preg_utils.h"
#include "ghfcns.h"

#ifndef  GH_PREG_NO_MYSQL
#include "config.h"
#endif

/**
 * @fn void pregSetLimits( pcre_extra *extra  ) 
 *
 * @brief
 *     sets safe match/recursion limits in a pcre extra
 * that should be low enough to prevent stack overflow
 * crashes.
 *
 * @param extra - a pointer to the pcre_extra struct to set
 *
 * @details This function sets safe limits as determined by
 * calculating the currently available stack space for the
 * this thread, it will also set the flags required to make
 * pcre_exec honour the limits
 *
 */
void pregSetLimits(pcre_extra *extra)
{
    /*
     * A match_limit_recursion of 100000 is way too high for this context
     * MySQL's default thread stack size is 256K on 64bit (192K on 32bit)
     *  - https://dev.mysql.com/doc/refman/5.5/en/server-system-variables.html#sysvar_thread_stack
     * man pcrestack(3) suggests a rule of thumb of 500 bytes per recursion.  Assuming _no_ other
     * stack usage that suggests a maximum safe limit of ~512 (64-bit) or ~384 (32-bit)
     * but better - get the stack size & usage and base the limit on the *available* stack space
     * if deeper recusrsion is needed then users can increase MySQL's thread_stack variable to
     * raise the limit
     *                                                - Travers Carter <tcarter@noggin.com.au>
     */

    size_t          thread_stack_size=0;
    size_t          thread_stack_avail=0;
    size_t          pcre_frame_size=0;

#ifdef HAVE_PTHREAD
#ifdef HAVE_PTHREAD_GETATTR_NP

    pthread_attr_t  thread_attr;
    void *          thread_stack_addr=0;
    void *          thread_stack_cur = alloca(1); // End of the stack (hopefully....)
    void *          thread_stack_next = alloca(1); // For direction....
    
    // Find the current thread's stack information
    if (pthread_getattr_np(pthread_self(), &thread_attr) == 0) {
        if (pthread_attr_getstack(&thread_attr, &thread_stack_addr, &thread_stack_size) == 0) {
            // NB: thread_stack_addr is always the _lowest_ addres redgardless of the direction of
            //     of growth (man pthread_attr_getstackaddr(3))
            if (thread_stack_cur > thread_stack_next) {
                // Stack grows downwards....
                thread_stack_avail = thread_stack_cur - thread_stack_addr;
            } else {
                thread_stack_avail = thread_stack_size - (thread_stack_cur - thread_stack_addr);
            }
        }
        pthread_attr_destroy(&thread_attr);
    }

#elifdef HAVE_PTHREAD_GET_STACKSIZE_NP
    // TODO: Figure this out and get thread_stack_avail properly set for OSX.
#endif
    
#ifdef GH_PREG_NO_MYSQL
    if( !thread_stack_avail ) {
        // Cannot get information from current thread; Use default thread information, since compiling for use outside of mysqld
        
        pthread_attr_t  thread_attr;
        
        // Not possible to get values from current thread. Instead, extract default stacksize
        if( !pthread_attr_init( &thread_attr ) ) {
            if( !pthread_attr_getstacksize(&thread_attr , &thread_stack_size ) ) {
                thread_stack_avail = thread_stack_size*0.75;  // assume a current usage of 25% (_wild_ guess!)
            }
        }
    }
#endif
#endif
    

    if (thread_stack_avail == 0) 
    {
        // Cannot get stacksize using normal means
        // Use information from mysqld global variable, if compiling for mysql
        //
#ifndef GH_PREG_NO_MYSQL
        extern unsigned long my_thread_stack_size;
        thread_stack_size = my_thread_stack_size ;
        //ghlogprintf( "stack_size %d\n",thread_stack_size) ;
#endif

        if( !thread_stack_size ) {
            // Checks failed, assume the MySQL defaults (64/32 bit)
            // Shouldn't ever really get here, though.
            ghlogprintf( "Ignoring mysqld:thread_stack. Using mysql defaults.\n") ; 
#ifdef _LP64
            thread_stack_size  = 256*1024;
#else
            thread_stack_size  = 192*1024;
#endif
        }
        // And assume a current usage of 25% (_wild_ guess!)
        thread_stack_avail = thread_stack_size*0.75;
    }
    ghlogprintf( "thread_stack_avaid333333 %d\n",thread_stack_avail ) ;


    // PCRE >= 8.30 has a magic call preg_exec(NULL, NULL, NULL, -1, ....) to determine the stack requirements
    // (returned as a negative number), but errors are also negative (currently down to -25)
    if ((pcre_frame_size = pcre_exec(NULL, NULL, NULL, -1, 0, 0, NULL, 0)) < -50) {
        pcre_frame_size = -pcre_frame_size;
    } else { 
#ifdef _LP64
        // 500 bytes (from pcrestack(3)) is too low, at least on x86_64
        pcre_frame_size = 1000;
#else
        pcre_frame_size = 500;
#endif
    }

    // TODO: Fix (or justify?) the 100,000 magic number here (taken from "from_php.c" but pcre defaults to 10,000,000!
    extra->match_limit           = 100000;
    extra->match_limit_recursion = (thread_stack_avail-4096)/pcre_frame_size;

    // Force the limits to be honoured....
    extra->flags |= PCRE_EXTRA_MATCH_LIMIT | PCRE_EXTRA_MATCH_LIMIT_RECURSION;
}

static const char *_pregExecErrorString[] = {
    "NO_ERROR",
    "PCRE_ERROR_NOMATCH",
    "PCRE_ERROR_NULL",
    "PCRE_ERROR_BADOPTION",
    "PCRE_ERROR_BADMAGIC",
    "PCRE_ERROR_UNKNOWN_OPCODE",
    "PCRE_ERROR_NOMEMORY",
    "PCRE_ERROR_NOSUBSTRING",
    "PCRE_ERROR_MATCHLIMIT",
    "PCRE_ERROR_CALLOUT",
    "PCRE_ERROR_BADUTF8",
    "PCRE_ERROR_BADUTF8_OFFSET",
    "PCRE_ERROR_PARTIAL",
    "PCRE_ERROR_BADPARTIAL",
    "PCRE_ERROR_INTERNAL",
    "PCRE_ERROR_BADCOUNT",
    "PCRE_ERROR_DFA_UITEM",
    "PCRE_ERROR_DFA_UCOND",
    "PCRE_ERROR_DFA_UMLIMIT",
    "PCRE_ERROR_DFA_WSSIZE",
    "PCRE_ERROR_DFA_RECURSE",
    "PCRE_ERROR_RECURSIONLIMIT, try increasing mysqld:thread_stack",
    "PCRE_ERROR_NULLWSLIMIT",
    "PCRE_ERROR_BADNEWLINE",
    "PCRE_ERROR_BADOFFSET",
    "PCRE_ERROR_SHORTUTF8",
    "UNKOWN_ERROR",
};

/**
 * @fn const char *pregExecErrorString( int pcre_errno  )
 *
 * @brief
 *     returns a pointer to string containing the sylbolic
 * name of the pcre error specified by pcre_errno
 *
 * @param pcre_errno - an error number returned by pcre_exec()
 *
 * @details This function return a pointer to a NULL terminated
 * string containing the symbolic name of a give pcre error.
 * The returned strings are static and the caller should not
 * attempt to either manipulate or free them.
 *
 */
const char *pregExecErrorString(int pcre_errno) {
    if (pcre_errno >= 0) {
        return _pregExecErrorString[0];
    } else if (pcre_errno >= -25) {
        return _pregExecErrorString[-pcre_errno];
    } else {
        return _pregExecErrorString[26];
    }
}
