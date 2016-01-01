#!/bin/sh
#-x

echo "\nARDP bootstrap script\n"\
     "=============================================================================="

echo "\x1B[32m[info]\033[0m Running autoheader ..."   ; autoheader
echo "\x1B[32m[info]\033[0m Running aclocal ..."      ; aclocal
echo "\x1B[32m[info]\033[0m Running automake ..."     ; automake --add-missing
echo "\x1B[32m[info]\033[0m Running autoreconf ..."   ; autoreconf
