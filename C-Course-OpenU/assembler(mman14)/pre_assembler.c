/* This file is responsible for the pre-processing stage.
 * Accoring to the project instruction, we are allowed to assume things about code structure and macros:
 *
 * Assumptions:
 *   - No recursive macro definition.
 *   - Macros are defined before their calls in the program.
 *   - No two macros are defined with the same name.
 *   - Every macro definition ends with an 'endmcr'.
 *   - The program needs to report only one error for each macro
 *    (if there is an error in the macro name defintion, an error in the macro end defintion will not be reported).
 *   - If an error is found, the .am file should be deleted and  processing sould not be continued.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "error.h"
#include "trie.h"
#include "lexer.h"
#include "pre_assembler.h"

/* private function prototypes */

/*
 * Processes the assembly file and writes the processed content into a new file.
 *
 * This function processes the assembly file 'as' and writes the processed content into a new file with the
 * same name but with the postfix changed to ".am". It also handles errors that occur during the processing.
 *
 * Parameters:
 * as - Pointer to the assembly file.
 * fileName - Pointer to the name of the assembly file.
 *
 * Returns:
 * - TRUE if any error occurs during the processing, and FALSE otherwise.
 */
static int processFile(FILE *fp, char *fileName);

/*
 * Processes a line from the assembly file.
 *
 * This function processes a line from the assembly file 'as'. If the line contains a macro name that exists
 * in the macro table, it writes the macro content into the assembly output file 'am'. If the line is a macro
 * definition line, it inserts the macro name into the macro table and retrieves the macro text. If the line
 * isn't a macro, it writes it into the output file 'am' without changes.
 *
 * Parameters:
 * macroTable - Pointer to the trie node representing the macro table.
 * line - Pointer to the line to be processed from the assembly file.
 * lineNum - Pointer to the current line number in the assembly file.
 * as - Pointer to the assembly file.
 * am - Pointer to the assembly output file.
 *
 * Returns:
 * - Pointer to an error struct containing the error code and description.
 */
static errorStruct *processLine(TrieNode *macroTable, char *line, int *lineNum, FILE *as, FILE *am);

/*
 * Retrieves the macro text from the assembly file and stores it in the macro table.
 *
 * This function retrieves the macro text from the assembly file 'as' until encountering 'endmcr' and stores it in the macro table.
 * The function updates the line number 'lineNum' and returns TRUE if any extraneous text is encountered after "endmcr".
 *
 * Parameters:
 * macroTable - Pointer to the trie node representing the macro table.
 * macroName - Pointer to the macro name.
 * lineNum - Pointer to the current line number in the assembly file.
 * as - Pointer to the assembly file.
 *
 * Returns:
 * - TRUE if an error occured, and FALSE otherwise.
 */
static int getMacroText(TrieNode *macroTable, char *macroName, int *lineNum, FILE *as);

/*
 * Checks if a given string is a legal macro name.
 *
 * This function checks if the given string is a legal macro name.
 * The rules for a legal macro name are as follows:
 * - It must begin with an alphabetic character.
 * - It must contain only printable characters.
 * - It cannot be "mcr" or "endmcr".
 * - It cannot be a register name (r0-7).
 * - It cannot be one of the predefined operation names.
 *
 * Parameters:
 * name - Pointer to the string to be checked.
 *
 * Returns:
 * - TRUE if the string is a legal macro name, and FALSE otherwise.
 */
static int isLegalMacroName(char *name);

/*
 * Checks if a line is a comment or empty.
 *
 * This function checks if the given line is a comment or empty. A line is considered a comment if it begins
 * with a semicolon (';'). It is considered empty if it contains only whitespace characters or a newline.
 *
 * Parameters:
 * line - Pointer to the beginning of the line to be checked.
 *
 * Returns:
 * - TRUE if the line is a comment or empty, and FALSE otherwise.
 */
static int isCommentOrEmpty(char *line);

/*
 * Extracts the next word from the input stream pointed to by 'nc'.
 *
 * This function extracts the next word from the input stream pointed to by 'nc'. It skips leading spaces and
 * reads characters until it encounters a space, a newline, or the end of the file. It updates the pointer 'word'
 * to point to the next position in the character array where the next character of the word will be stored.
 *
 * Parameters:
 * word - Pointer to the character array where the extracted word will be stored.
 * nc - Pointer to a pointer to the next character in the input stream. This pointer is updated to point to the
 *      next character after the extracted word.
 *
 * Returns:
 * - TRUE if there are more characters after the extracted word.
 * - FALSE if the extracted word reaches the end of the line.
 */
static int getWord(char *p, char **nc);

/* end of private function prototypes */

/*
 * Pre-processes the assembly file before assembling.
 *
 * Algorithm:
 * - Opens the assembly file with postfix ".as" for reading.
 * - If the file couldn't be opened, it prints an error message and returns EXIT_FAILURE.
 * - Process the file and handle any error that occured during the processing using the processFile function.
 * - If an error occures, handle it, remove the ".am" file and return EXIT_FAILURE.
 * - Otherwise, return EXIT_SUCCESS.
 *
 * Assumptions:
 * - 'fileName' is a valid pointer to a string representing the file name.
 */
int preAssembler(char *fileName)
{
    FILE *as;               /* file pointer for the .as file to be read */
    errorStruct *errStruct; /* stores the error returned from getErrorStruct, to avoid calling the funtion multiple times */

    /* open fileName.as and check if an error occured */
    if ((as = fopen(strcat(fileName, ".as"), "r")) == NULL)
    {
        setErrorStruct(FOPEN_FAILED, fileName);
        handleError(getErrorStruct(), NO_LINE_NUM, fileName);
        return EXIT_FAILURE;
    }

    /* process the file and check if an error occured */
    if (processFile(as, fileName))
    {
        errStruct = getErrorStruct();

        if (errStruct->error == FOPEN_FAILED)
        {
            handleError(errStruct, NO_LINE_NUM, fileName);
            return EXIT_FAILURE;
        }
        /* fileName's postfix was changed to .am by processFile */
        remove(fileName);

        if (errStruct->error == MALLOC_FAILED)
        {
            handleError(errStruct, NO_LINE_NUM, fileName);
            exit(EXIT_FAILURE);
        }

        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/*
 * Processes the assembly file and writes the processed content into a new file.
 *
 * Algorithm:
 * - Change the postfix of the file name to ".am" and open the ".am" file for writing the processed content.
 * - Change the postix of the file name to ".as" for error handling.
 * - Create a trie node representing the macro table.
 * - Read each line from the assembly file and append lineNum.
 * - Process the line read using the processLine function, only if it's not an empty or comment line.
 * - If any erros occured during processing, handle them accordingly.
 * - Free memory and close files after processing.
 * - Change the postfix of the file name to ".am" for further processing.
 *
 * Assumptions:
 * - 'fileName' is a valid pointer to a string representing the file name.
 *
 */
static int processFile(FILE *as, char *fileName)
{
    char line[MAX_LINE_LEN];
    int errorOcurred = FALSE;
    int lineNum;
    FILE *am; /* file pointer for the am file to be written */
    TrieNode *macroTable;

    /* change the postfix to .am */
    fileName[strlen(fileName) - 1] = 'm';
    /* open .am file for writing the processed file */
    if ((am = fopen(fileName, "w")) == NULL)
    {
        setErrorStruct(FOPEN_FAILED, fileName);
        return TRUE;
    }

    /* change the postfix back to .as for error handling */
    fileName[strlen(fileName) - 1] = 's';

    /* create macro table and check for error */
    if ((macroTable = createTrie()) == NULL)
    {
        setErrorStruct(MALLOC_FAILED, "");
        return TRUE;
    }

    for (lineNum = 1; fgets(line, MAX_LINE_LEN, as) != NULL; lineNum++)
    {
        /* process the line only if it's not an empty or comment line */
        if (!isCommentOrEmpty(line))
        {
            /* process the line and check for error */
            if (processLine(macroTable, line, &lineNum, as, am)->error != NO_ERROR)
            {
                if (getErrorStruct()->error == MALLOC_FAILED)
                {
                    /* free memory, close files and exit the program */
                    deleteTrie(macroTable);
                    fclose(am);
                    fclose(as);
                    exit(EXIT_FAILURE);
                }
                errorOcurred = TRUE;
                handleError(getErrorStruct(), lineNum, fileName);
            }
        }
    }

    /* free memory and close files */
    deleteTrie(macroTable);
    fclose(am);
    fclose(as);

    /* change postfix to .am for further processing */
    fileName[strlen(fileName) - 1] = 'm';

    return errorOcurred;
}

/*
 * Processes a line from the assembly file.
 *
 * Algorithm:
 * - Gets the first word from the line.
 * - Check if the word exists in the macro table. If it does, write the macro content into the output file 'am'.
 * - If the line is a macro definition line, insert the macro name into the macro table and retrieve the macro text.
 * - If the line isn't a macro, write it into the output file 'am' without changes.
 *
 * Assumptions:
 * - 'macroTable' points to the trie node representing the macro table.
 * - 'lineNum' points to the current line number in the assembly file.
 */
static errorStruct *processLine(TrieNode *macroTable, char *line, int *lineNum, FILE *as, FILE *am)
{
    char name[MAX_LABEL_LENGTH], macroName[MAX_LABEL_LENGTH];
    char *nc = line;
    int textAfterWord;
    TrieNode *searchResult;

    /* getting the first word */
    getWord(name, &nc);

    /* checkig if word exists in macro table */
    searchResult = search(macroTable, name);
    /* if it exists, write the macro content into the file */
    if (searchResult)
    {
        fputs((char *)searchResult->endOfWord, am);
    }

    /* if line is a macro definition line */
    else if (!strcmp(name, "mcr"))
    {
        /* get macro name and check for EX_TEXT */
        textAfterWord = getWord(macroName, &nc);

        /* error handling */
        if (textAfterWord)
            return setAndGetErrorStruct(EX_TEXT, "after macro definition");
        if (!isLegalMacroName(macroName))
            return setAndGetErrorStruct(ILLEGAL_MACRO_NAME, "");

        /* insert macro name into the tree */
        if (insertString(macroName, "", macroTable))
            return setAndGetErrorStruct(MALLOC_FAILED, "");

        /* get the macro text in the table and return an error if any occured*/
        if (getMacroText(macroTable, macroName, lineNum, as))
            return getErrorStruct();
    }
    /* if line isn't a macro, write it in the file without changes */
    else
    {
        fputs(line, am);
    }
    return setAndGetErrorStruct(NO_ERROR, "");
}

/*
 * Retrieves the macro text from the assembly file and stores it in the macro table.
 *
 * Algorithm:
 * - Reads the macro text from the assembly file 'as' until encountering "endmcr".
 * - Appends the macro text to the 'macroText' string.
 * - Updates the line number 'lineNum'.
 * - If any extraneous text is encountered after "endmcr", the function returns TRUE.
 * - Otherwise, it stores the macro text in the macro table under the 'macroName' and returns FALSE.
 *
 * Assumptions:
 * - 'macroTable' points to the trie node representing the macro table.
 * - 'lineNum' points to the current line number in the assembly file.
 */
static int getMacroText(TrieNode *macroTable, char *macroName, int *lineNum, FILE *as)
{
    char line[MAX_LINE_LEN];
    char name[MAX_LABEL_LENGTH];
    char *macroText, *nc;
    int flag = IN_MCR;
    int textAfterWord;

    /* allocate intial memory and check for error */
    if ((macroText = (char *)calloc(ONE_UNIT, sizeof(char))) == NULL)
    {
        setErrorStruct(MALLOC_FAILED, "");
        return TRUE;
    }

    while (flag == IN_MCR && fgets(line, MAX_LINE_LEN, as) != NULL)
    {
        (*lineNum)++;
        nc = line;

        if (!isCommentOrEmpty(line))
        {
            textAfterWord = getWord(name, &nc);
            /* if first word in line is not endmcr */
            if (strcmp(name, "endmcr"))
            {
                /* append the macro text length by the legnth of the line being added */
                macroText = realloc(macroText, (strlen(macroText) + strlen(line) + ONE_BIT_FOR_NULL_CHAR) * sizeof(char));

                /* check for memory allocation failure */
                if (macroText == NULL)
                {
                    setErrorStruct(MALLOC_FAILED, "");
                    return TRUE;
                }

                /* add the line to macro text */
                strcat(macroText, line);
            }
            /* if first word in line is endmcr */
            else
            {
                /* check for extreneous text after endmcr */
                if (textAfterWord)
                {
                    /* extreneous text after endmcr encountered */
                    setErrorStruct(EX_TEXT, "after endmcr keyword");
                    return TRUE;
                }
                /* no error encountered - assign the macro text into the macroName in the macroTable */
                search(macroTable, macroName)->endOfWord = macroText;
                /* no error encountered */
                return FALSE;
            }
        }
    }
    /* no error encountered */
    return FALSE;
}

/*
 * Checks if a given string is a legal macro name.
 *
 * Algorithm:
 * - If the first character of the string is not an alphabetic character, return FALSE.
 * - Iterate over the string until encountering a non-printable character.
 * - If the character encoutered is not the null '\0' character, return FALSE.
 * - Check if the string is "mcr" or "endmcr", if so, return FALSE.
 * - Check if the string is in the format of register names (r0-7), if so, return FALSE.
 * - Check if the string matches one of the predefined operation names, if so, return FALSE.
 * - Otherwise, return TRUE.
 *
 * Assumptions:
 * - 'name' points to the string to be checked.
 */
static int isLegalMacroName(char *name)
{
    char *p = name;
    int i;
    /* array containing all operation names in the assembly language */
    char *opNames[OPERATION_AMOUNT] =
        {"mov", "cmp", "add", "sub",
         "lea", "not", "clr", "inc",
         "dec", "jmp", "bne", "red",
         "prn", "jsr", "rts", "hlt"};
    if (!isalpha(*p))
        return FALSE;

    /* iterate over the string until encountering a non-printable character */
    while (isprint(*p))
    {
        p++;
    }
    /* if the non-printable character is not '\0', there is a non-printable character in the macro name */
    if (*p != '\0')
        return FALSE;

    /* check if macro name is a pre-defined word */
    if (!strcmp(name, "mcr") ||
        !strcmp(name, "endmcr"))
        return FALSE;

    /* check if macro name is a register name */
    if (name[0] == 'r' && '0' <= name[1] && name[1] <= '7')
        return FALSE;

    /* check if macro name is an operation name */
    for (i = 0; i < OPERATION_AMOUNT; i++)
        if (!strcmp(name, opNames[i]))
            return FALSE;

    return TRUE;
}

/*
 * Checks if a line is a comment or empty.
 *
 * Algorithm:
 * - If the first character of the line is a semicolon (';'), return TRUE.
 * - Skip leading whitespace characters in the line.
 * - If the line contains only whitespace characters or ends with a newline, return TRUE.
 * - Otherwise, return FALSE.
 *
 * Assumptions:
 * - 'line' points to a valid string.
 */
static int isCommentOrEmpty(char *line)
{
    if (*line == ';')
    {
        return TRUE;
    }
    while (isspace(*line) && *line != '\n')
        line++;
    if (*line == '\n')
        return TRUE;
    return FALSE;
}

/*
 * Extracts the next word from the input stream and updates the pointer to the next character.
 *
 * Algorithm:
 * - Skip leading spaces in the input stream using the 'skipSpace' function.
 * - Extract the word character by character until encountering a space, a newline, or the end of the file.
 * - Update the pointer 'word' to point to the next position where the next character of the word
 *   will be stored.
 * - Skip spaces after the word and advance nc until encoutering a non-space character or the end of line.
 * - If nc doesn't point to '\n', return TRUE (there are non-space characters after the end of word).
 * - If nc points to '\n', return FAKSE (there are no non-space characters after the end of word).
 *
 * Assumptions:
 * - 'nc' points to the beginning of the next character in the input stream.
 * - The line is not empty.
 */
static int getWord(char *word, char **nc)
{
    /* we assume line is not empty, can skip space */
    skipSpace(nc);

    /* get the word */
    while (**nc != EOF && !isspace(**nc))
    {
        *word++ = **nc;
        (*nc)++;
    }
    *word = '\0';

    /* get the first non-space char before end of line */
    while (isspace(**nc) && **nc != '\n')
        (*nc)++;

    /* return if there is text after the end of word */
    if (**nc != '\n')
        return TRUE;
    else
        return FALSE;
}
