/** @file hash.h
 *
 * Hashing function to choose from in hashmap.
 *
 * @author  Tomas Michalek <tomas.michalek.st@vsb.cz>
 * @version 1.0.2
 * @date    2015
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <ardp/util.h>
#include <ardp/hash.h>

/*!
 *  Key used in the map.
 */
static const int ARDP_MAP_USED    = (-4);
/*!
 *  No such element in map.
 */
static const int ARDP_MAP_MISSING = (-3);
/*!
 *  HashMap is full.
 */
static const int ARDP_MAP_FULL    = (-2);
/*!
 *  Out of memory.
 */
static const int ARDP_MAP_OUTMEM  = (-1);
/*!
 *  OK - operation was performend correctly.
 */
static const int ARDP_MAP_OK      = ( 0);


/**
  * Pointer to function that can take two var arguments and return an integer.
  *
  * @param[in] a  Argument A
  * @param[in] b  Argument B
  *
  * @return Returns status code.
  */
typedef int ( *map_callback )( var a, var b );

/**
  * Pointer to internally maintained data structure.
  *
  * HashMap client do not need to know how the hashmaps are represented.
  * They only manipulate the `map_t` data types.
  */
typedef var map_t;

#ifdef __cplusplus
extern "C" {
#endif /* C++ */

/**
  * Creates new hashmap
  *
  * @param[in] n Number of the preallocated space.
  *
  * @return Returns new map - or NULL if error occured.
  */
map_t map_create( size_t n );

/**
  * Resize the Hash map
  *
  * @param[out] in    Map to be manipulated.
  * @param[in]  room  Number of elements to add (will floor to nearest prime).
  *
  * @return `true` if resize is completed, `false` otherwise.
  */
bool map_resize( map_t *in, size_t room );

/**
  * Realloc the hashmap.
  */
map_t map_realloc( unsigned int n );

/**
  * Iteratively call function `f` with argument (item, data) for each element
  * data in the hashmap. The function must return a map status code.
  *
  * @note `f` must not reenter any hashmap functions, or deadlock may arise.
  *
  * @param[in] in   Map to be manipulated.
  * @param[in] f    Operation to be performend while iterating the map.
  * @param[in] item Item
  *
  * @return If it returns anything other than ARDP_MAP_OK the traversal is treminated.
  */
int map_iterate( map_t in, map_callback f, var item );

/**
  * Add an element to the hashmap.
  *
  * @param[in] in     Map to be manipulated.
  * @param[in] key    Key to be used.
  * @param[in] value  Value to put in the map.
  *
  * @return Return ARDP_MAP_OK or ARDP_MAP_OMEM.
  */
int map_push( map_t in, char *key, var value );

/**
  * Get an element from the hashmap.
  *
  * @param[in] in     Map to be manipulated.
  * @param[in] key    Key to be used.
  * @param[in] arg    Item taken from the map.
  *
  * @return Return MAP_OK or MAP_MISSING.
  */
int map_get( map_t in, char *key, var *arg );

/**
  * Remove an element from the hashmap.
  *
  * @param[in] in     Map to be manipulated.
  * @param[in] key    Key to be deleted.
  *
  * @return Return MAP_OK or MAP_MISSING.
  */
int map_remove( map_t in, char *key );

/**
  * Get any element.
  *
  * @param[in] in     Map to be manipulated.
  * @param[in] arg    item taken from the map.
  * @param[in] remove Indicates if the item should be deleted
  *
  * @return Return MAP_OK or MAP_MISSING.
  */
int map_get_one( map_t in, var *arg, int remove );

/**
  * Free the map
  *
  * @param[in] in Map to be unallocated.
  */
void map_free( map_t in );

/**
  * Get the current size of a hashmap.
  *
  * @param[in] in Map to be probed.
  *
  * @return Size of the hashmap.
  */
size_t map_size( map_t in );

/**
  * Get current capacity of the hashmap.
  *
  * @param[in] in Map to be probed.
  *
  * return Capacity of the hashmap.
  */
size_t map_capacity( map_t in );

#ifdef __cplusplus
}
#endif /* C++ */
