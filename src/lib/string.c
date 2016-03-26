/*! @file string.c
 *
 * Implements the 'fat-pointer' string interface.
 *
 * @author Tomas Michalek <tomas.michalek.st@vsb.cz>
 * @date   2015
 */

#include <ardp/string.h>
#include <ardp/color.h>

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <errno.h>

#define MAX_PREALLOC ( 1024 * 1024 )
#define INITIAL_CAPACITY 8


/*
 * Length of the string.
 */
size_t string_strlen( utf8 str ) {
        struct string_header *hdr = string_hdr( str );
        return hdr->length;
}

void string_debug( utf8 str ) {
        struct string_header *hdr = string_hdr( str );

        ardp_fprintf( stderr, ARDP_COLOR_GREEN, "Capacity: " );
        ardp_fprintf( stderr, ARDP_COLOR_NORMAL, "%3lu ", hdr->capacity );
        ardp_fprintf( stderr, ARDP_COLOR_YELLOW, "Lenght: " );
        ardp_fprintf( stderr, ARDP_COLOR_NORMAL, "%3lu ", hdr->length );
        ardp_fprintf( stderr, ARDP_COLOR_MAGENTA, "Content: " );
        ardp_fprintf( stderr, ARDP_COLOR_NORMAL, "%s \n", str );
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
        struct string_header *hdr = calloc( 1, sizeof( string_header_t ) + len + 1 );
        assert( hdr );
        hdr->capacity = len;
        hdr->length = 0;
        return ( utf8 )( &hdr[1] ); //((utf8) hdr) + sizeof(string_header_t);
}

utf8 string_create_n(const uint8_t* s, size_t n)
{
        struct string_header *hdr = calloc(1, sizeof(*hdr)  + (n + 1));
        if ( !hdr )
                return NULL;

        hdr->capacity = n;
        hdr->length   = n;

        utf8 tmp = (utf8)(&hdr[1]);

        memcpy(tmp, s, n);

        return (utf8) tmp;
}

utf8 string_create(const uint8_t* s)
{
        size_t len = strlen(s);

        return string_create_n(s,len);
}

//
// String JOIN manipulation
//

/*! @fn    string_append
 *  @brief Append C string to ardp string container.
 *
 *  Appends c string buffer to string. The string is resized if necessary. The
 *  string buffer being appended needs to be NULL terminated.
 *
 *  @param[in,out] src Pointer to string pointer. (indirection)
 *  @param[in]     mod C string buffer to be appended.
 *
 *  @return 0 on success, non-zero value if there is error.
 */
int string_append(utf8 *src, const uint8_t* mod)
{
        int len = strlen(mod);

        struct string_header *str = string_hdr(*src);
        struct string_header *s;

        size_t strlen = (str->length + len + 1);
        if ( str->capacity < strlen ) {
                s = realloc(str, sizeof(*s) + strlen);
                if (s == NULL)
                        return 1;

                s->capacity = strlen;
                *src = (utf8)(&s[1]);
                str = s;
        }

        memcpy(*src + str->length, mod, len);
        str->length = strlen - 1; // remove length of the terminator.
        return 0;
}

/*! @fn    string_prepend
 *  @brief Append the C string buffer to the beginning of the ARDP string.
 *
 *
 *  This function creates new string with the content of the C string buffer
 *  and then appends the rest using the `string_append` function. As the
 *  new string already has space for the new string, it doesn't require second
 *  realloc.
 *
 *  @param[in,out] src Source string to be appended to.
 *  @param[in]     str C string buffer to be prepended.
 *
 *  @return 0 on success, non-zero value otherwise.
 *
 *  @note err=1 - couldn't create new string.
 *  @note err=2 - couldn't append the string.
 */
int string_prepend(utf8 *src, const uint8_t* str)
{
        struct string_header *s = string_hdr(*src);

        size_t len = strlen(str);
        size_t req = len + s->length + 1;

        struct string_header *n = string_hdr(string_create_n(str, req));
        if (!n)
                return 1;

        utf8 x = (utf8)(&n[1]);

        if (!string_append_str(&x, *src)) {
                string_dealloc(*src);
                *src = NULL;
                return 2;
        }

        string_dealloc(*src);
        *src = x;

        return 0;
}

/*! @fn    string_append_str
 *  @brief Append one string to another.
 *
 *  Appending one string to another. The another string is being freed. If the string
 *  has capacity to take in the new string, no reallocation will occur.
 *
 *  @param[in,out] src String to be appended to.
 *  @param[in]     apd String to append.
 *
 *  @return 0 if successful, non-zero value otherwise.
 *
 *  @warning Function will deallocate the `apd` string.
 */
int string_append_str(utf8 *src, utf8 apd)
{
        struct string_header *a = string_hdr(*src);
        struct string_header *b = string_hdr(apd);

        const size_t len = a->length + b->length + 1;

        if (a->capacity < len) {
                struct string_header *s = realloc(a, sizeof(*a) + len);
                if (s == NULL)
                        return 1;

                s->capacity = len;
                *src = (utf8)(&s[1]);
                a = s; // switch pointer
        }

        memcpy(*src + a->length +1, b, b->length);
        a->length += b->length;

        string_dealloc(apd);

        return 0;
}

utf8 string_copy(utf8 src)
{
        struct string_header *hdr = string_hdr(src);
        struct string_header *cpy = calloc(1, (sizeof(*cpy) + hdr->capacity));

        memcpy(cpy, hdr, sizeof(*hdr) + hdr->capacity);

        return (utf8)(&cpy[1]);
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
        free( string_hdr( self ) );
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

        string_header_t *r = realloc( hdr, sizeof( string_header_t ) + cap + 1 );

        if ( r is NULL )
                return ARDP_FAILURE;

        *str          = ( utf8 )r + sizeof( string_header_t );
        hdr           = string_hdr( *str );
        hdr->capacity = cap;

        /* Remove garbage if there is any  after the string content */
        memset( *str + len, 0, cap - len + 1 );
        return ARDP_SUCCESS;
}

bool string_append_char( utf8 *str, char c ) {
        if ( string_resize( str, 1 ) isnt ARDP_SUCCESS )
                return ARDP_FAILURE;

        string_push( *str, c );
        string_finish( *str );
        return ARDP_SUCCESS;
}

bool string_append_utf8( utf8 *s, int cp ) {
        if ( cp < 0 or cp > 0x10ffff ) {
                return ARDP_FAILURE;
        } else if ( cp < 0x80 ) {
                return string_append_char( s, cp & 0x7F );
        } else if ( cp < 0x800 ) {
                if ( string_resize( s, 2 ) isnt ARDP_SUCCESS )
                        return ARDP_FAILURE;
                string_push( *s, 0xC0 | ( ( cp >> 6 ) & 0x1F ) );
                string_push( *s, 0x80 | ( cp & 0x3F ) );
        } else if ( cp < 0x10000 ) {
                if ( string_resize( s, 3 ) isnt ARDP_SUCCESS )
                        return ARDP_FAILURE;
                string_push( *s, 0xE0 | ( ( cp >> 12 ) & 0xF ) );
                string_push( *s, 0x80 | ( ( cp >> 6 ) & 0x3F ) );
                string_push( *s, 0x80 | ( cp & 0x3F ) );
        } else {
                if ( string_resize( s, 4 ) isnt ARDP_SUCCESS )
                        return ARDP_FAILURE;
                string_push( *s, 0xF0 | ( ( cp >> 18 ) & 0x7 ) );
                string_push( *s, 0x80 | ( ( cp >> 12 ) & 0x3F ) );
                string_push( *s, 0x80 | ( ( cp >> 6 ) & 0x3F ) );
                string_push( *s, 0x80 | ( cp & 0x3F ) );
        }
        string_finish( *s );
        return ARDP_SUCCESS;
}

void string_finish( utf8 str ) {
        string_header_t *hdr = string_hdr( str );
        *( str + hdr->length ) = '\0';
}

// http://mgronhol.github.io/fast-strcmp/
int string_generic_cmp(const uint8_t* a, const uint8_t* b, int len)
{
/*        int fast    = len/sizeof(size_t) + 1;
        int offset  = (fast - 1)*sizeof(size_t);
        int current = 0;

        if (len <= sizeof(size_t))
                fast = 0;

        size_t *la = (size_t*)a;
        size_t *lb = (size_t*)b;

        while (current < fast) {
                if ( (la[current] ^ lb[current]) ) {
                        int pos;
                        for(pos = current*sizeof(size_t); pos < len; ++pos) {
                                if (  (a[pos] ^ b[pos])
                                   || (la[pos] == 0)
                                   || (b[pos] == 0) ) {
                                        return (int)((uint8_t)a[pos] - (uint8_t)b[pos]);
                                }
                        }
                }
                ++current;
        }

        while ( len > offset ) {
                if ( (a[offset] ^ b[offset])) {
                        return (int)((uint8_t)a[offset] - (uint8_t)b[offset]);
                }
                ++offset;
        }

        return 0;
*/
        const char* ptr0 = (const char*)a;
        const char* ptr1 = (const char*)b;

        int fast    = len/sizeof(size_t) + 1;
        int offset  = (fast - 1) * sizeof(size_t);
        int current = 0;

        if ( len <= sizeof(size_t) )
                fast = 0;

        size_t *la = (size_t*)ptr0;
        size_t *lb = (size_t*)ptr1;

        while (current < fast) {
                if( (la[current] ^ lb[current] ) ){
                        int pos;
                        for(pos = current*sizeof(size_t); pos < len ; ++pos ){
                                if( (ptr0[pos] ^ ptr1[pos]) || (ptr0[pos] == 0) || (ptr1[pos] == 0)  ){
                                        return  (int)((unsigned char)ptr0[pos] - (unsigned char)ptr1[pos]);

                                }

                        }

                }
                    ++current;

        }
        while( len > offset  ){
                if( (ptr0[offset] ^ ptr1[offset] ) ){
                return (int)((unsigned char)ptr0[offset] - (unsigned char)ptr1[offset]);
                      }
                    ++offset;

        }
        return 0;

}
