/*
 * This file is responsible for error handling. It handles the static 'error' variable
 * which can be set and updated to indicate that an error occured, along with details about the error,
 * and a handleError function which gets an errorSturct and prints an appropriate error
 * to the standard error file, together with the location of the error in the assembly files.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

/*
 * Returns the address of the static variable 'error'.
 *
 * Alogrithm:
 * - Declare the static variable 'error'.
 * - Return the address of 'error'.
 */
errorStruct *getErrorStruct(void)
{
    static errorStruct error;
    return &error;
}

/*
 * Sets the static variable 'error' fields according to the error and name provided.
 *
 * Algorithm:
 * - Use 'getErrorSturct' to get the static 'error' variable.
 * - Set its fields: error and name to the error and name provided.
 *
 * Assumptions:
 * - 'name' points to a valid string.
 */
void setErrorStruct(Error error, char *name)
{
    /* get the error sturct */
    errorStruct *errStruct = getErrorStruct();

    /* change its field according to the error and name provided */
    errStruct->error = error;
    strcpy(errStruct->name, name);
}

/*
 * Sets the static variable 'error' according to the error and name provided, and returns its address.
 *
 * Algorithm:
 * - Set 'error' using 'setErrorStruct' according to the error and name provided.
 * - Return its address using 'getErrorStruct'.
 */
errorStruct *setAndGetErrorStruct(Error error, char *name)
{
    setErrorStruct(error, name);
    return getErrorStruct();
}

/*
 * Handles an error by printing an error message to stderr based on the error code.
 *
 * Algorithm:
 * - Check the error code stored in the 'error' field of the provided errorStruct.
 * - Print the corresponding error message to stderr.
 *
 * Notes:
 * - This function prints error messages to the standard error output (stderr).
 */
void handleError(errorStruct *error, int lineNum, char *fileName)
{
    if (lineNum == NO_LINE_NUM)
        fprintf(stderr, "%s: ", fileName);
    else
        fprintf(stderr, "%s in line %d: \n", fileName, lineNum);

    switch (error->error)
    {
    case FOPEN_FAILED:
        fprintf(stderr, "Error: File %s does not exist or count not be opened.\n", fileName);
        break;
    case MALLOC_FAILED:
        fprintf(stderr, "Error: Memory allocatoin failure.\n");
        break;

    case ILLEGAL_MACRO_NAME:
        fprintf(stderr, "Error: Illegal macro name.\n");
        break;

    case MISS_QUOTES_IN_END_OF_STRING:
        fprintf(stderr, "Error: String should end with a '\\\"' character.\n");
        break;
    case ILLEGAL_STRING_CHAR:
        fprintf(stderr, "Error: A string can include only printable characters.\n");
        break;
    case ILLEGAL_DIR_NAME:
        fprintf(stderr, "Error: directive name '%s' is illegal.\n", error->name);
        break;
    case ILLEGAL_IDENTIFIER_INT:
        fprintf(stderr, "Error: Identifier '%s' is illegal. \nNote: Label names can start only with an alphabetical character, and integers can't have letters in them.\n", error->name);
        break;
    case ILLEGAL_IDENTIFIER:
        fprintf(stderr, "Error: Identifier '%s' is illegal.\n", error->name);
        break;
    case INTEGER_EXCEEDING_LIMIT:
        fprintf(stderr, "Error: Integer size exceeding limit. An integer can be only between -2048 to 2047.\n");
        break;

    case MISS_STRING:
        fprintf(stderr, "Error: No string provided after .string directive.\n");
        break;
    case MISS_COMMA:
        fprintf(stderr, "Error: Missing comma %s.\n", error->name);
        break;
    case MISS_OPRD:
        fprintf(stderr, "Error: Missing operand.\n");
        break;
    case MISS_IDX:
        fprintf(stderr, "Error: Missing index for label '%s'.\n", error->name);
        break;
    case MISS_COLONS:
        fprintf(stderr, "Error: Missing colons after label definition.\n");
        break;
    case MISS_RIGHT_BRACKET:
        fprintf(stderr, "Error: Missing right bracket after index.\n");
        break;
    case MISS_DIR_OP_STATEMENT:
        fprintf(stderr, "Error: Missing directive or operation statement %s.\n", error->name);
        break;
    case MISS_LABEL_PARAM:
        fprintf(stderr, "Error: Missing label parameter after directive.\n");
        break;
    case MISS_EQUAL:
        fprintf(stderr, "Error: Missing equal sign after constant name.\n");
        break;
    case MISS_INT_VAL:
        fprintf(stderr, "Error: Missing integer value after equal sign.\n");
        break;
    case MISS_CONST:
        fprintf(stderr, "Error: Missing constant name after .define directive.\n");
        break;
    case MISS_DATA_VALS:
        fprintf(stderr, "Error: Missing values after .data directive.\n");
        break;

    case EX_COMMA:
        fprintf(stderr, "Error: Extraneous comma at the end of .data directive.\n");
        break;
    case EX_TEXT:
        fprintf(stderr, "Error: Extraneous text %s.\n", error->name);
        break;

    case EXPC_LABEL_PARAM:
        fprintf(stderr, "Error: Expected label after directive, got '%s' instead.\n", error->name);
        break;
    case EXPC_INT:
        fprintf(stderr, "Error: Expected integer, got '%s' instead.\n", error->name);
        break;
    case EXPC_INT_OR_CONST:
        fprintf(stderr, "Error: Expected constant name or integer, got '%s' instead.\n", error->name);
        break;
    case EXPC_STRING:
        fprintf(stderr, "Error: Expected string after .string directive, got '%s' insted.\n", error->name);
        break;
    case EXPC_EQUAL:
        fprintf(stderr, "Error: Expected equal sign after constant name, got '%s' instead.\n", error->name);
        break;
    case ILLEGAL_OPRD:
        fprintf(stderr, "Error: %s operand is illegal.\n", error->name);
        break;
    case ILLEGAL_IDX:
        fprintf(stderr, "Error: Index for label '%s' is illegal.\n", error->name);
        break;
    case ILLEGAL_CONST:
        fprintf(stderr, "Error: Constant name is illegal.\n");
        break;
    case ILLEGAL_COMMA:
        fprintf(stderr, "Error: Illegal comma.\n");
        break;

    case SPACE_BEFORE_TOKEN:
        fprintf(stderr, "Error: Space before %s. There should be no space there.\n", error->name);
        break;
    case LABEL_BEFORE_CONST_DEF:
        fprintf(stderr, "Error: A label can't be defined before a constant definition directive.\n");
        break;
    case MEANINGLESS_LABEL_DEF:
        fprintf(stderr, "Warning: Meaningless label definition before directive.\n");
        break;
    case LABEL_TOO_LONG:
        fprintf(stderr, "Error: Label '%s' is too long to be defined. Max label length is %d.\n", error->name, MAX_LABEL_LENGTH - 1);
        break;
    case CONST_TOO_LONG:
        fprintf(stderr, "Error: Constant name '%s' is too long to be defined. Max constant name length is %d.\n", error->name, MAX_LABEL_LENGTH - 1);
        break;
    case MUL_CON_COMMA:
        fprintf(stderr, "Error: Multiple consecutive commas.\n");
        break;

    case REDEFINITION_OF_CONSTANT:
        fprintf(stderr, "Error: The constant %s has already been already defined. Redefinition of a constant is illegal.\n", error->name);
        break;
    case UNDEFINED_CONSTANT:
        fprintf(stderr, "Error: The constant %s is undefined.\n", error->name);
        break;
    case UNDEFINED_LABEL:
        fprintf(stderr, "Error: The label %s is undefined.\n", error->name);
        break;
    case EXPECTED_CONST_GOT_LABEL:
        fprintf(stderr, "Error: Expected a constant, got label '%s' instead.\n", error->name);
        break;
    case EXPECTED_LABEL_GOT_CONST:
        fprintf(stderr, "Erorr: Expected a label, got constant '%s' instead.\n", error->name);
        break;
    case REDEFINITION_OF_SYMBOL:
        fprintf(stderr, "Error: The label '%s' has already been defined. Redefinition of a label is illegal/\n", error->name);
        break;
    case PROGRAM_MEMORY_RUN_OUT:
        fprintf(stderr, "Error: Program memory ran out.\n");
        break;
    case MISMATCH_DEST_OPERAND:
        fprintf(stderr, "Error: Destination operand addressing method is illegal for this operation.\n");
        break;
    case MISMATCH_SRC_OPERAND:
        fprintf(stderr, "Error: Source operand addressing method is illegal for this operation.\n");
        break;
    case ILLEGAL_IDX_ADDRESING:
        fprintf(stderr, "Error: Attempted index addressing on a non-data non-extern label '%s'.\n", error->name);
        break;
    case ARRAY_INDEX_OUT_OF_BOUNDS:
        fprintf(stderr, "Error: Array '%s' index out of bounds.\n", error->name);
        break;
    default:
        break;
    }

    fprintf(stderr, "\n");
}
