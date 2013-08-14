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

// This file contains comments intended to help doxygen generate the
// main page of the documentation.


/** @mainpage lib_mysqludf_preg
 *
 * @section intro_sec Introduction
 *
 * lib_mysqludf_preg is a library of mysql UDFs
 * (user-defined-functions) that provide access to the PCRE (perl
 * compatible-regular-expressions) library for pattern matching. The
 * PCRE library is a set of functions that implement regular
 * expression pattern matching using the same syntax and semantics as
 * Perl 5. This syntax can often handle more complex expressions and
 * capturing than standard regular expression implementations. For
 * more information about PCRE, please see: http://www.pcre.org/
 *
 * @n lib_mysqludf_preg is a useful performance optimization for those
 * applications that are already performing these regular expression
 * matches in a high level language (ie. PHP) on the client side. It
 * is also helpful when there is a need to capture a parenthesized
 * subexpression from a regular expression, or simply as a slight
 * performance boost over the builtin RLIKE/REGEXP functions. 
 *
 * @section Installation
 *
 * lib_mysqludf_preg is distributed as a source package.  (Binaries are
 * available for the Windows version.) The instructions below provide some 
 * information about how to configure and compile the library. Please 
 * consult the INSTALL file included with the source package for 
 * more details.  If installing from source for Windows please consult 
 * the doc/INSTALL.win file.
 *
 * @subsection FromSource From Source
 *
 * @subsubsection Prerequisites
 *
 * These UDFs require that the libpcre headers and library are
 * installed. For debian/ubuntu type systems, installing libpcre3-dev
 * should be sufficient. (<em>apt-get install libpcre3-dev</em>).
 *
 * @subsubsection Compilation
 * 
 * Most users should be able to simply type: 
 * <em>./configure ; make install</em>
 *
 * If mysql is an unusual place, you might need to 
 * add --with-mysql=&lt;mysql directory&gt;/bin/mysql_config.
 *
 * Similarly, if licpcre is in an unusual place, --with-pcre can be added.
 * 
 * Example (for osx using fink): ./configure --with-pcre=/sw  --with-mysql=/sw/bin/mysql_config
 *
 * Also, version 1.1 changes the way NULLs are handled. To restore the legacy NULL handling, add
 * --enable-legacy-nulls to the configure line
 *
 * @subsubsection "Installing the functions"
 *
 * Provided the library has been installled into a directory that
 * mysql server already has in its LD_LIBRARY_PATH, installation of
 * the functions should be as easy as typing: <em>make
 * installdb</em>. Any problems encountered are likely related to the
 * server's environment and the installation directory. If no problems
 * are encountered, type 
 * <em>make test</em>
 * to perform some basic
 * tests.
 *
 *
 * @subsection WindowsBinaries Windows Binaries
 *
 * There is a zip file, downloadable from this site, that contains the 
 * necessary dll files to run this UDF on windows.  One of these dll files
 * is the UDF, itself, and the other required dll file is the PCRE library.
 * These files both need to be installed in directories where the MySQL server
 * can find them.  For the 5.0 series of mysql server, this should be the 
 * bin directory directly under the Mysql\ Server installation.  For the 
 * 5.1 MySQL servers this is the directory as specified by the 
 * 'plugin_dir' MySQL variable.  After copying the files and restarting
 * the server, the 3rd file in the archive can be used to create
 * the functions.  
 *
 * For more information, please consult the doc/INSTALL.windows file in 
 * the source package.
 *
 * @n
 * @section Functions
 *     lib_mysqludf_preg provides the following functions that interface with
 * the PCRE library.
 *
 * @li @ref PREG_CAPTURE_SECTION "preg_capture" 
 * capture a parenthesized subexpression from a PCRE pattern
 *
 * @li @ref PREG_CHECK_SECTION "preg_check" 
 * check if a string is a valid perl-compatible regular expression
 *
 * @li @ref PREG_POSITION_SECTION "preg_position"
 * get position of the of a regular expression capture group in a string

 * @li @ref PREG_REPLACE_SECTION "preg_replace"
 * perform regular expression search & replace using PCRE.
 *
 * @li @ref PREG_RLIKE_SECTION "preg_rlike"
 * test if a string matches a perl-compatible regular expression
 *
 * @li @ref LIB_MYSQLUDF_PREG_INFO_SECTION "lib_mysqludf_preg_info"
 * get information about the installed lib_mysqludf_preg library
 *
 *
 * @n
 * @section PREG_CAPTURE_SECTION preg_capture
 * @copydoc PREG_CAPTURE
 *
 * @n
 * @section PREG_CHECK_SECTION preg_check
 * @copydoc PREG_CHECK
 *
 * @n
 * @section PREG_POSITION_SECTION preg_position 
 * @copydoc PREG_POSITION
 *
 * @n
 * @section PREG_REPLACE_SECTION preg_replace 
 * @copydoc PREG_REPLACE
 *
 * @n
 * @section PREG_RLIKE_SECTION preg_rlike 
 * @copydoc PREG_RLIKE
 *
 * @n
 * @section LIB_MYSQLUDF_PREG_INFO_SECTION lib_mysqludf_preg_info 
 * @copydoc LIB_MYSQLUDF_PREG_INFO
 *
 */


