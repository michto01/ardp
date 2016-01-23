ARDP - Another RDF document parser  
====================================

[![Build Status](https://travis-ci.org/michto01/ardp.svg?branch=master)](https://travis-ci.org/michto01/ardp)

RDF is  one of the building block of the `semantic web`. The designated format `XML/RDF` has complex and somewhat hard to read syntax. Therefore several alternative formats were created so that it's easier for humans to read them, but most of them are not compatibile with each other. The goal of this thesis is to create tool to serialize [Turtle](http://www.w3.org/TR/n-triples/) (and `N-Triples`) syntax to simple triples.

The implementation program called ARDP (another RDF document parser) should be focused on performance and correct handling of Unicode charset.


Supported formats:
* [RDF 1.1 N-Triples](http://www.w3.org/TR/n-triples/)
* [RDF 1.1 Turtle](http://www.w3.org/TR/tutle/)
