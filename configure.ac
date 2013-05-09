AC_INIT([service-poke],[1.0.2],[Kevin.Fox@pnl.gov])
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_SRCDIR([configure.ac])

AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE

AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL

AC_ARG_ENABLE([dist-only],
        AS_HELP_STRING([--enable-dist-only],[Don't check requirements. Only for building releases.]),
        [
                do_dist_only=yes;
		export GLIB_CFLAGS="-I.";
		export GLIB_LIBS="-L.";
	],do_dist_only=no)
AM_CONDITIONAL(DO_DIST_ONLY, test x"$do_dist_only" = "xyes")

if [[ x"$do_dist_only" != "xyes" ]]; then

AM_PATH_PYTHON

AX_PKG_SWIG(1.3.0, [], [ AC_MSG_ERROR([SWIG is required to build..]) ])
#AX_SWIG_MULTI_MODULE_SUPPORT
AX_SWIG_PYTHON

fi

PKG_CHECK_MODULES([GLIB],[glib-2.0])

docdir=${prefix}/share/doc/service-poke
AC_SUBST(docdir)

PACKAGE_NAME=AC_PACKAGE_NAME
AC_SUBST(PACKAGE_NAME)

AC_OUTPUT([
		Makefile
		src/Makefile
		src/server/Makefile
		src/libservice-poke/Makefile
		src/libservice-poke/service-poke.pc
		src/client/Makefile
		examples/Makefile
		man/Makefile
		service-poke.spec
])
