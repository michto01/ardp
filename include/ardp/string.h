#ifndef __ARDP2_STRING_H__
#define __ARDP2_STRING_H__

#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h> // uint8_t

#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t *utf8;

/**
  * Header to use with the string, holding the string informations.
  */
typedef struct __attribute__( ( __packed__ ) ) string_header {
        /*!
         * Current pre-allocated capacity of the string.
         */
        size_t capacity;
        /*!
         * Actual lenght of the string ( # of characters in buffer ).
         */
        size_t length;
} string_header_t;


/**
  * Push character into the string.
  *
  * The helper pushing the raw character to the string.
  *
  * @note			This function is unsafe and should be allways used with the proper
  *						bounds check.
  *
  * @param[in,out] s String being manipulated.
  * @param[in]		 c Character to be appended to string.
  */
static inline void string_push( utf8 s, char c ) {
        string_header_t *hdr = &( ( string_header_t * )s )[-1];
        s[hdr->length++]     = c; //*(s + hdr->length++) = (uint8_t) c;
}

/**
  * Helper function to shift the string to header position
  */
static inline struct string_header *string_hdr( utf8 str ) {
        return &( ( string_header_t * )str )[-1];
}

/**
  * Length of the fat-pointer string
  *
  * @param[in] str  String for measurement.
  *
  * @return Return the length of the string.
  */
size_t string_strlen( utf8 str );

/**
  * Resize the string
  *
  * This function checks if there is enougth space in the string or if the space
  * need to be added. In case that the space is inssuficient, it allocates twice
  * as much until the 'MAX_PREALLOC' is reached when the new preallocation is
  * then created allways with it's size.
  *
  * @param[in,out] str  Pointer to String currently being manipulated.
  * @param[in]		 size Size to check/allocate.
  *
  * @return True if string is ready to accomodate the size, no otherwise.
  */
bool string_resize( utf8 *str, size_t size );

/**
  * Append character (BYTE)
  *
  * Safe version with boundaries checking for apppeding the character to the
  * string.
  *
  * @param[in,out] str String being manipulated.
  * @param[in]	   c	 Character being pushed into the string.
  *
  * @return True if character was sucessfully appended, false otherwise.
  */
bool string_append_char( utf8 *str, char c );

/**
  * Append the UTF8 codepoint to the string.
  *
  * @TODO description
  *
  * @param[in,out] str String being manipulated.
  * @param[in]	   cp	 Character codepoint being pushed into the string.
  *
  * @return True if character was sucessfully appended, false otherwise.
  */
bool string_append_utf8( utf8 *str, int cp );

/**
  * Close finish the string.
  *
  * In the 'C' the string needs to be NULL-terminated with '\0' and this function
  * provides just that.
  *
  * @note we regard this function as safe 'cause the string has preallocated
  *			  the space to 'allways' accomodate the NUL terminator
  *
  * @param[in,out] str String being manipulated.
  *
  * @return True if the string was finished corretly, false otherwise.
  */
void string_finish( utf8 str );

/**
  * Allocate new string with the default length.
  *
  * @return Pointer to the newly created string.
  */
utf8 string_new( void );

/**
  * Allocate new string of length.
  *
  * @param[in] length Length of the newly allocated string.
  *
  * @return Pointer to the newly created string.
  */
utf8 string_alloc( size_t length );

/**
  * Destroy the string.
  *
  * As the 'utf8' allocates string header which is hidden from the user,
  * API need to back-shift in the buffer to access it and then delete the
  * whole pointer of the structure.
  *
  * @param[in] self Pointer to the manipulated string.
  */
void string_dealloc( utf8 self );

/**
  * Debbuging information of the string.
  *
  * @param[in] str	String being investigated.
  */
void string_debug( utf8 str );

/**
 *  Optimalized compare algorithm yealding little faster string comparison
 *  then build-in `strcmp` function.
 *
 *  @param[in] a   First string
 *  @param[in] b   Second string
 *  @param[in] len Length to compare.
 *
 *  @return 1 iff string are the same, 0 otherwise.
 */
int string_generic_cmp( const uint8_t* a, const uint8_t* b, int len );

#ifdef __cplusplus
}
#endif

#endif /* __ARDP2_STRING_H__ */
