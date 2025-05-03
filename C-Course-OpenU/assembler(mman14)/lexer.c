/*
 * This file is responsible for the tokenizing a given line and creating a token array containing all tokens extracted from the given line.
 * The functions in this file get a line passed through the 'line' argument of the 'tokenize' function, traverse through the line and divide
 * it into seperate tokens, containing their type and text they consist of, and add them into the tokens array.
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "lexer.h"
#include "error.h"

/* private function prototypes */

/*
 * Extracts the next token from the input stream and identifies its type.
 *
 * This function scans the input stream pointed to by 'nextToken', starting from the current position,
 * and extracts the next token. It identifies the token type and returns the token along with any error
 * encountered during the process.
 *
 * Parameters:
 * nextToken - Pointer to a pointer to the next character in the input stream. This pointer is updated
 *             to point to the next character after the extracted token.
 * error - Pointer to an Error variable to store any error encountered during token extraction.
 * lineNum - current line number, uesd for error handling.
 * fileName - current file name, used for error handling.
 *
 * Returns:
 * - A token struct representing the extracted token.
 */
static token getToken(char **nextToken, Error *error, int lineNum, char *lineName);

/*
 * Handles special cases encountered during tokenization.
 *
 * This function processes special cases that may arise during tokenization,
 * such as illegal characters or strings. It adjusts the 'endToken' pointer
 * to skip over illegal characters or the contents of a string, and updates
 * the 'error' parameter if any errors are encountered.
 *
 * Parameters:
 * endToken - Pointer to the character at the end of the current token.
 * error - Pointer to an Error variable to store any error encountered during tokenization.
 *
 * Returns:
 * A pointer to the next character in the input stream after handling special cases.
 */
static char *handleSpecialCases(char *endToken, Error *error);

/*
 * Identifies the type of the token based on its content.
 *
 * This function analyzes the content of the token specified by 'startToken' and 'endToken' to determine its type.
 * It returns a token struct containing the token's name and its identified type, along with any encountered error.
 *
 * Parameters:
 * startToken - Pointer to the beginning of the token.
 * endToken - Pointer to the end of the token.
 * error - Pointer to an Error variable to store any error encountered during token type identification.
 *
 * Returns:
 * - A token struct representing the identified type of the token.
 */
static token getTokenType(char *startToken, char *endToken, Error *error);

/*
 * Determines the type of directive based on the specified directive name.
 *
 * This function checks the provided directive name and returns the corresponding TokenType
 * representing the type of directive. It also updates the 'error' parameter if an illegal
 * directive name is encountered.
 *
 * Parameters:
 * dirName - Pointer to a string containing the directive name.
 * error - Pointer to an Error variable to store any error encountered during directive type determination.
 *
 * Returns:
 * - The TokenType representing the type of directive, or ERROR if an illegal directive name is encountered.
 */
static TokenType getDirType(char *dirName, Error *error);

/*
 * Determines the type of integer based on the specified string representation.
 *
 * This function checks if the provided string represents a valid integer.
 * If it is valid, it returns INTEGER; otherwise, it updates the 'error' parameter
 * with ILLEGAL_IDENTIFIER_INT and returns ERROR.
 *
 * Parameters:
 * intName - Pointer to a string containing the integer representation.
 * error - Pointer to an Error variable to store any error encountered during integer type determination.
 *
 * Returns:
 * - The TokenType representing the type INTEGER if string is a valid integer, otherwise ERROR.
 */
static TokenType getIntType(char *intName, Error *error);

/*
 * Determines the type of token based on the specified alphabetic string.
 *
 * This function checks if the provided string 'str' corresponds to an operation name,
 * register name, or label. It returns the TokenType representing the type of token
 * based on the match found. If 'str' does not match any known operation name, register
 * name, or label, it returns ERROR and updates the 'error' parameter accordingly.
 *
 * Parameters:
 * str - Pointer to a string to be evaluated.
 * error - Pointer to an Error variable to store any error encountered during type determination.
 *
 * Returns:
 * - The TokenType representing the type of token if 'str' is an operation name, register name,
 * or label. Otherwise, ERROR is returned,
 */
static TokenType getAlphabeticType(char *str, Error *error);

/*
 * Determines the type of special character based on the specified character.
 *
 * This function gets a character and checks if it's a special character.
 * If it is, it returns the corresponding TokenType representing it.
 * If it's not, it updates the 'error' parameter with ILLEGAL_IDENTIFIER and returns ERROR.
 *
 * Parameters:
 * character - The character to be evaluated.
 * error - Pointer to an Error variable to store any error encountered during special character type determination.
 *
 * Returns:
 * - The TokenType representing the special character provided, or ERROR if it's not a special character.
 */
static TokenType getSpecialCharType(char character, Error *error);

/*
 * Checks if a character is a character which allows the token reading proccess to be continued, and doesn't stop it.
 *
 * Parameters:
 * c - The character to be checked.
 *
 * Returns:
 * - TRUE if the character allows the reading proccess to be continued, FALSE otherwise.
 */
static int isContinueChar(const char c);

/*
 * Checks if a character is legal within the syntax of the assembly language.
 *
 * Parameters:
 * c - The character to be checked.
 *
 * Returns:
 * - TRUE if the character is legal, FALSE otherwise.
 */
static int isLegalChar(int c);

/*
 * Checks if the given string represents a valid integer.
 *
 * Parameters:
 * str - The string to be checked.
 *
 * Returns:
 * - TRUE if the string represents a valid integer, FALSE otherwise.
 */
static int isInteger(char *str);

/*
 * Checks if the string provided represents a valid label in the assembly language.
 *
 * This function takes a string and checks if it represents a valid label:
 * It checks if it starts with an alphabetical character which is followed by alphanumeric characters.
 *
 * Parameters:
 * str - The string to be checked.
 *
 * Returns:
 * - TRUE if the string represents a valid label, FALSE otherwise.
 */
static int isLabel(char *str);

/*
 * Checks if the string provided represents a valid register in the assembly language.
 *
 * This function takes a string and checks if it represents a valid register:
 * It checks if it starts with the 'r' character which is followed by a digit between 0-7.
 *
 * Parameters:
 * str - The string to be checked.
 *
 * Returns:
 * - TRUE if the string represents a valid register, FALSE otherwise.
 */
static int isRegister(char *str);

/* end of private function prototypes */

/*
 * Tokenizes the input string and stores the tokenArr in the provided array.
 *
 * Algorithm:
 * - Allocate initial memory for the tokenArr array.
 * - Iterate over the input string until a TERMINATOR token is encountered:
 *   - Extract the next token using the 'getToken' function and store it in the tokenArr array.
 *   - If an error occurs during token extraction, return the encountered error.
 *   - Increment 'tokenCount'.
 *   - If 'tokenCount' exceeds the current size of the tokenArr array, reallocate more memory for the array.
 * - Return TRUE if any error was encountered during tokenization, or FALSE if successful.
 *
 * Assumpitons:
 * - 'nc' points to a valid string representing the line to be tokenized.
 */
int tokenize(char *line, Token *tokenArr, int lineNum, char *fileName)
{
    Error error = NO_ERROR;               /* holds the error encountered or NO_ERROR otherwise */
    int tokenCount = 0;                   /* counter for token amount */
    int size = INITIAL_TOKENS_ARRAY_SIZE; /* size of memory to be allocated for the tokenArr array */
    char *nc = line;                      /* setting nc - next character to be read pointer to the first character of the line */

    /* allocating initial amount of memory and checking if it failed */
    if ((*tokenArr = (token *)malloc(size * sizeof(token))) == NULL)
    {
        setErrorStruct(MALLOC_FAILED, "");
        return TRUE;
    }

    do
    {
        /* changing nc and re-entering it every iteration */
        (*tokenArr)[tokenCount] = getToken(&nc, &error, lineNum, fileName);

        /* if an error was encountered during 'getToken', return it */
        if (error != NO_ERROR)
            return TRUE;

        tokenCount++;

        /* allocating more memory if needed */
        if (tokenCount >= size)
        {
            size *= SIZE_SCALER;
            /* checking for failed memory allocation */
            if ((*tokenArr = (token *)realloc(*tokenArr, size * sizeof(token))) == NULL)
            {
                setErrorStruct(MALLOC_FAILED, "");
                return TRUE;
            }
        }
    } while ((*tokenArr)[tokenCount - 1].type != TERMINATOR);

    return FALSE;
}

/*
 * Extracts the next token from the input stream and identifies its type.
 *
 * Algorithm:
 * - Check for leading spaces and skip them, set 'spaceBefore' accordingly.
 * - Find the end of the token by advancing 'endToken' until it reaches a non-continuation character.
 * - If the token is empty or consists of special characters, handle it using the 'handleSpecialCases' function.
 * - Identify the type of the token using the 'getTokenType' function.
 * - Set the 'spaceBefore' field of token and handle any encountered errors.
 * - Update 'nextToken' to point to the character following the extracted token.
 *
 * Assumptions:
 * - 'nextToken' points to the beginning of the next token to be extracted in the input stream.
 */
static token getToken(char **nextToken, Error *error, int lineNum, char *fileName)
{
    token token;                   /* token to be return and stored in the array */
    errorStruct *errStruct;        /* error struct to handle any error encountered */
    char *endToken;                /* pointer to be set to the first character after the end of the token */
    char *startToken = *nextToken; /* pointer to be set to the first character of the token */
    int spaceBefore;               /* boolean if there is space before the token */

    /* check for spaces and skip them */
    spaceBefore = skipSpace(&startToken);
    endToken = startToken;

    /* find the end of the token */
    while (isContinueChar(*endToken))
        endToken++;

    /* if the token is empty or is a special character */
    if (endToken == startToken)
    {
        endToken = handleSpecialCases(endToken, error);
    }

    token = getTokenType(startToken, endToken, error);
    token.spaceBefore = spaceBefore;

    /* error handling */
    errStruct = setAndGetErrorStruct(*error, token.name);
    if (errStruct->error != NO_ERROR)
        handleError(errStruct, lineNum, fileName);

    /* set up the pointer for the next token */
    *nextToken = endToken;
    return token;
}

/*
 * Handles special cases encountered during tokenization.
 *
 * Algorithm:
 * - Check if the current character pointed to by 'endToken' is an illegal character.
 *   If it is, skip over all following illegal characters until a legal character is found.
 * - If the current character is the start of a string ('\"'), iterate over the string
 *   and update the 'error' parameter if any non-printable characters or missing terminating '\"'
 *   characters are encountered.
 * - If the current character is a legal special character and not part of a string,
 *   advance the 'endToken' pointer to the next character.
 *
 * Assumptions:
 * - 'endToken' is a valid pointer to the character at the end of the current token in the input stream.
 */
static char *handleSpecialCases(char *endToken, Error *error)
{
    /* check if the character is illegal  */
    if (!isLegalChar(*endToken))
        /* if it is illegal - get all following illegal characters */
        while (!isLegalChar(*endToken))
            endToken++;
    /* if this is a start of a string */
    else if (*endToken == '\"')
    {
        /* a string starts with a \" character and includes all of the following characters (printable only)
        until the end of the line - exluding the terminating \" char */

        /* skip the first quote */
        endToken++;

        /* check if string is empty with no quote at the end */
        if ((*endToken) == '\0')
        {
            *error = MISS_QUOTES_IN_END_OF_STRING;
            return endToken;
        }

        /* get all printable characters after the " until encoutering a non-printable character */
        while (isprint(*endToken))
            endToken++;
        /* if there are no non-printable chracters in the string */
        if (*endToken == '\0')
        {
            /* if the last character is not a \" */
            if (*(endToken - 1) != '\"')
                *error = MISS_QUOTES_IN_END_OF_STRING;
        }
        /* if a non-printable character was encountered in the string */
        else
            *error = ILLEGAL_STRING_CHAR;
    }
    /* if this is a legal special character and not a string */
    else
    {
        endToken++;
    }

    return endToken;
}

/*
 * Identifies the type of the token based on its content.
 *
 * Algorithm:
 * - Save the first character after the end of the token.
 * - Set the first character after the end of the token to '\0' to terminate the token string.
 * - Copy the token to the 'name' field of the token struct.
 * - Restore the original character after the end of the token.
 * - Determine the type of the token based on its content:
 *   - If the token starts with a double quote, it is identified as a STRING.
 *   - If the token starts with a dot, its type is determined using the 'getDirType' function.
 *   - If the token starts with a plus sign, minus sign, or digit, its type is determined using the 'getIntType' function.
 *   - If the token starts with an alphabetic character, its type is determined using the 'getAlphabeticType' function.
 *   - Otherwise, its type is determined using the 'getSpecialCharType' function.
 *
 * Assumptions:
 * - 'startToken' and 'endToken' are valid pointers and delimit a valid string.
 */
static token getTokenType(char *startToken, char *endToken, Error *error)
{
    token token; /* token to be set and return for further operations */
    /* save the first character after the end of the token*/
    char tmp = *endToken;
    /* set the first character after the end of the token to '\0'*/
    *endToken = '\0';
    /* copy the token to token.name */
    strcpy(token.name, startToken);
    /* set the first character after the end of the token to its original value */
    *endToken = tmp;

    if (*startToken == '\"')
    {
        token.type = STRING;
    }
    else if (*startToken == '.')
    {
        token.type = getDirType(token.name, error);
    }
    else if (*startToken == '+' || *startToken == '-' || isdigit(*startToken))
    {
        token.type = getIntType(token.name, error);
    }
    else if (isalpha(*startToken))
    {
        token.type = getAlphabeticType(token.name, error);
    }
    else
    {
        token.type = getSpecialCharType(*startToken, error);
    }
    return token;
}

/*
 * Determines the type of directive based on the specified directive name.
 *
 * Algorithm:
 * - Check the specified 'dirName' against known directive names using string comparison, and return the corresponding type.
 * - If 'dirName' does not match any known directive names, update the 'error' parameter with ILLEGAL_DIR_NAME
 *   and return ERROR.
 *
 * Assumptions:
 * - 'dirName' points to a valid string.
 */
static TokenType getDirType(char *dirName, Error *error)
{
    if (!strcmp(dirName, ".data"))
        return DATA_DIR;
    else if (!strcmp(dirName, ".string"))
        return STRING_DIR;
    else if (!strcmp(dirName, ".extern"))
        return EXTERN_DIR;
    else if (!strcmp(dirName, ".entry"))
        return ENTRY_DIR;
    else if (!strcmp(dirName, ".define"))
        return DEF_DIR;
    else
    {
        *error = ILLEGAL_DIR_NAME;
        return ERROR;
    }
}

/*
 * Determines the type of integer based on the specified string representation.
 *
 * Algorithm:
 * - Check if 'intName' represents a valid integer using the 'isInteger' function.
 * - if it does, check if the integer exceeds limit. If it does, update the 'error' parameter and return ERROR.
 *  Otherwise, return INTEGER.
 * - If 'intName' is not a valid integer, update the 'error' parameter and return ERROR.
 *
 * Assumptions:
 * - The 'intName' parameter points to a valid string.
 */
static TokenType getIntType(char *intName, Error *error)
{
    int num;
    /* check if string represent a valid integer */
    if (isInteger(intName))
    {
        num = atoi(intName);
        /* check if integer exceeds limit */
        if (num < INTEGER_SIZE_LIMIT_NEG || INTEGER_SIZE_LIMIT_POS < num)
        {
            *error = INTEGER_EXCEEDING_LIMIT;
            return ERROR;
        }
        else /* the string represents a valid integer */
        {
            return INTEGER;
        }
    }
    else /* the string doesn't represent a valid integer */
    {
        *error = ILLEGAL_IDENTIFIER_INT;
        return ERROR;
    }
}

/*
 * Determines the type of token based on the specified alphabetic string.
 *
 * Algorithm:
 * - Check if the specified 'str' matches any known operation name in the 'opNames' array.
 *   If a match is found, return the corresponding TokenType representing the operation.
 * - If 'str' does not match any operation name, check if it is a valid register name or label.
 *   If it is a register name, return REGISTER; if it is a label, return LABEL.
 * - If 'str' does not match any known operation name, register name, or label, return ERROR.
 *
 * Assumptions:
 * - The 'str' parameter points to a valid string.
 * - The values of operation constant in the TokenType enum match the indexes of the array representing them.
 */
static TokenType getAlphabeticType(char *str, Error *error)
{
    int i;
    /* the values of the constants MOV, ... , HLT are identical to the values of the indexes containing thier string representation in this array,
    so that opNames[index] is representing the constant with the value index */
    char *opNames[OPERATION_AMOUNT] =
        {"mov", "cmp", "add", "sub",
         "lea", "not", "clr", "inc",
         "dec", "jmp", "bne", "red",
         "prn", "jsr", "rts", "hlt"};
    /* check if str is an operation name */
    for (i = MOV; i < OPERATION_AMOUNT; i++)
    {
        /* if str is an operation name - return the TokenType corresponding to it */
        if (!strcmp(str, opNames[i]))
        {
            return i; /* return the index number, which is identical to the value of the enum it represents */
        }
    }
    /* str is not an operation name - check for register name or label */
    if (isRegister(str))
        return REGISTER;
    else if (isLabel(str))
        return LABEL;
    return ERROR;
}

/*
 * Determines the type of special character based on the specified character.
 *
 * Algorithm:
 * - Use a switch statement to check if the 'character matches any known special character (':', ',', '[', ']', '#', '=', '\0'),
 *   return the corresponding TokenType representing the type of special character.
 * - If 'character' does not match any known special character, update the 'error' parameter
 *   with ILLEGAL_IDENTIFIER and return ERROR.
 */
static TokenType getSpecialCharType(char character, Error *error)
{
    switch (character)
    {
    case ':':
        return COLONS;
    case ',':
        return COMMA;
    case '[':
        return LEFT_SQ_BRACKET;
    case ']':
        return RIGHT_SQ_BRACKET;
    case '#':
        return NUMBER_SIGN;
    case '=':
        return EQUAL_SIGN;
    case '\0':
        return TERMINATOR;
    default:
        *error = ILLEGAL_IDENTIFIER;
        return ERROR;
    }
}

/*
 * Checks if a character is a character which allows the token reading proccess to be continued, and doesn't stop it.
 *
 * Algorithm:
 * - return if c is an alphabetical character or a digit using 'isalnum' function or '+' or '-' or '.'.
 */
static int isContinueChar(char c)
{
    return isalnum(c) || c == '+' || c == '-' || c == '.';
}

/*
 * Checks if a character is legal within the syntax of the assembly language.
 *
 * Algorithm:
 * - return if c is an continue char (using 'isContinueChar') or space (using 'isspace') or '=' or '#'
 *  or '[' or ']' or ':' or ',' or '\"' or '\0'.
 */
static int isLegalChar(int c)
{
    return isContinueChar(c) || isspace(c) || c == '=' || c == '#' || c == '[' || c == ']' || c == ':' || c == ',' || c == '\"' || c == '\0';
}

/*
 * Checks if the given string represents a valid integer.
 *
 * Algorithm:
 * - If 'str' is an empty string, return 0 - it doesn't represent an integer.
 * - Check for optional '+' or '-' at the start of the string.
 * - Loop through all the following digits until encountering a non-digit character.
 *  If this character is the null character - Return TRUE, the string represents an integer,
 *  it it isn't - Return FALSE, the string doesn't represent an integer.
 *
 * Assumptions:
 * - The 'str' parameter points to a valid string.
 */
static int isInteger(char *str)
{
    char *p = str; /* string pointer */
    /* if the first character of str is null, str is an emptry string and thus not representing an integer */
    if (*p == 0)
        return FALSE;

    /* checking for optional + or - sign at the start of int */
    if (*p == '+' || *p == '-')
        p++;
    /* advancing p to the first non digit character */
    while (isdigit(*p))
    {
        p++;
    }
    /* if p didn't reach the end of the string - there is a non digit character in the string (apart from + or -)*/
    if (*p)
        return FALSE;
    /* p reached the end of the string - there are no non digit characters in the string (apart from + or -) */
    return TRUE;
}

/*
 * Checks if the string provided represents a valid label in the assembly language.
 *
 * Algorithm:
 * - Check if the first character of 'str' is an alphabetial character. If it's not - return 0.
 * - Loop through the following characters until encountering the first non-alphanumerical character.
 *  If this character is the null character - Return 1, the string represents a valid label,
 *  it it isn't - Return 0, the string doesn't represent a valid label.
 *
 * Assumptions:
 * - The 'str' parameter points to a valid string.
 */
static int isLabel(char *str)
{
    char *p = str; /* string pointer */
    if (!isalpha(*p))
        return 0;
    p++;

    while (isalnum(*p))
        p++;

    if (*p)
        return FALSE;
    return TRUE;
}

/*
 * Checks if the string provided represents a valid register in the assembly language.
 *
 * Algorithm:
 * - Check if the first character of 'str' is 'r' and if the following character is a digit between 0-7.
 *  If it is - Return TRUE, the string represent a register. Otherwise return FALSE.
 *
 * Assmptions:
 * - 'str' is a valid pointer to a string.
 */
static int isRegister(char *str)
{
    if (*str == 'r' && '0' + MIN_REGISTER_NUM <= str[1] && str[1] <= '0' + MAX_REGISTER_NUM)
        return TRUE;

    return FALSE;
}

/*
 * Skips leading whitespace characters in the input string.
 *
 * Algorithm:
 * - Initialize 'ret' to FALSE.
 * - Iterate over the input string until a non-whitespace character is encountered:
 *   - Advance the pointer pointed to by 'nc' to the next character.
 *   - Set 'ret' to TRUE.
 * - Return 'ret' indicating whether any whitespace characters were skipped.
 *
 * Assumptions:
 * - 'nc' is a valid pointer to pointer to a valid string.
 */
int skipSpace(char **nc)
{
    int ret = FALSE; /* boolean to be returned, representing if there is space before the string to which we skip spaces */
    while (isspace(**nc))
    {
        (*nc)++;
        ret = TRUE;
    }
    return ret;
}