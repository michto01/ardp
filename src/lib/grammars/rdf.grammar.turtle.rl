/*
 */

%%{
  machine rdf.grammar.turtle.terminals;
  include "rdf.grammar.common.rl";

  ####
  #####
  ######  PRODUCTIONS FOR TERMINALS
  #####
  ####

  INTEGER  = SIGN? DIGIT+;                       #[19]	  INTEGER	::=	[+-]? [0-9]+
  DECIMAL  = SIGN? DIGIT* '.' DIGIT+             #[20]	  DECIMAL	::=	[+-]? [0-9]* '.' [0-9]+
  DOUBLE   = SIGN? (DIGIT+ '.' DIGIT* EXPONENT
                    |      '.' DIGIT+ EXPONENT
                    |          DIGIT+ EXPONENT); #[21]	  DOUBLE	::=	[+-]? ([0-9]+ '.' [0-9]* EXPONENT | '.' [0-9]+ EXPONENT | [0-9]+ EXPONENT)

  PERCENT  = '%' HEX{2};                         #[170s]	PERCENT	::=	'%' HEX HEX


  ANON     = '[' WS* ']'                         #[162s]	ANON	::=	'[' WS* ']'

  ECHAR    = '\\' . (   't'
                      | 'b'
                      | 'n'
                      | 'r'
                      | 'f'
                      | '"'
                      | '\''
                      | '\\' );                 #[159s]	 ECHAR	::=	'\' [tbnrf"'\]

  UCHAR         = '\\' . ( ('u' HEX{4}) | ('U' HEX{8}) ); #[26]	  UCHAR

  PN_CHARS_BASE = (
                ALPHA
                | 0xC3 0x80..0x96                   # [0x00C0 .. 0x00D6]
                | 0xC3 0x98..0xB6                   # [0x00D8 .. 0x00F6]
                | 0xC3 0xB8..0xFF                   # [0x00F8 .. 0x02FF]
                | 0xC4..0xCA 0x00..0xFF
                | 0xCB 0x00..0xBF
                | 0xCD 0xB0..0xBD                   # [0x0370 .. 0x037D]
                | 0xCD 0xBF..0xFF                   # [0x037F .. 0x1FFF = 0x037F .. 0x07FF | 0x0800 .. 0x1FFF]
                | 0xCE..0xDE 0x00..0xFF
                | 0xDF 0x00..0xBF
                | 0xE0 0xA0 0x80..0xFF
                | 0xE0 0xA1..0xFF 0x00..0xFF
                | 0xE1 0x00 0x00..0xFF
                | 0xE1 0x01..0xBE 0x00..0xFF
                | 0xE1 0xBF 0x00..0xBF
                | 0xE2 0x80 0x8C..0x8D              # [0x200C .. 0x200D]
                | 0xE2 0x81 0xB0..0xFF              # [0x2070 .. 0x218F]
                | 0xE2 0x82..0x85 0x00..0xFF
                | 0xE2 0x86 0x00..0x8F
                | 0xE2 0xB0 0x80..0xFF              # [0x2C00 .. 0x2FEF]
                | 0xE2 0xB1..0xBE 0x00..0xFF
                | 0xE2 0xBF 0x00..0xAF
                | 0xE3 0x80 0x81..0xFF              # [0x3001 .. 0xD7FF]
                | 0xE3 0x81..0xFF 0x00..0xFF
                | 0xE4..0xEC (0x00..0xFF){2}
                | 0xED 0x00 0x00..0xFF
                | 0xED 0x01..0x9E 0x00..0xFF
                | 0xED 0x9F 0x00..0xBF
                | 0xEF 0xA4 0x80..0xFF              # [0xF900 .. 0xFDCF]
                | 0xEF 0xA5..0xB6 0x00..0xFF
                | 0xEF 0xB7 0x00..0x8F
                | 0xEF 0xB7 0xB0..0xFF              # [0xFDF0 .. 0xFFFD]
                | 0xEF 0xB8..0xBE 0x00..0xFF
                | 0xEF 0xBF 0x00..0xBD
                | 0xF0 0x90 0x80 0x80..0xFF         # [0x10000 .. 0xEFFFF]
                | 0xF0 0x90 0x81..0xFF 0x00..0xFF
                | 0xF0 0x91..0xFF (0x00..0xFF){2}
                | 0xF1..0xF2 (0x00..0xFF){3}
                | 0xF3 0x00 0x00 0x00..0xFF
                | 0xF3 0x00 0x01..0xFF 0x00..0xFF
                | 0xF3 0x01..0xAE (0x00..0xFF){2}
                | 0xF3 0xAF 0x00 0x00..0xFF
                | 0xF3 0xAF 0x01..0xBE 0x00..0xFF
                | 0xF3 0xAF 0xBF 0x00..0xBF
                );                                #[163s]	PN_CHARS_BASE

  PN_CHARS_U    = PN_CHARS_BASE | '_';  #[164s]	PN_CHARS_U	::=	PN_CHARS_BASE | '_'

  PN_CHARS      = PN_CHARS_U | '-' | DIGIT                       #[166s]	PN_CHARS
                |  0x00B7                                        #  #x00B7
                | (0xCC 0x80..0xFF | 0xCD 0x00..0xAF)            #  [#x0300-#x036F]
                | (0xE2 0x80 0xBF..0xFF | 0xE2 0x81 0x00..0x80); #  [#x203F-#x2040]

  PN_PREFIX     = PN_CHARS_BASE ((PN_CHARS | '.')* PN_CHARS)?;   #[167s]	PN_PREFIX	::=	PN_CHARS_BASE ((PN_CHARS | '.')* PN_CHARS)?

  PLX           = PERCENT | PN_LOCAL_ESC;                        #[169s]	PLX	::=	PERCENT | PN_LOCAL_ESC

  PN_LOCAL      = (PN_CHARS_U | ':' | DIGIT | PLX)
                  ((PN_CHARS  | '.' | ':'   | PLX)* (PN_CHARS | ':' | PLX))?; #[168s]	PN_LOCAL

  PN_LOCAL_ESC	=	'\\' . (
                          '_' | '~' | '.' | '-'
                        | '!' | '$' | '&' | "'"
                        | '(' | ')' | '*' | '+'
                        | ',' | ';' | '=' | '/'
                        | '?' | '#' | '@' | '%' ); #[172s]	PN_LOCAL_ESC;

  STRING_VALUE  = (^(0x22 | 0x5C | 0xA | 0xD) | ECHAR | UCHAR)*;
  STRING_QUOTED_VALUE = (('\'' | '\'\'')? (^('\'' | '\\' ) | ECHAR | UCHAR))*;

  STRING_LITERAL_QUOTE =	'"' STRING_VALUE '"';                           #[22]	  STRING_LITERAL_QUOTE
  STRING_LITERAL_SINGLE_QUOTE = '\'' STRING_VALUE '\'';                   #[23]	  STRING_LITERAL_SINGLE_QUOTE
  STRING_LITERAL_LONG_SINGLE_QUOTE = '\''{3} STRING_QUOTED_VALUE '\''{3}; #[24]	  STRING_LITERAL_LONG_SINGLE_QUOTE
  STRING_LITERAL_LONG_QUOTE = '"'{3} STRING_QUOTED_VALUE '"'{3};          #[25]	  STRING_LITERAL_LONG_QUOTE


  LANGTAG = '@' ALPHA+ ('-' (ALPHA | DIGIT)+)*;  #[144s]	LANGTAG	::=	'@' [a-zA-Z]+ ('-' [a-zA-Z0-9]+)*
  BLANK_NODE_LABEL = ('_'.':') (PN_CHARS_U | DIGIT) ((PN_CHARS | '.')* PN_CHARS)?; #[141s]	BLANK_NODE_LABEL	::=	'_:' (PN_CHARS_U | [0-9]) ((PN_CHARS | '.')* PN_CHARS)?

  PNAME_NS = PN_PREFIX? ':';    #[139s]	PNAME_NS	::=	PN_PREFIX? ':'
  PNAME_LN = PNAME_NS PN_LOCAL; #[140s]	PNAME_LN	::=	PNAME_NS PN_LOCAL

  IRIREF_VALUE     = (^(0x00 .. 0x20 | '<' | '>' | '"' | '{' | '}' | '|' | '^' | '`' | '\\') | UCHAR)*;
  IRIREF           = '<' IRIREF_VALUE '>';

}%%

%%{
  machine rdf.grammar.turtle.terminals;

  ####
  #####
  ######  TERMINALS
  #####
  ####

}%%
