INCLUDES = $(GLIB_CFLAGS)

dist-hook:
	rm -f $(distdir)/servicepoke.py

lib_LTLIBRARIES = libservice-poke.la

pkginclude_HEADERS = service-poke.h

libservice_poke_la_SOURCES = service-poke.c service-poke.h
libservice_poke_la_LDFLAGS = $(GLIB_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = service-poke.pc

BUILT_SOURCES = service-poke-python.c service-poke-php.c php_servicepoke.h servicepoke.py servicepoke.php
CLEANFILES = $(BUILT_SOURCES)
SWIG_SOURCES = service-poke.i

pyfiles = servicepoke.py
python_PYTHON = $(pyfiles)
pyexec_LTLIBRARIES = _servicepoke.la
_servicepoke_la_SOURCES = $(SWIG_SOURCES)
nodist__servicepoke_la_SOURCES = service-poke-python.c
_servicepoke_la_CPPFLAGS = $(PYTHON_CPPFLAGS) -I$(srcdir)
_servicepoke_la_LDFLAGS = -module
_servicepoke_la_LIBADD = libservice-poke.la

phpextdir = `php-config --extension-dir`
phpext_LTLIBRARIES = _servicepokephp.la
_servicepokephp_la_SOURCES = $(SWIG_SOURCES)
nodist__servicepokephp_la_SOURCES = service-poke-php.c php_servicepoke.h
_servicepokephp_la_CPPFLAGS = `php-config --includes` -I$(srcdir)
_servicepokephp_la_LDFLAGS = -module
_servicepokephp_la_LIBADD = libservice-poke.la

pkgphpconfigdir = $(sysconfdir)/php.d
pkgphpconfig_DATA = php-service-poke.ini

EXTRA_DIST = $(pkgphpconfig_DATA)

install-data-hook:
	mv $(DESTDIR)$(pkgphpconfigdir)/php-service-poke.ini $(DESTDIR)$(pkgphpconfigdir)/service-poke.ini

if DO_DIST_ONLY
service-poke-python.c:
	touch service-poke-python.c
	touch servicepoke.py

service-poke-php.c:
	touch service-poke-php.c
	touch php_servicepoke.h
	touch servicepoke.php
else
service-poke-python.c : $(SWIG_SOURCES)
	$(SWIG) -python -I$(srcdir) -o $@ $<

service-poke-php.c : $(SWIG_SOURCES)
	$(SWIG) -php -I$(srcdir) -o $@ $<
endif

php_servicepoke.h : service-poke-php.c
servicepoke.php : service-poke-php.c
servicepoke.py : service-poke-python.c

