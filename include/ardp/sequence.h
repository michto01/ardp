/*! @file sequence.h
 *
 *  Specifies interface for storage with generic item type. Primarily to be
 *  used as storage for the RDF:collection.
 *
 *  @author Tomas Michalek <tomas.michalek.st@vsb.cz>
 *  @date   2016
 */
#pragma once

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <assert.h>
#include <ctype.h>
#include <errno.h>

#include "util.h"


#define SEQUENCE_MIN_CAPACITY 8

/*! @struct sequence
 *  @brief  Memory container for generic data storage of sequential data.
 *
 *  @verbatim
 * Sequence:
 *     0            <-- N consecutive items -->         C - 1
 * -----------------------------------------------------------
 * |      |      | data1 |  .....     data N |  ...  |       |
 * -----------------------------------------------------------
 * ------ O -----> offset of first data item
 *
 *  head = 0
 *  size = N
 *  cap. = C
 *  @endverbatim
 */
struct sequence {
        /*!
         * Current number of the items in the sequence.
         */
        size_t size;

        /*!
         * Actual length of the pre-allocated array of items.
         */
        size_t capacity;

        /*!
         * Offset of the first item of the data in sequence.
         */
        size_t  head;

        /*!
         * Array of size 'capacity' pointing to the data.
         */
        void  **items;

        /*!
         * @section not used currently {
         */
        void *context;

        /*!
         * @fn print
         * Print item in debug
         *
         * @deprecated Not used anymore
         */
        void (*print)(void *data);

        void (*error)(void *data);

        /*!
         * @fn free
         * Handler to free item.
         *
         * @param[in] data Data to be freed.
         *
         * @note Should do nothing if item is NULL;
         */
        void (*free)(void *data);
        /*! } @endsection */
};

/* Gem from K&R old function definition, no argument type check!!
void weir_function(v)
        int v;
{
        printf("%d",v);
}
*/

typedef void (*sequence_error_handler)(void *data);
typedef void (*sequence_free_handler)(void *data);
typedef void (*sequence_print_handler)(void *data);


/*! @fn    sequence_create
 *  @brief Create empty new container for the sequence.
 *
 *  @param[in] f Handler to free the items.
 *  @param[in] e Handler for error.
 *  @param[in] p Print handler in debug.
 *
 *  @return Sequence, NULL if error.
 *  @todo   Code for the handlers (print,error,free)
 */
struct sequence* sequence_create(sequence_free_handler  f,
                                 sequence_error_handler e,
                                 sequence_print_handler p);

/*! @fn    sequence_size
 *  @brief Get size (# of objects) of sequence.
 *
 *  @param[in] seq Sequence to be examined.
 *
 *  @return Number of items in the sequence.
 */
size_t sequence_size(struct sequence* seq);

/*! @fn    sequence_set_at
 *  @brief Put item at index.
 *
 *  @param[in] seq  Sequence of interest.
 *  @param[in] idx  Index of the item.
 *  @param[in] data Data to be inserted into sequence.
 *
 *  @return 0 iff no error occurred, 1 otherwise
 */
int sequence_set_at(struct sequence* seq, int idx, void *data);


/*! @fn    sequence_get_at
 *  @brief Get object from sequence at given index.
 *
 *  @param[in] seq Sequence to be probed.
 *  @param[in] idx Index of the item.
 *
 *  @return Generic pointer to object ( should be casted  ).
 */
void* sequence_get_at(struct sequence* seq, int idx);

/*! @fn    sequence_delete_at
 *  @brief Pop out from sequence and delete the item.
 *
 *  @param[in] seq Sequence of interest.
 *  @param[in] idx Index of selected item.
 *
 *  @return Deleted item.
 */
void* sequence_delete_at(struct sequence* seq, int idx);

/*! @fn    sequence_pop
 *  @brief Pop out the top-most item from sequence.
 *
 *  @param[in] seq Sequence of interest.
 *
 *  @return Top-most item
 */
void* sequence_pop(struct sequence* seq);

/*! @fn    sequence_push
 *  @brief Push item to the top of the sequence.
 *
 *  @param[in] s Sequence of interest.
 *  @param[in] data Data to be pushed into the sequence.
 *
 *  @return 0 iff no error, 1 otherwise.
 */
int sequence_push(struct sequence* s, void* data);

/*! @fn    sequence_shift
 *  @brief Push item to the Front-most position in the sequence.
 *
 *  @param[in] seq  Sequence of interest.
 *  @param[in] data Data to be inserted.
 *
 *  @return 0 if successful, 1 otherwise.
 */
int sequence_shift(struct sequence* seq, void *data);

/*! @fn    sequence_shift
 *  @brief Retrieve item at the beginning of the sequence.
 *
 *  @param[in] seq Sequence of interest.
 *
 *  @return Object or NULL if the sequence is empty.
 */
void* sequence_unshift(struct sequence* seq);

/*! @fn    sequence_free
 *  @brief Destroy sequence.
 *
 *  @param[in] seq Sequence to destroy.
 *
 *  @note Performs NULL-checking.
 */
void sequence_free(struct sequence* seq);
