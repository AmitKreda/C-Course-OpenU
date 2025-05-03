#ifndef LEXER_H
#define LEXER_H

#include "error.h"

#define INITIAL_TOKENS_ARRAY_SIZE 5 /* the initial size allocated for the tokens array */
#define MAX_REGISTER_NUM 7          /* biggest register serial number (there are 8 registers ordered between 0-7)*/
#define MIN_REGISTER_NUM 0          /* smallest register serial number (there are 8 registers ordered between 0-7)*/
#define SIZE_SCALER 2

/* enum representing the different token types */
typedef enum
{
    /* all different operations in the assembly language, ordered by number of operands */
    MOV,
    CMP,
    ADD,
    SUB,
    LEA,
    NOT,
    CLR,
    INC,
    DEC,
    JMP,
    BNE,
    RED,
    PRN,
    JSR,
    RTS,
    HLT,

    LABEL,
    REGISTER,
    DATA_DIR,
    STRING_DIR,
    EXTERN_DIR,
    ENTRY_DIR,
    DEF_DIR,
    CONSTANT,
    INTEGER,
    STRING,
    COLONS,
    COMMA,
    LEFT_SQ_BRACKET,
    RIGHT_SQ_BRACKET,
    NUMBER_SIGN,
    EQUAL_SIGN,
    TERMINATOR,
    ERROR
} TokenType;

typedef struct
{
    char name[MAX_LINE_LEN]; /* a token name can be the length of an entire legal line */
    TokenType type;          /* type of token */
    int spaceBefore;         /* boolean variable to hold wether there was space before the token or not */
} token;

typedef token *Token; /* represents a pointer to a token */

/*
 * Tokenizes the input string, dynamically allocated memory for the pointer provided and stores the tokens in it.
 *
 * This function tokenizes the input string 'nc' and stores the resulting tokens
 * in the 'tokens' array. It dynamically allocates memory for the tokens pointer and reallocates
 * more memory if needed during the tokenization process.
 *
 * Parameters:
 * line - Pointer to a null-terminated string representing the input to be tokenized.
 * tokens - Pointer to an array of tokens to store the resulting tokens.
 * lineNum - Current line number, used for error handling.
 * fileName - Current file name, used for error handling.
 *
 * Returns:
 * TRUE if any error was encountered during tokenization, or FALSE if successful.
 */
int tokenize(char *line, Token *tokens, int lineNum, char *fileName);

/*
 * Skips leading whitespace characters in the input string.
 *
 * This function advances the pointer pointed to by 'nc' past any leading whitespace characters
 * in the input string. It returns TRUE if at least one whitespace character is skipped, and
 * FALSE otherwise.
 *
 * Parameters:
 * nc - Pointer to a pointer to the input string. This pointer is updated to point to the next
 *      non-whitespace character in the string.
 *
 * Returns:
 * TRUE if at least one whitespace character is skipped, FALSE otherwise.
 */
int skipSpace(char **nc);

#endif