#####
#
# SYNOPSIS
#
#   AX_MYSQL_BIN()
#
# DESCRIPTION
#
#   This macro defines location of MySQL client (mysql), mysqladmin and mysqltest
#
#   This macro calls:
#
#     AC_SUBST(MYSQL)
#     AC_SUBST(MYSQLADMIN)
#     AC_SUBST(MYSQLTEST)
#
#####

AC_DEFUN([AX_MYSQL_BIN], [
  AC_MSG_CHECKING(for mysqlbin)
  AC_ARG_WITH(mysqlbin,
    [[  --with-mysqlbin[=CMD]   command to run mysql.]],
    [withmysqlbin="$withval"],
    [withmysqlbin="yes"]
  )

  AC_ARG_WITH(mysqladmin,
    [[  --with-mysqladmin[=CMD] command to run mysqladmin.]],
    [withmysqladmin="$withval"],
    [withmysqladmin="yes"]
  )

  AC_ARG_WITH(mysqlinclude,
    [[  --with-mysqlinclude[=PATH] path to mysql header files.]],
    [withmysqlinclude="$withval"],
    [withmysqlinclude="yes"]
  )

  AC_ARG_WITH(mysqltest,
    [[  --with-mysqltest[=CMD]  command to run mysqltest.]],
    [withmysqltest="$withval"],
    [withmysqltest="yes"]
  )

  if test -z "$withmysqlbin" -o "$withmysqlbin" = "yes"; then
    for i in `dirname $(which mysql)`  /usr/bin /usr/local/bin ; do
      if test -f "$i/mysql" ; then
        MYSQL="$i/mysql"
      fi
    done
  elif test "$withmysqlbin" != "no"; then
    MYSQL="$withmysqlbin"
  fi

  if test -n "$MYSQL" ; then
    MYSQLBIN_PATH=`dirname "$MYSQL"`

    if test -f /etc/mysql/debian.cnf ; then
        MYSQL_ARGS="--defaults-file=/etc/mysql/debian.cnf"
    else
        ${MYSQL} </dev/null || {
            MYSQL_ARGS="-p"
        }
        MYSQL="${MYSQL} ${MYSQL_ARGS}"
    fi
  fi
  
  if test -z "$withmysqladmin" -o "$withmysqladmin" = "yes"; then
    for i in "$MYSQLBIN_PATH" /usr/bin /usr/local/bin ; do
      if test -n "$i" -a -f "$i/mysqladmin" ; then
        MYSQLADMIN="$i/mysqladmin $MYSQL_ARGS"
      fi
    done
  elif test "$withmysqladmin" != "no"; then
    MYSQLADMIN="$withmysqladmin"
  fi

  if test "$withmysqlinclude" != "no"; then
    MYSQL_HEADERS="-I $withmysqlinclude"
  fi

  if test -z "$withmysqltest" -o "$withmysqltest" = "yes"; then
    for i in "$MYSQLBIN_PATH" `dirname $(which mysqltest)` /usr/bin /usr/local/bin  ; do
      if test -n "$i" -a -f "$i/mysqltest" ; then
        MYSQLTEST="$i/mysqltest $MYSQL_ARGS"
      fi
    done
  elif test "$withmysqltest" != "no"; then
    MYSQLTEST="$withmysqltest"
  fi

  AC_SUBST(MYSQL)
  AC_SUBST(MYSQL_HEADERS)
  AC_SUBST(MYSQLADMIN)
  AC_SUBST(MYSQLTEST)
])