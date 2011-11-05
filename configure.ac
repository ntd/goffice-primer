m4_define([goffice_primer_version],[0.0.1])
m4_define([goffice_prereq],[0.8.0])

m4_define([goffice_api],regexp(goffice_prereq,[^[0-9]+\.[0-9]+],[\&]))
m4_define([goffice_package],[libgoffice-]goffice_api)


# Initialization

AC_PREREQ([2.62])
AC_INIT([goffice-primer],goffice_primer_version,[http://dev.entidi.com/p/goffice/],[goffice],[http://goffice.entidi.com/])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_MACRO_DIR([build])
AM_INIT_AUTOMAKE([1.11 gnits no-dist-gzip dist-bzip2 -Wall -Wno-portability])
AM_SILENT_RULES([yes])


# Check for programs

AC_PROG_CC
AC_PROG_SED
PKG_PROG_PKG_CONFIG


# Check for required packages

PKG_CHECK_MODULES([GOFFICE],goffice_package >= goffice_prereq)


# Generation

AC_CONFIG_FILES([Makefile])
AC_OUTPUT


# Report

AC_MSG_NOTICE([generating report

AC_PACKAGE_NAME goffice_primer_version will be built with the following options:
----------------------------------------------------------
              GOffice version: $($PKG_CONFIG --modversion goffice_package)
])
