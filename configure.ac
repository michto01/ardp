dnl === configure.ac --------------------------------------------------------===
dnl            ARDP - Another RDF Document Parser - Infrastructure
dnl
dnl This file is distributed under the Open Sourcem License.
dnl See LICENSE or COPYING for details.
dnl
dnl===-----------------------------------------------------------------------===
dnl This is the ARDP configuration script. It is processed by the autoconf
dnl program to produce a script named configure. This script contains the
dnl configuration checks that ARDP needs in order to support multiple platforms.
dnl This file is composed of 10 sections per the recommended organization of
dnl autoconf input defined in the autoconf documentation. As this file evolves,
dnl please keep the various types of checks within their sections. The sections
dnl are as follows:
dnl
dnl SECTION 1:  Initialization & Setup
dnl SECTION 2:  Architecture, target, and host checks
dnl SECTION 3:  Command line arguments for the configure script.
dnl SECTION 4:  Check for programs we need and that they are the right version
dnl SECTION 5:  Check for libraries
dnl SECTION 6:  Check for header files
dnl SECTION 7:  Check for types and structures
dnl SECTION 8:  Check for specific functions needed
dnl SECTION 9:  Additional checks, variables, etc.
dnl SECTION 10: Specify the output files and generate it
dnl
dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 1: Initialization & Setup
dnl===
dnl===-----------------------------------------------------------------------===
dnl# Initialize autoconf and define the package name, version number and
dnl# address for reporting bugs.


AC_INIT(ardp,2.2.1,tomas.michalek.st@vsb.cz)

ARDP_VERSION_MAJOR=2
ARDP_VERSION_MINOR=2
ARDP_VERSION_PATCH=1

AC_DEFINE_UNQUOTED([ARDP_VERSION_MAJOR], $ARDP_VERSION_MAJOR, [Major version of the ARDP API])
AC_DEFINE_UNQUOTED([ARDP_VERSION_MINOR], $ARDP_VERSION_MINOR, [Minor version of the ARDP API])
AC_DEFINE_UNQUOTED([ARDP_VERSION_PATCH], $ARDP_VERSION_PATCH, [Patch version of the ARDP API])
AC_DEFINE_UNQUOTED([ARDP_VERSION_STRING], "$PACKAGE_VERSION", [ARDP version string])

AC_SUBST([ARDP_VERSION_MAJOR])
AC_SUBST([ARDP_VERSION_MINOR])
AC_SUBST([ARDP_VERSION_PATCH])

dnl# Provide a copyright substitution and ensure the copyright notice is included
dnl# in the output of --version option of the generated configure script.
AC_SUBST(ARDP_COPYRIGHT,["Copyright (c) 2015-2016 Tomas Michalek <tomas.michalek.st@vsb.cz> "])
AC_COPYRIGHT([Copyright (c) 2015-2016 Tomas Michalek <tomas.michalek.st@vsb.cz>.])

dnl# Indicate that we require autoconf 2.69 or later.
#AC_PREREQ(2.69)

dnl# Verify that the source directory is valid. This makes sure that we are
dnl# configuring LLVM and not some other package (it validates --srcdir argument)
AC_CONFIG_SRCDIR([src/bin/ardp.c])

dnl# use the clang C compiler for the following checks
AC_LANG([C])

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 2: Architecture, target, and host checks
dnl===
dnl===-----------------------------------------------------------------------===

dnl# Check the target for which we're compiling and the host that will do the
dnl# compilations. This needs to be done early because following tests depend on it.
AC_CANONICAL_TARGET

dnl# Determine the platform type and cache its value. This helps us configure
dnl# the System library to the correct build platform.
AC_CACHE_CHECK([type of operating system we're going to host on],
               [ardp_cv_os_type],
[case $host in
  *-*-cygwin*)
    ardp_cv_link_all_option="-Wl,--whole-archive"
    ardp_cv_no_link_all_option="-Wl,--no-whole-archive"
    ardp_cv_os_type="Cygwin"
    ardp_cv_platform_type="Unix" ;;
  *-*-darwin*)
    ardp_cv_link_all_option="-Wl,-all_load"
    ardp_cv_no_link_all_option="-Wl,-noall_load"
    ardp_cv_os_type="Darwin"
    ardp_cv_platform_type="Darwin" ;;
  *-*-*bsd*)
    ardp_cv_link_all_option="-Wl,--whole-archive"
    ardp_cv_no_link_all_option="-Wl,--no-whole-archive"
    ardp_cv_os_type="BSD"
    ardp_cv_platform_type="Unix" ;;
  *-*-*linux*)
    ardp_cv_link_all_option="-Wl,--whole-archive"
    ardp_cv_no_link_all_option="-Wl,--no-whole-archive"
    ardp_cv_os_type="Linux"
    ardp_cv_platform_type="Unix" ;;
  *-*-gnu*)
    ardp_cv_link_all_option="-Wl,--whole-archive"
    ardp_cv_no_link_all_option="-Wl,--no-whole-archive"
    ardp_cv_os_type="GNU"
    ardp_cv_platform_type="Unix" ;;
  *-*-win32*)
    ardp_cv_link_all_option="-Wl,--whole-archive"
    ardp_cv_no_link_all_option="-Wl,--no-whole-archive"
    ardp_cv_os_type="Win32"
    ardp_cv_platform_type="Win32" ;;
  *-*-mingw*)
    ardp_cv_link_all_option="-Wl,--whole-archive"
    ardp_cv_no_link_all_option="-Wl,--no-whole-archive"
    ardp_cv_os_type="MingW"
    ardp_cv_platform_type="Win32" ;;
  *-unknown-elf*)
    ardp_cv_link_all_option="-Wl,--whole-archive"
    ardp_cv_no_link_all_option="-Wl,--no-whole-archive"
    ardp_cv_os_type="Freestanding"
    ardp_cv_platform_type="Unix" ;;
  *)
    ardp_cv_link_all_option=""
    ardp_cv_no_link_all_option=""
    ardp_cv_os_type="Unknown"
    ardp_cv_platform_type="Unknown" ;;
esac])

AC_CACHE_CHECK([type of operating system we're going to target],
               [ardp_cv_target_os_type],
[case $target in
  *-*-cygwin*)
    ardp_cv_target_os_type="Cygwin" ;;
  *-*-darwin*)
    ardp_cv_target_os_type="Darwin" ;;
  *-*-*bsd*)
    ardp_cv_target_os_type="BSD" ;;
  *-*-*linux* | *-*-gnu*)
    ardp_cv_target_os_type="Linux" ;;
  *-*-win32*)
    ardp_cv_target_os_type="Win32" ;;
  *-*-mingw*)
    ardp_cv_target_os_type="MingW" ;;
  *)
    ardp_cv_target_os_type="Unknown" ;;
esac])

dnl# Make sure we aren't attempting to configure for an unknown system
if test "$ardp_cv_os_type" = "Unknown" ; then
  AC_MSG_ERROR([Operating system is unknown, configure can't continue])
fi

dnl# Set the "OS" Makefile variable based on the platform type so the
dnl# makefile can configure itself to specific build hosts
AC_SUBST(OS,$ardp_cv_os_type)
AC_SUBST(HOST_OS,$ardp_cv_os_type)
AC_SUBST(TARGET_OS,$ardp_cv_target_os_type)

dnl# Set the LINKALL and NOLINKALL Makefile variables based on the platform
AC_SUBST(LINKALL,$ardp_cv_link_all_option)
AC_SUBST(NOLINKALL,$ardp_cv_no_link_all_option)

dnl# Set the "ARDP_ON_*" variables based on ardp_cv_platform_type
dnl# This is used by lib/Support to determine the basic kind of implementation
dnl# to use.
case $ardp_cv_platform_type in
  Darwin)
    AC_DEFINE([ARDP_ON_OSX],[1],[Define if this is OSX platform])
    AC_SUBST(ARDP_ON_OSX,[1])
    AC_SUBST(ARDP_ON_UNIX,[0])
    AC_SUBST(ARDP_ON_WIN32,[0])
    ;;
  Unix)
    BLOCKS_CFLAGS="-fblocks"
    BLOCKS_LDFLAGS="-ldispatch -lBlocksRuntime"
    AC_SUBST(BLOCKS_CFLAGS)
    AC_SUBST(BLOCKS_LDFLAGS)
    AC_DEFINE([ARDP_ON_UNIX],[1],[Define if this is Unixish platform])
    AC_SUBST(ARDP_ON_OSX, [0])
    AC_SUBST(ARDP_ON_UNIX,[1])
    AC_SUBST(ARDP_ON_WIN32,[0])
    ;;
  Win32)
    AC_DEFINE([ARDP_ON_WIN32],[1],[Define if this is Win32-ish platform])
    AC_SUBST(ARDP_ON_OSX, [0])
    AC_SUBST(ARDP_ON_UNIX,[0])
    AC_SUBST(ARDP_ON_WIN32,[1])
    ;;
esac

AM_CONDITIONAL([ARDP_ON_UNIX], [test x"$ardp_cv_platform_type" = x"Unix"])
AM_CONDITIONAL([ARDP_ON_OSX], [test x"$ardp_cv_platform_type" = "xDarwin"])
AM_CONDITIONAL([ARDP_ON_WIN32], [test x"$ardp_cv_platform_type" = x"Win32"])

# ADD ARCH + CROSS_COMPILE to the target

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 3: Command line arguments for the configure script.
dnl===
dnl===-----------------------------------------------------------------------===

dnl# Check for sufficiently modern version of Clang (we don't care about).
dnl# These checks can be bypassed by passing a flag if necessary on a platform.
dnl# We have to do these checks here so that we have the configuration.

dnl# Remove the warning of order precedence
AM_INIT_AUTOMAKE()


# Checks for programs.
AC_PROG_CC(clang)
AC_PROG_CXX(clang++)
#AC_PROG_LIBTOOL
AC_PROG_RANLIB
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_MKDIR_P

dnl# PKG_CHECK_MODULES([CHECK], [check >= 0.9.10])

AC_CONFIG_HEADERS([config.h])

# Checks for libraries.
AC_CHECK_LIB([bz2], [BZ2_bzRead])
AC_CHECK_LIB([z], [gzopen])


# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h iso646.h inttypes.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove memset strcasecmp])

AC_CONFIG_FILES([Makefile
                  man/Makefile
                  doc/Makefile
                  tests/Makefile
                  scripts/Makefile
                  examples/Makefile
                  src/Makefile
                  src/bin/Makefile
                  src/lib/Makefile
                  ])

AC_OUTPUT


echo "
Configuration for $PACKAGE_NAME ($PACKAGE_VERSION)
=======================================================

  Prefix.........: $prefix
  Debug Build....: $debug
  Platform.......: $ardp_cv_os_type ($ardp_cv_platform_type)
  C Compiler.....: $CC $CFLAGS $CPPFLAGS
  Global linker..: $LD $LDFLAGS $LIBS
  Doxygen........: ${DOXYGEN:-NONE}

  You're now ready to build this project! Quick build:
        make && make install
"
