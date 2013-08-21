
lib_mysqludf_preg  - PREG functions for mysql
===============================================
lib_mysqludf_preg is a library of mysql UDFs (user-defined-functions)
that provide access to the PCRE (perl compatible-regular-expressions)
library for pattern matching. The PCRE library is a set of functions
that implement regular expression pattern matching using the same
syntax and semantics as Perl 5. This syntax can often handle more
complex expressions and capturing than standard regular expression
implementations. For more information about PCRE, please see:
http://www.pcre.org/.

lib_mysqludf_preg currently provides the following functions:  

PREG_RLIKE( pattern , subject ) - test whether subject matches pattern,
which is a perl compatible regular expression.   

PREG_CAPTURE(pattern, subject [, capture-group] [, occurence] ) - capture a 
named or numeric parenthesized subexpression from a pcre pattern.  Capture
from a specific match of the regex or the first match is occurence 
not specified.  

PREG_CHECK( pattern ) - test whether the given pattern is a valid perl 
compatible regular expression.   

PREG_POSITION(pattern, subject [, capture-group] [, occurence] ) - get the 
position in subject of a named or numeric parenthesized subexpression 
from a pcre pattern.  Capture from a specific match of the regex or 
the first match if occurence not specified.  

PREG_REPLACE(pattern, replacement, subject [ ,limit ] ) - perform
a regular expression search and replace using a PCRE pattern.

LIB_MYSQLUDF_PREG_INFO() - obtain information about the currently installed
version of lib_mysqludf_preg. 



Some examples:
-------------
- SELECT captured, description FROM
    (SELECT PREG_CAPTURE( '/(new)\\\\s+([a-zA-Z]*)(.*)/i' , description, 2  ) as captured FROM state WHERE description LIKE 'new%') as t1
  WHERE captured IS NOT NULL;

- SELECT position, description FROM
    (SELECT PREG_POSITION( '/(new)\\\\s+([a-zA-Z]*)(.*)/i' , description, 2  ) as position FROM state WHERE description LIKE 'new%') as t1
  WHERE position IS NOT NULL;

- SELECT * from products WHERE PREG_RLIKE( '/hemp/i' , products.title )

- SELECT CONVERT( PREG_REPLACE( '/fox/i' , 'dog' , 'The brown fox' ) USING UTF8) as replaced;

Please see test/lib_udfmysql_preg.test and test/lib_udfmysql_preg.result for 
more examples.



More Documentation
------------------
Please see doc/html/index.html for more detailed documentation 
of the SQL functions.



Installation
============
Please see the file INSTALL or (doc/INSTALL.windows) 
for the full installation instructions.

The short instructions are:

./configure; make  install; make installdb ; make test



Getting lib_mysqludf_preg
===========================
The best place to get the library is from the github repository at: https://github.com/mysqludf/lib_mysqludf_preg. Please help with the testing by using the code on the testing branch. You can also download tarred source archives from http://www.goodhumans.com/Misc/lib_mysqludf/.



Reporting Bugs & Feedback
=========================
Please send information regarding bugs and any other feedback to:
raw@goodhumans.net



Known Issues & Caveats
======================
- Version 1.2 respects mysqld stack limitations. This should reduce crashing, but you might need to set the thread_stack mysqld variable in order to accommodate some recursion intensive patterns.
- Version 1.1 changes the way NULLs are handled. To restore the legacy NULL handling, use configure --enable-legacy-nulls
- pcre_study should be used  (but isn't) for constant patterns;
- there is no localization or locale support
- some program locations that should be set in autoconf are not
- It would be nice if there were a persistent cache of compiled regexes
- It would also be nice if there were a peresistent cache of regex matches.
This would allow for a more efficient way of retrieving multiple matches than
repeated called with different 'occurence' arguments. 



When & When not to use these UDF's
==================================
These UDF's are useful in the following circumstances:
    - you already have pcre regex's that need to be applied in mysql
    - you need to use a more complex regex than is supported by RLIKE
    - you need to capture portions of a regex from mysql
    - you are looking for a slight performance improvement over RLIKE

For optimal performance, these (or any) UDF's should not be used:
    - as a replacement for a prefixed LIKE or RLIKE  (ie.  LIKE 'foo%')
    - as a replacement for MATCH .. AGAINST ... IN BOOLEAN MODE.
    - on large databases without other query constraints.  Often the PCRE (or
any function or UDF) can be used in conjunction with a fulltext index 
constraint in order to reduce the number of rows the need to be operated on.  
(ie. SELECT PREG_CAPTURE ... WHERE MATCH AGAINST)



Motivations & Explanations
==========================
-The 'occurence' argument to PREG_CAPTURE and PREG_POSITION was originally
thought not to be needed, since the {} notation in the regex itself
could be used.  For instance, /.{2}(.)/ could be used to get the
3rd character of a string.  This was found not to work for a 
large 'occurence'.  (ie.  /.{65536}(.)/)



Copyright and copying:
======================
Copyright (C) 2007-2013 Rich Waters <raw@goodhumans.net>.  

This file and most contents of this package are licensed under The
MIT License. Please see the COPYING file in this directory for details.


Acknowledgements
================
The amazing PCRE was written by Philip Hazel, and this project uses 
some of his code from the php preg extension in from_php.c

The documentation for this project is generated using
doxygen which is available at: http://www.stack.nl/~dimitri/doxygen/

I referenced the following projects while trying to put together this 
library:

http://udf-regexp.php-baustelle.de/trac/  - a UDF that implements Oracle-like
REGEX functions - written by Hartmut Holzgraefe

http://www.xcdsql.org/MySQL/UDF/lib_mysqludf_xql/lib_mysqludf_xql.html - A UDF 
that enables creation of XML - written by Arthur Daniels

Much of the documentation was generated using Doxygen, at
http://www.stack.nl/~dimitri/doxygen/ , which was written
by Dimitri van Heesch.

lib_mysqludf_preg bug fixes & improvements have been contributed by Dan Kozlowski, Serkan Serttop, Travers Carter, employees of the NY State Senate, and some other folks :>). If that includes you and you'd like to be listed here, please send me an email. 

