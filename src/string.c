#include "string.h"

#define MAX_PREALLOC     (1024 * 1024)
#define INITIAL_CAPACITY  8

/*
 * Length of the string.
 */
size_t string_strlen( utf8 str ) {
    struct string_header *hdr = (string_header_t *) (str - sizeof(string_header_t));
    return hdr->length;
}

void string_debug( utf8 str ) {
    struct string_header *hdr = (string_header_t *) (str - sizeof(string_header_t));
    printf(CLR_GREEN  "Capacity:"
           CLR_RESET  "%3lu" ",\t"
           CLR_YELLOW "Lenght:"
           CLR_RESET  "%3lu" ",\t"
           CLR_MAGENTA"Content:"
           CLR_RESET  "%s\n",
              hdr->capacity,
              hdr->length,
              str
          );
}

/*
 * Allocate the string with the prefered length.
 */
utf8 string_alloc( size_t len ) {
    struct string_header *hdr = calloc(1, sizeof(string_header_t) + sizeof(uint8_t) * len);
    assert( hdr );
    hdr->capacity = len;
    hdr->length   = 0;
    return ((utf8) hdr) + sizeof(string_header_t);
}

/*
 * Allocate the new string with the initial default capacity.
 */
utf8 string_new() {
    return string_alloc( INITIAL_CAPACITY );
}

/*
 * Delete the string.
 */
void string_dealloc( utf8 self ) {
    if ( self is NULL )
        return;
    string_header_t *hdr = (string_header_t *) (self - sizeof(string_header_t));
    free(hdr);
}

bool string_resize( utf8 *str, size_t room ) {
    struct string_header *hdr = (string_header_t *) (*str - sizeof(string_header_t));

    size_t cap = hdr->capacity;
    size_t len = hdr->length;

    if ( len + room <= cap )
        return true;

    cap = len + room;

    if ( cap < MAX_PREALLOC ) {
        cap *= 2;
    } else {
        cap += MAX_PREALLOC;
    }

    void * old = hdr;
    void * new = realloc( old, sizeof(string_header_t) + cap + 1 );

    if ( new is NULL )
        return false;

    *str = (utf8) new + sizeof(string_header_t);
    string_header_t *nhdr = (string_header_t *) (*str - sizeof(string_header_t));
    nhdr->capacity = cap;
    return true;
}

bool string_append_char( utf8 str, char c ) {
    if ( string_resize(&str, 1) isnt ARDP_SUCCESS )
        return ARDP_FAILURE;

    string_push( str, c );
    string_finish( str );
    return ARDP_SUCCESS;
}

bool string_append_utf8( utf8 s, int cp ) {
    if ( cp < 0 or cp > 0x10ffff ) {
        return false;
    }
    else if ( cp < 0x80 ) {
        return string_append_char(s, cp & 0x7F);
    }
    else if ( cp < 0x800 ) {
        if ( string_resize( &s, 2 ) isnt ARDP_SUCCESS )
            return false;
        string_push( s, 0xC0 | ((cp >> 6) & 0x1F) );
        string_push( s, 0x80 | (cp & 0x3F) );
    }
    else if ( cp < 0x10000 ) {
        if ( string_resize( &s, 3 ) isnt ARDP_SUCCESS )
            return false;
        string_push( s, 0xE0 | ((cp >> 12) & 0xF) );
        string_push( s, 0x80 | ((cp >> 6)  & 0x3F) );
        string_push( s, 0x80 |  (cp & 0x3F) );
    }
    else {
        if ( string_resize( &s, 4 ) isnt ARDP_SUCCESS )
            return false;
        string_push( s, 0xF0 | ((cp >> 18) & 0x7) );
        string_push( s, 0x80 | ((cp >> 12) & 0x3F) );
        string_push( s, 0x80 | ((cp >> 6)  & 0x3F) );
        string_push( s, 0x80 |  (cp & 0x3F) );
    }
    string_finish(s);
    return true;
}

bool string_finish( utf8 str ) {
    if ( string_resize(&str, 1) )
        return false;

    string_header_t *hdr = (string_header_t *) (str - sizeof(string_header_t));

    *(str + hdr->length) = (uint8_t) '\0';
    return true;
}
