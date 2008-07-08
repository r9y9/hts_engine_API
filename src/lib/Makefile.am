## Process this file with automake to produce Makefile.in
INCLUDES=-I$(top_srcdir)/include
lib_LIBRARIES = libHTSEngine.a
libHTSEngine_a_SOURCES = HTS_audio.c  HTS_engine.c  HTS_hidden.h  HTS_misc.c  HTS_pstream.c  HTS_sstream.c  HTS_model.c  HTS_vocoder.c HTS_gstream.c HTS_label.c

# Files to remove by 'make distclean'
DISTCLEANFILES = *.log *.out *~

# Autotools-generated files to remove by 'make maintainer-clean'.
MAINTAINERCLEANFILES = Makefile.in

