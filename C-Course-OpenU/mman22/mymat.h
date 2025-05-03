#ifndef MY_MAY_H
#define MY_MAY_H 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* 
 * Number of elements in a matrix.
 */
#define  NUM_OF_NUMBERS_IN_MAT 16

/* 
 * Number of elements in one line of input.
 */
#define  NUM_OF_NUMBERS_IN_ONE_LINE 4

/* 
 * Number of matrices.
 */
#define NUM_OF_MATS 6

#define ONE 1
#define SUCCESS 0             /* Success indicator */

/* 
 * Structure to represent a matrix.
 */


#define INITIALIZE_MAT_ARRAY() /* Initializes the matrix array with empty matrices */ \
    { \
        int i = 0; \
        for (i = 0; i < NUM_OF_MATS; i++) \
            matArr[i].matrix = createMat(); \
    }

typedef struct mat
{
    double numbers[NUM_OF_NUMBERS_IN_MAT];  /* Array to store matrix elements */
} mat;

/* 
 * Structure to associate a name with a matrix.
 */
typedef struct matrixsWithName
{
    char *name;  /* Name of the matrix */
    mat *matrix; /* Pointer to the matrix */
} matrixsWithName;

void freeMatArray(matrixsWithName *matArr); /* Frees memory allocated for matrices */ 
/* 
 * Creates a new matrix.
 * returns: Pointer to the newly created matrix.
 */
mat *createMat(void);

/* 
 * Reads matrix elements from an array and stores them in the given matrix.
 */
void read_mat(mat *matrix, double numbers[]);

/* 
 * Prints the elements of the given matrix.
 */
void print_mat(mat matrix);

/* 
 * Adds two matrices and stores the result in another matrix.
 */
void add_mat(mat matrix1, mat matrix2, mat *matrix3);

/* 
 * Subtracts one matrix from another and stores the result in another matrix.
 */
void sub_mat(mat matrix1, mat matrix2, mat *matrix3);

/* 
 * Multiplies two matrices and stores the result in another matrix.
 */
void mul_mat(mat matrix1, mat matrix2, mat *matrix3);

/* 
 * Multiplies a matrix by a scalar and stores the result in another matrix.
 */
void mul_scalar(mat matrix1, double scalar, mat *matrix2);

/* 
 * Transposes the given matrix and stores the result in another matrix.
 */
void trans_mat(mat matrix1, mat *matrix2);

/* 
 * Stops the program.
 */
void stop(void);

#endif
