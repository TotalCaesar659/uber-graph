ACLOCAL_AMFLAGS = -I build ${ACLOCAL_FLAGS}

SUBDIRS = .

DISTCHECK_CONFIGURE_FLAGS =
MAINTAINERCLEANFILES =
EXTRA_DIST =
CLEANFILES =
noinst_PROGRAMS =

lib_LTLIBRARIES = src/libuber.la

uberdir = $(pkgincludedir)/src
uber_HEADERS = 				\
	src/uber.h				\
	src/uber-graph.h		\
	src/uber-frame-source.h	\
	src/uber-heat-map.h		\
	src/uber-line-graph.h	\
	src/uber-label.h		\
	src/uber-range.h		\
	src/uber-scale.h		\
	src/uber-scatter.h		\
	src/uber-timeout-interval.h		\
	src/uber-window.h		\
	src/g-ring.h

src_libuber_la_SOURCES = 	\
	src/uber-graph.c		\
	src/uber-frame-source.c	\
	src/uber-heat-map.c		\
	src/uber-line-graph.c	\
	src/uber-label.c		\
	src/uber-range.c		\
	src/uber-scale.c		\
	src/uber-scatter.c		\
	src/uber-timeout-interval.c		\
	src/uber-window.c		\
	src/g-ring.c

src_libuber_la_CPPFLAGS = 	\
	-I$(top_srcdir)			\
	-I$(top_srcdir)/src		\
	$(AM_CPPFLAGS)

src_libuber_la_CFLAGS = 		\
	$(DEPS_CFLAGS)			\
	$(AM_CFLAGS)

src_libuber_la_LIBADD = 		\
	$(DEPS_LIBS)			\
	$(AM_LIBADD)

# Introspection
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --warn-all
INTROSPECTION_COMPILER_ARGS =
DISTCHECK_CONFIGURE_FLAGS += --enable-introspection
EXTRA_DIST += introspection.m4

if HAVE_INTROSPECTION
src/Uber-1.0.gir: src/libuber.la
src_Uber_1_0_gir_INCLUDES = Gtk-3.0
src_Uber_1_0_gir_CFLAGS = $(DEPS_CFLAGS) $(src_libuber_la_CPPFLAGS)
src_Uber_1_0_gir_LIBS = src/libuber.la
src_Uber_1_0_gir_FILES = $(src_libuber_la_SOURCES) $(uber_HEADERS)
src_Uber_1_0_gir_NAMESPACE = Uber
src_Uber_1_0_gir_EXPORT_PACKAGES = libuber
src_Uber_1_0_gir_SCANNERFLAGS = 	\
	--nsversion=1.0 		\
	--symbol-prefix=uber 	\
	--c-include="uber/uber.h"

INTROSPECTION_GIRS += src/Uber-1.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)

DISTCHECK_CONFIGURE_FLAGS += --enable-introspection
EXTRA_DIST += introspection.m4
endif

noinst_PROGRAMS += demos/c/simple

demos_c_simple_SOURCES = 	\
	demos/c/simple.c

demos_c_simple_CPPFLAGS = 	\
	-I$(top_srcdir)/ 	\
	-I$(top_srcdir)/src \
	$(AM_CPPFLAGS)

demos_c_simple_CFLAGS = 	\
	$(DEPS_CFLAGS) 		\
	$(AM_CFLAGS)

demos_c_simple_LDADD = 	\
	$(top_builddir)/src/libuber.la \
	$(DEPS_LIBS) 		\
	$(AM_LDADD)


