
AUTOMAKE_OPTIONS = foreign

.PHONY : test mrproper

lib_LTLIBRARIES = lib_mysqludf_preg.la

CFILES=	\
	preg.c \
	ghmysql.c \
	ghfcns.c \
	from_php.c \
	lib_mysqludf_preg_capture.c  \
	lib_mysqludf_preg_check.c \
	lib_mysqludf_preg_info.c \
	lib_mysqludf_preg_position.c \
	lib_mysqludf_preg_replace.c \
	lib_mysqludf_preg_rlike.c

HFILES = \
	preg.h \
	ghmysql.h \
	ghfcns.h \
	from_php.h

lib_mysqludf_preg_la_SOURCES = \
	$(CFILES) \
	$(HFILES)

DLL_OBJS=$(CFILES:%.c=.libs/lib_mysqludf_preg_la-%.o)

SUBDIRS=test doc
DIFFPROGRAM:=kompare -

lib_mysqludf_preg_la_CFLAGS = -DSTANDARD -DMYSQL_SERVER @MYSQL_CFLAGS@ @MYSQL_HEADERS@ @PCRE_CFLAGS@ @GHMYSQL_CFLAGS@ @PTHREAD_CFLAGS@
#lib_mysqludf_preg_la_LDFLAGS = -module -avoid-version -no-undefined @PCRE_LIBS@ @PTHREAD_LIBS@
lib_mysqludf_preg_la_LDFLAGS = -module -avoid-version @PCRE_LIBS@ @PTHREAD_LIBS@

EXTRA_DIST = *.sql

mrproper: clean maintainer-clean 
	for i in $(SUBDIRS) . ; do ( cd $$i &&	rm -rf config/config.guess config.h.* config/config.status configure config/missing config/config.sub config/ltmain.sh config/depcomp aclocal.m4 config/install-sh config.log installdb_win.sql config/compile Makefile.in *.tar.gz  *.loT config/mkinstalldirs *~); done  

mysqlbin:
	@if test -z "$(MYSQL)" ; then echo "mysql client app (mysql) not found"; exit 1; fi

installdb_win.sql:installdb.sql
		cat installdb.sql | sed 's/\.so/.dll/g' >installdb_win.sql

installdb: uninstalldb installdb_win.sql
	if test -f .libs/lib_mysqludf_preg.dll; then  \
		$(MYSQL) <./installdb_win.sql; \
	else \
		$(MYSQL) <./installdb.sql;\
	fi

uninstalldb: mysqlbin
	$(MYSQL) <./uninstalldb.sql

test: 
	cd test; make test

dist-hook:
	rm -rf `find $(distdir) -name .svn`
	rm -rf `find $(distdir) -name .git`
	rm -rf `find $(distdir) -name .DS_Store`

maintainer-dist:
	make dist
	git tag "$(PACKAGE)-$(VERSION)"

maintainer-diff:
	git diff "$(PACKAGE)-$(DIFFVERSION)" . | $(DIFFPROGRAM)

lib_mysqludf_preg.dll: $(DLL_OBJS)
	$(CC) -shared -o lib_mysqludf_preg.dll -Wl,"--strip-all" -Wl,"--export-all-symbols" $(DLL_OBJS) /opt/mingw/pcre/lib/libpcre.a
