dnl **************************************************************************
dnl Versioning Macros
dnl **************************************************************************
m4_define([uber_major_version], [0])
m4_define([uber_minor_version], [1])
m4_define([uber_micro_version], [10])
m4_define([uber_version],
          [uber_major_version.uber_minor_version.uber_micro_version])
m4_define([uber_binary_age], [m4_eval(100 * uber_minor_version +
           uber_micro_version)])
m4_define([lt_current], [m4_eval(100 * uber_minor_version +
           uber_micro_version - uber_interface_age)])
m4_define([lt_revision], [uber_interface_age])
m4_define([lt_age], [m4_eval(uber_binary_age - uber_interface_age)])


dnl **************************************************************************
dnl Initialize Automake
dnl **************************************************************************
AC_PREREQ(2.65)
AC_INIT([uber],[0.1.0])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 dist-bzip2 no-dist-gzip foreign no-define subdir-objects])
AM_MAINTAINER_MODE([enable])

AC_PROG_CC
AC_HEADER_STDC
AM_PATH_GLIB_2_0

LT_PREREQ([2.2.6])
LT_INIT([])

AM_SILENT_RULES([yes])


dnl **************************************************************************
dnl Versioning Definitions
dnl **************************************************************************
UBER_MAJOR_VERSION=uber_major_version
UBER_MINOR_VERSION=uber_minor_version
UBER_MICRO_VERSION=uber_micro_version
UBER_API_VERSION=1.0
UBER_VERSION=uber_version
AC_SUBST(UBER_MAJOR_VERSION)
AC_SUBST(UBER_MINOR_VERSION)
AC_SUBST(UBER_MICRO_VERSION)
AC_SUBST(UBER_API_VERSION)
AC_SUBST(UBER_VERSION)


dnl **************************************************************************
dnl Check for Required Modules
dnl **************************************************************************
PKG_CHECK_MODULES(GTK, [gtk+-3.0])


dnl **************************************************************************
dnl Unit Tests
dnl **************************************************************************
AC_PATH_PROG([GTESTER], [gtester])
AC_PATH_PROG([GTESTER_REPORT], [gtester-report])
AM_CONDITIONAL(ENABLE_GLIB_TEST, test "x$enable_glibtest" = "xyes")

dnl **************************************************************************
dnl Enable extra debugging options
dnl **************************************************************************
m4_define([debug_default],
	  [m4_if(m4_eval(uber_minor_version % 2), [1],
	  [yes], [minimum])])
AC_ARG_ENABLE([debug],
	      [AS_HELP_STRING([--enable-debug=@<:@no/minimum/yes@:>@],
	      		      [turn on debugging @<:@default=debug_default@:>@])],
	      		      [],
	      		      [enable_debug=debug_default])
AS_CASE([$enable_debug],
	[yes], [
		CFLAGS="$CFLAGS -g"
		CFLAGS="$CFLAGS -Wall"
		CFLAGS="$CFLAGS -Werror"
		CFLAGS="$CFLAGS -Wold-style-definition"
		CFLAGS="$CFLAGS -Wdeclaration-after-statement"
		CFLAGS="$CFLAGS -Wredundant-decls"
		CFLAGS="$CFLAGS -Wmissing-noreturn"
		CFLAGS="$CFLAGS -Wcast-align"
		CFLAGS="$CFLAGS -Wwrite-strings"
		CFLAGS="$CFLAGS -Winline"
		CFLAGS="$CFLAGS -Wformat-nonliteral"
		CFLAGS="$CFLAGS -Wformat-security"
		CFLAGS="$CFLAGS -Wswitch-enum"
		CFLAGS="$CFLAGS -Wswitch-default"
		CFLAGS="$CFLAGS -Winit-self"
		CFLAGS="$CFLAGS -Wmissing-include-dirs"
		CFLAGS="$CFLAGS -Wundef"
		CFLAGS="$CFLAGS -Waggregate-return"
		CFLAGS="$CFLAGS -Wmissing-format-attribute"
		CFLAGS="$CFLAGS -Wnested-externs"
		CFLAGS="$CFLAGS -Wshadow"
	],
	[minimum], [
		CFLAGS="$CFLAGS -DG_DISABLE_CAST_CHECKS"
	],
	[no], [
		CFLAGS="$CFLAGS -DG_DISABLE_ASSERT"
		CFLAGS="$CFLAGS -DG_DISABLE_CHECKS"
		CFLAGS="$CFLAGS -DG_DISABLE_CAST_CHECKS"
	],
	[AC_MSG_ERROR([Unknown argument to --enable-debug])]
)
AC_SUBST(UBER_DEBUG_CFLAGS)


dnl **************************************************************************
dnl Miscellaneous
dnl **************************************************************************
GTK_DOC_CHECK([1.11])
GOBJECT_INTROSPECTION_CHECK([1.29])


AC_CONFIG_FILES([
	uber-1.0.pc
	Makefile
	doc/version.xml
	doc/Makefile
])
AC_OUTPUT


echo ""
echo "Uber $VERSION"
echo ""
echo "  Prefix.....................: ${prefix}"
echo "  Debug Level................: ${enable_debug}"
echo "  Compiler Flags.............: ${CFLAGS}"
echo "  Enable API Reference.......: ${enable_gtk_doc}"
echo "  Enable Test Suite..........: ${enable_glibtest}"
echo ""
