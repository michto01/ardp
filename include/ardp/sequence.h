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
#include <string.h>
#include <stdlib.h>

#include <assert.h>
#include <ctype.h>

#include "util.h"

//// SEQUENCE
/* SEQUENCE {{{ */
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

struct sequence* sequence_create()
{
        struct sequence* seq = calloc(1, sizeof(*seq));

        if (!seq)
                return NULL;

        //TODO: code to input handlers

        return seq;
}

#define SEQUENCE_MIN_CAPACITY 8

static int sequence_ensure(struct sequence* seq, size_t capacity, int grow_at_front )
{
        //FIXME: grow_at_front should be moved to use boolean

        assert(seq); // Sanity check should be expanded to allow null-checking

        if (capacity && seq->capacity >= capacity)
                return 0;

        /* Sanity check for minimum size */
        if (capacity < SEQUENCE_MIN_CAPACITY)
                capacity = SEQUENCE_MIN_CAPACITY;

        void** new_seq = calloc(capacity, sizeof(void*));
        if (!new_seq)
                return 1;

        uint64_t offset = (grow_at_front ? capacity - seq->capacity: 0) + seq->head;
        if (seq->size) {
                memcpy(&new_seq[offset], &seq->items[seq->head], sizeof(void*) *seq->size );
                free(seq->items);
        }

        seq->head     = offset;
        seq->items    = new_seq;
        seq->capacity = capacity;

        return 0;
}

int64_t sequence_size(struct sequence* seq)
{
        if (!seq)
                return 0;
        return seq->size;
}

int sequence_set_at(struct sequence* seq, int idx, void *data)
{
        if (!seq)
                return 1;

        /* Cannot provide a negative index */
        if (idx < 0) {
                if (data) {
                        free(data);
                }
                //else if (seq->context_free_handler); free->handler

                return 1;
        }

        size_t  need_capacity = seq->head + idx + 1;
        if (need_capacity > seq->capacity) {
                if (seq->capacity * 2 > need_capacity)
                        need_capacity = seq->capacity * 2;

                if (sequence_ensure(seq, need_capacity, 0)) {
                        if (data) {
                                free(data);
                        }

                        return 1;
                }
        }

        if (idx < seq->size) {
                /* delete old data */
                if (seq->items[seq->head + idx])
                        free(seq->items[seq->head + idx]);
                /* size remains the same */
        } else {
                /*
                 * In case that there is no old data, increase size.
                 * There should be seq-size # items in seq starting at seq->head.
                 */
                seq->size = idx + 1;
        }

        seq->items[seq->head + idx] = data;
        return 0;
}

void* sequence_get_at(struct sequence* seq, int idx)
{
        if (!seq)
                return NULL;

        return (idx < 0 || idx > seq->size - 1) ? NULL : seq->items[seq->head+ idx];
}


void* sequence_delete_at(struct sequence* seq, int idx)
{
        if(!seq)
                return NULL;
        if(idx < 0 || idx > seq->size - 1)
                return NULL;

        void* data = seq->items[seq->head + idx];
        seq->items[seq->head + idx] = NULL;

        return data;
}

void* sequence_pop(struct sequence* seq)
{
        if(!seq || !seq->size)
                return NULL;

        seq->size--;

        size_t i = seq->head + seq->size;
        void* data = seq->items[i];

        seq->items[i] = NULL;

        return data;
}

int sequence_push(struct sequence* s, void* data)
{
        if ( !s )
                return 1;

        if(s->head + s->size == s->capacity) {
                if(sequence_ensure(s, s->capacity * 2, 0)) {
                        if(data) {
                                free(data);
                        }
                        return 1;
                }
        }

        s->items[s->head + s->size] = data;
        s->size++;

          return 0;
}

int sequence_shift(struct sequence* seq, void *data)
{
        if(!seq)
                return 1;

        if(!seq->head) {
                if(sequence_ensure(seq, seq->capacity * 2, 1)) {
                        if(data) {
                                free(data);
                        }
                        return 1;
                }
        }

        seq->items[--seq->head] = data;
        seq->size++;

        return 0;
}


void* sequence_unshift(struct sequence* seq)
{
        if(!seq || !seq->size)
                return NULL;

        size_t i = seq->head++;
        void* data = seq->items[i];
        seq->size--;
        seq->items[i] = NULL;

        return data;
}
/*}}}*/

