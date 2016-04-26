#User guide
In order to simply run the ARDP with the default options, one can simply call:
    
```sh
        ardp ../example/filename.ttl
```


ARDP, in its default setting, is supposed to recognize support for color escape sequences. If the output is piped to non-color-supported environment, color is automatically disabled.
    
The color helps with the visual perception of the parsed data, but have measurable impact on the ARDP performance while processing larger files. To fine-tune this, use option **-c** or **--color** with one of the values: `auto`, `never`, `always`.
    
User can option to suppress `URI` expansion with two modes: realative path expansion and `CURIE` expansion. As the ARDP is not serializer, it doesn't place importance on missing prefixes and displays the un-expanded `CURIE` if the prefix is not found. Options to fine-tune this are: **-x**, **--disable-uri-expansion** and **-z**, **--disable-cuire-expansion**.
    
    
> While **-z** has no impact of document parsing, the **-x** switch will impact the document with the empty default URI: <>. As the content of the URI is empty, the printed triple will be missing the empty node.
    
The default setting allows the ARDP to read the normal plaintext files such as `*.ttl`, `*.nt` as well as GZip archives with the `*.gz` extension. To enable Bzip archive readout, option **-b**, **--use-bzip** should be used.

> At the time of writing this thesis, the **-s, --syntax** option behavior was not yet implemented. This option will enforce stricter checks on conformance to particular grammar.
    
The options for debugging the ARDP are changing with each release as more in-depth logging and error-checking is introduced to the parser.

The options are: **-w --verbose**, **-y --enable-lexer-debug** and **-q --enable-parser-debug**. The selection of either of the options select particular additional information from parser.
    
For deeper debugging, one should also enable debug while building the ARDP, which then adds flags to compiler. This is done in './configure' step of installation.
    
Options **-u, --usage** and **-h, --help** displays short description and intended used of the options.
    
Option **-v, --version** shows the current build of the ARDP.
