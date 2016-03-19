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

#include "util.h"


#define SEQUENCE_MIN_CAPACITY 8

struct sequence {
        size_t size;
        size_t capacity;

        // replaced off_t or int with uint
        size_t  head; // start

        void  **items;

        void *context;

        void (*print)();
        void (*error)();
        void (*free)();
};

/* Gem from K&R old function definition, no argument type check!!
void weir_function(v)
        int v;
{
        printf("%d",v);
}
*/

/*! @fn    sequence_create
 *  @brief Create empty new container for the sequence.
 *
 *  @return Sequence, NULL if error.
 *  @todo   Code for the handlers (print,error,free)
 */
struct sequence* sequence_create();

/*! @fn    sequence_size
 *  @brief Get size (# of objects) of sequence.
 *
 *  @param[in] seq Sequence to be examined.
 *
 *  @return Number of items in the sequence.
 */
size_t sequence_size(struct sequence* seq);

/*! @fn    sequence_set_at
 */
int sequence_set_at(struct sequence* seq, int idx, void *data);

void* sequence_get_at(struct sequence* seq, int idx);
void* sequence_delete_at(struct sequence* seq, int idx);
void* sequence_pop(struct sequence* seq);
int sequence_push(struct sequence* s, void* data);
int sequence_shift(struct sequence* seq, void *data);
void* sequence_unshift(struct sequence* seq);

