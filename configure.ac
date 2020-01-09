#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(acbmtree, 1.0, msxiu@163.com)
AM_INIT_AUTOMAKE(acbmtree, 1.0)
AC_CONFIG_SRCDIR([example/acbmfilter.c])
AC_CONFIG_HEADERS([config.h])

AC_PROG_RANLIB
AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CC

# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])


# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netinet/in.h stdint.h stdlib.h string.h sys/socket.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strcasecmp strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
