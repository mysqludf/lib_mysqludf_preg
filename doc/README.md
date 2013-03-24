
************ lliibb__mmyyssqqlluuddff__pprreegg ************
********** IInnttrroodduuccttiioonn **********
lib_mysqludf_preg is a library of mysql UDFs (user-defined-functions) that
provide access to the PCRE (perl compatible-regular-expressions) library for
pattern matching. The PCRE library is a set of functions that implement regular
expression pattern matching using the same syntax and semantics as Perl 5. This
syntax can often handle more complex expressions and capturing than standard
regular expression implementations. For more information about PCRE, please
see: _h_t_t_p_:_/_/_w_w_w_._p_c_r_e_._o_r_g_/

lib_mysqludf_preg is a useful performance optimization for those applications
that are already performing these regular expression matches in a high level
language (ie. PHP) on the client side. It is also helpful when there is a need
to capture a parenthesized subexpression from a regular expression, or simply
as a slight performance boost over the builtin RLIKE/REGEXP functions.
********** IInnssttaallllaattiioonn **********
lib_mysqludf_preg is distributed as a source package. (Binaries are available
for the Windows version.) The instructions below provide some information about
how to configure and compile the library. Please consult the INSTALL file
included with the source package for more details. If installing from source
for Windows please consult the doc/INSTALL.win file.
******** FFrroomm SSoouurrccee ********
****** PPrreerreeqquuiissiitteess ******
These UDFs require that the libpcre headers and library are installed. For
debian/ubuntu type systems, installing libpcre3-dev should be sufficient. (aapptt--
ggeett iinnssttaallll lliibbppccrree33--ddeevv).
****** CCoommppiillaattiioonn ******
Most users should be able to simply type: ..//ccoonnffiigguurree ;; mmaakkee iinnssttaallll
If mysql is an unusual place, you might need to add --with-mysql=<mysql
directory>/bin/mysql_config.
Similarly, if licpcre is in an unusual place, --with-pcre can be added.
Example (for osx using fink): ./configure --with-pcre=/sw --with-mysql=/sw/bin/
mysql_config
Also, version 1.1 changes the way NULLs are handled. To restore the legacy NULL
handling, add --enable-legacy-nulls to the configure line
Provided the library has been installled into a directory that mysql server
already has in its LD_LIBRARY_PATH, installation of the functions should be as
easy as typing: mmaakkee iinnssttaallllddbb. Any problems encountered are likely related to
the server's environment and the installation directory. If no problems are
encountered, type mmaakkee tteesstt to perform some basic tests.
******** WWiinnddoowwss BBiinnaarriieess ********
There is a zip file, downloadable from this site, that contains the necessary
dll files to run this UDF on windows. One of these dll files is the UDF,
itself, and the other required dll file is the PCRE library. These files both
need to be installed in directories where the MySQL server can find them. For
the 5.0 series of mysql server, this should be the bin directory directly under
the Mysql\ Server installation. For the 5.1 MySQL servers this is the directory
as specified by the 'plugin_dir' MySQL variable. After copying the files and
restarting the server, the 3rd file in the archive can be used to create the
functions.
For more information, please consult the doc/INSTALL.windows file in the source
package.

********** FFuunnccttiioonnss **********
lib_mysqludf_preg provides the following functions that interface with the PCRE
library.
    * _p_r_e_g___c_a_p_t_u_r_e capture a parenthesized subexpression from a PCRE pattern
    * _p_r_e_g___c_h_e_c_k check if a string is a valid perl-compatible regular
      expression
    * _p_r_e_g___p_o_s_i_t_i_o_n get position of the of a regular expression capture group
      in a string
    * _p_r_e_g___r_e_p_l_a_c_e perform regular expression search & replace using PCRE.
    * _p_r_e_g___r_l_i_k_e test if a string matches a perl-compatible regular expression
    * _l_i_b___m_y_s_q_l_u_d_f___p_r_e_g___i_n_f_o get information about the installed
      lib_mysqludf_preg library

********** pprreegg__ccaappttuurree **********
capture a parenthesized subexpression from a PCRE pattern
  FFuunnccttiioonn IInnssttaallllaattiioonn
      CREATE FUNCTION preg_capture RETURNS STRING SONAME
      'lib_mysqludf_preg.so';
  SSyynnooppssiiss
      PREG_CAPTURE( pattern , subject [, group] [, occurence] )
  PPaarraammeetteerrss::
       ppaatttteerrnn    - is a string that is a perl compatible regular expression as
                  documented at: _h_t_t_p_:_/_/_u_s_._p_h_p_._n_e_t_/_m_a_n_u_a_l_/_e_n_/_r_e_f_._p_c_r_e_._p_h_p This
                  expression passed to this function should have delimiters and
                  can contain the standard perl modifiers after the ending
                  delimiter.
       ssuubbjjeecctt    -is the data to perform the match & capture on
       ggrroouupp      - is the capture group that should be returned. This can be a
                  numeric capture group or a named capture group. Numeric
                  groups should be passed in as integers while named groups
                  should be strings. If not speficied, this defaults to 0,
                  which will capture the entire matching regular expression.
       ooccccuurreennccee  - which match of the regex to perform capture on. This is
                  useful for subjects that have multiple matches of the
                  pattern. If not speficied, this defaults to 1, which will
                  capture the requested group, from the first matching
                  occurence of the pattern.
  RReettuurrnnss::
      - string that was captured - if there was a match and the desired capture
      group is valid
      - string that is the entire portion of subject which matches the pattern
      - if 0 is passed in as the group and pattern matches subject
      - NULL - if pattern does not match the subject or group is not a valid
      capture group for the given pattern and subject.
preg_capture is a udf that captures parenthesized sub-expressions from a pcre
pattern.
  EExxaammpplleess::
SELECT PREG_CAPTURE('/(.*?)(fox)/' , 'the quick brown fox' ,2 );
YYiieellddss::
+----------------------------------------------------------+
| PREG_CAPTURE('/(.*?)(fox)/' , 'the quick brown fox' ,2 ) |
+----------------------------------------------------------+
| fox                                                      |
+----------------------------------------------------------+
SELECT PREG_CAPTURE( '/"([^"]+)"/' , 'the "quick" brown fox "jumped" over the
"lazy" dog' , 1,2 );
+------------------------------------------------------------------------------
--------------+
| PREG_CAPTURE( '/"([^"]+)"/' , 'the "quick" brown fox "jumped" over the "lazy"
dog' , 1,2 ) |
+------------------------------------------------------------------------------
--------------+
| jumped
|
+------------------------------------------------------------------------------
--------------+
SELECT PREG_CAPTURE( '/b[^\s]+/' , 'the quick brown fox jumped over' )
YYiieellddss::
+------------------------------------------------------------------+
| PREG_CAPTURE( '/b[^\\s]+/' , 'the quick brown fox jumped over' ) |
+------------------------------------------------------------------+
| brown                                                            |
+------------------------------------------------------------------+
  NNoottee::
      Remember to add a backslash to escape patterns that use \ notation

********** pprreegg__cchheecckk **********
Test if a perl-compatible regular expression is valid
  FFuunnccttiioonn IInnssttaallllaattiioonn
      CREATE FUNCTION preg_check RETURNS INTEGER SONAME 'lib_mysqludf_preg.so';
  SSyynnooppssiiss
      PREG_CHECK( pattern )
  PPaarraammeetteerrss::
       ppaatttteerrnn  - is a string that might be a perl compatible regular
                expression as documented at: _h_t_t_p_:_/_/_u_s_._p_h_p_._n_e_t_/_m_a_n_u_a_l_/_e_n_/
                _r_e_f_._p_c_r_e_._p_h_p
  RReettuurrnnss::
      1 - the pcre is valid
      0 - the pcre is NULL, empty, or a bad regex
preg_check is a udf that tests if whether or not the given perl compatible
regular expression is valid. This is a useful companion to the other functions
in the lib_mysqludf_pref library in that those functions all return errors (and
stop processing) when empty, NULL, or incorrect regular expressions are passed
in to them. If pcre patterns are stored in rows of a database and it is not
confirmed that the patterns are valid, PCRE_CHECK is useful either as a filter
used in conjunction with those calls or as a separate query to help cleanup the
database before using those other functions.
  EExxaammpplleess::
SELECT PREG_CHECK('/The quick brown fox/i' );
YYiieellddss::
   +---------------------------------------------------------------+
   | PREG_CHECK('/The quick brown fox/i' )                         |
   +---------------------------------------------------------------+
   |                                                             1 |
   +---------------------------------------------------------------+
SELECT * from patterns WHERE PREG_CHECK( pattern );
Yields: all of the rows containing valid pcre's.

********** pprreegg__ppoossiittiioonn **********
get position of the of a regular expression capture group in a string
  FFuunnccttiioonn IInnssttaallllaattiioonn
      CREATE FUNCTION preg_position RETURNS INTEGER SONAME
      'lib_mysqludf_preg.so';
  SSyynnooppssiiss
      PREG_POSITION( pattern , subject [, group] [, occurence] )
  PPaarraammeetteerrss::
       ppaatttteerrnn    - is a string that is a perl compatible regular expression as
                  documented at: _h_t_t_p_:_/_/_u_s_._p_h_p_._n_e_t_/_m_a_n_u_a_l_/_e_n_/_r_e_f_._p_c_r_e_._p_h_p This
                  expression passed to this function should have delimiters and
                  can contain the standard perl modifiers after the ending
                  delimiter.
       ssuubbjjeecctt    -is the data to perform the match & position capture on
       ggrroouupp      - the capture group whose position that should be returned.
                  This can be a numeric capture group or a named capture group.
                  Numeric groups should be passed in as integers, while named
                  groups should be strings. 0 should be used to request to
                  position of the entire matching expression. This parameter
                  defaults to 0.
       ooccccuurreennccee  - which match of the regex to perform capture on. This is
                  useful for subjects that have multiple matches of the
                  pattern. This parameter defaults to 1.
  RReettuurrnnss::
      - integer position of the string that was captured - if there was a match
      and the desired capture group and occurence is valid
      - NULL if pattern does not match the subject or group is not a valid
      capture group or the occurence is larger than the number of matches for
      the given pattern and subject.
preg_position is a udf that captures the position of matching regular
expression from a pcre pattern. It can be useful when strings need to be split
by a pcre pattern or if the location of the pattern within the string is needed
for some other reason.
  EExxaammpplleess::
SELECT PREG_POSITION('/(.*?)(fox)/' , 'the quick brown fox' ,2 );
YYiieellddss::
+-----------------------------------------------------------+
| PREG_POSITION('/(.*?)(fox)/' , 'the quick brown fox' ,2 ) |
+-----------------------------------------------------------+
|                                                        17 |
+-----------------------------------------------------------+
SELECT PREG_POSITION('/"[^"]+"/' , '"quick","brown","fox" "jumped"',0,4)
YYiieellddss::
+-------------------------------------------------------------------+
| PREG_POSITION('/"[^"]+"/' , '"quick","brown","fox" "jumped"',0,4) |
+-------------------------------------------------------------------+
|                                                                23 |
+-------------------------------------------------------------------+
  NNoottee::
      Remember to add a backslash to escape patterns that use \ notation. The
      returned position starts at 1 for the first character, as is standard for
      MySQL string functions.

********** pprreegg__rreeppllaaccee **********
performs regular expression search & replace using PCRE.
  FFuunnccttiioonn IInnssttaallllaattiioonn
      CREATE FUNCTION preg_replace RETURNS STRING SONAME
      'lib_mysqludf_preg.so';
  SSyynnooppssiiss
      PREG_REPLACE( pattern , replacement , subject [ , limit ] )
  PPaarraammeetteerrss::
       ppaatttteerrnn      - is a string that is a perl compatible regular expression
                    as documented at: _h_t_t_p_:_/_/_u_s_._p_h_p_._n_e_t_/_m_a_n_u_a_l_/_e_n_/_r_e_f_._p_c_r_e_._p_h_p
                    This expression passed to this function should have
                    delimiters and can contain the standard perl modifiers
                    after the ending delimiter.
       rreeppllaacceemmeenntt  - is the string to use as the replacement. This string may
                    contain capture group references such as \1. You can also
                    use $1 for these in a similar fashion as in PHP.
       ssuubbjjeecctt      -is the data to perform the match & replace on
       lliimmiitt        - optional number that is the maximum replacements to
                    perform. Use -1 (or leave empty) for no limit.
  RReettuurrnnss::
      - string - 'subject' with the instances of pattern replaced
      - string - the same as passed in if there were no matches
preg_replace is a udf that performs a regular expression search and replace on
a given piece of data using a PCRE as the replacement pattern. If limit is not
speficied or is -1, preg_replace works on all of the ocurrences of the pattern
in the subject data. Otherwise, preg_replace will only replace the first
<limit> occurences.
  EExxaammpplleess::
SELECT PREG_REPLACE('/(.*?)(fox)/' , '$1dog' , 'the quick brown fox' );
YYiieellddss::
+-----------------------------------------------------------------+
| PREG_REPLACE('/(.*?)(fox)/' , '$1dog' , 'the quick brown fox' ) |
+-----------------------------------------------------------------+
| the quick brown dog                                             |
+-----------------------------------------------------------------+
SELECT PREG_REPLACE('/\s\s/+', ' ' , products.title FROM products;
Yields: The product names with all of the extra whitespace removed
  NNoottee::
      Remember to add a backslash to escape patterns that use \ notation. Also,
      using $ notation makes things a little clearer when using backreferences
      in the replacement.

********** pprreegg__rrlliikkee **********
Test if a string matches a perl-compatible regular expression
  FFuunnccttiioonn IInnssttaallllaattiioonn
      CREATE FUNCTION preg_rlike RETURNS INTEGER SONAME 'lib_mysqludf_preg.so';
  SSyynnooppssiiss
      PREG_RLIKE( pattern , subject )
  PPaarraammeetteerrss::
       ppaatttteerrnn  - is a string that is a perl compatible regular expression as
                documented at: _h_t_t_p_:_/_/_u_s_._p_h_p_._n_e_t_/_m_a_n_u_a_l_/_e_n_/_r_e_f_._p_c_r_e_._p_h_p This
                expression passed to this function should have delimiters and
                can contain the standard perl modifiers after the ending
                delimiter.
       ssuubbjjeecctt  - is the data to perform the test on.
  RReettuurrnnss::
      1 - a match was found
      0 - no match
preg_rlike is a udf that tests if whether or not the given perl compatible
regular expression matches the given data.
  EExxaammpplleess::
SELECT PREG_RLIKE('/The quick brown fox/i' , 'the quick brown fox' );
YYiieellddss::
   +---------------------------------------------------------------+
   | PREG_RLIKE('/The quick brown fox/i' , 'the quick brown fox' ) |
   +---------------------------------------------------------------+
   |                                                             1 |
   +---------------------------------------------------------------+
SELECT * from products WHERE PREG_RLIKE( '/organic/i' , products.title )
Yields: all of the products with 'organic' in their titles

********** lliibb__mmyyssqqlluuddff__pprreegg__iinnffoo **********
Return version information for lib_mysqludf_preg package
  FFuunnccttiioonn IInnssttaallllaattiioonn
      CREATE FUNCTION lib_mysqludf_preg_info RETURNS STRING SONAME
      'lib_mysqludf_preg.so' ;
  SSyynnooppssiiss
      LIB_MYSQLUDF_PREG_INFO()
  RReettuurrnnss::
      string - version information for the lib_mysqludf_preg package
  EExxaammpplleess::
      SELECT LIB_MYSQLUDF_PREG_INFO();
YYiieellddss::
+--------------------------+
| LIB_MYSQLUDF_PREG_INFO() |
+--------------------------+
| lib_mysqludf_preg 0.6.1  |
+--------------------------+
===============================================================================
     Generated on Sat Mar 23 18:24:07 2013 for lib_mysqludf_preg by 
     _[_d_o_x_y_g_e_n_] 1.5.9
