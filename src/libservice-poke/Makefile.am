INCLUDES = $(GLIB_CFLAGS)

lib_LTLIBRARIES = libservice-poke.la

pkginclude_HEADERS = service-poke.h

libservice_poke_la_SOURCES = service-poke.c service-poke.h
libservice_poke_la_LDFLAGS = $(GLIB_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = service-poke.pc

BUILT_SOURCES = service-poke-python.c service-poke-php.c php_servicepoke.h servicepoke.py servicepoke.php
CLEANFILES = $(BUILT_SOURCES)
SWIG_SOURCES = service-poke.i

python_PYTHON = servicepoke.py
pyexec_LTLIBRARIES = _servicepoke.la
_servicepoke_la_SOURCES = service-poke-python.c $(SWIG_SOURCES)
_servicepoke_la_CPPFLAGS = $(PYTHON_CPPFLAGS) -I$(srcdir)
_servicepoke_la_LDFLAGS = -module
_servicepoke_la_LIBADD = libservice-poke.la

phpextdir = `php-config --extension-dir`
phpext_LTLIBRARIES = _servicepokephp.la
_servicepokephp_la_SOURCES = service-poke-php.c php_servicepoke.h $(SWIG_SOURCES)
_servicepokephp_la_CPPFLAGS = `php-config --includes` -I$(srcdir)
_servicepokephp_la_LDFLAGS = -module
_servicepokephp_la_LIBADD = libservice-poke.la

pkgphpconfigdir = $(sysconfdir)/php.d
pkgphpconfig_DATA = php-service-poke.ini

EXTRA_DIST = $(pkgphpconfig_DATA)

install-data-hook:
	mv $(DESTDIR)$(pkgphpconfigdir)/php-service-poke.ini $(DESTDIR)$(pkgphpconfigdir)/service-poke.ini

service-poke-python.c : $(SWIG_SOURCES)
	$(SWIG) -python -I$(srcdir) -o $@ $<

service-poke-php.c : $(SWIG_SOURCES)
	$(SWIG) -php -I$(srcdir) -o $@ $<

php_servicepoke.h : service-poke-php.c
servicepoke.php: service-poke-php.c
servicepoke.py: service-poke-python.c

