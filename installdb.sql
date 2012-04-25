
USE mysql;
CREATE FUNCTION lib_mysqludf_preg_info RETURNS STRING SONAME 'lib_mysqludf_preg.so';
CREATE FUNCTION preg_capture RETURNS STRING SONAME 'lib_mysqludf_preg.so';
CREATE FUNCTION preg_check RETURNS INTEGER SONAME 'lib_mysqludf_preg.so';
CREATE FUNCTION preg_replace RETURNS STRING SONAME 'lib_mysqludf_preg.so';
CREATE FUNCTION preg_rlike RETURNS INTEGER SONAME 'lib_mysqludf_preg.so';
CREATE FUNCTION preg_position RETURNS INTEGER SONAME 'lib_mysqludf_preg.so';


