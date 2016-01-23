# Introduction

For humans extracting information from reading the web content is not very challenging, but for machine it's exponentially more complex. To resolve this complexion in extraction important and relevant information from the web the extension of WWW was proposed. This extension is called `semantic web`. 

It helps with extracting and processing information from web by human but also more importantly to machines.

Probably most important goal of the semantic web is to describe relationships between individual data sources. For this exact purpose as one of the pillars of the semantic web `Resource Description Framework` - `(RDF)` exists. The RDF denotes those relationships as `triples` which then form vast graphs.

Primary format for description of triples in RDF become `XML/RDF`. Due to it's markup based syntax of XML it's easy for the machines to extrapolate the data from it using already available tools and libraries. The cons of this format are huge file sizes because of the markup overhead and the XML syntax is not optimalized for human readability. This predicament gave birth to other formats how to improve the description of the triples. Those formats are usually not compatible between each other and present different solution to the problems.

One of the most know formats is `RDF/Turtle` which is a subset of larger and more powerful syntax Notation-3 usually denoted as simply `N3`.