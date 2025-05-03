/*
 * Maman 14 assembler project - Semester 2024a.
 *
 * Date: 25/04/2024
 *
 * This program is an implementation of an assembly processor. It performs the stages of the assembling process,
 * including preprocessing, lexical analysis, syntax analysis, the first pass , the second pass (including code generation),
 * and generating output files. The program executes the different stages while managing memory allocation, file handling,
 * error detection, and ensures efficient execution of the assembly process.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "ast.h"
#include "error.h"
#include "pre_assembler.h"
#include "first_pass.h"
#include "second_pass.h"
#include "output.h"

#define NO_COMMAND_LINE_ARGS 1   /* if no command line arguments were provided, argc is 1 */
#define MAX_FILE_EXTENSION_LEN 4 /* maximum length of a file extension in this project is 4 ('.ent' or '.ext')*/

/*
 * Assumptions:
 * - 'fileName' is a valid pointer to a string representing the file name.
 * - 'firstPassBuffer' is a valid pointer to a first pass AST buffer structure.
 * - 'transUnit' is a valid pointer to a translation unit structure.
 * - 'ast' is a valid pointer to an astNode structure.
 * - 'token' is a valid pointer to the current token in the token array.
 * - 'nc' is a valid pointer to a valid string.
 * - 'str' points to a valid string...
 * - 'fp' is a valid file pointer.
 */

/*
 * Processes a pre-processed (.am) assembly file and produces output files.
 *
 * This function processes a pre-processed assembly file (with macros expanded and empty lines and comments removed)
 * by performing the stages of the assembly process, including lexical anylasis (tokenization and ast creation), the first pass, the second pass,
 * and generating output files. It manages memory allocation, file opening and error handling. If an error occures, output files will not be generated.
 *
 * Parameters:
 * fileName - Pointer to the name of the pre-processed (.am) file to be processed.
 *
 * Returns:
 * - EXIT_FAILURE if any errors occured, otherwise EXIT_SUCCESS.
 */
static int assembler(char *fileName);

/*
 * Processes an source assembly source file and produces output files.
 *
 * Parameters:
 * fileName - Pointer to the name of the assembly source file (.as) to be processed.
 *
 * Returns:
 * - EXIT_FAILURE if any errors occured, otherwise EXIT_SUCCESS.
 */
static int processFile(char *fileName);

int main(int argc, char *argv[])
{
    int i, exitStatus = EXIT_SUCCESS;

    if (argc == NO_COMMAND_LINE_ARGS)
    {
        fprintf(stderr, "Error: No files were provided as command line arguments. Please provide files in order to process them.\n");
        return EXIT_FAILURE;
    }

    /* iterate thorugh all the file names */
    for (i = 1; argv[i] != NULL; i++)
    {
        /* process the file and check if an error occured */
        if (processFile(argv[i]))
        {
            /* an error occured during processing - set exit status to failure */
            exitStatus = EXIT_FAILURE;
        }
    }

    return exitStatus;
}

/*
 * Processes an source assembly source file and produces output files.
 *
 * Algorithm:
 * - Allocate memory and create a copy of the file name, with additional space for file extensions. Handle memory allocation errors if any.
 * - Call pre-assembler to pre-process the file: remove comments, empty lines and expand macros.
 * - If an error occured during the pre-processing stage, return EXIT_FAILURE.
 * - Call assembler to process the pre-process file: Preform lexical analysis, first pass, second pass, and produce output files.
 * - If an error occured during the processing stage, return EXIT_FAILURE.
 * - Otherwise, return EXIT_SUCCESS.
 *
 * Assumptions:
 * - 'fileName' is a valid pointer to a string representing the file name.
 */
static int processFile(char *fileName)
{
    char *fileNameCopy;
    /* allocate memory for a copy of the file name with additional space for file extensions */
    fileNameCopy = malloc(((strlen(fileName) + MAX_FILE_EXTENSION_LEN) + ONE_BIT_FOR_NULL_CHAR) * sizeof(char));

    /* check if memory allocation failed an handle it */
    if (fileNameCopy == NULL)
        handleError(setAndGetErrorStruct(MALLOC_FAILED, ""), NO_LINE_NUM, fileName);

    strcpy(fileNameCopy, fileName);

    /* pre-assemble the file and check if an error is encoutered */
    if (preAssembler(fileNameCopy))
    {
        free(fileNameCopy);
        return EXIT_FAILURE;
    }
    /* assemble the file and check if an error is encountered */
    if (assembler(fileNameCopy))
    {
        free(fileNameCopy);
        return EXIT_FAILURE;
    }

    free(fileNameCopy);

    return EXIT_SUCCESS;
}

/*
 * Processes a pre-processed (.am) assembly file and produces output files.
 *
 * Algorithm:
 * - Open the .am file for reading and handle errors if any.
 * - Call 'firstPass' and save if an error occured.
 * - Call 'secondPass' and save if an error occured in first or second pass.
 * - If an error occured, print a message and return EXIT_FAILURE.
 * - Otherwise, call 'createOutputFiles' to create the output files, and return EXIT_SUCCESS.
 *
 * Assumptions:
 * - 'fileName' is a valid pointer to a string representing the file name.
 */
int assembler(char *fileName)
{
    FILE *fp;                        /* file pointer to hold the ".am" file */
    translationUnit transUnit = {0}; /* intializing transUnit to zero */
    FirstPassAstBuffer data = {0};   /* intializing FirstPassAstBuffer to zero */
    int error = FALSE;

    /* open the .am file and check if an error occured */
    if ((fp = fopen(fileName, "r")) == NULL)
    {
        setErrorStruct(FOPEN_FAILED, "");
        handleError(getErrorStruct(), NO_LINE_NUM, fileName);
        return EXIT_FAILURE;
    }

    /* call firstPass and store if an error occured */
    error = firstPass(&data, &transUnit, fp, fileName);

    /* handle memory allocation failure in firstPass */
    if (getErrorStruct()->error == MALLOC_FAILED)
    {
        fclose(fp);
        handleError(getErrorStruct(), NO_LINE_NUM, fileName);
        exit(EXIT_FAILURE);
    }
    if (getErrorStruct()->error == PROGRAM_MEMORY_RUN_OUT)
    {
        fclose(fp);
        handleError(getErrorStruct(), NO_LINE_NUM, fileName);
        return EXIT_FAILURE;
    }

    /* call secondPass and store if an error occured in first or second pass */
    error |= secondPass(&transUnit, &data, fileName);

    /* handle memory allocation failure for secondPass */
    if (getErrorStruct()->error == MALLOC_FAILED)
    {
        fclose(fp);
        handleError(getErrorStruct(), NO_LINE_NUM, fileName);
        exit(EXIT_FAILURE);
    }

    /* check if an error occured during processing, before producing output files */
    if (error)
    {
        fprintf(stderr, "An error occured during assembling, output files will not be created.\n");
        /* free memory that wasn't freed by firstPass and secondPass */
        if (transUnit.extArr != NULL)
            free(transUnit.extArr);
        if (transUnit.entryArr != NULL)
            free(transUnit.entryArr);
        return EXIT_FAILURE;
    }

    createOutputFiles(&transUnit, fileName);

    /* free memory that wasn't freed at the end of processing the file */
    if (transUnit.extArr != NULL)
        free(transUnit.extArr);
    if (transUnit.entryArr != NULL)
        free(transUnit.entryArr);

    /* close the file at the end of processing */
    fclose(fp);

    return EXIT_SUCCESS;
}
