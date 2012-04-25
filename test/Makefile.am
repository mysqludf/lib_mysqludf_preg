

# Force test to be a target instead of shell-builtin
.PHONY : test

# clean up these files too during make clean
CLEANFILES=*.log *.reject

# Include these extensions in dist
EXTRA_DIST = *.test *.result *.sql 

#Get filenames from OS
PREG_TESTS=$(wildcard *.test)

MYSQLTEST_ARGS= --include=create_testdb.sql  --result-file=$*.result

############################

mysqltest:
	@if test -z "$(MYSQLTEST)" ; then echo "mysqltest not found"; exit 1; fi

%.result : %.test
	$(MYSQLTEST) $(MYSQLTEST_ARGS) --record < $*.test

%.run : %.test
	$(MYSQLTEST) $(MYSQLTEST_ARGS) < $*.test

results : mysqltest $(PREG_TESTS:%.test=%.result)

test: mysqltest $(PREG_TESTS:%.test=%.run)

