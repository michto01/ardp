# GNU Make toolchain

Most of the open-source project use the build tools to help people to access
the program easier and to unify the building process.

One of such tools is the GNU Autotools toolchain. It consists of several programs
such as `autoconf`, `aclocal` and `make`.

This toolchain helps to automate creation of Makefiles which then compile programs
and it`s  dependencies. 

The normal working process for building process is then simplified for the user.
It usually consist of four steps:
```
	./configure [-optional-paramaters]
	make
	make check
	make install
```

In first step we call the script which then populates all of the Makefile templates
in our project and check for all dependencies and requirements.

If all of the requirements are met then we can simply follow the next command
`make` which calls the tools such as compilers and preprocessors to compile
the project. This step takes place in the build directory if the prefix was not
specified. 

The next (optional) step is to run self test on the compiled binaries and libraries.

If all is well we then proceed to the final step of installing the necessery binaries
and libraries into the host system. In this step the `sudo` (or equivalent) is 
required because the application may be copied to locations requiring administator
priviledges to modify.
