/** @file hash.h
  *
  * Hashing function to choose from in hashmap.
  *
  * @author  Tomas Michalek <tomas.michalek.st@vsb.cz>
  * @version 1.0.2
  * @date    2015
  */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif /* C++ */

/**
  * A Simple Hash Function
  *
  * @param  str   String to generate hash from
  *
  * @return Hash for the string.
  */
unsigned int hash_simple( char *str );

/**
  * RS Hash Function
  *
  * @param  str   String to generate hash from
  *
  * @return Hash for the string.
  */
unsigned int hash_rs( char *str );

/**
  * JS Hash Function
  *
  * @param  str   String to generate hash from
  *
  * @return Hash for the string.
  */
unsigned int hash_js( char *str );

/**
  * P. J. Weinberger Hash Function
  *
  * @param  str   String to generate hash from
  *
  * @return Hash for the string.
  */
unsigned int hash_pjw( char *str );

/**
  * ELF Hash Function
  *
  * @param  str   String to generate hash from
  *
  * @return Hash for the string.
  */
unsigned int hash_elf( char *str );

/**
  * BKDR Hash Function
  *
  * @param  str   String to generate hash from
  *
  * @return Hash for the string.
  */
unsigned int hash_bkdr( char *str );

/**
  * SDBM Hash Function
  *
  * @param  str   String to generate hash from
  *
  * @return Hash for the string.
  */
unsigned int hash_sdbm( char *str );

/**
  * DJB Hash Function
  *
  * @param  str   String to generate hash from
  *
  * @return Hash for the string.
  */
unsigned int hash_djb( char *str );

/**
  * AP Hash Function
  *
  * @param  str   String to generate hash from
  *
  * @return Hash for the string.
  */
unsigned int hash_ap( char *str );

/**
  * CRC Hash Function
  *
  * @param  str   String to generate hash from
  *
  * @return Hash for the string.
  */
unsigned int hash_crc( char *str );

#ifdef __cplusplus
}
#endif /* C++ */
