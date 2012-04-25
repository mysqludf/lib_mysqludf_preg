
USE mysql;

# deprecated functions
DROP FUNCTION IF EXISTS ghpcre_capture ;
DROP FUNCTION IF EXISTS ghpcre_rlike ;
DROP FUNCTION IF EXISTS preg_offset ;

# current function
DROP FUNCTION IF EXISTS lib_mysqludf_preg_info ;
DROP FUNCTION IF EXISTS preg_capture ;
DROP FUNCTION IF EXISTS preg_check ;
DROP FUNCTION IF EXISTS preg_position ;
DROP FUNCTION IF EXISTS preg_rlike ;
DROP FUNCTION IF EXISTS preg_replace ;