ARDP - Another RDF document parser  
====================================


RDF is  one of the building block of the `semantic web`. The designated format `XML/RDF` has complex and somewhat hard to read syntax. Therefore several alternative formats were created so that it's easier for humans to read them, but most of them are not compatible with each other. The goal of this thesis is to create tool to serialize [Turtle](http://www.w3.org/TR/tutle/) (and [N-Triples](http://www.w3.org/TR/n-triples/)) syntax to simple triples.

The implementation program called ARDP (another RDF document parser) should be focused on performance and correct handling of Unicode charset. The implementation is done in `C` language with use of the newest syntax `C11`. It uses the `GNU Autotools` as it's build system and `clang` as compiler ( `GCC` is not tested due to code reliability on clang's features, extensions and syntactic sugar). 

The primary development and deployment platforms are `Mac OSX` and `GNU\Linux` or other `*nix` system with support of the Autotools and Clang(LLVM) compiler. The `Windows` platform is supported via `cygwin` but is not being tested. The primary platforms are tested via `Continuous Integration` service `Travis-CI`.

Run compile this project one should execute following steps:
```
    git clone https://github.com/michto01/ardp.git
    cd ardp
    ./autogen.sh
    mkdir build  #optional
    cd build     #optional
    ../configure # ./configure if optional steps were not taken
    make 
    make check   #optional
    make install #may require `sudo` access
```

Current implementation build status: [![Build Status](https://travis-ci.org/michto01/ardp.svg?branch=master)](https://travis-ci.org/michto01/ardp)