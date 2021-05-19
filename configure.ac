# ===============================================================================
# Autoconf script for the g7ctrl daemon
# Use autoreconf to rebuild the build environment
# ===============================================================================
AC_PREREQ([2.71])
AC_INIT([unitable],[1.0.0],[johan162@gmail.com])
AC_COPYRIGHT([Part of configure script Copyright Johan Persson 2021 (johan162@gmail.com)])
AC_PREFIX_DEFAULT(/usr)
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE([dist-xz silent-rules])
AM_SILENT_RULES
AM_PROG_AR
AC_PROG_LN_S
AC_PROG_CC
AC_CONFIG_HEADERS([src/config.h])


AC_CONFIG_FILES([
Makefile
src/Makefile
src/libunitbl/Makefile
])


# ===============================================================================
# Avoid warning from newer version of automake since the default is "cru"
# ===============================================================================
AC_SUBST(ARFLAGS,"cr")
AC_SUBST(AR_FLAGS,"cr")

AC_ARG_ENABLE([stacktrace],
    [  --enable-stacktrace    Enable a stacktrace dump in case of SIGSEGV or SIGBUS error],
    [enable_stacktrace=${enableval}],
    [enable_stacktrace=no])

AC_ARG_ENABLE([pie],
    [  --disable-pie    Disable PIE code in executable. Creates old style ELF executable (non shared) ],
    [enable_pie=${enableval}],
    [enable_pie=yes])

AM_CONDITIONAL(DISABLE_PIE, [test "x${enable_pie}" = xno])

SYSTEM_NAME=`uname -s`
if test "x${enable_stacktrace}" = xyes; then
    if test x$SYSTEM_NAME = xDarwin; then
        AC_MSG_ERROR([Compiling with --enable-stacktrace is not supported when running on Darwin/OSX])
    else
        AC_DEFINE(SIGSEGV_HANDLER,1,[Enable a stacktrace dump in case of SIGSEGV or SIGBUS error])
   fi
fi

# This is needed so we can test for stracktrace build and in that case
# include the -rdynamic flag to gcc in order to have proper symbol names in
# the stacktrace
AM_CONDITIONAL(STACKTRACE, [test "x${enable_stacktrace}" = xyes])

# Add the system type (Linux or OS X) in config.h
if test x$SYSTEM_NAME = xDarwin; then
    AC_DEFINE(IS_OSX,1,[True if system type is Apple OSX])
    AC_SUBST(DEFAULT_STTY_DEVICE,1411)
else
    AC_DEFINE(IS_OSX,0,[True if system type is Apple OSX])
    AC_SUBST(DEFAULT_STTY_DEVICE,0)
fi

AM_CONDITIONAL(is_osx,[test x$SYSTEM_NAME = xDarwin])

# ===============================================================================
# OSX does not ship with readline and the BSD libedit which ships with OSX
# does not have all necessary functionality nor does it work very well. We
# therefore check if the user has installed the real readline()
# ===============================================================================
AC_DEFUN([AX_OSX_READLINE],[
AC_MSG_CHECKING([If it is OSX and GNU readline is installed])
OSX_GNU_READLINE=no
if test x$SYSTEM_NAME = xDarwin; then
    if test ! -d /usr/local/opt/readline; then
        AC_MSG_RESULT([no])
    else
        AC_MSG_RESULT([yes])
        OSX_GNU_READLINE=yes
    fi
else
    AC_MSG_RESULT([Not OSX])
fi
AM_CONDITIONAL(has_opt_readline, test -d /usr/local/opt/readline)
])


# ===============================================================================
# Standard checks for headers and C-compiler
# ===============================================================================
AC_PROG_INSTALL
AC_PROG_MAKE_SET


# ===============================================================================
# We need to determine which ranlib to use in order to build our static
# mail utility library
# ===============================================================================
AC_PROG_RANLIB

# ===============================================================================
# Check for standard Unix types that we use. This is almost unnecessary since this
# should be available in all sane systems
# ===============================================================================
AC_TYPE_UID_T
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T


# ===============================================================================
# Output all generated files
# ===============================================================================
AC_OUTPUT


# ===============================================================================
# Finally give some information to the user about the configuration setup
# ===============================================================================


AC_MSG_NOTICE([--------------------------------------------------------------------------------])
AC_MSG_NOTICE([INSTALLATION SUMMARY])

if test "x${enable_stacktrace}" = xyes; then
    AC_MSG_NOTICE([  - Will install stacktrace handler for debug support ])
    AC_MSG_NOTICE([ ])
fi

AC_MSG_NOTICE([--------------------------------------------------------------------------------])
