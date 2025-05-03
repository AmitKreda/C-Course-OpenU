#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "lexer.h"
#include "trie.h"
#include "ast.h"

/* Translation Unit Constants */
#define DC transUnit->dc
#define IC transUnit->ic
#define SYM_TABLE transUnit->root

/* Addressing Method Constants */
#define IMMEDIATE_ADDRESSING 0 /* value code of the 'immediate addressing' addressing method */
#define DIRECT_ADDRESSING 1    /* value code of the 'direct addressing' addressing method */
#define IDX_ADDRESSING 2       /* value code of the 'direct index addressing' addressing method */
#define REGISTER_ADDRESSING 3  /* value code of the 'register addressing' addressing method */

#define INDEX_WORD_AMOUNT 2       /* amount of machine words a label with an index occupies */
#define TWO_OPERANDS 2            /* constant representing two operands */
#define ONE_OPERAND 1             /* constant representing one operand */
#define MACHINE_WORD_SIZE 14      /* size of machine word in the imaginary computer */
#define SIZE_OF_MEMORY 3996       /* 4096 size of all computer memory - 100 where the addressing starts */
#define MEMORY_STARTING_POINT 100 /* where the memory addressing starts */

typedef TrieNode *trie;
typedef unsigned short int machineWord;
typedef unsigned int index;

/* structure representing a line, together with its AST */
typedef struct astLine
{
    astNode *ast;     /* the AST of the line */
    index lineNumber; /* the line number */
} astLine;

/* structure representing all the lines that the second pass should continue processing - containing all the ASTs of those lines */
typedef struct
{
    index bufferSize;   /* the of the ast buffer */
    astLine *astBuffer; /* buffer containing all ASTs of the lines */
} FirstPassAstBuffer;

/* structure repreesnting a referance of an extern label */
typedef struct
{
    index extAddress;            /* the address of the extern reference */
    char name[MAX_LABEL_LENGTH]; /* the name of the label referenced */
} externReference;

/* structure representing an appearance of an entry label */
typedef struct
{
    index entAddress;            /* the address of the entry label */
    char name[MAX_LABEL_LENGTH]; /* the name of the label */
} entryAppearance;

/* structure representing the translate data of the assembly */
typedef struct
{
    index ic;                               /* instruction counter */
    index dc;                               /* data counter */
    trie root;                              /* root of the symbol table */
    unsigned int extArrSize;                /* size of the array of extern references */
    unsigned int entryArrSize;              /* size of the array of entry appearances */
    externReference *extArr;                /* array of extern references */
    entryAppearance *entryArr;              /* array of entry appearances */
    machineWord machWrdArr[SIZE_OF_MEMORY]; /* array of all machine words (program memory image)*/
} translationUnit;

/* Function Prototypes */

/*
 * Performs the first pass of assembly code processing.
 *
 * This function builds the first pass ast buffer and the translation unit for the second pass.
 *
 *
 * Parameters:
 * - firstPass: Pointer to the first pass AST buffer.
 * - transUnit: The translation unit containing the symbol table and other data.
 * - toRead Pointer to the file stream containing the assembly code to be processed.
 * - fileName: the name of the assmbly file.
 * Returns:
 * - TRUE if an error was encountered during processing, FALSE otherwise.
 */
bool firstPass(FirstPassAstBuffer *firstPass, translationUnit *transUnit, FILE *toRead, char *fileName);

/*
 * Reallocates memory for a given pointer.
 *
 * Parameters:
 * - objectSize: Size of each object in bytes.
 * - newSize: New size in number of objects after reallocation.
 * - ptrToReallocate: Pointer to the pointer that needs to be reallocated.
 *
 */
void reallocateMemory(unsigned int objectSize, unsigned int newSize, void **ptrToReallocate);
/*
 * Frees memory allocated during the first pass of assembly.
 *
 * Parameters:
 * - firstPass: Pointer to the first pass AST buffer.
 * - transUnit: The translation unit containing the symbol table and other data.
 */
void freeFirstPassMemory(translationUnit *transUnit, FirstPassAstBuffer *firstPass);
/*
 * Sets a specific range of bits in a machine word to a given value.
 *
 * This function modifies a machine word by setting a specified range of bits
 * to a given value while leaving other bits unchanged.
 *
 * Parameters:
 * - num Pointer to the machine word to be modified.
 * - k Starting position of the range of bits to be set.
 * - numBits Number of bits to be set.
 * - value Value to be set in the specified range of bits.
 */
void setBits(machineWord *num, int k, int numBits, short int value);

#endif /* FIRST_PASS_H */