
AC_DEFUN([AX_PTHREAD_NP], [


                # from tcl.m4  http://svn.apache.org/repos/asf/trafficserver/attic/traffic/trunk/build/tcl.m4
	# Does the pthread-implementation provide
	# 'pthread_attr_setstacksize' ?

	ac_saved_libs=$LIBS
	LIBS="$LIBS $PTHREAD_LIBS"
	AC_CHECK_FUNCS(pthread_attr_setstacksize)
	AC_CHECK_FUNC(pthread_attr_get_np,tcl_ok=yes,tcl_ok=no)
	if test $tcl_ok = yes ; then
	    AC_DEFINE(HAVE_PTHREAD_ATTR_GET_NP, 1,
		[Do we want a BSD-like thread-attribute interface?])
	    AC_CACHE_CHECK([for pthread_attr_get_np declaration],
		tcl_cv_grep_pthread_attr_get_np, [
		AC_EGREP_HEADER(pthread_attr_get_np, pthread.h,
		    tcl_cv_grep_pthread_attr_get_np=present,
		    tcl_cv_grep_pthread_attr_get_np=missing)])
	    if test $tcl_cv_grep_pthread_attr_get_np = missing ; then
		AC_DEFINE(ATTRGETNP_NOT_DECLARED, 1,
		    [Is pthread_attr_get_np() declared in <pthread.h>?])
	    fi
	else
	    AC_CHECK_FUNC(pthread_getattr_np,tcl_ok=yes,tcl_ok=no)
	    if test $tcl_ok = yes ; then
		AC_DEFINE(HAVE_PTHREAD_GETATTR_NP, 1,
		    [Do we want a Linux-like thread-attribute interface?])
		AC_CACHE_CHECK([for pthread_getattr_np declaration],
		    tcl_cv_grep_pthread_getattr_np, [
		    AC_EGREP_HEADER(pthread_getattr_np, pthread.h,
			tcl_cv_grep_pthread_getattr_np=present,
			tcl_cv_grep_pthread_getattr_np=missing)])
		if test $tcl_cv_grep_pthread_getattr_np = missing ; then
		    AC_DEFINE(GETATTRNP_NOT_DECLARED, 1,
			[Is pthread_getattr_np declared in <pthread.h>?])
		fi
	    fi
	fi
	if test $tcl_ok = no; then
	    # Darwin thread stacksize API
	    AC_CHECK_FUNCS(pthread_get_stacksize_np)
	fi
	LIBS=$ac_saved_libs
] )