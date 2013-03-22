#####
#
# SYNOPSIS
#
#   AX_GHMYSQL
#
# DESCRIPTION
#
#
#####

AC_DEFUN([AX_GHMYSQL], [
  AC_ARG_ENABLE(
    [legacy-nulls],
    AC_HELP_STRING([--enable-legacy-nulls],[replace NULLs with empty strings]) ,   
    [GHMYSQL_CFLAGS="-DGH_1_0_NULL_HANDLING=1"],
    [GHMYSQL_CFLAGS=""]

  )
    AC_SUBST(GHMYSQL_CFLAGS)
])

