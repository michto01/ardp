#include "string.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#define MAX_PREALLOC     (1024 * 1024)
#define INITIAL_CAPACITY  8

#define string_hdr(ptr) &((string_header_t *) ptr)[-1]

/*
 * Length of the string.
 */
size_t string_strlen( utf8 str ) {
    struct string_header *hdr = string_hdr( str );
    return hdr->length;
}

void string_debug( utf8 str ) {
    struct string_header *hdr = string_hdr( str );
    printf(CLR_GREEN  "Capacity:"
           CLR_RESET  "%3lu" ",\t"
           CLR_YELLOW "Lenght:"
           CLR_RESET  "%3lu" ",\t"
           CLR_MAGENTA"Content:"
           CLR_RESET  "%s \n",
              hdr->capacity,
              hdr->length,
              str
          );
}

/*
 * Allocate the string with the prefered length.
 */
utf8 string_alloc( size_t len ) {
    /*
     *  Zero the input (save one memset call) on initialization and initialize
     *  with length specified + space for last '\0' character. ((<-Valgrind))
     *
     *  sizeof(char) or sizeof(uint8_t) is 1 so we can ommit it.
     */
    struct string_header *hdr = calloc(1, sizeof(string_header_t) + len + 1);
    assert( hdr );
    hdr->capacity = len;
    hdr->length   = 0;
    return (utf8) (&hdr[1]);  //((utf8) hdr) + sizeof(string_header_t);
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
    free( string_hdr(self) );
}

/*
 * Resize string
 */
bool string_resize( utf8 *str, size_t room ) {
    struct string_header *hdr = string_hdr( *str );

    size_t cap = hdr->capacity;
    size_t len = hdr->length;

    if ( len + room <= cap )
        return ARDP_SUCCESS;

    cap = len + room;

    if ( cap < MAX_PREALLOC ) {
        cap *= 2;
    } else {
        cap += MAX_PREALLOC;
    }

    string_header_t* r = realloc( hdr, sizeof(string_header_t) + cap + 1 );

    if ( r is NULL )
        return ARDP_FAILURE;

    *str = (utf8) r + sizeof(string_header_t);
    hdr = string_hdr( *str );
    hdr->capacity = cap;

    /* Remove garbage if there is any  after the string content */
    memset( *str+len, 0, cap-len + 1 );
    return ARDP_SUCCESS;
}

bool string_append_char( utf8 *str, char c ) {
    if ( string_resize(str, 1) isnt ARDP_SUCCESS )
        return ARDP_FAILURE;

    string_push( *str, c );
    string_finish( *str );
    return ARDP_SUCCESS;
}

bool string_append_utf8( utf8 *s, int cp ) {
    if ( cp < 0 or cp > 0x10ffff ) {
        return ARDP_FAILURE;
    }
    else if ( cp < 0x80 ) {
        return string_append_char(s, cp & 0x7F);
    }
    else if ( cp < 0x800 ) {
        if ( string_resize( s, 2 ) isnt ARDP_SUCCESS )
            return ARDP_FAILURE;
        string_push( *s, 0xC0 | ((cp >> 6) & 0x1F) );
        string_push( *s, 0x80 | (cp & 0x3F) );
    }
    else if ( cp < 0x10000 ) {
        if ( string_resize( s, 3 ) isnt ARDP_SUCCESS )
            return ARDP_FAILURE;
        string_push( *s, 0xE0 | ((cp >> 12) & 0xF) );
        string_push( *s, 0x80 | ((cp >> 6)  & 0x3F) );
        string_push( *s, 0x80 |  (cp & 0x3F) );
    }
    else {
        if ( string_resize( s, 4 ) isnt ARDP_SUCCESS )
            return ARDP_FAILURE;
        string_push( *s, 0xF0 | ((cp >> 18) & 0x7) );
        string_push( *s, 0x80 | ((cp >> 12) & 0x3F) );
        string_push( *s, 0x80 | ((cp >> 6)  & 0x3F) );
        string_push( *s, 0x80 |  (cp & 0x3F) );
    }
    string_finish( *s );
    return ARDP_SUCCESS;
}

void string_finish( utf8 str ) {
    string_header_t *hdr =  string_hdr( str );
    *(str + hdr->length) = '\0';
}
