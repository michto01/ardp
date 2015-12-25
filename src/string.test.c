#import "string.h"

void test_string(void) {
    utf8 str = string_new();

    string_debug( str );
    string_append_char( str, '<'); string_debug( str );
    string_append_char( str, 'h'); string_debug( str );
    string_append_char( str, 't'); string_debug( str );
    string_append_char( str, 't'); string_debug( str );
    string_append_char( str, 'p'); string_debug( str );
    string_append_char( str, ':'); string_debug( str );
    string_append_char( str, '/'); string_debug( str );
    string_append_char( str, '/'); string_debug( str );
    string_append_char( str, 'g'); string_debug( str );
    string_append_char( str, 'o'); string_debug( str );
    string_append_char( str, 'o'); string_debug( str );
    string_append_char( str, 'g'); string_debug( str );
    string_append_char( str, 'l'); string_debug( str );
    string_append_char( str, 'e'); string_debug( str );
    string_append_char( str, '.'); string_debug( str );
    string_append_char( str, 'c'); string_debug( str );
    string_append_char( str, 'o'); string_debug( str );
    string_append_char( str, 'm'); string_debug( str );
    string_append_char( str, '/'); string_debug( str );
    string_append_char( str, '?'); string_debug( str );
    string_append_char( str, 's'); string_debug( str );
    string_append_char( str, '='); string_debug( str );
    string_append_char( str, 's'); string_debug( str );
    string_append_char( str, 'a'); string_debug( str );
    string_append_char( str, 'x'); string_debug( str );
    string_append_char( str, '>'); string_debug( str );

    string_finish(str);
    string_dealloc(str);
}
