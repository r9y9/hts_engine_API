#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(hts_engine_API, 1.01, hts-engine-users@lists.sourceforge.net)
AC_CONFIG_AUX_DIR([config])
AC_COPYRIGHT(Copyright 2001-2008 Nagoya Institute of Technology)
AC_COPYRIGHT(Copyright 2001-2008 Tokyo Institute of Technology)
AM_INIT_AUTOMAKE(hts_engine_API, 1.01, nostdinc no-installinfo no-installman)

# Checks for C compiler
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_RANLIB
AN_MAKEVAR([AR], [AC_PROG_AR])
AN_PROGRAM([ar], [AC_PROG_AR])
AC_DEFUN([AC_PROG_AR], [AC_CHECK_TOOL(AR, ar, :)])
AC_PROG_AR


# Checks for libraries.
AC_CHECK_LIB([m], [log])


# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([sqrt strchr strrchr strstr])

       
# Checks for embedded device compile
AC_ARG_ENABLE(embedded, [  --enable-embedded   turn on compiling for embedded devices (default=no)],,enable_embedded=no)
AC_MSG_CHECKING(whether to enable compiling for embedded devices)
if test x$enable_embedded = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(HTS_EMBEDDED)
else
	AC_MSG_RESULT(no)
fi


# Checks for using festival
AC_ARG_ENABLE(festival, [  --enable-festival   use memory allocation/free functions of speech tools (default=no)],,enable_festival=no)
AC_MSG_CHECKING(whether to use memory allocation/free functions of speech tools)
if test x$enable_festival = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(FESTIVAL)
else
	AC_MSG_RESULT(no)
fi


# Checks for Windows32 Audio
AC_ARG_ENABLE(win32audio, [  --enable-win32audio   compile for Windows 2000/XP/Vista audio device (default=no)],,enable_win32audio=no)
AC_MSG_CHECKING(whether to enable compilation for Windows 2000/XP/Vista audio device)
if test x$enable_win32audio = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(AUDIO_PLAY_WIN32)
else
	AC_MSG_RESULT(no)
fi


# Checks for Windows CE/Mobile Audio
AC_ARG_ENABLE(winceaudio, [  --enable-winceaudio   compile for Windows CE/Mobile audio device (default=no)],,enable_winceaudio=no)
AC_MSG_CHECKING(whether to enable compilation for Windows CE/Mobile audio device)
if test x$enable_winceaudio = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(AUDIO_PLAY_WINCE)
else
	AC_MSG_RESULT(no)
fi


AC_CANONICAL_HOST
AC_C_BIGENDIAN


AC_CONFIG_FILES([Makefile
                 bin/Makefile
                 lib/Makefile])

AC_OUTPUT

