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


AC_INIT([ardp],[1.0.1],[tomas.michalek.st@vsb.cz])



ARDP_VERSION_MAJOR=1
ARDP_VERSION_MINOR=0
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
#AC_CONFIG_HEADERS([include/config.h])

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
    ardp_cv_platform_type="Unix" ;;
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
  Unix)
    AC_DEFINE([ARDP_ON_UNIX],[1],[Define if this is Unixish platform])
    AC_SUBST(ARDP_ON_UNIX,[1])
    AC_SUBST(ARDP_ON_WIN32,[0])
    ;;
  Win32)
    AC_DEFINE([ARDP_ON_WIN32],[1],[Define if this is Win32ish platform])
    AC_SUBST(ARDP_ON_UNIX,[0])
    AC_SUBST(ARDP_ON_WIN32,[1])
    ;;
esac

# ADD ARCH + CROSS_COMPILE to the target

dnl===-----------------------------------------------------------------------===
dnl===
dnl=== SECTION 3: Command line arguments for the configure script.
dnl===
dnl===-----------------------------------------------------------------------===

dnl# Check for sufficiently modern version of Clang (we don't care about).
dnl# These checks can be bypassed by passing a flag if necessary on a platform.
dnl# We have to do these checks here so that we have the configuration
dnl# of the standard C++ library finished.

AC_ARG_ENABLE(compiler-version-checks,
              AS_HELP_STRING([--enable-compiler-version-checks],
                             [Check the version of the host compiler (default is YES)]),,
                             enableval=default)
case "$enableval" in
  no)
    ;;
  yes|default)
    AC_LANG_PUSH([C++])
    case "$llvm_cv_cxx_compiler" in
    clang)
      AC_MSG_CHECKING([whether Clang is new enough])
      AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#if __clang_major__ < 3 || (__clang_major__ == 5 && __clang_minor__ < 1)
#error This version of Clang is too old to build ARDP
#endif
]])],
          [AC_MSG_RESULT([yes])],
          [AC_MSG_RESULT([no])
           AC_MSG_ERROR([
The selected Clang compiler is not new enough to build LLVM. Please upgrade to
Clang 3.5. You may pass --disable-compiler-version-checks to configure to
bypass these sanity checks.])])

      dnl# Note that libstdc++4.6 is known broken for C++11 builds. The errors
      dnl# are sometimes deeply confusing though. Here we test for an obvious
      dnl# incomplete feature in 4.6's standard library that was completed in
      dnl# 4.7's. We also have to disable this test if 'ENABLE_LIBCPP' is set
      dnl# because the enable flags don't actually fix CXXFLAGS, they rely on
      dnl# that happening in the Makefile.
      if test "$ENABLE_LIBCPP" -eq 0 ; then
        AC_MSG_CHECKING([whether Clang will select a modern C++ standard library])
        llvm_cv_old_cxxflags="$CXXFLAGS"
        CXXFLAGS="$CXXFLAGS -std=c++0x"
        AC_LINK_IFELSE([AC_LANG_SOURCE([[
#include <atomic>
std::atomic<float> x(0.0f);
int main() { return (float)x; }
]])],
            [AC_MSG_RESULT([yes])],
            [AC_MSG_RESULT([no])
             AC_MSG_ERROR([
We detected a missing feature in the standard C++ library that was known to be
missing in libstdc++4.6 and implemented in libstdc++4.7. There are numerous
C++11 problems with 4.6's library, and we don't support GCCs or libstdc++ older
than 4.7. You will need to update your system and ensure Clang uses the newer
standard library.

If this error is incorrect or you need to force things to work, you may pass
'--disable-compiler-version-checks' to configure to bypass this test.])])
        CXXFLAGS="$llvm_cv_old_cxxflags"
      fi
      ;;
    gcc)
      AC_MSG_CHECKING([whether GCC is new enough])
      AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 7)
#error This version of GCC is too old to build LLVM
#endif
]])],
          [AC_MSG_RESULT([yes])],
          [AC_MSG_RESULT([no])
           AC_MSG_ERROR([
The selected GCC C++ compiler is not new enough to build LLVM. Please upgrade
to GCC 4.7. You may pass --disable-compiler-version-checks to configure to
bypass these sanity checks.])])
      ;;
    unknown)
      ;;
    esac
    AC_LANG_POP([C++])
    ;;
  *)
    AC_MSG_ERROR([Invalid setting for --enable-compiler-version-checks. Use "yes" or "no"])
    ;;
esac







dnl# Remove the warning of order precedence
AM_INIT_AUTOMAKE()


# Checks for programs.
#AC_PROG_CC
AC_PROG_CC(clang)
AC_PROG_RANLIB



AC_CONFIG_HEADERS([config.h])

# Checks for libraries.
#AC_CHECK_LIB(zlib, zlib)
#AC_CHECK_LIB(bzlib, bzlib)

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove memset])

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
