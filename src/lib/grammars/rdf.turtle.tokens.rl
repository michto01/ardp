%%{
    machine rdf_turtle_tokens;
    alphtype unsigned char;

    #> helper token (not in specification)
    SIGN      = ('-' | '+')
              ;

    EXPONENT  = [eE] SIGN? digit+
              ;

    INTEGER   = SIGN? digit+
              ;

    DECIMAL   = SIGN? digit* '.' digit+
              ;

    DOUBLE    = SIGN?
              ( ( digit+ '.' digit* EXPONENT )
              | (        '.' digit+ EXPONENT )
              | (            digit+ EXPONENT )
              )
              ;

    #HEX      = xdigit;

    PERCENT   = '%' . xdigit{2}
              ;

    ##> Alphanumerical tokens

    EOL       = '\r'
              | '\n'
              | '\r' . '\n' #Windows variation of LF
              ;

    WS        = ' '
              | '\t'
              | EOL
              ;

    #PN_LOCAL_ESC
    #          = '\\' . [_~\.-!$&\'()*+,;=/?#@%]
    #          ;

    PN_LOCAL_ESC = '\\'
                  ( '_'
                  | '~' | '.' | '-' | '!' | '$'
                  | '&' | "'" | '(' | ')' | '*'
                  | '+' | ',' | ';' | '=' | '/'
                  | '?' | '#' | '@' | '%'
                  )
                  ;

    PLX       = PERCENT
              | PN_LOCAL_ESC
              ;

    PN_CHARS_BASE
              = alpha
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
              ;

    PN_CHARS_U
              = PN_CHARS_BASE
              | '_'
              ;

    PN_CHARS  = PN_CHARS_U
              | '-'
              | digit
              | 0x00B7
              | 0xCC 0x80..0xFF      | 0xCD 0x00..0xAF        # 0x0300 .. 0x036F
              | 0xE2 0x80 0xBF..0xFF | 0xE2 0x81 0x00..0x80   # 0x203F .. 0x2040
              ;

    PN_PREFIX = PN_CHARS_BASE
                ( (PN_CHARS | '.')* PN_CHARS )?
              ;

    PN_LOCAL  = (PN_CHARS_U | ":" | digit | PLX)
                ( (PN_CHARS | '.' | ':' | PLX)* (PN_CHARS | ':' | PLX) )?
              ;


    LANGTAG   = '@' alpha+ ( '-' ( alpha | digit )+ )* #FIXME: should add '_' ?
              ;



    ##> operational tokens

    A         = 'a';

    DOT       = '.';
    COMMA     = ',';
    SEMICOLON = ';';

    L_SQUARE  = '[';
    R_SQUARE  = ']';
    L_ROUND   = '(';
    R_ROUND   = ')';
    L_CURLY   = '{';
    R_CURLY   = '}';

    HAT       = '^^'; #double hats

    BOOLEAN   = 'true'
              | 'false'
              ;

    PREFIX        = '@prefix';
    BASE          = '@base';
    SPARQL_PREFIX = /PREFIX/i;
    SPARQL_BASE   = /BASE/i;

    ##> String literals

    COMMENT  = '#' :>> (any - EOL)* EOL;

}%%
