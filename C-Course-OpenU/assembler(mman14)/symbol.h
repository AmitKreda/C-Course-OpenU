#ifndef SYMBOL_H
#define SYMBOL_H 1

/*standard libraries*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*
 * Enum: symbolType
 * -----------------
 * Represents the different types of symbols in the program.
 *
 */
typedef enum
{
    externS,   /* Represents an external symbol */
    dataS,     /* Represents a data symbol */
    constS,    /* Represents a constant symbol*/
    operationS /* Represents an operation symbol*/
} symbolType;

/*
 * Struct: symbol
 * ---------------
 * This structure represents a constant/label in the assembly language, it contains all information
 * about the symbol, the type of the symbol, and the address or the constant value of the symbol.
 */
typedef struct symbol
{
    symbolType typeOfSymbol; /* Type of the symbol */
    union
    {
        unsigned short int address; /* Address value */
        short int constant;         /* Constant value */
    } sValue;                       /* Union containing either an address or a constant value */
    unsigned short int numOfValues; /* Number of values associated with the symbol (used only in data symbols) */
} symbol;

#endif