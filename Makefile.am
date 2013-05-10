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

MOCKDIST=fedora-18-x86_64
MOCK=/usr/bin/mock

mock: dist
	rm -rf packages/"$(MOCKDIST)" || true
	mkdir -p packages/"$(MOCKDIST)"/srpms
	mkdir -p packages/"$(MOCKDIST)"/bin
	LOCALREVISION="`git rev-list HEAD | wc -l`"; \
	if [ $$LOCALREVISION -lt ${OTHERREVISION} ]; then \
		LOCALREVISION=${OTHERREVISION}; \
	fi; \
	LOCALREVISION="git$$LOCALREVISION"; \
	echo revision="$$LOCALREVISION"; \
	$(MOCK) -r "$(MOCKDIST)" --buildsrpm --spec service-poke.spec --sources "`pwd`" --define "LOCALREVISION `echo $$LOCALREVISION`"; \
	mv "/var/lib/mock/$(MOCKDIST)/result/"*.src.rpm packages/"$(MOCKDIST)"/srpms/; \
	$(MOCK) -r "$(MOCKDIST)" --result "$(CURDIR)"/packages/"$(MOCKDIST)"/bin --define "LOCALREVISION `echo $$LOCALREVISION`" "$(CURDIR)"/packages/"$(MOCKDIST)"/srpms/*.src.rpm; \
if [ $$? -ne 0 ]; then \
	code=$$?; \
	cat "$(CURDIR)"/packages/"$(MOCKDIST)"/bin/build.log; \
	exit $$code; \
fi
