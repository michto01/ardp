#include <string.h>
#include <ctype.h>

#include <stdio.h>

#include <ardp/parser.h>
#include <ardp/parser_internal.h>
#include <ardp/util.h>

#define BUFSIZE 0x10000

// TODO:
// - error handling, also when string methods fail

static unsigned int parse_hex( const uint8_t *src, unsigned int len ) {
    unsigned int i = 0;

    for ( unsigned int j = 0; j < len; j++ ) {
        i *= 16;
        char c = src[j];
        if ( isxdigit(c) ) {
            if ( isdigit(c) ) {
                i += c - '0';
            } else {
                i += toupper(c) - 'A' + 10;
            }
        }
    }
    return i;
}

static void emit( ardp_parser *parser, ardp_token_type type ) {
        parser->handler( type, parser->string, parser->handler_arg );
}

static const int  ntriples_start  = 149;
static const int  ntriples_first_final  = 149;
static const int  ntriples_error  = 0;
static const int  ntriples_en_skip_line  = 148;
static const int  ntriples_en_main  = 149;
ardp_parser *ardp_new_parser( void ) {
    ardp_parser *parser              = malloc(sizeof(ardp_parser));
                  parser->line        = 1;
                  parser->finished    = 0;
                  parser->string      = NULL;
                  parser->reader      = NULL;
                  parser->reader_arg  = NULL;
                  parser->handler     = NULL;
                  parser->handler_arg = NULL;
    return parser;
}

void ardp_free_parser( ardp_parser *parser ) {
    if ( parser->string isnt NULL )
        string_dealloc( parser->string );
    free(parser);
}

void ardp_parser_set_reader( ardp_parser *parser, ardp_reader reader, void *reader_arg ) {
    parser->reader     = reader;
    parser->reader_arg = reader_arg;
}

void ardp_parser_set_handler( ardp_parser *parser, ardp_handler handler, void *handler_arg ) {
    parser->handler     = handler;
    parser->handler_arg = handler_arg;
}

//typedef unsigned char uchar;

int ardp_parser_parse(ardp_parser *parser) {
    if ( parser->reader is NULL or parser->handler is NULL )
        return 0;

    int cs;

    
	{
	cs = ntriples_start;
}
uint8_t buf[BUFSIZE];
    uint8_t *mark = 0;

    int have = 0;
    int res  = 1;

    ardp_reader reader = parser->reader;

    while ( parser->finished is 0 ) {
        uint8_t *pe;
        uint8_t *p   = buf + have;
        uint8_t *eof = 0;

        int space = BUFSIZE - have;
        if ( space is 0 ) {
            // out of buffer space
            res = 0;
            break;
        }

        int len = reader( p, space, parser->reader_arg );
        pe = p + len;

        if ( len < space ) {
            eof = pe;
            parser->finished = 1;
        }

        
	{
	if ( p == pe  )
	goto _test_eof;
	
switch ( cs  ) {
	case 149:
goto st_case_149;
	case 0:
goto st_case_0;
	case 1:
goto st_case_1;
	case 2:
goto st_case_2;
	case 3:
goto st_case_3;
	case 4:
goto st_case_4;
	case 5:
goto st_case_5;
	case 6:
goto st_case_6;
	case 7:
goto st_case_7;
	case 8:
goto st_case_8;
	case 9:
goto st_case_9;
	case 10:
goto st_case_10;
	case 11:
goto st_case_11;
	case 12:
goto st_case_12;
	case 13:
goto st_case_13;
	case 14:
goto st_case_14;
	case 15:
goto st_case_15;
	case 16:
goto st_case_16;
	case 17:
goto st_case_17;
	case 18:
goto st_case_18;
	case 19:
goto st_case_19;
	case 20:
goto st_case_20;
	case 21:
goto st_case_21;
	case 22:
goto st_case_22;
	case 23:
goto st_case_23;
	case 24:
goto st_case_24;
	case 25:
goto st_case_25;
	case 26:
goto st_case_26;
	case 27:
goto st_case_27;
	case 28:
goto st_case_28;
	case 29:
goto st_case_29;
	case 30:
goto st_case_30;
	case 31:
goto st_case_31;
	case 32:
goto st_case_32;
	case 33:
goto st_case_33;
	case 34:
goto st_case_34;
	case 35:
goto st_case_35;
	case 36:
goto st_case_36;
	case 37:
goto st_case_37;
	case 38:
goto st_case_38;
	case 39:
goto st_case_39;
	case 40:
goto st_case_40;
	case 41:
goto st_case_41;
	case 42:
goto st_case_42;
	case 43:
goto st_case_43;
	case 44:
goto st_case_44;
	case 45:
goto st_case_45;
	case 46:
goto st_case_46;
	case 47:
goto st_case_47;
	case 48:
goto st_case_48;
	case 49:
goto st_case_49;
	case 50:
goto st_case_50;
	case 51:
goto st_case_51;
	case 52:
goto st_case_52;
	case 53:
goto st_case_53;
	case 54:
goto st_case_54;
	case 55:
goto st_case_55;
	case 56:
goto st_case_56;
	case 57:
goto st_case_57;
	case 58:
goto st_case_58;
	case 59:
goto st_case_59;
	case 60:
goto st_case_60;
	case 61:
goto st_case_61;
	case 62:
goto st_case_62;
	case 63:
goto st_case_63;
	case 64:
goto st_case_64;
	case 65:
goto st_case_65;
	case 66:
goto st_case_66;
	case 67:
goto st_case_67;
	case 68:
goto st_case_68;
	case 69:
goto st_case_69;
	case 70:
goto st_case_70;
	case 71:
goto st_case_71;
	case 72:
goto st_case_72;
	case 73:
goto st_case_73;
	case 74:
goto st_case_74;
	case 75:
goto st_case_75;
	case 76:
goto st_case_76;
	case 77:
goto st_case_77;
	case 78:
goto st_case_78;
	case 79:
goto st_case_79;
	case 80:
goto st_case_80;
	case 81:
goto st_case_81;
	case 82:
goto st_case_82;
	case 83:
goto st_case_83;
	case 84:
goto st_case_84;
	case 85:
goto st_case_85;
	case 86:
goto st_case_86;
	case 87:
goto st_case_87;
	case 88:
goto st_case_88;
	case 89:
goto st_case_89;
	case 90:
goto st_case_90;
	case 91:
goto st_case_91;
	case 92:
goto st_case_92;
	case 93:
goto st_case_93;
	case 94:
goto st_case_94;
	case 95:
goto st_case_95;
	case 96:
goto st_case_96;
	case 97:
goto st_case_97;
	case 98:
goto st_case_98;
	case 99:
goto st_case_99;
	case 100:
goto st_case_100;
	case 101:
goto st_case_101;
	case 102:
goto st_case_102;
	case 103:
goto st_case_103;
	case 104:
goto st_case_104;
	case 105:
goto st_case_105;
	case 106:
goto st_case_106;
	case 107:
goto st_case_107;
	case 108:
goto st_case_108;
	case 109:
goto st_case_109;
	case 110:
goto st_case_110;
	case 111:
goto st_case_111;
	case 112:
goto st_case_112;
	case 113:
goto st_case_113;
	case 114:
goto st_case_114;
	case 115:
goto st_case_115;
	case 116:
goto st_case_116;
	case 117:
goto st_case_117;
	case 118:
goto st_case_118;
	case 119:
goto st_case_119;
	case 120:
goto st_case_120;
	case 121:
goto st_case_121;
	case 122:
goto st_case_122;
	case 123:
goto st_case_123;
	case 124:
goto st_case_124;
	case 125:
goto st_case_125;
	case 126:
goto st_case_126;
	case 127:
goto st_case_127;
	case 128:
goto st_case_128;
	case 129:
goto st_case_129;
	case 130:
goto st_case_130;
	case 131:
goto st_case_131;
	case 132:
goto st_case_132;
	case 133:
goto st_case_133;
	case 134:
goto st_case_134;
	case 135:
goto st_case_135;
	case 136:
goto st_case_136;
	case 137:
goto st_case_137;
	case 138:
goto st_case_138;
	case 139:
goto st_case_139;
	case 140:
goto st_case_140;
	case 141:
goto st_case_141;
	case 142:
goto st_case_142;
	case 143:
goto st_case_143;
	case 144:
goto st_case_144;
	case 145:
goto st_case_145;
	case 146:
goto st_case_146;
	case 147:
goto st_case_147;
	case 148:
goto st_case_148;
	case 150:
goto st_case_150;
	
}
goto st_out;
ctr2:
	{parser->line++;
    }


	goto st149;
ctr40:
	{}
{parser->line++;
    }


	goto st149;
st149:
	p+= 1;
if ( p == pe  )
	goto _test_eof149;

st_case_149:
	switch ( ( (*( p  ))
)  ) {
	case 9u:
{
goto st1;
	}
		 case 10u:
{
goto ctr2;
	}
		 case 13u:
{
goto ctr2;
	}
		 case 32u:
{
goto st1;
	}
		 case 35u:
{
goto st2;
	}
		 case 60u:
{
goto st3;
	}
		 case 95u:
{
goto ctr5;
	}
	
}
if ( ( (*( p  ))
) < 14u )
	{
		if ( ( (*( p  ))
) > 8u )
	{
			if ( 11u<= ( (*( p  ))
) && ( (*( p  ))
) <= 12u )
	{
				goto ctr0;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 31u )
	{
		if ( ( (*( p  ))
) < 36u )
	{
			if ( 33u<= ( (*( p  ))
) && ( (*( p  ))
) <= 34u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 59u )
	{
			if ( 61u<= ( (*( p  ))
) && ( (*( p  ))
) <= 94u )
	{
				goto ctr0;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr0:
	{printf(_CLR_RED "[ERROR]" _CLR_RESET " on line: %d\n", parser->line );
//        parser_error_callback(parser->line);
        { p = p - 1;
} {goto st148;}}


	goto st0;
st_case_0:
st0:
cs = 0;
goto _out;
st1:
	p+= 1;
if ( p == pe  )
	goto _test_eof1;

st_case_1:
	switch ( ( (*( p  ))
)  ) {
	case 9u:
{
goto st1;
	}
		 case 10u:
{
goto ctr2;
	}
		 case 13u:
{
goto ctr2;
	}
		 case 32u:
{
goto st1;
	}
		 case 35u:
{
goto st2;
	}
		 case 60u:
{
goto st3;
	}
		 case 95u:
{
goto ctr5;
	}
	
}
if ( ( (*( p  ))
) < 14u )
	{
		if ( ( (*( p  ))
) > 8u )
	{
			if ( 11u<= ( (*( p  ))
) && ( (*( p  ))
) <= 12u )
	{
				goto ctr0;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 31u )
	{
		if ( ( (*( p  ))
) < 36u )
	{
			if ( 33u<= ( (*( p  ))
) && ( (*( p  ))
) <= 34u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 59u )
	{
			if ( 61u<= ( (*( p  ))
) && ( (*( p  ))
) <= 94u )
	{
				goto ctr0;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr41:
	{}


	goto st2;
st2:
	p+= 1;
if ( p == pe  )
	goto _test_eof2;

st_case_2:
	switch ( ( (*( p  ))
)  ) {
	case 10u:
{
goto ctr2;
	}
		 case 13u:
{
goto ctr2;
	}
	
}
if ( ( (*( p  ))
) > 9u )
	{
		if ( 11u<= ( (*( p  ))
) && ( (*( p  ))
) <= 12u )
	{
			goto st2;
		}
	
} 
else
	{
		goto st2;
	}

{
	goto st2;
}
st3:
	p+= 1;
if ( p == pe  )
	goto _test_eof3;

st_case_3:
	if ( ( (*( p  ))
) < 65u )
	{
		{
			goto ctr0;
		}
	} 
else if ( ( (*( p  ))
) > 90u )
	{
		if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 122u )
	{
				goto ctr6;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr6;
	}

{
	goto ctr0;
}
ctr6:
	{if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st4;
ctr7:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st4;
st4:
	p+= 1;
if ( p == pe  )
	goto _test_eof4;

st_case_4:
	switch ( ( (*( p  ))
)  ) {
	case 43u:
{
goto ctr7;
	}
		 case 44u:
{
goto ctr0;
	}
		 case 47u:
{
goto ctr0;
	}
		 case 58u:
{
goto ctr8;
	}
	
}
if ( ( (*( p  ))
) < 59u )
	{
		if ( ( (*( p  ))
) < 45u )
	{
			if ( ( (*( p  ))
) <= 42u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 46u )
	{
			if ( 48u<= ( (*( p  ))
) && ( (*( p  ))
) <= 57u )
	{
				goto ctr7;
			}
		
} 
else
	{
			goto ctr7;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 91u )
	{
			{
				goto ctr7;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 122u )
	{
				goto ctr7;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr8:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st5;
ctr169:
	{unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 ) {
            parser->finished = 1;
        }
    }
{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st5;
st5:
	p+= 1;
if ( p == pe  )
	goto _test_eof5;

st_case_5:
	switch ( ( (*( p  ))
)  ) {
	case 33u:
{
goto ctr8;
	}
		 case 34u:
{
goto ctr0;
	}
		 case 60u:
{
goto ctr0;
	}
		 case 61u:
{
goto ctr8;
	}
		 case 62u:
{
goto st6;
	}
		 case 92u:
{
goto st106;
	}
		 case 93u:
{
goto ctr8;
	}
		 case 94u:
{
goto ctr0;
	}
		 case 95u:
{
goto ctr8;
	}
		 case 96u:
{
goto ctr0;
	}
	
}
if ( ( (*( p  ))
) < 63u )
	{
		if ( ( (*( p  ))
) > 32u )
	{
			if ( 35u<= ( (*( p  ))
) && ( (*( p  ))
) <= 59u )
	{
				goto ctr8;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 91u )
	{
		if ( ( (*( p  ))
) > 122u )
	{
			if ( ( (*( p  ))
) <= 125u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) >= 97u )
	{
			goto ctr8;
		}
	
} 
else
	{
		goto ctr8;
	}

{
	goto ctr8;
}
ctr170:
	{unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 ) {
            parser->finished = 1;
        }
    }


	goto st6;
st6:
	p+= 1;
if ( p == pe  )
	goto _test_eof6;

st_case_6:
	switch ( ( (*( p  ))
)  ) {
	case 9u:
{
goto ctr11;
	}
		 case 32u:
{
goto ctr11;
	}
		 case 60u:
{
goto ctr12;
	}
	
}
if ( ( (*( p  ))
) < 10u )
	{
		if ( ( (*( p  ))
) <= 8u )
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 31u )
	{
		if ( 33u<= ( (*( p  ))
) && ( (*( p  ))
) <= 59u )
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr11:
	{emit(parser, ARDP_IRI); }


	goto st7;
ctr189:
	{emit(parser, ARDP_BLANK_NODE); }


	goto st7;
st7:
	p+= 1;
if ( p == pe  )
	goto _test_eof7;

st_case_7:
	switch ( ( (*( p  ))
)  ) {
	case 9u:
{
goto st7;
	}
		 case 32u:
{
goto st7;
	}
		 case 60u:
{
goto st8;
	}
	
}
if ( ( (*( p  ))
) < 10u )
	{
		if ( ( (*( p  ))
) <= 8u )
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 31u )
	{
		if ( 33u<= ( (*( p  ))
) && ( (*( p  ))
) <= 59u )
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr12:
	{emit(parser, ARDP_IRI); }


	goto st8;
ctr191:
	{emit(parser, ARDP_BLANK_NODE); }


	goto st8;
st8:
	p+= 1;
if ( p == pe  )
	goto _test_eof8;

st_case_8:
	if ( ( (*( p  ))
) < 65u )
	{
		{
			goto ctr0;
		}
	} 
else if ( ( (*( p  ))
) > 90u )
	{
		if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 122u )
	{
				goto ctr15;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr15;
	}

{
	goto ctr0;
}
ctr15:
	{if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st9;
ctr16:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st9;
st9:
	p+= 1;
if ( p == pe  )
	goto _test_eof9;

st_case_9:
	switch ( ( (*( p  ))
)  ) {
	case 43u:
{
goto ctr16;
	}
		 case 44u:
{
goto ctr0;
	}
		 case 47u:
{
goto ctr0;
	}
		 case 58u:
{
goto ctr17;
	}
	
}
if ( ( (*( p  ))
) < 59u )
	{
		if ( ( (*( p  ))
) < 45u )
	{
			if ( ( (*( p  ))
) <= 42u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 46u )
	{
			if ( 48u<= ( (*( p  ))
) && ( (*( p  ))
) <= 57u )
	{
				goto ctr16;
			}
		
} 
else
	{
			goto ctr16;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 91u )
	{
			{
				goto ctr16;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 122u )
	{
				goto ctr16;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr17:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st10;
ctr155:
	{unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 ) {
            parser->finished = 1;
        }
    }
{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st10;
st10:
	p+= 1;
if ( p == pe  )
	goto _test_eof10;

st_case_10:
	switch ( ( (*( p  ))
)  ) {
	case 33u:
{
goto ctr17;
	}
		 case 34u:
{
goto ctr0;
	}
		 case 60u:
{
goto ctr0;
	}
		 case 61u:
{
goto ctr17;
	}
		 case 62u:
{
goto st11;
	}
		 case 92u:
{
goto st95;
	}
		 case 93u:
{
goto ctr17;
	}
		 case 94u:
{
goto ctr0;
	}
		 case 95u:
{
goto ctr17;
	}
		 case 96u:
{
goto ctr0;
	}
	
}
if ( ( (*( p  ))
) < 63u )
	{
		if ( ( (*( p  ))
) > 32u )
	{
			if ( 35u<= ( (*( p  ))
) && ( (*( p  ))
) <= 59u )
	{
				goto ctr17;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 91u )
	{
		if ( ( (*( p  ))
) > 122u )
	{
			if ( ( (*( p  ))
) <= 125u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) >= 97u )
	{
			goto ctr17;
		}
	
} 
else
	{
		goto ctr17;
	}

{
	goto ctr17;
}
ctr156:
	{unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 ) {
            parser->finished = 1;
        }
    }


	goto st11;
st11:
	p+= 1;
if ( p == pe  )
	goto _test_eof11;

st_case_11:
	switch ( ( (*( p  ))
)  ) {
	case 9u:
{
goto ctr20;
	}
		 case 32u:
{
goto ctr20;
	}
		 case 33u:
{
goto ctr0;
	}
		 case 34u:
{
goto ctr21;
	}
		 case 60u:
{
goto ctr22;
	}
		 case 95u:
{
goto ctr23;
	}
	
}
if ( ( (*( p  ))
) < 10u )
	{
		if ( ( (*( p  ))
) <= 8u )
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 31u )
	{
		if ( ( (*( p  ))
) > 59u )
	{
			if ( 61u<= ( (*( p  ))
) && ( (*( p  ))
) <= 94u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) >= 35u )
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr20:
	{emit(parser, ARDP_IRI); }


	goto st12;
st12:
	p+= 1;
if ( p == pe  )
	goto _test_eof12;

st_case_12:
	switch ( ( (*( p  ))
)  ) {
	case 9u:
{
goto st12;
	}
		 case 32u:
{
goto st12;
	}
		 case 33u:
{
goto ctr0;
	}
		 case 34u:
{
goto st13;
	}
		 case 60u:
{
goto st50;
	}
		 case 95u:
{
goto ctr27;
	}
	
}
if ( ( (*( p  ))
) < 10u )
	{
		if ( ( (*( p  ))
) <= 8u )
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 31u )
	{
		if ( ( (*( p  ))
) > 59u )
	{
			if ( 61u<= ( (*( p  ))
) && ( (*( p  ))
) <= 94u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) >= 35u )
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr21:
	{emit(parser, ARDP_IRI); }


	goto st13;
st13:
	p+= 1;
if ( p == pe  )
	goto _test_eof13;

st_case_13:
	switch ( ( (*( p  ))
)  ) {
	case 10u:
{
goto ctr0;
	}
		 case 13u:
{
goto ctr0;
	}
		 case 34u:
{
goto ctr29;
	}
		 case 92u:
{
goto ctr30;
	}
	
}
if ( ( (*( p  ))
) < 11u )
	{
		if ( ( (*( p  ))
) <= 9u )
	{
			goto ctr28;
		}
	
} 
else if ( ( (*( p  ))
) > 12u )
	{
		if ( ( (*( p  ))
) > 33u )
	{
			if ( 35u<= ( (*( p  ))
) && ( (*( p  ))
) <= 91u )
	{
				goto ctr28;
			}
		
} 
else if ( ( (*( p  ))
) >= 14u )
	{
			goto ctr28;
		}
	
} 
else
	{
		goto ctr28;
	}

{
	goto ctr28;
}
ctr28:
	{if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st14;
ctr31:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st14;
ctr89:
	{unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 ) {
            parser->finished = 1;
        }
    }
{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st14;
ctr71:
	{string_append_char( &parser->string, '\"'); }


	goto st14;
ctr72:
	{string_append_char( &parser->string, '\''); }


	goto st14;
ctr74:
	{string_append_char( &parser->string, '\\'); }


	goto st14;
ctr75:
	{string_append_char( &parser->string, '\b'); }


	goto st14;
ctr76:
	{string_append_char( &parser->string, '\f'); }


	goto st14;
ctr77:
	{string_append_char( &parser->string, '\n'); }


	goto st14;
ctr78:
	{string_append_char( &parser->string, '\r'); }


	goto st14;
ctr79:
	{string_append_char( &parser->string, '\t'); }


	goto st14;
st14:
	p+= 1;
if ( p == pe  )
	goto _test_eof14;

st_case_14:
	switch ( ( (*( p  ))
)  ) {
	case 10u:
{
goto ctr0;
	}
		 case 13u:
{
goto ctr0;
	}
		 case 34u:
{
goto st15;
	}
		 case 92u:
{
goto st39;
	}
	
}
if ( ( (*( p  ))
) < 11u )
	{
		if ( ( (*( p  ))
) <= 9u )
	{
			goto ctr31;
		}
	
} 
else if ( ( (*( p  ))
) > 12u )
	{
		if ( ( (*( p  ))
) > 33u )
	{
			if ( 35u<= ( (*( p  ))
) && ( (*( p  ))
) <= 91u )
	{
				goto ctr31;
			}
		
} 
else if ( ( (*( p  ))
) >= 14u )
	{
			goto ctr31;
		}
	
} 
else
	{
		goto ctr31;
	}

{
	goto ctr31;
}
ctr29:
	{if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }


	goto st15;
ctr90:
	{unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 ) {
            parser->finished = 1;
        }
    }


	goto st15;
st15:
	p+= 1;
if ( p == pe  )
	goto _test_eof15;

st_case_15:
	switch ( ( (*( p  ))
)  ) {
	case 9u:
{
goto ctr34;
	}
		 case 32u:
{
goto ctr34;
	}
		 case 46u:
{
goto ctr35;
	}
		 case 64u:
{
goto ctr36;
	}
		 case 94u:
{
goto ctr37;
	}
	
}
if ( ( (*( p  ))
) < 33u )
	{
		if ( ( (*( p  ))
) > 8u )
	{
			if ( 10u<= ( (*( p  ))
) && ( (*( p  ))
) <= 31u )
	{
				goto ctr0;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 45u )
	{
		if ( ( (*( p  ))
) > 63u )
	{
			if ( 65u<= ( (*( p  ))
) && ( (*( p  ))
) <= 93u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) >= 47u )
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr98:
	{emit(parser, ARDP_IRI); }


	goto st16;
ctr34:
	{emit(parser, ARDP_SIMPLE_LITERAL_VALUE); }


	goto st16;
ctr43:
	{emit(parser, ARDP_LANGUAGE_TAGGED_LITERAL_LANGUAGE); }


	goto st16;
ctr55:
	{emit(parser, ARDP_DATATYPE_LITERAL_IRI); }


	goto st16;
ctr130:
	{emit(parser, ARDP_BLANK_NODE); }


	goto st16;
st16:
	p+= 1;
if ( p == pe  )
	goto _test_eof16;

st_case_16:
	switch ( ( (*( p  ))
)  ) {
	case 9u:
{
goto st16;
	}
		 case 32u:
{
goto st16;
	}
		 case 46u:
{
goto st17;
	}
	
}
if ( ( (*( p  ))
) < 10u )
	{
		if ( ( (*( p  ))
) <= 8u )
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 31u )
	{
		if ( 33u<= ( (*( p  ))
) && ( (*( p  ))
) <= 45u )
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr99:
	{emit(parser, ARDP_IRI); }


	goto st17;
ctr35:
	{emit(parser, ARDP_SIMPLE_LITERAL_VALUE); }


	goto st17;
ctr45:
	{emit(parser, ARDP_LANGUAGE_TAGGED_LITERAL_LANGUAGE); }


	goto st17;
ctr56:
	{emit(parser, ARDP_DATATYPE_LITERAL_IRI); }


	goto st17;
ctr131:
	{emit(parser, ARDP_BLANK_NODE); }


	goto st17;
st17:
	p+= 1;
if ( p == pe  )
	goto _test_eof17;

st_case_17:
	switch ( ( (*( p  ))
)  ) {
	case 9u:
{
goto st17;
	}
		 case 10u:
{
goto ctr40;
	}
		 case 13u:
{
goto ctr40;
	}
		 case 32u:
{
goto st17;
	}
		 case 35u:
{
goto ctr41;
	}
	
}
if ( ( (*( p  ))
) < 11u )
	{
		if ( ( (*( p  ))
) <= 8u )
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 12u )
	{
		if ( ( (*( p  ))
) > 31u )
	{
			if ( 33u<= ( (*( p  ))
) && ( (*( p  ))
) <= 34u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) >= 14u )
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr36:
	{emit(parser, ARDP_LANGUAGE_TAGGED_LITERAL_VALUE); }


	goto st18;
st18:
	p+= 1;
if ( p == pe  )
	goto _test_eof18;

st_case_18:
	if ( ( (*( p  ))
) < 65u )
	{
		{
			goto ctr0;
		}
	} 
else if ( ( (*( p  ))
) > 90u )
	{
		if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 122u )
	{
				goto ctr42;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr42;
	}

{
	goto ctr0;
}
ctr42:
	{if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st19;
ctr46:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st19;
st19:
	p+= 1;
if ( p == pe  )
	goto _test_eof19;

st_case_19:
	switch ( ( (*( p  ))
)  ) {
	case 9u:
{
goto ctr43;
	}
		 case 32u:
{
goto ctr43;
	}
		 case 45u:
{
goto ctr44;
	}
		 case 46u:
{
goto ctr45;
	}
	
}
if ( ( (*( p  ))
) < 47u )
	{
		if ( ( (*( p  ))
) < 10u )
	{
			if ( ( (*( p  ))
) <= 8u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 31u )
	{
			if ( 33u<= ( (*( p  ))
) && ( (*( p  ))
) <= 44u )
	{
				goto ctr0;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 91u )
	{
			{
				goto ctr46;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 122u )
	{
				goto ctr46;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr44:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st20;
st20:
	p+= 1;
if ( p == pe  )
	goto _test_eof20;

st_case_20:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto ctr47;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 91u )
	{
			{
				goto ctr47;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 122u )
	{
				goto ctr47;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr47:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st21;
st21:
	p+= 1;
if ( p == pe  )
	goto _test_eof21;

st_case_21:
	switch ( ( (*( p  ))
)  ) {
	case 9u:
{
goto ctr43;
	}
		 case 32u:
{
goto ctr43;
	}
		 case 45u:
{
goto ctr44;
	}
		 case 46u:
{
goto ctr45;
	}
		 case 47u:
{
goto ctr0;
	}
	
}
if ( ( (*( p  ))
) < 48u )
	{
		if ( ( (*( p  ))
) < 10u )
	{
			if ( ( (*( p  ))
) <= 8u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 31u )
	{
			if ( 33u<= ( (*( p  ))
) && ( (*( p  ))
) <= 44u )
	{
				goto ctr0;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 57u )
	{
		if ( ( (*( p  ))
) < 65u )
	{
			{
				goto ctr0;
			}
		} 
else if ( ( (*( p  ))
) > 90u )
	{
			if ( ( (*( p  ))
) > 96u )
	{
				if ( ( (*( p  ))
) <= 122u )
	{
					goto ctr47;
				}
			
} 
else
	{
				goto ctr0;
			}
		
} 
else
	{
			goto ctr47;
		}
	
} 
else
	{
		goto ctr47;
	}

{
	goto ctr0;
}
ctr37:
	{emit(parser, ARDP_DATATYPE_LITERAL_VALUE); }


	goto st22;
st22:
	p+= 1;
if ( p == pe  )
	goto _test_eof22;

st_case_22:
	if ( ( (*( p  ))
) == 94u )
	{
		goto st23;
	}
	
if ( ( (*( p  ))
) <= 93u )
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st23:
	p+= 1;
if ( p == pe  )
	goto _test_eof23;

st_case_23:
	if ( ( (*( p  ))
) == 60u )
	{
		goto st24;
	}
	
if ( ( (*( p  ))
) <= 59u )
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st24:
	p+= 1;
if ( p == pe  )
	goto _test_eof24;

st_case_24:
	if ( ( (*( p  ))
) < 65u )
	{
		{
			goto ctr0;
		}
	} 
else if ( ( (*( p  ))
) > 90u )
	{
		if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 122u )
	{
				goto ctr50;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr50;
	}

{
	goto ctr0;
}
ctr50:
	{if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st25;
ctr51:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st25;
st25:
	p+= 1;
if ( p == pe  )
	goto _test_eof25;

st_case_25:
	switch ( ( (*( p  ))
)  ) {
	case 43u:
{
goto ctr51;
	}
		 case 44u:
{
goto ctr0;
	}
		 case 47u:
{
goto ctr0;
	}
		 case 58u:
{
goto ctr52;
	}
	
}
if ( ( (*( p  ))
) < 59u )
	{
		if ( ( (*( p  ))
) < 45u )
	{
			if ( ( (*( p  ))
) <= 42u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 46u )
	{
			if ( 48u<= ( (*( p  ))
) && ( (*( p  ))
) <= 57u )
	{
				goto ctr51;
			}
		
} 
else
	{
			goto ctr51;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 91u )
	{
			{
				goto ctr51;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 122u )
	{
				goto ctr51;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr52:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st26;
ctr67:
	{unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 ) {
            parser->finished = 1;
        }
    }
{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st26;
st26:
	p+= 1;
if ( p == pe  )
	goto _test_eof26;

st_case_26:
	switch ( ( (*( p  ))
)  ) {
	case 33u:
{
goto ctr52;
	}
		 case 34u:
{
goto ctr0;
	}
		 case 60u:
{
goto ctr0;
	}
		 case 61u:
{
goto ctr52;
	}
		 case 62u:
{
goto st27;
	}
		 case 92u:
{
goto st28;
	}
		 case 93u:
{
goto ctr52;
	}
		 case 94u:
{
goto ctr0;
	}
		 case 95u:
{
goto ctr52;
	}
		 case 96u:
{
goto ctr0;
	}
	
}
if ( ( (*( p  ))
) < 63u )
	{
		if ( ( (*( p  ))
) > 32u )
	{
			if ( 35u<= ( (*( p  ))
) && ( (*( p  ))
) <= 59u )
	{
				goto ctr52;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 91u )
	{
		if ( ( (*( p  ))
) > 122u )
	{
			if ( ( (*( p  ))
) <= 125u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) >= 97u )
	{
			goto ctr52;
		}
	
} 
else
	{
		goto ctr52;
	}

{
	goto ctr52;
}
ctr68:
	{unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 ) {
            parser->finished = 1;
        }
    }


	goto st27;
st27:
	p+= 1;
if ( p == pe  )
	goto _test_eof27;

st_case_27:
	switch ( ( (*( p  ))
)  ) {
	case 9u:
{
goto ctr55;
	}
		 case 32u:
{
goto ctr55;
	}
		 case 46u:
{
goto ctr56;
	}
	
}
if ( ( (*( p  ))
) < 10u )
	{
		if ( ( (*( p  ))
) <= 8u )
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 31u )
	{
		if ( 33u<= ( (*( p  ))
) && ( (*( p  ))
) <= 45u )
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr69:
	{unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 ) {
            parser->finished = 1;
        }
    }


	goto st28;
st28:
	p+= 1;
if ( p == pe  )
	goto _test_eof28;

st_case_28:
	switch ( ( (*( p  ))
)  ) {
	case 85u:
{
goto st29;
	}
		 case 117u:
{
goto st38;
	}
	
}
if ( ( (*( p  ))
) > 84u )
	{
		if ( 86u<= ( (*( p  ))
) && ( (*( p  ))
) <= 116u )
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st29:
	p+= 1;
if ( p == pe  )
	goto _test_eof29;

st_case_29:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto ctr59;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto ctr59;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto ctr59;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr59:
	{mark = p;
    }


	goto st30;
st30:
	p+= 1;
if ( p == pe  )
	goto _test_eof30;

st_case_30:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st31;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st31;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st31;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st31:
	p+= 1;
if ( p == pe  )
	goto _test_eof31;

st_case_31:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st32;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st32;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st32;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st32:
	p+= 1;
if ( p == pe  )
	goto _test_eof32;

st_case_32:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st33;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st33;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st33;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st33:
	p+= 1;
if ( p == pe  )
	goto _test_eof33;

st_case_33:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st34;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st34;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st34;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr70:
	{mark = p;
    }


	goto st34;
st34:
	p+= 1;
if ( p == pe  )
	goto _test_eof34;

st_case_34:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st35;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st35;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st35;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st35:
	p+= 1;
if ( p == pe  )
	goto _test_eof35;

st_case_35:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st36;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st36;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st36;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st36:
	p+= 1;
if ( p == pe  )
	goto _test_eof36;

st_case_36:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st37;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st37;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st37;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st37:
	p+= 1;
if ( p == pe  )
	goto _test_eof37;

st_case_37:
	switch ( ( (*( p  ))
)  ) {
	case 33u:
{
goto ctr67;
	}
		 case 34u:
{
goto ctr0;
	}
		 case 60u:
{
goto ctr0;
	}
		 case 61u:
{
goto ctr67;
	}
		 case 62u:
{
goto ctr68;
	}
		 case 92u:
{
goto ctr69;
	}
		 case 93u:
{
goto ctr67;
	}
		 case 94u:
{
goto ctr0;
	}
		 case 95u:
{
goto ctr67;
	}
		 case 96u:
{
goto ctr0;
	}
	
}
if ( ( (*( p  ))
) < 63u )
	{
		if ( ( (*( p  ))
) > 32u )
	{
			if ( 35u<= ( (*( p  ))
) && ( (*( p  ))
) <= 59u )
	{
				goto ctr67;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 91u )
	{
		if ( ( (*( p  ))
) > 122u )
	{
			if ( ( (*( p  ))
) <= 125u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) >= 97u )
	{
			goto ctr67;
		}
	
} 
else
	{
		goto ctr67;
	}

{
	goto ctr67;
}
st38:
	p+= 1;
if ( p == pe  )
	goto _test_eof38;

st_case_38:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto ctr70;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto ctr70;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto ctr70;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr30:
	{if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }


	goto st39;
ctr91:
	{unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 ) {
            parser->finished = 1;
        }
    }


	goto st39;
st39:
	p+= 1;
if ( p == pe  )
	goto _test_eof39;

st_case_39:
	switch ( ( (*( p  ))
)  ) {
	case 34u:
{
goto ctr71;
	}
		 case 39u:
{
goto ctr72;
	}
		 case 85u:
{
goto st40;
	}
		 case 92u:
{
goto ctr74;
	}
		 case 98u:
{
goto ctr75;
	}
		 case 102u:
{
goto ctr76;
	}
		 case 110u:
{
goto ctr77;
	}
		 case 114u:
{
goto ctr78;
	}
		 case 115u:
{
goto ctr0;
	}
		 case 116u:
{
goto ctr79;
	}
		 case 117u:
{
goto st49;
	}
	
}
if ( ( (*( p  ))
) < 86u )
	{
		if ( ( (*( p  ))
) < 35u )
	{
			if ( ( (*( p  ))
) <= 33u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 38u )
	{
			if ( 40u<= ( (*( p  ))
) && ( (*( p  ))
) <= 84u )
	{
				goto ctr0;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 91u )
	{
		if ( ( (*( p  ))
) < 99u )
	{
			if ( 93u<= ( (*( p  ))
) && ( (*( p  ))
) <= 97u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 101u )
	{
			if ( ( (*( p  ))
) > 109u )
	{
				if ( 111u<= ( (*( p  ))
) && ( (*( p  ))
) <= 113u )
	{
					goto ctr0;
				}
			
} 
else if ( ( (*( p  ))
) >= 103u )
	{
				goto ctr0;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st40:
	p+= 1;
if ( p == pe  )
	goto _test_eof40;

st_case_40:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto ctr81;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto ctr81;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto ctr81;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr81:
	{mark = p;
    }


	goto st41;
st41:
	p+= 1;
if ( p == pe  )
	goto _test_eof41;

st_case_41:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st42;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st42;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st42;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st42:
	p+= 1;
if ( p == pe  )
	goto _test_eof42;

st_case_42:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st43;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st43;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st43;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st43:
	p+= 1;
if ( p == pe  )
	goto _test_eof43;

st_case_43:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st44;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st44;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st44;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st44:
	p+= 1;
if ( p == pe  )
	goto _test_eof44;

st_case_44:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st45;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st45;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st45;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr92:
	{mark = p;
    }


	goto st45;
st45:
	p+= 1;
if ( p == pe  )
	goto _test_eof45;

st_case_45:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st46;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st46;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st46;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st46:
	p+= 1;
if ( p == pe  )
	goto _test_eof46;

st_case_46:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st47;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st47;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st47;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st47:
	p+= 1;
if ( p == pe  )
	goto _test_eof47;

st_case_47:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st48;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st48;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st48;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st48:
	p+= 1;
if ( p == pe  )
	goto _test_eof48;

st_case_48:
	switch ( ( (*( p  ))
)  ) {
	case 10u:
{
goto ctr0;
	}
		 case 13u:
{
goto ctr0;
	}
		 case 34u:
{
goto ctr90;
	}
		 case 92u:
{
goto ctr91;
	}
	
}
if ( ( (*( p  ))
) < 11u )
	{
		if ( ( (*( p  ))
) <= 9u )
	{
			goto ctr89;
		}
	
} 
else if ( ( (*( p  ))
) > 12u )
	{
		if ( ( (*( p  ))
) > 33u )
	{
			if ( 35u<= ( (*( p  ))
) && ( (*( p  ))
) <= 91u )
	{
				goto ctr89;
			}
		
} 
else if ( ( (*( p  ))
) >= 14u )
	{
			goto ctr89;
		}
	
} 
else
	{
		goto ctr89;
	}

{
	goto ctr89;
}
st49:
	p+= 1;
if ( p == pe  )
	goto _test_eof49;

st_case_49:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto ctr92;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto ctr92;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto ctr92;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr22:
	{emit(parser, ARDP_IRI); }


	goto st50;
st50:
	p+= 1;
if ( p == pe  )
	goto _test_eof50;

st_case_50:
	if ( ( (*( p  ))
) < 65u )
	{
		{
			goto ctr0;
		}
	} 
else if ( ( (*( p  ))
) > 90u )
	{
		if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 122u )
	{
				goto ctr93;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr93;
	}

{
	goto ctr0;
}
ctr93:
	{if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }
{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st51;
ctr94:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st51;
st51:
	p+= 1;
if ( p == pe  )
	goto _test_eof51;

st_case_51:
	switch ( ( (*( p  ))
)  ) {
	case 43u:
{
goto ctr94;
	}
		 case 44u:
{
goto ctr0;
	}
		 case 47u:
{
goto ctr0;
	}
		 case 58u:
{
goto ctr95;
	}
	
}
if ( ( (*( p  ))
) < 59u )
	{
		if ( ( (*( p  ))
) < 45u )
	{
			if ( ( (*( p  ))
) <= 42u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 46u )
	{
			if ( 48u<= ( (*( p  ))
) && ( (*( p  ))
) <= 57u )
	{
				goto ctr94;
			}
		
} 
else
	{
			goto ctr94;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 91u )
	{
			{
				goto ctr94;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 122u )
	{
				goto ctr94;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr95:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st52;
ctr110:
	{unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 ) {
            parser->finished = 1;
        }
    }
{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st52;
st52:
	p+= 1;
if ( p == pe  )
	goto _test_eof52;

st_case_52:
	switch ( ( (*( p  ))
)  ) {
	case 33u:
{
goto ctr95;
	}
		 case 34u:
{
goto ctr0;
	}
		 case 60u:
{
goto ctr0;
	}
		 case 61u:
{
goto ctr95;
	}
		 case 62u:
{
goto st53;
	}
		 case 92u:
{
goto st54;
	}
		 case 93u:
{
goto ctr95;
	}
		 case 94u:
{
goto ctr0;
	}
		 case 95u:
{
goto ctr95;
	}
		 case 96u:
{
goto ctr0;
	}
	
}
if ( ( (*( p  ))
) < 63u )
	{
		if ( ( (*( p  ))
) > 32u )
	{
			if ( 35u<= ( (*( p  ))
) && ( (*( p  ))
) <= 59u )
	{
				goto ctr95;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 91u )
	{
		if ( ( (*( p  ))
) > 122u )
	{
			if ( ( (*( p  ))
) <= 125u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) >= 97u )
	{
			goto ctr95;
		}
	
} 
else
	{
		goto ctr95;
	}

{
	goto ctr95;
}
ctr111:
	{unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 ) {
            parser->finished = 1;
        }
    }


	goto st53;
st53:
	p+= 1;
if ( p == pe  )
	goto _test_eof53;

st_case_53:
	switch ( ( (*( p  ))
)  ) {
	case 9u:
{
goto ctr98;
	}
		 case 32u:
{
goto ctr98;
	}
		 case 46u:
{
goto ctr99;
	}
	
}
if ( ( (*( p  ))
) < 10u )
	{
		if ( ( (*( p  ))
) <= 8u )
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 31u )
	{
		if ( 33u<= ( (*( p  ))
) && ( (*( p  ))
) <= 45u )
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr112:
	{unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 ) {
            parser->finished = 1;
        }
    }


	goto st54;
st54:
	p+= 1;
if ( p == pe  )
	goto _test_eof54;

st_case_54:
	switch ( ( (*( p  ))
)  ) {
	case 85u:
{
goto st55;
	}
		 case 117u:
{
goto st64;
	}
	
}
if ( ( (*( p  ))
) > 84u )
	{
		if ( 86u<= ( (*( p  ))
) && ( (*( p  ))
) <= 116u )
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st55:
	p+= 1;
if ( p == pe  )
	goto _test_eof55;

st_case_55:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto ctr102;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto ctr102;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto ctr102;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr102:
	{mark = p;
    }


	goto st56;
st56:
	p+= 1;
if ( p == pe  )
	goto _test_eof56;

st_case_56:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st57;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st57;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st57;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st57:
	p+= 1;
if ( p == pe  )
	goto _test_eof57;

st_case_57:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st58;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st58;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st58;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st58:
	p+= 1;
if ( p == pe  )
	goto _test_eof58;

st_case_58:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st59;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st59;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st59;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st59:
	p+= 1;
if ( p == pe  )
	goto _test_eof59;

st_case_59:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st60;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st60;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st60;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr113:
	{mark = p;
    }


	goto st60;
st60:
	p+= 1;
if ( p == pe  )
	goto _test_eof60;

st_case_60:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st61;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st61;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st61;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st61:
	p+= 1;
if ( p == pe  )
	goto _test_eof61;

st_case_61:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st62;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st62;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st62;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st62:
	p+= 1;
if ( p == pe  )
	goto _test_eof62;

st_case_62:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st63;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st63;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st63;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st63:
	p+= 1;
if ( p == pe  )
	goto _test_eof63;

st_case_63:
	switch ( ( (*( p  ))
)  ) {
	case 33u:
{
goto ctr110;
	}
		 case 34u:
{
goto ctr0;
	}
		 case 60u:
{
goto ctr0;
	}
		 case 61u:
{
goto ctr110;
	}
		 case 62u:
{
goto ctr111;
	}
		 case 92u:
{
goto ctr112;
	}
		 case 93u:
{
goto ctr110;
	}
		 case 94u:
{
goto ctr0;
	}
		 case 95u:
{
goto ctr110;
	}
		 case 96u:
{
goto ctr0;
	}
	
}
if ( ( (*( p  ))
) < 63u )
	{
		if ( ( (*( p  ))
) > 32u )
	{
			if ( 35u<= ( (*( p  ))
) && ( (*( p  ))
) <= 59u )
	{
				goto ctr110;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 91u )
	{
		if ( ( (*( p  ))
) > 122u )
	{
			if ( ( (*( p  ))
) <= 125u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) >= 97u )
	{
			goto ctr110;
		}
	
} 
else
	{
		goto ctr110;
	}

{
	goto ctr110;
}
st64:
	p+= 1;
if ( p == pe  )
	goto _test_eof64;

st_case_64:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto ctr113;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto ctr113;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto ctr113;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr27:
	{if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }


	goto st65;
ctr23:
	{emit(parser, ARDP_IRI); }
{if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }


	goto st65;
st65:
	p+= 1;
if ( p == pe  )
	goto _test_eof65;

st_case_65:
	if ( ( (*( p  ))
) == 58u )
	{
		goto st66;
	}
	
if ( ( (*( p  ))
) <= 57u )
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st66:
	p+= 1;
if ( p == pe  )
	goto _test_eof66;

st_case_66:
	switch ( ( (*( p  ))
)  ) {
	case 95u:
{
goto ctr115;
	}
		 case 96u:
{
goto ctr0;
	}
		 case 195u:
{
goto ctr116;
	}
		 case 203u:
{
goto ctr118;
	}
		 case 204u:
{
goto ctr0;
	}
		 case 205u:
{
goto ctr119;
	}
		 case 223u:
{
goto ctr118;
	}
		 case 224u:
{
goto ctr120;
	}
		 case 225u:
{
goto ctr121;
	}
		 case 226u:
{
goto ctr122;
	}
		 case 227u:
{
goto ctr123;
	}
		 case 237u:
{
goto ctr125;
	}
		 case 238u:
{
goto ctr0;
	}
		 case 239u:
{
goto ctr126;
	}
		 case 240u:
{
goto ctr127;
	}
		 case 243u:
{
goto ctr129;
	}
	
}
if ( ( (*( p  ))
) < 97u )
	{
		if ( ( (*( p  ))
) < 59u )
	{
			if ( ( (*( p  ))
) > 47u )
	{
				{
					goto ctr115;
				}
			} 
else
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 64u )
	{
			if ( ( (*( p  ))
) > 90u )
	{
				if ( ( (*( p  ))
) <= 94u )
	{
					goto ctr0;
				}
			
} 
else
	{
				goto ctr115;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 122u )
	{
		if ( ( (*( p  ))
) < 228u )
	{
			if ( ( (*( p  ))
) > 202u )
	{
				if ( 206u<= ( (*( p  ))
) && ( (*( p  ))
) <= 222u )
	{
					goto ctr117;
				}
			
} 
else if ( ( (*( p  ))
) >= 196u )
	{
				goto ctr117;
			}
		
} 
else if ( ( (*( p  ))
) > 236u )
	{
			if ( ( (*( p  ))
) > 242u )
	{
				if ( 244u<= ( (*( p  ))
)  )
	{
					goto ctr0;
				}
			
} 
else if ( ( (*( p  ))
) >= 241u )
	{
				goto ctr128;
			}
		
} 
else
	{
			goto ctr124;
		}
	
} 
else
	{
		goto ctr115;
	}

{
	goto ctr0;
}
ctr115:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st67;
st67:
	p+= 1;
if ( p == pe  )
	goto _test_eof67;

st_case_67:
	switch ( ( (*( p  ))
)  ) {
	case 9u:
{
goto ctr130;
	}
		 case 32u:
{
goto ctr130;
	}
		 case 45u:
{
goto ctr115;
	}
		 case 46u:
{
goto ctr131;
	}
		 case 47u:
{
goto ctr0;
	}
		 case 95u:
{
goto ctr115;
	}
		 case 96u:
{
goto ctr0;
	}
		 case 183u:
{
goto ctr115;
	}
		 case 195u:
{
goto ctr116;
	}
		 case 203u:
{
goto ctr118;
	}
		 case 204u:
{
goto ctr132;
	}
		 case 205u:
{
goto ctr133;
	}
		 case 223u:
{
goto ctr118;
	}
		 case 224u:
{
goto ctr120;
	}
		 case 225u:
{
goto ctr121;
	}
		 case 226u:
{
goto ctr134;
	}
		 case 227u:
{
goto ctr123;
	}
		 case 237u:
{
goto ctr125;
	}
		 case 238u:
{
goto ctr0;
	}
		 case 239u:
{
goto ctr126;
	}
		 case 240u:
{
goto ctr127;
	}
		 case 243u:
{
goto ctr129;
	}
	
}
if ( ( (*( p  ))
) < 91u )
	{
		if ( ( (*( p  ))
) < 33u )
	{
			if ( ( (*( p  ))
) > 8u )
	{
				if ( 10u<= ( (*( p  ))
) && ( (*( p  ))
) <= 31u )
	{
					goto ctr0;
				}
			
} 
else
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 44u )
	{
			if ( ( (*( p  ))
) < 59u )
	{
				if ( 48u<= ( (*( p  ))
)  )
	{
					goto ctr115;
				}
			
} 
else if ( ( (*( p  ))
) > 64u )
	{
				{
					goto ctr115;
				}
			} 
else
	{
				goto ctr0;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 94u )
	{
		if ( ( (*( p  ))
) < 206u )
	{
			if ( ( (*( p  ))
) < 184u )
	{
				if ( 97u<= ( (*( p  ))
) && ( (*( p  ))
) <= 122u )
	{
					goto ctr115;
				}
			
} 
else if ( ( (*( p  ))
) > 194u )
	{
				if ( 196u<= ( (*( p  ))
) && ( (*( p  ))
) <= 202u )
	{
					goto ctr117;
				}
			
} 
else
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 222u )
	{
			if ( ( (*( p  ))
) < 241u )
	{
				if ( 228u<= ( (*( p  ))
) && ( (*( p  ))
) <= 236u )
	{
					goto ctr124;
				}
			
} 
else if ( ( (*( p  ))
) > 242u )
	{
				if ( 244u<= ( (*( p  ))
)  )
	{
					goto ctr0;
				}
			
} 
else
	{
				goto ctr128;
			}
		
} 
else
	{
			goto ctr117;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr116:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st68;
st68:
	p+= 1;
if ( p == pe  )
	goto _test_eof68;

st_case_68:
	switch ( ( (*( p  ))
)  ) {
	case 151u:
{
goto ctr0;
	}
		 case 183u:
{
goto ctr0;
	}
	
}
if ( ( (*( p  ))
) < 152u )
	{
		if ( 128u<= ( (*( p  ))
) && ( (*( p  ))
) <= 150u )
	{
			goto ctr115;
		}
	
} 
else if ( ( (*( p  ))
) > 182u )
	{
		if ( 184u<= ( (*( p  ))
)  )
	{
			goto ctr115;
		}
	
} 
else
	{
		goto ctr115;
	}

{
	goto ctr0;
}
ctr117:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st69;
st69:
	p+= 1;
if ( p == pe  )
	goto _test_eof69;

st_case_69:
{
	goto ctr115;
}
ctr118:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st70;
st70:
	p+= 1;
if ( p == pe  )
	goto _test_eof70;

st_case_70:
	if ( 192u<= ( (*( p  ))
)  )
	{
		goto ctr0;
	}

{
	goto ctr115;
}
ctr132:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st71;
st71:
	p+= 1;
if ( p == pe  )
	goto _test_eof71;

st_case_71:
	if ( 128u<= ( (*( p  ))
)  )
	{
		goto ctr115;
	}

{
	goto ctr0;
}
ctr133:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st72;
st72:
	p+= 1;
if ( p == pe  )
	goto _test_eof72;

st_case_72:
	if ( ( (*( p  ))
) == 190u )
	{
		goto ctr0;
	}
	
if ( 191u<= ( (*( p  ))
)  )
	{
		goto ctr115;
	}

{
	goto ctr115;
}
ctr120:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st73;
st73:
	p+= 1;
if ( p == pe  )
	goto _test_eof73;

st_case_73:
	if ( ( (*( p  ))
) == 160u )
	{
		goto ctr132;
	}
	
if ( 161u<= ( (*( p  ))
)  )
	{
		goto ctr117;
	}

{
	goto ctr0;
}
ctr121:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st74;
st74:
	p+= 1;
if ( p == pe  )
	goto _test_eof74;

st_case_74:
	if ( ( (*( p  ))
) == 191u )
	{
		goto ctr118;
	}
	
if ( 192u<= ( (*( p  ))
)  )
	{
		goto ctr0;
	}

{
	goto ctr117;
}
ctr134:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st75;
st75:
	p+= 1;
if ( p == pe  )
	goto _test_eof75;

st_case_75:
	switch ( ( (*( p  ))
)  ) {
	case 128u:
{
goto ctr135;
	}
		 case 129u:
{
goto ctr136;
	}
		 case 134u:
{
goto ctr137;
	}
		 case 176u:
{
goto ctr132;
	}
		 case 191u:
{
goto ctr138;
	}
	
}
if ( ( (*( p  ))
) < 135u )
	{
		if ( 130u<= ( (*( p  ))
) && ( (*( p  ))
) <= 133u )
	{
			goto ctr117;
		}
	
} 
else if ( ( (*( p  ))
) > 175u )
	{
		if ( ( (*( p  ))
) > 190u )
	{
			if ( 192u<= ( (*( p  ))
)  )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) >= 177u )
	{
			goto ctr117;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr135:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st76;
st76:
	p+= 1;
if ( p == pe  )
	goto _test_eof76;

st_case_76:
	if ( ( (*( p  ))
) < 142u )
	{
		if ( 140u<= ( (*( p  ))
)  )
	{
			goto ctr115;
		}
	
} 
else if ( ( (*( p  ))
) > 190u )
	{
		{
			goto ctr115;
		}
	} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr136:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st77;
st77:
	p+= 1;
if ( p == pe  )
	goto _test_eof77;

st_case_77:
	if ( ( (*( p  ))
) > 175u )
	{
		{
			goto ctr115;
		}
	} 
else if ( ( (*( p  ))
) >= 129u )
	{
		goto ctr0;
	}

{
	goto ctr115;
}
ctr137:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st78;
st78:
	p+= 1;
if ( p == pe  )
	goto _test_eof78;

st_case_78:
	if ( 144u<= ( (*( p  ))
)  )
	{
		goto ctr0;
	}

{
	goto ctr115;
}
ctr138:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st79;
st79:
	p+= 1;
if ( p == pe  )
	goto _test_eof79;

st_case_79:
	if ( 176u<= ( (*( p  ))
)  )
	{
		goto ctr0;
	}

{
	goto ctr115;
}
ctr123:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st80;
st80:
	p+= 1;
if ( p == pe  )
	goto _test_eof80;

st_case_80:
	if ( ( (*( p  ))
) == 128u )
	{
		goto ctr139;
	}
	
if ( 129u<= ( (*( p  ))
)  )
	{
		goto ctr117;
	}

{
	goto ctr0;
}
ctr139:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st81;
st81:
	p+= 1;
if ( p == pe  )
	goto _test_eof81;

st_case_81:
	if ( 129u<= ( (*( p  ))
)  )
	{
		goto ctr115;
	}

{
	goto ctr0;
}
ctr124:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st82;
st82:
	p+= 1;
if ( p == pe  )
	goto _test_eof82;

st_case_82:
{
	goto ctr117;
}
ctr125:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st83;
st83:
	p+= 1;
if ( p == pe  )
	goto _test_eof83;

st_case_83:
	if ( ( (*( p  ))
) == 159u )
	{
		goto ctr118;
	}
	
if ( 160u<= ( (*( p  ))
)  )
	{
		goto ctr0;
	}

{
	goto ctr117;
}
ctr126:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st84;
st84:
	p+= 1;
if ( p == pe  )
	goto _test_eof84;

st_case_84:
	switch ( ( (*( p  ))
)  ) {
	case 164u:
{
goto ctr132;
	}
		 case 183u:
{
goto ctr140;
	}
		 case 191u:
{
goto ctr141;
	}
	
}
if ( ( (*( p  ))
) < 184u )
	{
		if ( 165u<= ( (*( p  ))
) && ( (*( p  ))
) <= 182u )
	{
			goto ctr117;
		}
	
} 
else if ( ( (*( p  ))
) > 190u )
	{
		if ( 192u<= ( (*( p  ))
)  )
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr117;
	}

{
	goto ctr0;
}
ctr140:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st85;
st85:
	p+= 1;
if ( p == pe  )
	goto _test_eof85;

st_case_85:
	if ( ( (*( p  ))
) > 175u )
	{
		{
			goto ctr115;
		}
	} 
else if ( ( (*( p  ))
) >= 144u )
	{
		goto ctr0;
	}

{
	goto ctr115;
}
ctr141:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st86;
st86:
	p+= 1;
if ( p == pe  )
	goto _test_eof86;

st_case_86:
	if ( 190u<= ( (*( p  ))
)  )
	{
		goto ctr0;
	}

{
	goto ctr115;
}
ctr127:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st87;
st87:
	p+= 1;
if ( p == pe  )
	goto _test_eof87;

st_case_87:
	if ( ( (*( p  ))
) == 144u )
	{
		goto ctr142;
	}
	
if ( 145u<= ( (*( p  ))
)  )
	{
		goto ctr124;
	}

{
	goto ctr0;
}
ctr142:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st88;
st88:
	p+= 1;
if ( p == pe  )
	goto _test_eof88;

st_case_88:
	if ( ( (*( p  ))
) == 128u )
	{
		goto ctr132;
	}
	
if ( 129u<= ( (*( p  ))
)  )
	{
		goto ctr117;
	}

{
	goto ctr0;
}
ctr128:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st89;
st89:
	p+= 1;
if ( p == pe  )
	goto _test_eof89;

st_case_89:
{
	goto ctr124;
}
ctr129:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st90;
st90:
	p+= 1;
if ( p == pe  )
	goto _test_eof90;

st_case_90:
	if ( ( (*( p  ))
) == 175u )
	{
		goto ctr121;
	}
	
if ( 176u<= ( (*( p  ))
)  )
	{
		goto ctr0;
	}

{
	goto ctr124;
}
ctr119:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st91;
st91:
	p+= 1;
if ( p == pe  )
	goto _test_eof91;

st_case_91:
	if ( ( (*( p  ))
) == 190u )
	{
		goto ctr0;
	}
	
if ( ( (*( p  ))
) > 189u )
	{
		if ( 191u<= ( (*( p  ))
)  )
	{
			goto ctr115;
		}
	
} 
else if ( ( (*( p  ))
) >= 176u )
	{
		goto ctr115;
	}

{
	goto ctr0;
}
ctr122:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st92;
st92:
	p+= 1;
if ( p == pe  )
	goto _test_eof92;

st_case_92:
	switch ( ( (*( p  ))
)  ) {
	case 128u:
{
goto ctr143;
	}
		 case 129u:
{
goto ctr144;
	}
		 case 134u:
{
goto ctr137;
	}
		 case 176u:
{
goto ctr132;
	}
		 case 191u:
{
goto ctr138;
	}
	
}
if ( ( (*( p  ))
) < 135u )
	{
		if ( 130u<= ( (*( p  ))
) && ( (*( p  ))
) <= 133u )
	{
			goto ctr117;
		}
	
} 
else if ( ( (*( p  ))
) > 175u )
	{
		if ( ( (*( p  ))
) > 190u )
	{
			if ( 192u<= ( (*( p  ))
)  )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) >= 177u )
	{
			goto ctr117;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr143:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st93;
st93:
	p+= 1;
if ( p == pe  )
	goto _test_eof93;

st_case_93:
	if ( ( (*( p  ))
) > 141u )
	{
		{
			goto ctr0;
		}
	} 
else if ( ( (*( p  ))
) >= 140u )
	{
		goto ctr115;
	}

{
	goto ctr0;
}
ctr144:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st94;
st94:
	p+= 1;
if ( p == pe  )
	goto _test_eof94;

st_case_94:
	if ( 176u<= ( (*( p  ))
)  )
	{
		goto ctr115;
	}

{
	goto ctr0;
}
ctr157:
	{unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 ) {
            parser->finished = 1;
        }
    }


	goto st95;
st95:
	p+= 1;
if ( p == pe  )
	goto _test_eof95;

st_case_95:
	switch ( ( (*( p  ))
)  ) {
	case 85u:
{
goto st96;
	}
		 case 117u:
{
goto st105;
	}
	
}
if ( ( (*( p  ))
) > 84u )
	{
		if ( 86u<= ( (*( p  ))
) && ( (*( p  ))
) <= 116u )
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st96:
	p+= 1;
if ( p == pe  )
	goto _test_eof96;

st_case_96:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto ctr147;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto ctr147;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto ctr147;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr147:
	{mark = p;
    }


	goto st97;
st97:
	p+= 1;
if ( p == pe  )
	goto _test_eof97;

st_case_97:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st98;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st98;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st98;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st98:
	p+= 1;
if ( p == pe  )
	goto _test_eof98;

st_case_98:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st99;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st99;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st99;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st99:
	p+= 1;
if ( p == pe  )
	goto _test_eof99;

st_case_99:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st100;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st100;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st100;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st100:
	p+= 1;
if ( p == pe  )
	goto _test_eof100;

st_case_100:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st101;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st101;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st101;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr158:
	{mark = p;
    }


	goto st101;
st101:
	p+= 1;
if ( p == pe  )
	goto _test_eof101;

st_case_101:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st102;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st102;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st102;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st102:
	p+= 1;
if ( p == pe  )
	goto _test_eof102;

st_case_102:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st103;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st103;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st103;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st103:
	p+= 1;
if ( p == pe  )
	goto _test_eof103;

st_case_103:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st104;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st104;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st104;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st104:
	p+= 1;
if ( p == pe  )
	goto _test_eof104;

st_case_104:
	switch ( ( (*( p  ))
)  ) {
	case 33u:
{
goto ctr155;
	}
		 case 34u:
{
goto ctr0;
	}
		 case 60u:
{
goto ctr0;
	}
		 case 61u:
{
goto ctr155;
	}
		 case 62u:
{
goto ctr156;
	}
		 case 92u:
{
goto ctr157;
	}
		 case 93u:
{
goto ctr155;
	}
		 case 94u:
{
goto ctr0;
	}
		 case 95u:
{
goto ctr155;
	}
		 case 96u:
{
goto ctr0;
	}
	
}
if ( ( (*( p  ))
) < 63u )
	{
		if ( ( (*( p  ))
) > 32u )
	{
			if ( 35u<= ( (*( p  ))
) && ( (*( p  ))
) <= 59u )
	{
				goto ctr155;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 91u )
	{
		if ( ( (*( p  ))
) > 122u )
	{
			if ( ( (*( p  ))
) <= 125u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) >= 97u )
	{
			goto ctr155;
		}
	
} 
else
	{
		goto ctr155;
	}

{
	goto ctr155;
}
st105:
	p+= 1;
if ( p == pe  )
	goto _test_eof105;

st_case_105:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto ctr158;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto ctr158;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto ctr158;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr171:
	{unsigned int codepoint = parse_hex( mark, p - mark );
        mark = 0;
        if ( string_append_utf8( &parser->string, codepoint) is 0 ) {
            parser->finished = 1;
        }
    }


	goto st106;
st106:
	p+= 1;
if ( p == pe  )
	goto _test_eof106;

st_case_106:
	switch ( ( (*( p  ))
)  ) {
	case 85u:
{
goto st107;
	}
		 case 117u:
{
goto st116;
	}
	
}
if ( ( (*( p  ))
) > 84u )
	{
		if ( 86u<= ( (*( p  ))
) && ( (*( p  ))
) <= 116u )
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st107:
	p+= 1;
if ( p == pe  )
	goto _test_eof107;

st_case_107:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto ctr161;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto ctr161;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto ctr161;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr161:
	{mark = p;
    }


	goto st108;
st108:
	p+= 1;
if ( p == pe  )
	goto _test_eof108;

st_case_108:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st109;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st109;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st109;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st109:
	p+= 1;
if ( p == pe  )
	goto _test_eof109;

st_case_109:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st110;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st110;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st110;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st110:
	p+= 1;
if ( p == pe  )
	goto _test_eof110;

st_case_110:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st111;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st111;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st111;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st111:
	p+= 1;
if ( p == pe  )
	goto _test_eof111;

st_case_111:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st112;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st112;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st112;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr172:
	{mark = p;
    }


	goto st112;
st112:
	p+= 1;
if ( p == pe  )
	goto _test_eof112;

st_case_112:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st113;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st113;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st113;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st113:
	p+= 1;
if ( p == pe  )
	goto _test_eof113;

st_case_113:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st114;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st114;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st114;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st114:
	p+= 1;
if ( p == pe  )
	goto _test_eof114;

st_case_114:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto st115;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto st115;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto st115;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st115:
	p+= 1;
if ( p == pe  )
	goto _test_eof115;

st_case_115:
	switch ( ( (*( p  ))
)  ) {
	case 33u:
{
goto ctr169;
	}
		 case 34u:
{
goto ctr0;
	}
		 case 60u:
{
goto ctr0;
	}
		 case 61u:
{
goto ctr169;
	}
		 case 62u:
{
goto ctr170;
	}
		 case 92u:
{
goto ctr171;
	}
		 case 93u:
{
goto ctr169;
	}
		 case 94u:
{
goto ctr0;
	}
		 case 95u:
{
goto ctr169;
	}
		 case 96u:
{
goto ctr0;
	}
	
}
if ( ( (*( p  ))
) < 63u )
	{
		if ( ( (*( p  ))
) > 32u )
	{
			if ( 35u<= ( (*( p  ))
) && ( (*( p  ))
) <= 59u )
	{
				goto ctr169;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 91u )
	{
		if ( ( (*( p  ))
) > 122u )
	{
			if ( ( (*( p  ))
) <= 125u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) >= 97u )
	{
			goto ctr169;
		}
	
} 
else
	{
		goto ctr169;
	}

{
	goto ctr169;
}
st116:
	p+= 1;
if ( p == pe  )
	goto _test_eof116;

st_case_116:
	if ( ( (*( p  ))
) < 58u )
	{
		if ( ( (*( p  ))
) > 47u )
	{
			{
				goto ctr172;
			}
		} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 64u )
	{
		if ( ( (*( p  ))
) < 71u )
	{
			{
				goto ctr172;
			}
		} 
else if ( ( (*( p  ))
) > 96u )
	{
			if ( ( (*( p  ))
) <= 102u )
	{
				goto ctr172;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr5:
	{if ( parser->string isnt NULL )
            string_dealloc( parser->string );

        utf8 string = string_new();

        parser->string = string;

        if ( parser->string is NULL )
            parser->finished = 1;
    }


	goto st117;
st117:
	p+= 1;
if ( p == pe  )
	goto _test_eof117;

st_case_117:
	if ( ( (*( p  ))
) == 58u )
	{
		goto st118;
	}
	
if ( ( (*( p  ))
) <= 57u )
	{
		goto ctr0;
	}

{
	goto ctr0;
}
st118:
	p+= 1;
if ( p == pe  )
	goto _test_eof118;

st_case_118:
	switch ( ( (*( p  ))
)  ) {
	case 95u:
{
goto ctr174;
	}
		 case 96u:
{
goto ctr0;
	}
		 case 195u:
{
goto ctr175;
	}
		 case 203u:
{
goto ctr177;
	}
		 case 204u:
{
goto ctr0;
	}
		 case 205u:
{
goto ctr178;
	}
		 case 223u:
{
goto ctr177;
	}
		 case 224u:
{
goto ctr179;
	}
		 case 225u:
{
goto ctr180;
	}
		 case 226u:
{
goto ctr181;
	}
		 case 227u:
{
goto ctr182;
	}
		 case 237u:
{
goto ctr184;
	}
		 case 238u:
{
goto ctr0;
	}
		 case 239u:
{
goto ctr185;
	}
		 case 240u:
{
goto ctr186;
	}
		 case 243u:
{
goto ctr188;
	}
	
}
if ( ( (*( p  ))
) < 97u )
	{
		if ( ( (*( p  ))
) < 59u )
	{
			if ( ( (*( p  ))
) > 47u )
	{
				{
					goto ctr174;
				}
			} 
else
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 64u )
	{
			if ( ( (*( p  ))
) > 90u )
	{
				if ( ( (*( p  ))
) <= 94u )
	{
					goto ctr0;
				}
			
} 
else
	{
				goto ctr174;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 122u )
	{
		if ( ( (*( p  ))
) < 228u )
	{
			if ( ( (*( p  ))
) > 202u )
	{
				if ( 206u<= ( (*( p  ))
) && ( (*( p  ))
) <= 222u )
	{
					goto ctr176;
				}
			
} 
else if ( ( (*( p  ))
) >= 196u )
	{
				goto ctr176;
			}
		
} 
else if ( ( (*( p  ))
) > 236u )
	{
			if ( ( (*( p  ))
) > 242u )
	{
				if ( 244u<= ( (*( p  ))
)  )
	{
					goto ctr0;
				}
			
} 
else if ( ( (*( p  ))
) >= 241u )
	{
				goto ctr187;
			}
		
} 
else
	{
			goto ctr183;
		}
	
} 
else
	{
		goto ctr174;
	}

{
	goto ctr0;
}
ctr174:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st119;
st119:
	p+= 1;
if ( p == pe  )
	goto _test_eof119;

st_case_119:
	switch ( ( (*( p  ))
)  ) {
	case 9u:
{
goto ctr189;
	}
		 case 32u:
{
goto ctr189;
	}
		 case 45u:
{
goto ctr174;
	}
		 case 46u:
{
goto ctr190;
	}
		 case 47u:
{
goto ctr0;
	}
		 case 59u:
{
goto ctr0;
	}
		 case 60u:
{
goto ctr191;
	}
		 case 95u:
{
goto ctr174;
	}
		 case 96u:
{
goto ctr0;
	}
		 case 183u:
{
goto ctr174;
	}
		 case 195u:
{
goto ctr175;
	}
		 case 203u:
{
goto ctr177;
	}
		 case 204u:
{
goto ctr192;
	}
		 case 205u:
{
goto ctr193;
	}
		 case 223u:
{
goto ctr177;
	}
		 case 224u:
{
goto ctr179;
	}
		 case 225u:
{
goto ctr180;
	}
		 case 226u:
{
goto ctr194;
	}
		 case 227u:
{
goto ctr182;
	}
		 case 237u:
{
goto ctr184;
	}
		 case 238u:
{
goto ctr0;
	}
		 case 239u:
{
goto ctr185;
	}
		 case 240u:
{
goto ctr186;
	}
		 case 243u:
{
goto ctr188;
	}
	
}
if ( ( (*( p  ))
) < 91u )
	{
		if ( ( (*( p  ))
) < 33u )
	{
			if ( ( (*( p  ))
) > 8u )
	{
				if ( 10u<= ( (*( p  ))
) && ( (*( p  ))
) <= 31u )
	{
					goto ctr0;
				}
			
} 
else
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 44u )
	{
			if ( ( (*( p  ))
) < 61u )
	{
				if ( 48u<= ( (*( p  ))
) && ( (*( p  ))
) <= 58u )
	{
					goto ctr174;
				}
			
} 
else if ( ( (*( p  ))
) > 64u )
	{
				{
					goto ctr174;
				}
			} 
else
	{
				goto ctr0;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 94u )
	{
		if ( ( (*( p  ))
) < 206u )
	{
			if ( ( (*( p  ))
) < 184u )
	{
				if ( 97u<= ( (*( p  ))
) && ( (*( p  ))
) <= 122u )
	{
					goto ctr174;
				}
			
} 
else if ( ( (*( p  ))
) > 194u )
	{
				if ( 196u<= ( (*( p  ))
) && ( (*( p  ))
) <= 202u )
	{
					goto ctr176;
				}
			
} 
else
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 222u )
	{
			if ( ( (*( p  ))
) < 241u )
	{
				if ( 228u<= ( (*( p  ))
) && ( (*( p  ))
) <= 236u )
	{
					goto ctr183;
				}
			
} 
else if ( ( (*( p  ))
) > 242u )
	{
				if ( 244u<= ( (*( p  ))
)  )
	{
					goto ctr0;
				}
			
} 
else
	{
				goto ctr187;
			}
		
} 
else
	{
			goto ctr176;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr190:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st120;
st120:
	p+= 1;
if ( p == pe  )
	goto _test_eof120;

st_case_120:
	switch ( ( (*( p  ))
)  ) {
	case 45u:
{
goto ctr174;
	}
		 case 46u:
{
goto ctr190;
	}
		 case 47u:
{
goto ctr0;
	}
		 case 95u:
{
goto ctr174;
	}
		 case 96u:
{
goto ctr0;
	}
		 case 183u:
{
goto ctr174;
	}
		 case 195u:
{
goto ctr175;
	}
		 case 203u:
{
goto ctr177;
	}
		 case 204u:
{
goto ctr192;
	}
		 case 205u:
{
goto ctr193;
	}
		 case 223u:
{
goto ctr177;
	}
		 case 224u:
{
goto ctr179;
	}
		 case 225u:
{
goto ctr180;
	}
		 case 226u:
{
goto ctr194;
	}
		 case 227u:
{
goto ctr182;
	}
		 case 237u:
{
goto ctr184;
	}
		 case 238u:
{
goto ctr0;
	}
		 case 239u:
{
goto ctr185;
	}
		 case 240u:
{
goto ctr186;
	}
		 case 243u:
{
goto ctr188;
	}
	
}
if ( ( (*( p  ))
) < 97u )
	{
		if ( ( (*( p  ))
) < 59u )
	{
			if ( ( (*( p  ))
) > 44u )
	{
				if ( 48u<= ( (*( p  ))
)  )
	{
					goto ctr174;
				}
			
} 
else
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 64u )
	{
			if ( ( (*( p  ))
) > 90u )
	{
				if ( ( (*( p  ))
) <= 94u )
	{
					goto ctr0;
				}
			
} 
else
	{
				goto ctr174;
			}
		
} 
else
	{
			goto ctr0;
		}
	
} 
else if ( ( (*( p  ))
) > 122u )
	{
		if ( ( (*( p  ))
) < 206u )
	{
			if ( ( (*( p  ))
) > 194u )
	{
				if ( 196u<= ( (*( p  ))
) && ( (*( p  ))
) <= 202u )
	{
					goto ctr176;
				}
			
} 
else if ( ( (*( p  ))
) >= 184u )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) > 222u )
	{
			if ( ( (*( p  ))
) < 241u )
	{
				if ( 228u<= ( (*( p  ))
) && ( (*( p  ))
) <= 236u )
	{
					goto ctr183;
				}
			
} 
else if ( ( (*( p  ))
) > 242u )
	{
				if ( 244u<= ( (*( p  ))
)  )
	{
					goto ctr0;
				}
			
} 
else
	{
				goto ctr187;
			}
		
} 
else
	{
			goto ctr176;
		}
	
} 
else
	{
		goto ctr174;
	}

{
	goto ctr0;
}
ctr175:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st121;
st121:
	p+= 1;
if ( p == pe  )
	goto _test_eof121;

st_case_121:
	switch ( ( (*( p  ))
)  ) {
	case 151u:
{
goto ctr0;
	}
		 case 183u:
{
goto ctr0;
	}
	
}
if ( ( (*( p  ))
) < 152u )
	{
		if ( 128u<= ( (*( p  ))
) && ( (*( p  ))
) <= 150u )
	{
			goto ctr174;
		}
	
} 
else if ( ( (*( p  ))
) > 182u )
	{
		if ( 184u<= ( (*( p  ))
)  )
	{
			goto ctr174;
		}
	
} 
else
	{
		goto ctr174;
	}

{
	goto ctr0;
}
ctr176:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st122;
st122:
	p+= 1;
if ( p == pe  )
	goto _test_eof122;

st_case_122:
{
	goto ctr174;
}
ctr177:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st123;
st123:
	p+= 1;
if ( p == pe  )
	goto _test_eof123;

st_case_123:
	if ( 192u<= ( (*( p  ))
)  )
	{
		goto ctr0;
	}

{
	goto ctr174;
}
ctr192:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st124;
st124:
	p+= 1;
if ( p == pe  )
	goto _test_eof124;

st_case_124:
	if ( 128u<= ( (*( p  ))
)  )
	{
		goto ctr174;
	}

{
	goto ctr0;
}
ctr193:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st125;
st125:
	p+= 1;
if ( p == pe  )
	goto _test_eof125;

st_case_125:
	if ( ( (*( p  ))
) == 190u )
	{
		goto ctr0;
	}
	
if ( 191u<= ( (*( p  ))
)  )
	{
		goto ctr174;
	}

{
	goto ctr174;
}
ctr179:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st126;
st126:
	p+= 1;
if ( p == pe  )
	goto _test_eof126;

st_case_126:
	if ( ( (*( p  ))
) == 160u )
	{
		goto ctr192;
	}
	
if ( 161u<= ( (*( p  ))
)  )
	{
		goto ctr176;
	}

{
	goto ctr0;
}
ctr180:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st127;
st127:
	p+= 1;
if ( p == pe  )
	goto _test_eof127;

st_case_127:
	if ( ( (*( p  ))
) == 191u )
	{
		goto ctr177;
	}
	
if ( 192u<= ( (*( p  ))
)  )
	{
		goto ctr0;
	}

{
	goto ctr176;
}
ctr194:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st128;
st128:
	p+= 1;
if ( p == pe  )
	goto _test_eof128;

st_case_128:
	switch ( ( (*( p  ))
)  ) {
	case 128u:
{
goto ctr195;
	}
		 case 129u:
{
goto ctr196;
	}
		 case 134u:
{
goto ctr197;
	}
		 case 176u:
{
goto ctr192;
	}
		 case 191u:
{
goto ctr198;
	}
	
}
if ( ( (*( p  ))
) < 135u )
	{
		if ( 130u<= ( (*( p  ))
) && ( (*( p  ))
) <= 133u )
	{
			goto ctr176;
		}
	
} 
else if ( ( (*( p  ))
) > 175u )
	{
		if ( ( (*( p  ))
) > 190u )
	{
			if ( 192u<= ( (*( p  ))
)  )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) >= 177u )
	{
			goto ctr176;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr195:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st129;
st129:
	p+= 1;
if ( p == pe  )
	goto _test_eof129;

st_case_129:
	if ( ( (*( p  ))
) < 142u )
	{
		if ( 140u<= ( (*( p  ))
)  )
	{
			goto ctr174;
		}
	
} 
else if ( ( (*( p  ))
) > 190u )
	{
		{
			goto ctr174;
		}
	} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr196:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st130;
st130:
	p+= 1;
if ( p == pe  )
	goto _test_eof130;

st_case_130:
	if ( ( (*( p  ))
) > 175u )
	{
		{
			goto ctr174;
		}
	} 
else if ( ( (*( p  ))
) >= 129u )
	{
		goto ctr0;
	}

{
	goto ctr174;
}
ctr197:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st131;
st131:
	p+= 1;
if ( p == pe  )
	goto _test_eof131;

st_case_131:
	if ( 144u<= ( (*( p  ))
)  )
	{
		goto ctr0;
	}

{
	goto ctr174;
}
ctr198:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st132;
st132:
	p+= 1;
if ( p == pe  )
	goto _test_eof132;

st_case_132:
	if ( 176u<= ( (*( p  ))
)  )
	{
		goto ctr0;
	}

{
	goto ctr174;
}
ctr182:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st133;
st133:
	p+= 1;
if ( p == pe  )
	goto _test_eof133;

st_case_133:
	if ( ( (*( p  ))
) == 128u )
	{
		goto ctr199;
	}
	
if ( 129u<= ( (*( p  ))
)  )
	{
		goto ctr176;
	}

{
	goto ctr0;
}
ctr199:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st134;
st134:
	p+= 1;
if ( p == pe  )
	goto _test_eof134;

st_case_134:
	if ( 129u<= ( (*( p  ))
)  )
	{
		goto ctr174;
	}

{
	goto ctr0;
}
ctr183:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st135;
st135:
	p+= 1;
if ( p == pe  )
	goto _test_eof135;

st_case_135:
{
	goto ctr176;
}
ctr184:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st136;
st136:
	p+= 1;
if ( p == pe  )
	goto _test_eof136;

st_case_136:
	if ( ( (*( p  ))
) == 159u )
	{
		goto ctr177;
	}
	
if ( 160u<= ( (*( p  ))
)  )
	{
		goto ctr0;
	}

{
	goto ctr176;
}
ctr185:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st137;
st137:
	p+= 1;
if ( p == pe  )
	goto _test_eof137;

st_case_137:
	switch ( ( (*( p  ))
)  ) {
	case 164u:
{
goto ctr192;
	}
		 case 183u:
{
goto ctr200;
	}
		 case 191u:
{
goto ctr201;
	}
	
}
if ( ( (*( p  ))
) < 184u )
	{
		if ( 165u<= ( (*( p  ))
) && ( (*( p  ))
) <= 182u )
	{
			goto ctr176;
		}
	
} 
else if ( ( (*( p  ))
) > 190u )
	{
		if ( 192u<= ( (*( p  ))
)  )
	{
			goto ctr0;
		}
	
} 
else
	{
		goto ctr176;
	}

{
	goto ctr0;
}
ctr200:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st138;
st138:
	p+= 1;
if ( p == pe  )
	goto _test_eof138;

st_case_138:
	if ( ( (*( p  ))
) > 175u )
	{
		{
			goto ctr174;
		}
	} 
else if ( ( (*( p  ))
) >= 144u )
	{
		goto ctr0;
	}

{
	goto ctr174;
}
ctr201:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st139;
st139:
	p+= 1;
if ( p == pe  )
	goto _test_eof139;

st_case_139:
	if ( 190u<= ( (*( p  ))
)  )
	{
		goto ctr0;
	}

{
	goto ctr174;
}
ctr186:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st140;
st140:
	p+= 1;
if ( p == pe  )
	goto _test_eof140;

st_case_140:
	if ( ( (*( p  ))
) == 144u )
	{
		goto ctr202;
	}
	
if ( 145u<= ( (*( p  ))
)  )
	{
		goto ctr183;
	}

{
	goto ctr0;
}
ctr202:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st141;
st141:
	p+= 1;
if ( p == pe  )
	goto _test_eof141;

st_case_141:
	if ( ( (*( p  ))
) == 128u )
	{
		goto ctr192;
	}
	
if ( 129u<= ( (*( p  ))
)  )
	{
		goto ctr176;
	}

{
	goto ctr0;
}
ctr187:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st142;
st142:
	p+= 1;
if ( p == pe  )
	goto _test_eof142;

st_case_142:
{
	goto ctr183;
}
ctr188:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st143;
st143:
	p+= 1;
if ( p == pe  )
	goto _test_eof143;

st_case_143:
	if ( ( (*( p  ))
) == 175u )
	{
		goto ctr180;
	}
	
if ( 176u<= ( (*( p  ))
)  )
	{
		goto ctr0;
	}

{
	goto ctr183;
}
ctr178:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st144;
st144:
	p+= 1;
if ( p == pe  )
	goto _test_eof144;

st_case_144:
	if ( ( (*( p  ))
) == 190u )
	{
		goto ctr0;
	}
	
if ( ( (*( p  ))
) > 189u )
	{
		if ( 191u<= ( (*( p  ))
)  )
	{
			goto ctr174;
		}
	
} 
else if ( ( (*( p  ))
) >= 176u )
	{
		goto ctr174;
	}

{
	goto ctr0;
}
ctr181:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st145;
st145:
	p+= 1;
if ( p == pe  )
	goto _test_eof145;

st_case_145:
	switch ( ( (*( p  ))
)  ) {
	case 128u:
{
goto ctr203;
	}
		 case 129u:
{
goto ctr204;
	}
		 case 134u:
{
goto ctr197;
	}
		 case 176u:
{
goto ctr192;
	}
		 case 191u:
{
goto ctr198;
	}
	
}
if ( ( (*( p  ))
) < 135u )
	{
		if ( 130u<= ( (*( p  ))
) && ( (*( p  ))
) <= 133u )
	{
			goto ctr176;
		}
	
} 
else if ( ( (*( p  ))
) > 175u )
	{
		if ( ( (*( p  ))
) > 190u )
	{
			if ( 192u<= ( (*( p  ))
)  )
	{
				goto ctr0;
			}
		
} 
else if ( ( (*( p  ))
) >= 177u )
	{
			goto ctr176;
		}
	
} 
else
	{
		goto ctr0;
	}

{
	goto ctr0;
}
ctr203:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st146;
st146:
	p+= 1;
if ( p == pe  )
	goto _test_eof146;

st_case_146:
	if ( ( (*( p  ))
) > 141u )
	{
		{
			goto ctr0;
		}
	} 
else if ( ( (*( p  ))
) >= 140u )
	{
		goto ctr174;
	}

{
	goto ctr0;
}
ctr204:
	{if ( string_append_char( &parser->string, *p ) is false )
            parser->finished = 1;
    }


	goto st147;
st147:
	p+= 1;
if ( p == pe  )
	goto _test_eof147;

st_case_147:
	if ( 176u<= ( (*( p  ))
)  )
	{
		goto ctr174;
	}

{
	goto ctr0;
}
st148:
	p+= 1;
if ( p == pe  )
	goto _test_eof148;

st_case_148:
	switch ( ( (*( p  ))
)  ) {
	case 10u:
{
goto ctr206;
	}
		 case 13u:
{
goto ctr206;
	}
	
}
if ( ( (*( p  ))
) > 9u )
	{
		if ( 11u<= ( (*( p  ))
) && ( (*( p  ))
) <= 12u )
	{
			goto st148;
		}
	
} 
else
	{
		goto st148;
	}

{
	goto st148;
}
ctr206:
	{parser->line++;
    }
{{goto st149;}}


	goto st150;
st150:
	p+= 1;
if ( p == pe  )
	goto _test_eof150;

st_case_150:
{
	goto st0;
}
	st_out:
	_test_eof149: cs = 149;
goto _test_eof; 
	_test_eof1: cs = 1;
goto _test_eof; 
	_test_eof2: cs = 2;
goto _test_eof; 
	_test_eof3: cs = 3;
goto _test_eof; 
	_test_eof4: cs = 4;
goto _test_eof; 
	_test_eof5: cs = 5;
goto _test_eof; 
	_test_eof6: cs = 6;
goto _test_eof; 
	_test_eof7: cs = 7;
goto _test_eof; 
	_test_eof8: cs = 8;
goto _test_eof; 
	_test_eof9: cs = 9;
goto _test_eof; 
	_test_eof10: cs = 10;
goto _test_eof; 
	_test_eof11: cs = 11;
goto _test_eof; 
	_test_eof12: cs = 12;
goto _test_eof; 
	_test_eof13: cs = 13;
goto _test_eof; 
	_test_eof14: cs = 14;
goto _test_eof; 
	_test_eof15: cs = 15;
goto _test_eof; 
	_test_eof16: cs = 16;
goto _test_eof; 
	_test_eof17: cs = 17;
goto _test_eof; 
	_test_eof18: cs = 18;
goto _test_eof; 
	_test_eof19: cs = 19;
goto _test_eof; 
	_test_eof20: cs = 20;
goto _test_eof; 
	_test_eof21: cs = 21;
goto _test_eof; 
	_test_eof22: cs = 22;
goto _test_eof; 
	_test_eof23: cs = 23;
goto _test_eof; 
	_test_eof24: cs = 24;
goto _test_eof; 
	_test_eof25: cs = 25;
goto _test_eof; 
	_test_eof26: cs = 26;
goto _test_eof; 
	_test_eof27: cs = 27;
goto _test_eof; 
	_test_eof28: cs = 28;
goto _test_eof; 
	_test_eof29: cs = 29;
goto _test_eof; 
	_test_eof30: cs = 30;
goto _test_eof; 
	_test_eof31: cs = 31;
goto _test_eof; 
	_test_eof32: cs = 32;
goto _test_eof; 
	_test_eof33: cs = 33;
goto _test_eof; 
	_test_eof34: cs = 34;
goto _test_eof; 
	_test_eof35: cs = 35;
goto _test_eof; 
	_test_eof36: cs = 36;
goto _test_eof; 
	_test_eof37: cs = 37;
goto _test_eof; 
	_test_eof38: cs = 38;
goto _test_eof; 
	_test_eof39: cs = 39;
goto _test_eof; 
	_test_eof40: cs = 40;
goto _test_eof; 
	_test_eof41: cs = 41;
goto _test_eof; 
	_test_eof42: cs = 42;
goto _test_eof; 
	_test_eof43: cs = 43;
goto _test_eof; 
	_test_eof44: cs = 44;
goto _test_eof; 
	_test_eof45: cs = 45;
goto _test_eof; 
	_test_eof46: cs = 46;
goto _test_eof; 
	_test_eof47: cs = 47;
goto _test_eof; 
	_test_eof48: cs = 48;
goto _test_eof; 
	_test_eof49: cs = 49;
goto _test_eof; 
	_test_eof50: cs = 50;
goto _test_eof; 
	_test_eof51: cs = 51;
goto _test_eof; 
	_test_eof52: cs = 52;
goto _test_eof; 
	_test_eof53: cs = 53;
goto _test_eof; 
	_test_eof54: cs = 54;
goto _test_eof; 
	_test_eof55: cs = 55;
goto _test_eof; 
	_test_eof56: cs = 56;
goto _test_eof; 
	_test_eof57: cs = 57;
goto _test_eof; 
	_test_eof58: cs = 58;
goto _test_eof; 
	_test_eof59: cs = 59;
goto _test_eof; 
	_test_eof60: cs = 60;
goto _test_eof; 
	_test_eof61: cs = 61;
goto _test_eof; 
	_test_eof62: cs = 62;
goto _test_eof; 
	_test_eof63: cs = 63;
goto _test_eof; 
	_test_eof64: cs = 64;
goto _test_eof; 
	_test_eof65: cs = 65;
goto _test_eof; 
	_test_eof66: cs = 66;
goto _test_eof; 
	_test_eof67: cs = 67;
goto _test_eof; 
	_test_eof68: cs = 68;
goto _test_eof; 
	_test_eof69: cs = 69;
goto _test_eof; 
	_test_eof70: cs = 70;
goto _test_eof; 
	_test_eof71: cs = 71;
goto _test_eof; 
	_test_eof72: cs = 72;
goto _test_eof; 
	_test_eof73: cs = 73;
goto _test_eof; 
	_test_eof74: cs = 74;
goto _test_eof; 
	_test_eof75: cs = 75;
goto _test_eof; 
	_test_eof76: cs = 76;
goto _test_eof; 
	_test_eof77: cs = 77;
goto _test_eof; 
	_test_eof78: cs = 78;
goto _test_eof; 
	_test_eof79: cs = 79;
goto _test_eof; 
	_test_eof80: cs = 80;
goto _test_eof; 
	_test_eof81: cs = 81;
goto _test_eof; 
	_test_eof82: cs = 82;
goto _test_eof; 
	_test_eof83: cs = 83;
goto _test_eof; 
	_test_eof84: cs = 84;
goto _test_eof; 
	_test_eof85: cs = 85;
goto _test_eof; 
	_test_eof86: cs = 86;
goto _test_eof; 
	_test_eof87: cs = 87;
goto _test_eof; 
	_test_eof88: cs = 88;
goto _test_eof; 
	_test_eof89: cs = 89;
goto _test_eof; 
	_test_eof90: cs = 90;
goto _test_eof; 
	_test_eof91: cs = 91;
goto _test_eof; 
	_test_eof92: cs = 92;
goto _test_eof; 
	_test_eof93: cs = 93;
goto _test_eof; 
	_test_eof94: cs = 94;
goto _test_eof; 
	_test_eof95: cs = 95;
goto _test_eof; 
	_test_eof96: cs = 96;
goto _test_eof; 
	_test_eof97: cs = 97;
goto _test_eof; 
	_test_eof98: cs = 98;
goto _test_eof; 
	_test_eof99: cs = 99;
goto _test_eof; 
	_test_eof100: cs = 100;
goto _test_eof; 
	_test_eof101: cs = 101;
goto _test_eof; 
	_test_eof102: cs = 102;
goto _test_eof; 
	_test_eof103: cs = 103;
goto _test_eof; 
	_test_eof104: cs = 104;
goto _test_eof; 
	_test_eof105: cs = 105;
goto _test_eof; 
	_test_eof106: cs = 106;
goto _test_eof; 
	_test_eof107: cs = 107;
goto _test_eof; 
	_test_eof108: cs = 108;
goto _test_eof; 
	_test_eof109: cs = 109;
goto _test_eof; 
	_test_eof110: cs = 110;
goto _test_eof; 
	_test_eof111: cs = 111;
goto _test_eof; 
	_test_eof112: cs = 112;
goto _test_eof; 
	_test_eof113: cs = 113;
goto _test_eof; 
	_test_eof114: cs = 114;
goto _test_eof; 
	_test_eof115: cs = 115;
goto _test_eof; 
	_test_eof116: cs = 116;
goto _test_eof; 
	_test_eof117: cs = 117;
goto _test_eof; 
	_test_eof118: cs = 118;
goto _test_eof; 
	_test_eof119: cs = 119;
goto _test_eof; 
	_test_eof120: cs = 120;
goto _test_eof; 
	_test_eof121: cs = 121;
goto _test_eof; 
	_test_eof122: cs = 122;
goto _test_eof; 
	_test_eof123: cs = 123;
goto _test_eof; 
	_test_eof124: cs = 124;
goto _test_eof; 
	_test_eof125: cs = 125;
goto _test_eof; 
	_test_eof126: cs = 126;
goto _test_eof; 
	_test_eof127: cs = 127;
goto _test_eof; 
	_test_eof128: cs = 128;
goto _test_eof; 
	_test_eof129: cs = 129;
goto _test_eof; 
	_test_eof130: cs = 130;
goto _test_eof; 
	_test_eof131: cs = 131;
goto _test_eof; 
	_test_eof132: cs = 132;
goto _test_eof; 
	_test_eof133: cs = 133;
goto _test_eof; 
	_test_eof134: cs = 134;
goto _test_eof; 
	_test_eof135: cs = 135;
goto _test_eof; 
	_test_eof136: cs = 136;
goto _test_eof; 
	_test_eof137: cs = 137;
goto _test_eof; 
	_test_eof138: cs = 138;
goto _test_eof; 
	_test_eof139: cs = 139;
goto _test_eof; 
	_test_eof140: cs = 140;
goto _test_eof; 
	_test_eof141: cs = 141;
goto _test_eof; 
	_test_eof142: cs = 142;
goto _test_eof; 
	_test_eof143: cs = 143;
goto _test_eof; 
	_test_eof144: cs = 144;
goto _test_eof; 
	_test_eof145: cs = 145;
goto _test_eof; 
	_test_eof146: cs = 146;
goto _test_eof; 
	_test_eof147: cs = 147;
goto _test_eof; 
	_test_eof148: cs = 148;
goto _test_eof; 
	_test_eof150: cs = 150;
goto _test_eof; 

	_test_eof: {}
	if ( p == eof  )
	{
	switch ( cs  ) {
	case 1:
case 2:
case 3:
case 4:
case 5:
case 6:
case 7:
case 8:
case 9:
case 10:
case 11:
case 12:
case 13:
case 14:
case 15:
case 16:
case 17:
case 18:
case 19:
case 20:
case 21:
case 22:
case 23:
case 24:
case 25:
case 26:
case 27:
case 28:
case 29:
case 30:
case 31:
case 32:
case 33:
case 34:
case 35:
case 36:
case 37:
case 38:
case 39:
case 40:
case 41:
case 42:
case 43:
case 44:
case 45:
case 46:
case 47:
case 48:
case 49:
case 50:
case 51:
case 52:
case 53:
case 54:
case 55:
case 56:
case 57:
case 58:
case 59:
case 60:
case 61:
case 62:
case 63:
case 64:
case 65:
case 66:
case 67:
case 68:
case 69:
case 70:
case 71:
case 72:
case 73:
case 74:
case 75:
case 76:
case 77:
case 78:
case 79:
case 80:
case 81:
case 82:
case 83:
case 84:
case 85:
case 86:
case 87:
case 88:
case 89:
case 90:
case 91:
case 92:
case 93:
case 94:
case 95:
case 96:
case 97:
case 98:
case 99:
case 100:
case 101:
case 102:
case 103:
case 104:
case 105:
case 106:
case 107:
case 108:
case 109:
case 110:
case 111:
case 112:
case 113:
case 114:
case 115:
case 116:
case 117:
case 118:
case 119:
case 120:
case 121:
case 122:
case 123:
case 124:
case 125:
case 126:
case 127:
case 128:
case 129:
case 130:
case 131:
case 132:
case 133:
case 134:
case 135:
case 136:
case 137:
case 138:
case 139:
case 140:
case 141:
case 142:
case 143:
case 144:
case 145:
case 146:
case 147:
{printf(_CLR_RED "[ERROR]" _CLR_RESET " on line: %d\n", parser->line );
//        parser_error_callback(parser->line);
        { p = p - 1;
} {goto st148;}}

	break;
	
}
}

	
_out: {}
	}
if ( cs is ntriples_error ) {
            res = 0;
            break;
        } else if (mark) {
            have = pe - mark;
            memmove( buf, mark, have );
            mark = buf;
        } else {
            have = 0;
        }
    }

    return res;
}
