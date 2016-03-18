/*! @file lexer.turtle.h
 *
 * Formalized Turtle and N-Triples specific options and constants for the lexer. 
 *
 * @author Tomas Michalek <tomas.michalek.st@vsb.cz
 * @date   2015
 */
#pragma once

/*!
 * @enum  turtle_token_type
 * @brief Enumerates token types to explicit values for the turtle parsing.
 */
/* enum(turtle_token_type) {{{*/
enum turtle_token_type {
        A = 201,
        HAT,
        DOT,
        COMMA,
        SEMICOLON,

        L_SQUARE,
        R_SQUARE,
        L_ROUND,
        R_ROUND,
        L_CURLY,
        R_CURLY,

        PREFIX,
        BASE,
        SPARQL_PREFIX,
        SPARQL_BASE,

        STRING_LITERAL,
        BLANK_LITERAL,
        QNAME_LITERAL,
        URI_LITERAL,

        IDENTIFIER,
        LANGTAG,

        INTEGER_LITERAL,
        DECIMAL_LITERAL,
        DOUBLE_LITERAL,
        BOOLEAN_LITERAL,

        INVALID /* deprecated -> to be removed in next release*/
}; /*}}}*/


