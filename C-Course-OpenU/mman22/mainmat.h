#ifndef MAIN_MAT
#define MAIN_MAT 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "function_handler.h"
#include "mymat.h"

/* Constants */

#define MAX_WORD_AMOUNT 20    /* Maximum number of words allowed in a line */
#define MAX_WORD_LEN 21       /* Maximum length of a word (including the null terminator) */
#define NUM_OF_FUNCTIONS 8 /* Total number of supported functions */
#define MEMORY_ERROR 1        /* Memory allocation error indicator */
#define START_OF_THE_LINE 0   /* Index indicating the start of a line or array */
#define START_OF_THE_WORD 0   /* Index indicating the start of a word */
#define PRVE_WORD 1 /*The number that needed to substract from line to get the previos word*/
/* Macros */

#define NOT_ENOUGH_SPACE(ptr) /* Checks if there is enough space for memory allocation */ \
    if (ptr == NULL) { \
        handleErrors(notEnoughSpace); \
        exit(MEMORY_ERROR); \
    }

#define PRINT_LINE() /* Prints a line of characters until a newline is encountered */ \
    { \
        int c; \
        while ((c = getchar()) != EOF && c != '\n') { \
            printf("%c", c); \
        } \
        if (c == '\n') \
            printf("\n"); \
    }

#define CHECK_COMMA_ERRORS() /* Checks for comma-related errors in the getWord function */ \
    if (lastCharWasComma) { /*if there was a comma befor and now without any char that isn't space sperating between*/\
        PRINT_LINE(); /*printing the rest of the line*/\
        handleErrors(Multiple_consecutive_commas);/*reporting the error*/ \
        return True; /*return error*/\
    } \
    if (!inWord) /*if there is a between the last word and this word, it belong to the end of the last word*/ \
        *ThereIsCommaInTheEndOfThePrevWord = True; \
    else \
        currentWord->thereIsCommaInTheEndOfTheWord = True; /*if we are in a word and there is a comma*/ \
    lastCharWasComma = True;/*update the flag*/

/* Structs */

typedef struct functionWithName {/*struct that have a pointer to a function and the function name*/
    errors (*functionPtr)(int, word *, matrixsWithName *);/*fuction pointer*/
    char *name;/*function name*/
} functionWithName;

/* Function Declarations */
void freeLine(int numOfWords, word *line); /* Frees memory allocated for a line of words */ 
/*
 * Retrieves a line of words from the standard input stream.
 * Reads characters until reaching the end of the file or the end of the line.
 * Dynamically allocates memory for words and handles illigal comma error.
 * Returns a pointer to the array of words read from the line. Returns NULL if there are errors or no words are read.
 */
word *myGetLine(int *numOfWords, flag *endOfFile);
/*
 * This function retrieves a word from the standard input stream.
 * It reads characters until encountering a space, comma, or newline character, 
 * stores the characters in the provided word structure, and handles multipale consacutive commas error.
 * Returns False if the word retrieval was successful, indicating no errors.
 */
flag getWord(flag *endOfFile, flag *endOfLine, word *currentWord, flag *ThereIsCommaInTheEndOfThePrevWord, flag *emptyLine);
/*
 * Reallocates memory for a given pointer to accommodate a new size.
 * If reallocation fails, it frees the original memory, handles the error, and exits.
 */
void *reallocMemory(int newSize, void *ptrToFree, void *ptrToReallocate);
/*
 * Searches for a command in a list of possible commands and returns a pointer to the corresponding functionWithName structure.
 * If the command is not found, it returns NULL.*/
functionWithName *getcommand(word *line, functionWithName *possibleCommands);

#endif
