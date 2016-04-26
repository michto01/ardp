#Character encoding
Historically the programs were using the most basic set of characters standardized as ASCII. Since then many encoding been created to support languages which uses different symbols than latin alphabet. 

The N-Triples file specification defined the N-Triples files to be ASCII encoded with escapes to specify the nonstandard characters. The Turtle and XML specifications on the other hand specifies the UTF-8 as the file character encoding format. Since then the new N-Triples files are also allowed to be UTF-8 encoded.
 
##Unicode
Unicode provides unique number for every character, no matter the platform, program or language. That way it is able to provide consistent encoding across all systems and platforms supporting it. It contains more then _120 000_ characters in _129_ modern and historic scripts and multiple symbol sets. As of June 2015, the most recent standard is _Unicode 8.0_.
    
The unicode character can be expressed using 4B types, for space efficiency purposes 2B encodings are more commonly used. The 2B encoding allows to save first 2^16 characters, also called the **Basic Multilingual Plane** (BMP), which contains most of modern scripts, without any change. Rest of the characters are expressed using two 2B characters. Unicode has special characters for this purpose, creating _surrogate pairs_.
    
Unicode also has **Private Use Area** (PUA). It is range of codepoint which the Unicode consortium vowed not to assign to allow custom characters and symbols to be used by the consumers.  

##ASCII
ASCII stands for _American Standard Code for Information Interchange_. It was developed from telegraphic codes. It defines 127 ($0x00 ... 0x7F$) characters, from which the first 32 characters are are non-printing characters, also called control-characters.
    
N-Triples define ASCII escape sequences for Unicode codepoints. Table below shows the groups of Unicode characters and their respective escapes in N-Triples. The 'H' character stands for hexadecimal character and the case of the 'u' letter marks short or long codepoint.
   
| _u_ character code | N-Triples escape   |
|  ----------------  |  ----------------  |
| #x0 - #x8          | \\uHHHH            |
| #x9, #xA           | \\t, \\n           |
| #xB - #xC          | \\uHHHH            |
|#xD                 | \\r                |
|#xE - #x1F          | \\uHHHH            |
|#x20 - #x21         | _u_ character      |
|#x22                | \\"                |
|#x23 - #x5B         | _u_ character      |
|#x5C                | \\\\               |
|#x5D - #x7E         | _u_ character      |
|#x7F - #xFFFF       | \\uHHHH            |
|#x10000 - #x10FFFF  | \\UHHHHHHHH        |
    
    
##UTF-8
The UTF-8 is character encoding which is capable of expressing most of the characters and code-points defined by Unicode. The design idea around this encoding was clever hack to allow its backward compatibility with the ASCII character encoding.
    
To express the ASCII character using the UTF-8 encoding, no change to existing code is required. For rest of the Unicode characters, 2-4B are used. The original proposal contained up to 6B characters, but RFC 3639 restricted the UTF-8 to end at U+10FFFF to match constraint of UTF-16 character encoding. This lead to removal of proposed 5 and 6 byte sequences as well as almost 1000000 of 4 byte ones. Table below demonstrates this encoding.
    
| _u_ character code | Bytes in seqence   | Encoded bits      |
|  ----------------  |  ----------------  | ----------------  |
|U+00000000 - U+0000007F | 1 | 0xxxxxxx |
|U+00000080 - U+000007FF | 2 | 110xxxxx 10xxxxxx |
|U+00000800 - U+0000FFFF | 3 | 1110xxxx 10xxxxxx 10xxxxxx |
|U+00010000 - U+001FFFFF | 4 | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx|