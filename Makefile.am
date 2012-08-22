SUBDIRS = 
if CAN_USE_GUILE
SUBDIRS += wrapper-gen
endif
SUBDIRS += src scheme #doc lib-config-src # man

#.PHONY: doc
#doc:
#	cd doc; make doc

BUILT_SOURCES = revision

revision:
	git log -n1 | head -n1 > $@

revisiondir = $(pkgdatadir)
revision_DATA = revision

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libcgl.pc
EXTRA_DIST = libcgl.pc revision

ACLOCAL_AMFLAGS = -I m4
