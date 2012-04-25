
dnl
dnl AM_PATH_PCRE(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
dnl based on cppunit.m4 found in wxWidgets distrubution
dnl
AC_DEFUN([AM_PATH_PCRE],
[

AC_ARG_WITH(pcre-prefix,[  --with-pcre-prefix=PFX   Prefix where PCRE is installed (optional)],
            pcre_config_prefix="$withval", pcre_config_prefix="")

AC_ARG_WITH(pcre,[  --with-pcre=PFX   Prefix where PCRE is installed (deprecated)],
            pcre_config_prefix="$withval", pcre_config_prefix="")

AC_ARG_WITH(pcre-exec-prefix,[  --with-pcre-exec-prefix=PFX  Exec prefix where PCRE is installed (optional)],
            pcre_config_exec_prefix="$withval", pcre_config_exec_prefix="")

  if test x$pcre_config_exec_prefix != x ; then
     pcre_config_args="$pcre_config_args --exec-prefix=$pcre_config_exec_prefix"
     if test x${PCRE_CONFIG+set} != xset ; then
        PCRE_CONFIG=$pcre_config_exec_prefix/bin/pcre-config
     fi
  fi
  if test x$pcre_config_prefix != x ; then
     pcre_config_args="$pcre_config_args --prefix=$pcre_config_prefix"
     if test x${PCRE_CONFIG+set} != xset ; then
        PCRE_CONFIG=$pcre_config_prefix/bin/pcre-config
     fi
  fi

  AC_PATH_PROG(PCRE_CONFIG, pcre-config, no)
  pcre_version_min=$1

  AC_MSG_CHECKING(for PCRE - version >= $pcre_version_min)
  no_pcre=""
  if test "$PCRE_CONFIG" = "no" ; then
    AC_MSG_RESULT(no)
    no_pcre=yes
  else
    PCRE_CFLAGS=`$PCRE_CONFIG --cflags`
    PCRE_LIBS=`$PCRE_CONFIG --libs`
    pcre_version=`$PCRE_CONFIG --version`

    pcre_major_version=`echo $pcre_version | cut -d. -f1`
    pcre_minor_version=`echo $pcre_version | cut -d. -f2`

    pcre_major_min=`echo $pcre_version_min | cut -d. -f1`
    pcre_minor_min=`echo $pcre_version_min | cut -d. -f2`

    pcre_version_proper=`expr \
        $pcre_major_version \> $pcre_major_min \| \
        $pcre_major_version \= $pcre_major_min \& \
        $pcre_minor_version \> $pcre_minor_min \| \
        $pcre_major_version \= $pcre_major_min \& \
        $pcre_minor_version \= $pcre_minor_min`

    if test "$pcre_version_proper" = "1" ; then
      AC_MSG_RESULT([$pcre_major_version.$pcre_minor_version])
    else
      AC_MSG_RESULT(no)
      no_pcre=yes
    fi
  fi

  if test "x$no_pcre" = x ; then
     ifelse([$2], , :, [$2])
  else
     PCRE_CFLAGS=""
     PCRE_LIBS=""
     ifelse([$3], , :, [$3])
  fi

  AC_SUBST(PCRE_CFLAGS)
  AC_SUBST(PCRE_LIBS)
])

