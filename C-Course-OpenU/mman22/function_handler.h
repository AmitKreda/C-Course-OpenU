#ifndef FUNCTION_HANDELER_H
#define FUNCTION_HANDELER_H 1
#include <stdio.h>      /* Standard input-output library */
#include <string.h>     /* String manipulation library */
#include <stdlib.h>     /* Standard library for memory allocation and others */
#include <ctype.h>      /* Character handling functions */
#include "mymat.h"      /* Header file for matrix operations */

#define EQUAL 0               /* Equal comparison indicator */
#define EMPTY_NUM_ARRAY 0
#define MIN_NUM_OF_NUMBERS_TO_READ_MAT 1
#define NUM_TO_REDUCE_TO_GET_THE_ARRAY_LEN 1
#define MIN_NUM_OF_WORDS_FOR_READ_MAT 3 /*there sholud be at least 3 words in read mat, the first is the command the second is the matriz and the last is the first number*/
#define NUM_OF_WORDS_FOR_PRINT_MAT 2/*there should be exacly 2 words in print mat: first word is the command, second word is the matrix to print*/
#define NUM_OF_WORDS_FOR_ADD_MAT 4 /*there should be exactly 4 words in add_mat: first word is the command, second word is the first matrix the third word is the second matrix and the last word is the third matrix*/
#define NUM_OF_WORDS_FOR_SUB_MAT 4 /*there should be exactly 4 words in sub_mat: first word is the command, second word is the first matrix the third word is the second matrix and the last word is the third matrix*/
#define NUM_OF_WORDS_FOR_MUL_MAT 4 /*there should be exactly 4 words in mul_mat: first word is the command, second word is the first matrix the third word is the second matrix and the last word is the third matrix*/
#define NUM_OF_WORDS_FOR_MUL_SCALAR 4/*there should be exactly 4 words in mul_scalar: first word is the command, second word is the first matrix the third word is the scalar and the last word is the second matrix*/
#define NUM_OF_WORDS_FOR_TRANS_MAT 3/*there should be exactly 3 words in trans_mat: first word is the command, second word is the first matrix and the last word is the second matrix*/
#define NUM_OF_WORDS_FOR_STOP 1 /*there should be exactly 1 word in stop and its the command*/
/* Enumeration for boolean values */
typedef enum {
    False = 0,
    True
} flag;

/* Enumeration for error types */
typedef enum {
    no_error = 0,                           /* No error */
    Undefined_matrix_name,                  /* Undefined matrix name */
    Undefined_command_name,                 /* Undefined command name */
    Argument_is_not_a_real_number,          /* Argument is not a real number */
    Extraneous_text_after_end_of_command,    /* Extraneous text after end of command */
    Missing_argument,                       /* Missing argument */
    Illegal_comma,                          /* Illegal comma */
    Missing_comma,                          /* Missing comma */
    Multiple_consecutive_commas,            /* Multiple consecutive commas */
    Argument_is_not_a_scalar,               /* Argument is not a scalar */
    endWithoutStop,                         /* End of file without stop command */
    notEnoughSpace                          /* Insufficient memory space */
}errors;

/* Structure for storing words */
typedef struct word{
    char *buffer;                           /* Buffer to store the word */
    flag thereIsCommaInTheEndOfTheWord;     /* Flag indicating comma presence at the end of the word */
}word;

/* Function Declarations */

/* Function to handle errors */
void handleErrors(errors error);

/* Function to retrieve matrix */
/*
 * Retrieves a matrix from the matrix array based on the given name.
 * word: The name of the matrix to retrieve.
 * matrixArr: Pointer to the array of matrices.
 * numOfMats: The number of matrices in the array.
 * returns: A pointer to the matrix if found, otherwise NULL.
 */
matrixsWithName *getMat(char *word, matrixsWithName *matrixArr, int numOfMats);

/* Error checking functions for different matrix operations */
/*
 * Checks for errors in the read_mat command and handles them accordingly.
 * returns: An error code indicating the status of the read_mat command.
 */
errors read_matCheckError(int numOfWords, word *line, matrixsWithName *matrixArray);
/*
 * Checks for errors in the print_mat command and handles them accordingly.
 * returns: An error code indicating the status of the print_mat command.
 */
errors print_matCheckError(int numOfWords, word *line, matrixsWithName *matrixArray);
/* 
 * Checks for errors in the add_mat command and handles them accordingly.
 * returns: An error code indicating the status of the add_mat command.
 */
errors add_matCheckError(int numOfWords, word *line, matrixsWithName *matrixArray);
/* 
 * Checks for errors in the sub_mat command and handles them accordingly.
 * returns: An error code indicating the status of the sub_mat command.
 */
errors sub_matCheckError(int numOfWords, word *line, matrixsWithName *matrixArray);
/* 
 * Checks for errors in the mul_mat command and handles them accordingly.
 * returns: An error code indicating the status of the mul_mat command.
 */
errors mul_matCheckError(int numOfWords, word *line, matrixsWithName *matrixArray);
/* 
 * Checks for errors in the mul_scalar command and handles them accordingly.
 * returns: An error code indicating the status of the mul_scalar command.
 */
errors mul_scalarCheckError(int numOfWords, word *line, matrixsWithName *matrixArray);
/* 
 * Checks for errors in the trans_mat command and handles them accordingly.
 * returns: An error code indicating the status of the trans_mat command.
 */
errors trans_matCheckError(int numOfWords, word *line, matrixsWithName *matrixArray);
/* 
 * Checks for errors in the stop command and handles them accordingly.
 * returns: An error code indicating the status of the stop command.
 */
errors stopCheckError(int numOfWords, word *line, matrixsWithName *);

#endif