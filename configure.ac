AC_INIT([service-poke],[1.0.1],[Kevin.Fox@pnl.gov])
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AC_CONFIG_SRCDIR([configure.ac])

AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE

AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL
AM_PATH_PYTHON

AX_PKG_SWIG(1.3.0, [], [ AC_MSG_ERROR([SWIG is required to build..]) ])
#AX_SWIG_MULTI_MODULE_SUPPORT
AX_SWIG_PYTHON

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
