
UDF_DOCS_HTML=index.html doxygen.css doxygen.png

UDF_DOCS_HTML_D=$(UDF_DOCS_HTML:%=html/%)


EXTRA_DIST = $(UDF_DOCS_HTML_D) INSTALL.windows

DOXYGEN=doxygen

all:

doc: 
	$(DOXYGEN) Doxyfile
	cat html/index.html | awk '/<body>/,/<hr/ { print }'  |grep -v '<body>'  | sed 's/<hr.*//' | sed 's/<h2>/<h1>/g' | sed 's/<\/h2>/<\/h1>/g' | sed 's/index.html/index.php/g'   > lib_mysqludf_preg.frag 
	cat index.php.in | sed 's/<DISTFILE>/$(PACKAGE_TARNAME)-$(PACKAGE_VERSION).tar.gz/g'| sed 's/<__VERSION__>/$(PACKAGE_VERSION)/g' | sed 's/<WINBINFILE>/$(PACKAGE)-$(PACKAGE_VERSION)-bin.zip/g' >index.php

$(UDF_DOCS_HTML_D):doc

clean-local:
	rm -rf html *.frag index.php

