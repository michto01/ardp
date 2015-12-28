#!/bin/sh
#-x

echo "ARDP bootstrap script"

echo "[info] Running autoheader ..."
	autoheader
echo "[info] Running aclocal"
	aclocal
echo "[info] Running automake"
	automake
echo "[info] Runnning auroreconf"
	autoreconf

