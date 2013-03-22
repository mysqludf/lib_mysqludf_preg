/*
 *  ghfcns.c
 *  eMailGanizer
 *
 *  Created by Rich Waters on 9/3/10.
 *  Copyright 2010 GoodHumans. All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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


