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

#define is ==
#define isnt !=

#ifndef _ISO646_H_ /* clang-format off */
#define or  ||
#define and &&
#define not !
#endif /* _ISO646_H_ *//* clang-format on */

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
     */
    ARDP_SUCCESS,
    /*!
     * Operation didn't finish correctly.
     */
    ARDP_FAILURE,
    /*!
     * Operation aborted or undefined behaviour encountered.
     */
    ARDP_UNKNOWN
} ardp_status_t;

/* Brach prediction optimalizations */

/** @def    likely(x)
  * @brief  Branch prediction macro: likely to occur.
  */
#define likely( x ) __builtin_expect( !!( x ), 1 )

/** @def    unlikely(x)
  * @brief  Branch prediction macro: unlikely to occur.
  */
#define unlikely( x ) __builtin_expect( !!( x ), 0 )
