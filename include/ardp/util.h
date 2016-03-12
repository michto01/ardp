/** @file util.h
  *
  * Utilites for the ARDP programme. It includes some guard defitions for 
  * non-standard C extensions used in code. As well status codes and format
  * definitions.
  *
  * @author  Tomas Michalek <tomas.michalek.st@vsb.cz>
  * @version 1.4.3
  * @date    2015
  * @bug     A known bug
  */

#pragma once /* Header guarde for C11 */

#include <stdlib.h>
#include <errno.h>

#define is ==
#define isnt !=

#ifndef _ISO646_H_ /* clang-format off */
#define or  ||
#define and &&
#define not !
#endif /* _ISO646_H_ *//* clang-format on */


#if __has_feature( nullability )
#else
#define _Nullable
#define _Nonnull
#define _Null_unspecified
#endif

//#define ARDP_SUCCESS true
//#define ARDP_FAILURE false

/** @typedef var
  *
  * Shortcut for void pointer.
  */
typedef void* var;

/** @var nullptr
  * @brief Nullptr refers to own definition of C++ nullptr.
  */
static const int nullptr = 0;

/** @typedef ardp_status_t
  * @brief Status codes to designate explicit states of operation.
  */
typedef enum _ardp_status_t {
    /*!
     * Operation finished sucessfully.
     *
     * @note The success value is denoted as '0' in the spirit of *nix-ness.
     */
    ARDP_SUCCESS = 0,
    /*!
     * Operation didn't finish correctly.
     */
    ARDP_FAILURE,
    /*!
     * Operation aborted or undefined behaviour encountered.
     */
    ARDP_UNKNOWN
} ardp_status_t;

/*!
 * Helper function to kill the program on unrecoverable error.
 */

static 
inline void _Noreturn die(const char* message) {
	if (message)
		fprintf(stderr, "%s", message);
	exit(EXIT_FAILURE);
}


/* Brach prediction optimalizations */

/** @def    likely(x)
  * @brief  Branch prediction macro: likely to occur.
  */
#define likely( x ) __builtin_expect( !!( x ), 1 )

/** @def    unlikely(x)
  * @brief  Branch prediction macro: unlikely to occur.
  */
#define unlikely( x ) __builtin_expect( !!( x ), 0 )
