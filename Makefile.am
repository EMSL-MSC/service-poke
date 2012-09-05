ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src examples man

dist-hook:
	$(top_srcdir)/svnver.sh $(top_srcdir) > $(distdir)/svnver.txt
	$(top_srcdir)/svnbranch.sh $(top_srcdir) > $(distdir)/svnbranch.txt

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(docdir);
	$(top_srcdir)/svnver.sh $(top_srcdir) > $(DESTDIR)$(docdir)/svnver.txt
	$(top_srcdir)/svnbranch.sh $(top_srcdir) > $(DESTDIR)$(docdir)/svnbranch.txt

EXTRA_DIST = service-poke.spec svnver.sh svnbranch.sh

dist_doc_DATA = README

DEV=1
REVISION=1

rpm: dist
	mkdir -p packages
	rm -rf packages
	mkdir -p packages/bin
	mkdir -p packages/src
	if [ "x${DEV}" != "x1" ]; \
	then \
		rpmbuild -ta @PACKAGE_NAME@-@VERSION@.tar.gz --define '_rpmdir '`pwd`'/packages/bin' --define '_srcrpmdir '`pwd`'/packages/src' --define "LOCALREVISION ${REVISION}"; \
	else \
		LOCALVER="$$(($$(test -f .buildver && cat .buildver || echo 0) + 1))"; \
		echo $$LOCALVER > .buildver; \
		rpmbuild -ta @PACKAGE_NAME@-@VERSION@.tar.gz --define '_rpmdir '`pwd`'/packages/bin' --define '_srcrpmdir '`pwd`'/packages/src' --define "LOCALREVISION dev$$LOCALVER"; \
	fi

rpms: rpm

