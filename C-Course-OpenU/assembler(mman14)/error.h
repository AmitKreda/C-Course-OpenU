#ifndef ERROR_H
#define ERROR_H

/*
 * maximum and minimum values for an integer in the assembly language:
 * integers exceeding this limit take up more than 12 bits of memory and can't be used inside operation statements,
 * because a word size is 14 bits and the first two bits are A,R,E bits. Thus, only integers within those
 * limits can be stored inside operantion statements words in the assembly.
 */
#define INTEGER_SIZE_LIMIT_POS 2047
#define INTEGER_SIZE_LIMIT_NEG -2048

#define MAX_LABEL_LENGTH 32 /* max label length is 31 + '\0' character */
#define MAX_LINE_LEN 81     /* maximum length of line is 80 + '\n' character */
#define OPERATION_AMOUNT 16 /* number of operation in the assembly language */

#define TRUE 1
#define FALSE 0

#define NO_LINE_NUM -1 /* passed to the handleError function to indicate that no line number should be printed */
#define ONE_UNIT 1     /* constant for allocating only one unit of memory */

#define MAX_ERROR_NAME_LEN 81 /* maximum name length for error struct name */

/* enum containing all errors in the assembly language */
typedef enum error
{
    /* common errors */
    NO_ERROR,
    EX_TEXT,
    MALLOC_FAILED,
    FOPEN_FAILED,

    /* pre-assembler errors */

    ILLEGAL_MACRO_NAME,

    /* lexing errors */
    MISS_QUOTES_IN_END_OF_STRING,
    ILLEGAL_STRING_CHAR,
    ILLEGAL_DIR_NAME,
    ILLEGAL_IDENTIFIER_INT,
    ILLEGAL_IDENTIFIER,
    INTEGER_EXCEEDING_LIMIT,

    /* parsing errors */
    MISS_STRING,
    MISS_COMMA,
    MISS_OPRD,
    MISS_IDX,
    MISS_COLONS,
    MISS_RIGHT_BRACKET,
    MISS_DIR_OP_STATEMENT,
    MISS_LABEL_PARAM,
    MISS_EQUAL,
    MISS_INT_VAL,
    MISS_CONST,
    MISS_DATA_VALS,

    EXPC_LABEL_PARAM,
    EXPC_INT,
    EXPC_INT_OR_CONST,
    EXPC_STRING,
    EXPC_EQUAL,

    ILLEGAL_OPRD,
    ILLEGAL_IDX,
    ILLEGAL_CONST,
    ILLEGAL_COMMA,

    SPACE_BEFORE_TOKEN,
    LABEL_BEFORE_CONST_DEF,
    MEANINGLESS_LABEL_DEF,
    LABEL_TOO_LONG,
    CONST_TOO_LONG,
    MUL_CON_COMMA,
    EX_COMMA,

    /* first pass erros */
    REDEFINITION_OF_CONSTANT,
    UNDEFINED_CONSTANT,
    EXPECTED_CONST_GOT_LABEL,
    REDEFINITION_OF_SYMBOL,
    PROGRAM_MEMORY_RUN_OUT,
    MISMATCH_DEST_OPERAND,
    MISMATCH_SRC_OPERAND,

    /* second pass errors */
    UNDEFINED_LABEL,
    EXPECTED_LABEL_GOT_CONST,
    ARRAY_INDEX_OUT_OF_BOUNDS,
    ILLEGAL_IDX_ADDRESING
} Error;

typedef struct
{
    Error error;
    char name[MAX_ERROR_NAME_LEN];
} errorStruct;

/*
 * Returns the address of the static variable 'error'.
 *
 * Returns:
 * - A pointer to an errorStruct containing the address of the static variable 'error'.
 */
errorStruct *getErrorStruct(void);

/*
 * Sets the static variable 'error' fields according to the error and name provided.
 *
 * Parametes:
 * error - The Error to set the 'error' variable error field to.
 * name - The string to set the 'error' variable name field to.
 */
void setErrorStruct(Error error, char *name);

/*
 * Sets the static variable 'error' according to the error and name provided, and returns its address.
 *
 * Parametes:
 * error - The Error to set the 'error' variable error field to.
 * name - The string to set the 'error' variable name field to.
 *
 * Returns:
 * - A pointer to an errorStruct containing the address of the static variable 'error' after the changes.
 */
errorStruct *setAndGetErrorStruct(Error error, char *name);

/*
 * Handles an error by printing an error message to stderr based on the error code.
 *
 * This function takes an errorStruct pointer as input and prints an appropriate error message
 * to the standard error output based on the error code stored in the errorStruct,
 * with the exact location (line number and file name) where the error occured.
 *
 * Parameters:
 * error - Pointer to an errorStruct containing error information.
 * lineNum - The line number where the error occured.
 * fileName - Pointer to the name of the file where the error ocurred.
 */
void handleError(errorStruct *error, int lineNum, char *fileName);

#endif