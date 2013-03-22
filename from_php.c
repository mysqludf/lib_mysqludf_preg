/*
 * This code was copied from the php project (ext/pcre) and modified.
 * Here's the notices from that file:
 */




/* PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language.

                       Written by Philip Hazel
           Copyright (c) 1997-2006 University of Cambridge

-----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
      
    * Neither the name of the University of Cambridge nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
*/

/** @file from_php.c
 *
 *  @brief Functions taken from php and modified.
 */

/*
 * This file was modified my Rich Waters <raw@goodhumans.com> for use
 * with a mysql udf.
 */

#include "pcre.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include "ghmysql.h"
//#include "preg.h"

#include "ghfcns.h"

#undef HAVE_SETLOCALE   // R.A.W


// R.A.W.
// Some older versions of PCRE do not have a separate recursion
// limit.  In these cases just use the match limit
//
#ifndef PCRE_EXTRA_MATCH_LIMIT_RECURSION
#define PCRE_EXTRA_MATCH_LIMIT_RECURSION PCRE_EXTRA_MATCH_LIMIT
#define match_limit_recursion match_limit
#endif



 /** @fn pcre *compileRegex( char *regex,int regex_len,char *msg, int msglen ) 
  * 
  * @brief Compile a pcre regular expression
  * 
  *    @param regex - a STRING pcre regular expression to be compiled
  *    @param regex_len - the length of the passed in regex
  *    @param msg - a buffer to store potential error an info messages
  *    @param msglen  - size of the message buffer
  *
  * @returns
  *    a point to te compiled regular expression information - on success
  *    NULL - on error - some errors will copy a more detailed info into msg
  *
  * @details
  *
  * @note
  *    This function requires a NULL terminated string as the regex parameter.
  * This is a limitation to the original php function from which this was
  * copied.
  *    
  */

//PHPAPI pcre_cache_entry* pcre_get_compiled_regex_cache(char *regex, int regex_len TSRMLS_DC)
pcre *compileRegex( char *regex , int regex_len , char *msg , int msglen ) 
{
	pcre				*re = NULL;
	pcre_extra			*extra;
	int					 coptions = 0;
	int					 soptions = 0;
	const char			*error;
	int					 erroffset;
	char				 delimiter;
	char				 start_delimiter;
	char				 end_delimiter;
	char				*p, *pp;
	char				*pattern;
	int					 do_study = 0;
	//int					 poptions = 0;
	unsigned const char *tables = NULL;
    char buf[ 1024 ] ;

#if HAVE_SETLOCALE
	char				*locale = setlocale(LC_CTYPE, NULL);
#endif

    //R.A.W.
	//pcre_cache_entry	*pce;
	//pcre_cache_entry	 new_entry;
    if( msglen )
    {
        *msg = '\0';
    }

	/* Try to lookup the cached regex entry, and if successful, just pass
	   back the compiled pattern, otherwise go on and compile it. */
	//regex_len = strlen(regex);

//R.A.W.
#if 0  

	if (zend_hash_find(&PCRE_G(pcre_cache), regex, regex_len+1, (void **)&pce) == SUCCESS) {
		/*
		 * We use a quick pcre_info() check to see whether cache is corrupted, and if it
		 * is, we flush it and compile the pattern from scratch.
		 */
		if (pcre_info(pce->re, NULL, NULL) == PCRE_ERROR_BADMAGIC) {
			zend_hash_clean(&PCRE_G(pcre_cache));
		} else {
#if HAVE_SETLOCALE
			if (!strcmp(pce->locale, locale)) {
#endif
				return pce;
#if HAVE_SETLOCALE
			}
#endif
		}
	}
#endif
	p = regex;
	
	/* Parse through the leading whitespace, and display a warning if we
	   get to the end without encountering a delimiter. */
	while (isspace((int)*(unsigned char *)p)) p++;
	if (*p == 0) {

        // R.A.W.
		//php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty regular expression");
        strncpy(msg, "Empty regular expression" , msglen );

		return NULL;
	}
	
	/* Get the delimiter and display a warning if it is alphanumeric
	   or a backslash. */
	delimiter = *p++;
	if (isalnum((int)*(unsigned char *)&delimiter) || delimiter == '\\') {
		//php_error_docref(NULL TSRMLS_CC,E_WARNING, "Delimiter must not be alphanumeric or backslash");
		strncpy( msg, "Delimiter must not be alphanumeric or backslash",msglen);
		return NULL;
	}

	start_delimiter = delimiter;
	if ((pp = strchr("([{< )]}> )]}>", delimiter)))
		delimiter = pp[5];
	end_delimiter = delimiter;

	if (start_delimiter == end_delimiter) {
		/* We need to iterate through the pattern, searching for the ending delimiter,
		   but skipping the backslashed delimiters.  If the ending delimiter is not
		   found, display a warning. */
		pp = p;
		while (*pp != 0) {
			if (*pp == '\\' && pp[1] != 0) pp++;
			else if (*pp == delimiter)
				break;
			pp++;
		}
		if (*pp == 0) {
			//php_error_docref(NULL TSRMLS_CC,E_WARNING, "No ending delimiter '%c' found", delimiter);

            //R.A.W.
            strncpy( msg, "No ending delimiter found" ,msglen ) ;
			return NULL;
		}
	} else {
		/* We iterate through the pattern, searching for the matching ending
		 * delimiter. For each matching starting delimiter, we increment nesting
		 * level, and decrement it for each matching ending delimiter. If we
		 * reach the end of the pattern without matching, display a warning.
		 */
		int brackets = 1; 	/* brackets nesting level */
		pp = p;
		while (*pp != 0) {
			if (*pp == '\\' && pp[1] != 0) pp++;
			else if (*pp == end_delimiter && --brackets <= 0)
				break;
			else if (*pp == start_delimiter)
				brackets++;
			pp++;
		}
		if (*pp == 0) {
			//php_error_docref(NULL TSRMLS_CC,E_WARNING, "No ending matching delimiter '%c' found", end_delimiter);
			strncpy( msg,"No ending matching delimiter found",msglen ) ;
			return NULL;
		}
	}
	
	/* Make a copy of the actual pattern. */
    // R.A.W.
	//pattern = strndup(p, pp-p);
    // osx compile is complaining about strndup and since I have te
    // other function anyway and since I'll someday rewrite this fn, just
    // call that other function now :>)
    pattern = ghstrndup( p,pp-p) ;


	/* Move on to the options */
	pp++;

	/* Parse through the options, setting appropriate flags.  Display
	   a warning if we encounter an unknown modifier. */	
	while (*pp != 0) {
		switch (*pp++) {
			/* Perl compatible options */
			case 'i':	coptions |= PCRE_CASELESS;		break;
			case 'm':	coptions |= PCRE_MULTILINE;		break;
			case 's':	coptions |= PCRE_DOTALL;		break;
			case 'x':	coptions |= PCRE_EXTENDED;		break;
			
			/* PCRE specific options */
			case 'A':	coptions |= PCRE_ANCHORED;		break;
			case 'D':	coptions |= PCRE_DOLLAR_ENDONLY;break;
			case 'S':	do_study  = 1;					break;
			case 'U':	coptions |= PCRE_UNGREEDY;		break;
			case 'X':	coptions |= PCRE_EXTRA;			break;
			case 'u':	coptions |= PCRE_UTF8;			break;

                // R.A.W.
			/* Custom preg options */
                //case 'e':	poptions |= PREG_REPLACE_EVAL;	break;
			
			case ' ':
			case '\n':
				break;

			default:
				//php_error_docref(NULL TSRMLS_CC,E_WARNING, "Unknown modifier '%c'", pp[-1]);
                strncpy( msg,"Unknown modifier",msglen ) ;
				free(pattern);
				return NULL;
		}
	}

    //R.A.W.
    tables = NULL ;
#if 0 
#if HAVE_SETLOCALE
	if (strcmp(locale, "C"))
		tables = pcre_maketables();
#endif
#endif

	/* Compile pattern and display a warning if compilation failed. */
	re = pcre_compile(pattern,
					  coptions,
					  &error,
					  &erroffset,
					  tables);

	if (re == NULL) {
		//php_error_docref(NULL TSRMLS_CC,E_WARNING, "Compilation failed: %s at offset %d", error, erroffset);

        // R.A.W.
		sprintf(buf, "Compilation of /%s/ failed: %s at offset %d",pattern, error, erroffset);
		//sprintf(buf, "Compilation failed: %s at offset %d",error, erroffset);
        strncpy( msg, buf , msglen ) ;
		free(pattern);
        // R.A.W.
		//if (tables) {
			//pefree((void*)tables, 1);
        //}
		return NULL;
	}

	/* If study option was specified, study the pattern and
	   store the result in extra for passing to pcre_exec. */
	if (do_study) {
		extra = pcre_study(re, soptions, &error);
		if (extra) {
			extra->flags |= PCRE_EXTRA_MATCH_LIMIT | PCRE_EXTRA_MATCH_LIMIT_RECURSION;
		}
		if (error != NULL) {
			strncpy( msg, "Error while studying pattern",msglen);
		}
	} else {
		extra = NULL;
	}

	free(pattern);


//R.A.W.
#if 0 
	/*
	 * If we reached cache limit, clean out the items from the head of the list;
	 * these are supposedly the oldest ones (but not necessarily the least used
	 * ones).
	 */
	if (zend_hash_num_elements(&PCRE_G(pcre_cache)) == PCRE_CACHE_SIZE) {
		int num_clean = PCRE_CACHE_SIZE / 8;
		zend_hash_apply_with_argument(&PCRE_G(pcre_cache), pcre_clean_cache, &num_clean TSRMLS_CC);
	}

	/* Store the compiled pattern and extra info in the cache. */
	new_entry.re = re;
	new_entry.extra = extra;
	new_entry.preg_options = poptions;
	new_entry.compile_options = coptions;
#if HAVE_SETLOCALE
	new_entry.locale = pestrdup(locale, 1);
	new_entry.tables = tables;
#endif
	zend_hash_update(&PCRE_G(pcre_cache), regex, regex_len+1, (void *)&new_entry,
						sizeof(pcre_cache_entry), (void**)&pce);

#endif

    //	return pce;
    //R.A.W.
    return re ;
}


/* {{{ preg_get_backref
 */
static int preg_get_backref(char **str, int *backref)
{
	register char in_brace = 0;
	register char *walk = *str;

	if (walk[1] == 0)
		return 0;

	if (*walk == '$' && walk[1] == '{') {
		in_brace = 1;
		walk++;
	}
	walk++;

	if (*walk >= '0' && *walk <= '9') {
		*backref = *walk - '0';
		walk++;
	} else
		return 0;
	
	if (*walk && *walk >= '0' && *walk <= '9') {
		*backref = *backref * 10 + *walk - '0';
		walk++;
	}

	if (in_brace) {
		if (*walk == 0 || *walk != '}')
			return 0;
		else
			walk++;
	}
	
	*str = walk;
	return 1;	
}



/* {{{ php_pcre_replace_impl() */
//char *php_pcre_replace_impl(pcre_cache_entry *pce, char *subject, int subject_len, zval *replace_val, 
char *pregReplace(pcre *re , pcre_extra *extra , 
                     char *subject, int subject_len, char *replace, 
                     int replace_len , 
                     int is_callable_replace, int *result_len, int limit, 
                     int *replace_count, char *msg , int msglen )
{
    // R.A.W.
	//pcre_extra		*extra = pce->extra;/* Holds results of studying */
	pcre_extra		 extra_data;		/* Used locally for exec options */
	int				 exoptions = 0;		/* Execution options */
	int				 count = 0;			/* Count of matched subpatterns */
	int				*offsets;			/* Array of subpattern offsets */
	int				 size_offsets;		/* Size of the offsets array */
	int				 new_len;			/* Length of needed storage */
	int				 alloc_len;			/* Actual allocated length */
	//int				 eval_result_len=0;	/* Length of the eval'ed or
    //function-returned string */
	int				 match_len;			/* Length of the current match */
	int				 backref;			/* Backreference number */
	int				 eval;				/* If the replacement string should be eval'ed */
	int				 start_offset;		/* Where the new search starts */
	int				 g_notempty=0;		/* If the match should not be empty */
	//int				 replace_len=0;		/* Length of replacement string */
	char			*result,			/* Result of replacement */
    //*replace=NULL,		/* Replacement string */  R.A.W.
					*new_buf,			/* Temporary buffer for re-allocation */
					*walkbuf,			/* Location of current replacement in the result */
					*walk,				/* Used to walk the replacement string */
					*match,				/* The current match */
					*piece,				/* The current piece of subject */
					*replace_end=NULL,	/* End of replacement string */
					 walk_last;			/* Last walked character */
    //*eval_result,		/* Result of eval or custom function */
	int				 rc;

	if (extra == NULL) {
        // R.A.W.
        memset( &extra_data , 0 , sizeof( extra_data ) ) ;
		extra_data.flags = PCRE_EXTRA_MATCH_LIMIT | PCRE_EXTRA_MATCH_LIMIT_RECURSION;
		extra = &extra_data;
	}

    // R.A.W.  -- from php.ini-reccommended
    // These might be too big. Crashes can occur with this large recursion_limit
	extra->match_limit = 100000;
	extra->match_limit_recursion = 100000;
	//extra->match_limit = PCRE_G(backtrack_limit);
	//extra->match_limit_recursion = PCRE_G(recursion_limit);


    // R.A.W.  - Eval'ing or callable replace. There might be something 
    // similar to do for MySQL, but not for now.
    //
    eval = 0 ;
    is_callable_replace = 0 ;
	//eval = pce->preg_options & PREG_REPLACE_EVAL;

	if (is_callable_replace) {
		if (eval) {
            //R.A.W.
			//php_error_docref(NULL TSRMLS_CC, E_WARNING, "Modifier /e cannot be used with replacement callback");
			return NULL;
		}
	} else {
        // R.A.W.
		//replace = Z_STRVAL_P(replace_val);
		//replace_len = Z_STRLEN_P(replace_val);
		replace_end = replace + replace_len;
	}

	/* Calculate the size of the offsets array, and allocate memory for it. */
	//rc = pcre_fullinfo(pce->re, extra, PCRE_INFO_CAPTURECOUNT, &size_offsets);
	rc = pcre_fullinfo(re, extra, PCRE_INFO_CAPTURECOUNT, &size_offsets);
	if (rc < 0) {
		//php_error_docref(NULL TSRMLS_CC, E_WARNING, "Internal pcre_fullinfo() error %d", rc);
        strncpy( msg , "Internal pcre_fullinfo() error" , msglen ) ;
		return NULL;
	}
	size_offsets = (size_offsets + 1) * 3;
	//offsets = (int *)safe_emalloc(size_offsets, sizeof(int), 0);
    // R.A.W.
	offsets = (int *)calloc(size_offsets, sizeof(int));
    if( !offsets ) {
        strncpy( msg , "Out of memory for offsets" , msglen ) ;
		return NULL;
    }
                            
	
	//alloc_len = 2 * subject_len + 1;
	alloc_len = 2 * subject_len + 1;
	//result = safe_emalloc(alloc_len, sizeof(char), 0);
    // R.A.W.
	result = calloc(alloc_len, sizeof(char));
    if( !result )
    {
        strncpy( msg , "Out of memory for result" , msglen ) ;
        free( offsets ) ;
		return NULL;
    }
    

	/* Initialize */
	match = NULL;
	*result_len = 0;
	start_offset = 0;
	
    // R.A.W.  - not sure what this does
    //PCRE_G(error_code) = PHP_PCRE_NO_ERROR;
	
	while (1) {
		/* Execute the regular expression. */
        /*
          R.A.W.
		count = pcre_exec(pce->re, extra, subject, subject_len, start_offset,
						  exoptions|g_notempty, offsets, size_offsets);
        */
		count = pcre_exec(re, extra, subject, subject_len, start_offset,
						  exoptions|g_notempty, offsets, size_offsets);
		
		/* Check for too many substrings condition. */
		if (count == 0) {
            //R.A.W.
			//php_error_docref(NULL TSRMLS_CC,E_NOTICE, "Matched, but too many substrings");
			strncpy(msg , "Matched, but too many substrings",msglen);
			count = size_offsets/3;
		}

		piece = subject + start_offset;

		if (count > 0 && (limit == -1 || limit > 0)) {
			if (replace_count) {
				++*replace_count;
			}
			/* Set the match location in subject */
			match = subject + offsets[0];

			new_len = *result_len + offsets[0] - start_offset; /* part before the match */

            // R.A.W.  - No callable replace or eval (see above).  Don't let
            // functions be linked in.
#if 0           
            
			/* If evaluating, do it and add the return string's length */
			if (eval) {
				eval_result_len = preg_do_eval(replace, replace_len, subject,
											   offsets, count, &eval_result TSRMLS_CC);
				new_len += eval_result_len;
			} else if (is_callable_replace) {
				/* Use custom function to get replacement string and its length. */
				eval_result_len = preg_do_repl_func(replace_val, subject, offsets,
													count, &eval_result TSRMLS_CC);
				new_len += eval_result_len;
			} else 
#endif

            { /* do regular substitution */
				walk = replace;
				walk_last = 0;
				while (walk < replace_end) {
					if ('\\' == *walk || '$' == *walk) {
						if (walk_last == '\\') {
							walk++;
							walk_last = 0;
							continue;
						}
						if (preg_get_backref(&walk, &backref)) {
							if (backref < count)
								new_len += offsets[(backref<<1)+1] - offsets[backref<<1];
							continue;
						}
					}
					new_len++;
					walk++;
					walk_last = walk[-1];
				}
			}

			if (new_len + 1 > alloc_len) {
				alloc_len = 1 + alloc_len + 2 * new_len;

                // R.A.W. 
				//new_buf = emalloc(alloc_len);
                new_buf = malloc( alloc_len ) ;
                if( !new_buf )
                {
                    strncpy( msg , "Out of memory for new_buf " , msglen ) ;
                    free( offsets ) ;
                    free( result ) ;
                    return NULL;
                }
                    
				memcpy(new_buf, result, *result_len);
				//efree(result);
                free( result ) ;
				result = new_buf;
			}
			/* copy the part of the string before the match */
			memcpy(&result[*result_len], piece, match-piece);
			*result_len += match-piece;

			/* copy replacement and backrefs */
			walkbuf = result + *result_len;
			
            // R.A.W.  Don't link in the eval stuff
#if 0            
			/* If evaluating or using custom function, copy result to the buffer
			 * and clean up. */
			if (eval || is_callable_replace) {
				memcpy(walkbuf, eval_result, eval_result_len);
				*result_len += eval_result_len;
				STR_FREE(eval_result);
			} else 
#endif
{ /* do regular backreference copying */
				walk = replace;
				walk_last = 0;
				while (walk < replace_end) {
					if ('\\' == *walk || '$' == *walk) {
						if (walk_last == '\\') {
							*(walkbuf-1) = *walk++;
							walk_last = 0;
							continue;
						}
						if (preg_get_backref(&walk, &backref)) {
							if (backref < count) {
								match_len = offsets[(backref<<1)+1] - offsets[backref<<1];
								memcpy(walkbuf, subject + offsets[backref<<1], match_len);
								walkbuf += match_len;
							}
							continue;
						}
					}
					*walkbuf++ = *walk++;
					walk_last = walk[-1];
				}
				*walkbuf = '\0';
				/* increment the result length by how much we've added to the string */
				*result_len += walkbuf - (result + *result_len);
			}

			if (limit != -1)
				limit--;

		} else if (count == PCRE_ERROR_NOMATCH || limit == 0) {
			/* If we previously set PCRE_NOTEMPTY after a null match,
			   this is not necessarily the end. We need to advance
			   the start offset, and continue. Fudge the offset values
			   to achieve this, unless we're already at the end of the string. */
			if (g_notempty != 0 && start_offset < subject_len) {
				offsets[0] = start_offset;
				offsets[1] = start_offset + 1;
				memcpy(&result[*result_len], piece, 1);
				(*result_len)++;
			} else {
				new_len = *result_len + subject_len - start_offset;
				if (new_len + 1 > alloc_len) {
					alloc_len = new_len + 1; /* now we know exactly how long it is */
					//new_buf = safe_emalloc(alloc_len, sizeof(char), 0);
                    new_buf = calloc(alloc_len, sizeof(char));
                    if( !new_buf )
                    {
                        strncpy( msg , "Out of memory for new_buf" , msglen ) ;
                        free( offsets ) ;
                        free( result ) ;
                        return NULL;
                    }
                        
					memcpy(new_buf, result, *result_len);
					free(result);
					//efree(result);
					result = new_buf;
				}
				/* stick that last bit of string on our output */
				memcpy(&result[*result_len], piece, subject_len - start_offset);
				*result_len += subject_len - start_offset;
				result[*result_len] = '\0';
				break;
			}
		} else {
			//pcre_handle_exec_error(count TSRMLS_CC);
            // R.A.W.
			//pcre_handle_exec_error(count);
            free( result ) ;
			//efree(result);
			result = NULL;
			break;
		}
			

		/* If we have matched an empty string, mimic what Perl's /g options does.
		   This turns out to be rather cunning. First we set PCRE_NOTEMPTY and try
		   the match again at the same point. If this fails (picked up above) we
		   advance to the next character. */
		g_notempty = (offsets[1] == offsets[0])? PCRE_NOTEMPTY | PCRE_ANCHORED : 0;
		/* Advance to the next piece. */
		start_offset = offsets[1];
	}
	
    free( offsets ) ;
	//efree(offsets);

	return result;
}
/* }}} */




//#endif /* HAVE_DLOPEN */
