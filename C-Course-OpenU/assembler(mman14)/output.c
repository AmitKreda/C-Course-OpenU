/*
 * This file is responsible for generating the output files for the assembler program, if no errors occured during processing.
 * The functions in this file take the translation unit of the file containig the machine memory picture (machine word array),
 * the extern array and the entry array. They convert the machine word array into encrypted base 4 and print it to the .ob file
 * together with the address of each word, and print the extern and the entry arrays into the .ext and .ent files, respectively.
 * In the .ext file, the address of each referance to the extern label is printed next to it, and in the .ent file,
 * the address of the label itself is printed next to it.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "output.h"

/* private function prototypes */

/*
 * Creates the .ob file, containing an encrypted base 4 representation of the machine code image (array of machine words).
 *
 * Parameters:
 * transUnit - Pointer to a translationUnit containing all the data from the first and second passes.
 * fileName - Pointer to the name of the object file.
 *
 * Returns:
 * - EXIT_FAILURE if any errors occured, otherwise EXIT_SUCCESS.
 */
static int createObFile(translationUnit *transUnit, char *fileName);

/*
 * Prints an encrypted base 4 representation of machine words in a given array between the two indexes specified into the specified file.
 *
 * Parameters:
 * machineWordArr - Array containing the machine code image.
 * startIdx - The index to start printing the base 4 representation from.
 * endIdx - The index to stop printing the base 4 representation at.
 * startAdress - The address from which the machine word address counting starts.
 * obFile - File pointer to the object file to print the base 4 representation to.
 */
static void printBase4(unsigned short *machineWordArr, int startIdx, int endIdx, int startAddress, FILE *obFile);

/*
 * Creates the .ext file, contatining a list of extern symbols together with their addresses.
 *
 * Parameters:
 * arr - Array containing all extern appearances.
 * arrSize - The size of 'arr'.
 * fileName - Pointer to the name of the extern file.
 *
 * Returns:
 * - EXIT_FAILURE if any errors occured, otherwise EXIT_SUCCESS.
 */
static int createExtFile(externReference *arr, int arrSize, char *fileName);

/*
 * Creates the .ent file, contatining a list of entry symbols together with their addresses.
 *
 * Parameters:
 * arr - Array containing all entry appearances.
 * arrSize - The size of 'arr'.
 * fileName - Pointer to the name of the extern file.
 *
 * Returns:
 * - EXIT_FAILURE if any errors occured, otherwise EXIT_SUCCESS.
 */
static int createEntFile(entryAppearance *arr, int arrSize, char *fileName);

/* end of private function prototypes */

/*
 * Creates the three output files .ob .ext .ent with their data.
 *
 * Algorithm:
 * - Create .ob file using 'createObFile' and handle errors.
 * - Create .ext file using 'createExtFile' and handle errors.
 * - Create .ent file using 'createEntFile' and handle errors.
 *
 * Assumptions:
 * - 'transUnit' is a valid pointer to a translation unit structure.
 * - 'fileName' is a valid pointer to a string representing the file name.
 */
void createOutputFiles(translationUnit *transUnit, char *fileName)
{
    /* the current posfix of fileName is 'am' */
    fileName[strlen(fileName) - AM_POSTFIX_LENGTH] = '\0';
    strcat(fileName, "ob");
    /* create the .ob file and handle errors */
    if (createObFile(transUnit, fileName))
        handleError(getErrorStruct(), NO_LINE_NUM, fileName);

    /* the current posfix of fileName is 'ob' */
    fileName[strlen(fileName) - OB_POSTFIX_LENGTH] = '\0';
    strcat(fileName, "ext");
    /* create the .ext file and handle errors */
    if (createExtFile(transUnit->extArr, transUnit->extArrSize, fileName))
        handleError(getErrorStruct(), NO_LINE_NUM, fileName);

    /* the current posfix of fileName is 'ext' */
    fileName[strlen(fileName) - EXT_POSTIX_LENGTH] = '\0';
    strcat(fileName, "ent");
    /* create the .ent file and handle errors */
    if (createEntFile(transUnit->entryArr, transUnit->entryArrSize, fileName))
        handleError(getErrorStruct(), NO_LINE_NUM, fileName);
}

/*
 * Creates the .ob file, containing an encrypted base 4 representation of the machine code image (array of machine words).
 *
 * Algorithm:
 * - Create the .ob file and return EXIT_FAILURE if an error occured.
 * - Print the headline for the .ob file.
 * - Print the encrypted base 4 representation of the operation array using 'printBase4'.
 * - Print the encrypted base 4 representation of the data array using 'printBase4'.
 * - Close the .ob file.
 *
 * Assumptions:
 * - 'transUnit' is a valid pointer to a translation unit structure.
 */
static int createObFile(translationUnit *transUnit, char *fileName)
{
    /* create the .ob file and check if an error occured */
    FILE *obFile;
    if ((obFile = fopen(fileName, "w")) == NULL)
    {
        setErrorStruct(FOPEN_FAILED, "");
        return EXIT_FAILURE;
    }

    /* print headline for the .ob file */
    fprintf(obFile, "\t%d %d\n", transUnit->ic, transUnit->dc);

    /* print ic part of memory */
    printBase4(transUnit->machWrdArr, transUnit->dc, transUnit->ic + transUnit->dc, MEMORY_STARTING_POINT, obFile);

    /* print dc part of memory */
    printBase4(transUnit->machWrdArr, 0, transUnit->dc, transUnit->ic + MEMORY_STARTING_POINT, obFile);

    fclose(obFile);

    return EXIT_SUCCESS;
}

/*
 * Prints an encrypted base 4 representation of machine words in a given array between the two indexes specified into the specified file.
 *
 * Algorithm:
 * - Iterate through the array from startIdx to endIdx.
 * - Each iteration, print the address of the current word in memory.
 * - Loop through the current machine word, two bits at a time, convert them to one of the four encypted symbols corresponding to thier value, an print it.
 * - After finishing printing a word, advance to the next line.
 *
 * Assumptions:
 * - 'machineWordArr' is a valid unsigned short pointer.
 * - 'fp' is a valid file pointer.
 */
static void printBase4(unsigned short *machineWordArr, int startIdx, int endIdx, int startAddress, FILE *obFile)
{
    int i, j;
    char symbol;
    unsigned short num, binaryDigits, mask;

    for (i = startIdx; i < endIdx; i++, startAddress++)
    {
        num = machineWordArr[i];

        /* print the location of the current word in memory */
        if (i + MEMORY_STARTING_POINT <= MAX_THREE_DIGIT_NUMBER)
            fprintf(obFile, "0");
        fprintf(obFile, "%d ", startAddress);

        /* convert the word into encrypted base 4 and print it */
        for (j = MACHINE_WORD_SIZE; j >= NUM_BINARY_DIGITS_TO_CONVERT; j -= NUM_BINARY_DIGITS_TO_CONVERT)
        {
            binaryDigits = num >> (j - NUM_BINARY_DIGITS_TO_CONVERT); /* moving the two binary digits to the start of the word */
            mask = MASK_OF_TWO_BITS;                                  /* mask of two binary digits - 00 0000 0000 0011 */
            binaryDigits &= mask;                                     /* masking the other bits to zero, so that only the two binary digits remain */

            /* setting symbol according to the value of the binary digits */
            switch (binaryDigits)
            {
            case 0:
                symbol = '*';
                break;
            case 1:
                symbol = '#';
                break;
            case 2:
                symbol = '%';
                break;
            case 3:
                symbol = '!';
                break;
            }

            /* print the converted encrypted symbol */
            fprintf(obFile, "%c", symbol);
        }

        fprintf(obFile, "\n");
    }
}

/*
 * Creates the .ext file, contatining a list of extern symbols together with their addresses.
 *
 * Algorithm:
 * - If the size of the array is greater than 0, create the .ext file and return EXIT_FAILURE if an error occured.
 * - Loop through 'arr' and print each cell's name and address to the .ext file.
 * - Close the .ext file and return EXIT_SUCCESS.
 *
 * Assumptions:
 * - 'arr' is a valid pointer to an externAppearance strcuture.
 */
static int createExtFile(externReference *arr, int arrSize, char *fileName)
{
    int i;
    FILE *extFile;

    if (arrSize > 0)
    {
        if ((extFile = fopen(fileName, "w")) == NULL)
        {
            setErrorStruct(FOPEN_FAILED, "");
            return EXIT_FAILURE;
        }
        for (i = 0; i < arrSize; i++)
        {
            fprintf(extFile, "%s\t", arr[i].name);
            if (arr[i].extAddress + MEMORY_STARTING_POINT <= MAX_THREE_DIGIT_NUMBER)
                fprintf(extFile, "0");
            fprintf(extFile, "%d\n", arr[i].extAddress);
        }
        fclose(extFile);
    }

    return EXIT_SUCCESS;
}

/*
 * Creates the .ent file, contatining a list of entry symbols together with their addresses.
 *
 * Algorithm:
 * - If the size of the array is greater than 0, create the .ent file and return EXIT_FAILURE if an error occured.
 * - Loop through 'arr' and print each cell's name and address to the .ent file.
 * - Close the .ent file and return EXIT_SUCCESS.
 *
 * Assumptions:
 * - 'arr' is a valid pointer to an entryAppearance strcuture.
 */
static int createEntFile(entryAppearance *arr, int arrSize, char *fileName)
{
    int i;
    FILE *entFile;

    if (arrSize > 0)
    {
        if ((entFile = fopen(fileName, "w")) == NULL)
        {
            setErrorStruct(FOPEN_FAILED, "");
            return EXIT_FAILURE;
        }
        /* arr size is the length */
        for (i = 0; i < arrSize; i++)
        {
            fprintf(entFile, "%s\t", arr[i].name);
            if (arr[i].entAddress + MEMORY_STARTING_POINT <= MAX_THREE_DIGIT_NUMBER)
                fprintf(entFile, "0");
            fprintf(entFile, "%d\n", arr[i].entAddress);
        }
        fclose(entFile);
    }

    return EXIT_SUCCESS;
}