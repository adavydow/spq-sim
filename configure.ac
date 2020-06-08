#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([spq-sim], [0.1], [adavydow@gmail.com])

# Inits automake.
AM_INIT_AUTOMAKE([foreign -Wall -Werror subdir-objects])

AM_MAINTAINER_MODE


# Source directory check.
AC_CONFIG_SRCDIR([src/simulator.c])


# Checks for programs.
AC_PROG_CC_C99

# Checks for libraries.
AC_CHECK_LIB([m], [fminf]) 
# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Config Makefiles

AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT
