#ifndef __ARDP2_STRING_H__
#define __ARDP2_STRING_H__

#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h> // uint8_t

#include "util.h"
#include "color.h"

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
  * @note This function is unsafe and should be allways used with the proper
  *	  bounds check.
  *
  * @param[in,out] s String being manipulated.
  * @param[in]		 c Character to be appended to string.
  */
static inline void string_push( utf8 s, char c ) {
        string_header_t *hdr = &( ( string_header_t * )s )[-1];
        s[hdr->length++]     = c; //*(s + hdr->length++) = (uint8_t) c;
}

/**
  * Helper function to shift the string to header positio
  *
  * @param[in] str String to access.
  *
  * @return Returns the hidden star of the string.
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

/*! @fn    string_create_n
 *  @brief Create new string from content of the buffer of specified length.
 *
 *  @param[in] s String buffer to be placed in the new string.
 *  @param[in] n Length of the buffer.
 *
 *  @return New string on success, NULL otherwise.
 *
 *  @note Doesn't check the memory, copies with for `n`.
 */
utf8 string_create_n(const uint8_t* s, size_t n);

/*! @fn    string_create
 *  @brief Create new string from string buffer.
 *
 *  @param[in] s String buffer to be place in the buffer.
 *
 *  @return Pointer to a new string.
 */
utf8 string_create(const uint8_t* s);

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
int string_prepend(utf8 *src, const uint8_t* str);

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
int string_append(utf8 *src, const uint8_t* mod);

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
int string_append_str(utf8 *src, utf8 apd);

/*! @fn    string_copy
 *  @brief Returns new copy of the specified string.
 *
 *  @param[in] src String to be copied.
 *
 *  @return Copy of the string.
 */
utf8 string_copy(utf8 src);

#ifdef __cplusplus
}
#endif

#endif /* __ARDP2_STRING_H__ */
