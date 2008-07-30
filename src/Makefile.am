## Process this file with automake to produce Makefile.in
##

# Source subdirectories to be included during the build.
SUBDIRS = lib bin

# Header files to be included in the distribution.
include_HEADERS = include/HTS_engine.h

INCLUDES = -I$(top_srcdir)/include

# Additional files to be included in the distribution.
EXTRA_DIST = AUTHORS COPYING ChangeLog INSTALL NEWS README

# Files to remove by 'make distclean'
DISTCLEANFILES = *.log *.out *~ bin/*~ config/*~ include/*~ lib/*~

# Autotools-generated files to remove by 'make maintainer-clean'.
MAINTAINERCLEANFILES = aclocal.m4 configure Makefile.in        \
                       config/config.guess config/config.sub   \
                       config/depcomp config/install-sh        \
                       config/missing

