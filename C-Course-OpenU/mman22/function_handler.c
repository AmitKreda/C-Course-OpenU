#include "function_handler.h"
/*
 * Checks for errors in the read_mat command and handles them accordingly.
 * returns: An error code indicating the status of the read_mat command.
 */
errors read_matCheckError(int numOfWords, word *line, matrixsWithName *matrixArray)
{
    int i = 1, numCounter = 0;                             /* Index and counter variables */
    double currentNum, inputNumArray[NUM_OF_NUMBERS_IN_MAT] = {EMPTY_NUM_ARRAY};  /* Array to store input numbers */
    matrixsWithName *inputMat;                              /* Pointer to the input matrix */
    char *currentNumString;                                 /* Pointer to the current number string */

    /* Check if there are enough words in the line */
    if (numOfWords < MIN_NUM_OF_WORDS_FOR_READ_MAT)
        return Missing_argument;                            /* Missing argument error */

    /* Retrieve the input matrix */
    inputMat = getMat((line + i)->buffer, matrixArray, NUM_OF_MATS);
    if (inputMat == NULL)
        return Undefined_matrix_name;                       /* Undefined matrix name error */

    /* Iterate through the words in the input line */
    while (i < numOfWords-NUM_TO_REDUCE_TO_GET_THE_ARRAY_LEN)
    {
        /* Check for missing comma */
        if (!((line + i++)->thereIsCommaInTheEndOfTheWord))
            return Missing_comma;

        /* Convert the word to a double */
        currentNum = strtod((line + i)->buffer, &currentNumString);

        /* Check if the conversion was successful */
        if ((line + i)->buffer == currentNumString||*currentNumString!='\0')
            return (numCounter > MIN_NUM_OF_NUMBERS_TO_READ_MAT) ? Extraneous_text_after_end_of_command : Argument_is_not_a_real_number;

        /* Store the number in the input array */
        if (numCounter < NUM_OF_NUMBERS_IN_MAT)
            inputNumArray[numCounter++] = currentNum;
    }

    /* Check for extraneous text after the end of command */
    if ((line + i)->thereIsCommaInTheEndOfTheWord)
        return Extraneous_text_after_end_of_command;

    /* Call the read_mat function to read the matrix */
    read_mat(inputMat->matrix, inputNumArray);

    return no_error;    /* No error */
}

/*
 * Checks for errors in the print_mat command and handles them accordingly.
 * returns: An error code indicating the status of the print_mat command.
 */
errors print_matCheckError(int numOfWords, word *line, matrixsWithName *matrixArray)
{
    matrixsWithName *inputMat;
    line++;/*we start from the second word*/
    if (numOfWords < NUM_OF_WORDS_FOR_PRINT_MAT)
        return Missing_argument;
    else if(numOfWords > NUM_OF_WORDS_FOR_PRINT_MAT)
        return Extraneous_text_after_end_of_command;
    inputMat = getMat(line->buffer, matrixArray, NUM_OF_MATS);
    if (inputMat == NULL) 
        return Undefined_matrix_name;
    if (line->thereIsCommaInTheEndOfTheWord)
        return Extraneous_text_after_end_of_command;
    print_mat(*(inputMat->matrix));
    return no_error;
}

/* 
 * Checks for errors in the add_mat command and handles them accordingly.
 * returns: An error code indicating the status of the add_mat command.
 */
errors add_matCheckError(int numOfWords, word *line, matrixsWithName *matrixArray)
{
    matrixsWithName *inputMat1, *inputMat2, *inputMat3;
    line++;/*we start from the second word*/
    if (numOfWords < NUM_OF_WORDS_FOR_ADD_MAT)
        return Missing_argument;
    if (numOfWords > NUM_OF_WORDS_FOR_ADD_MAT)
        return Extraneous_text_after_end_of_command;
    inputMat1 = getMat(line->buffer, matrixArray, NUM_OF_MATS);
    if (inputMat1 == NULL) 
        return Undefined_matrix_name;
    if (!(((line++)->thereIsCommaInTheEndOfTheWord)))
            return Missing_comma;
    inputMat2 = getMat(line->buffer, matrixArray, NUM_OF_MATS);
    if (inputMat2 == NULL) 
        return Undefined_matrix_name;
    if (!(((line++)->thereIsCommaInTheEndOfTheWord)))
            return Missing_comma;
    inputMat3 = getMat(line->buffer, matrixArray, NUM_OF_MATS);
    if (inputMat3 == NULL) 
        return Undefined_matrix_name;
    if (line->thereIsCommaInTheEndOfTheWord)
        return Extraneous_text_after_end_of_command;
    add_mat(*(inputMat1->matrix), *(inputMat2->matrix), inputMat3->matrix);
    return no_error;
}

/* 
 * Checks for errors in the sub_mat command and handles them accordingly.
 * returns: An error code indicating the status of the sub_mat command.
 */
errors sub_matCheckError(int numOfWords, word *line, matrixsWithName *matrixArray)
{
    matrixsWithName *inputMat1, *inputMat2, *inputMat3;
    line++;/*we start from the second word*/
    if (numOfWords < NUM_OF_WORDS_FOR_SUB_MAT)
        return Missing_argument;
    if (numOfWords > NUM_OF_WORDS_FOR_SUB_MAT)
        return Extraneous_text_after_end_of_command;    
    inputMat1 = getMat(line->buffer, matrixArray, NUM_OF_MATS);
    if (inputMat1 == NULL) 
        return Undefined_matrix_name;
    if (!(((line++)->thereIsCommaInTheEndOfTheWord)))
            return Missing_comma;
    inputMat2 = getMat(line->buffer, matrixArray, NUM_OF_MATS);
    if (inputMat2 == NULL) 
        return Undefined_matrix_name;
    if (!(((line++)->thereIsCommaInTheEndOfTheWord)))
            return Missing_comma;
    inputMat3 = getMat(line->buffer, matrixArray, NUM_OF_MATS);
    if (inputMat3 == NULL) 
        return Undefined_matrix_name;
    if (line->thereIsCommaInTheEndOfTheWord)
        return Extraneous_text_after_end_of_command;
    sub_mat(*(inputMat1->matrix), *(inputMat2->matrix), inputMat3->matrix);
    return no_error;
}

/* 
 * Checks for errors in the mul_mat command and handles them accordingly.
 * returns: An error code indicating the status of the mul_mat command.
 */
errors mul_matCheckError(int numOfWords, word *line, matrixsWithName *matrixArray)
{
    matrixsWithName *inputMat1, *inputMat2, *inputMat3;
    line++;/*we start from the second word*/
    if (numOfWords < NUM_OF_WORDS_FOR_MUL_MAT)/*there should be exactly 4 words in this line: first word is the command, second word is the first matrix*/
        return Missing_argument;/* the third word is the second matrix and the last word is the third matrix*/
    if (numOfWords > NUM_OF_WORDS_FOR_MUL_MAT)/*there should be exactly 4 words in this line: first word is the command, second word is the first matrix*/
        return Extraneous_text_after_end_of_command;/* the third word is the second matrix and the last word is the third matrix*/    
    inputMat1 = getMat(line->buffer, matrixArray, NUM_OF_MATS);
    if (inputMat1 == NULL) 
        return Undefined_matrix_name;
    if (!(((line++)->thereIsCommaInTheEndOfTheWord)))
            return Missing_comma;
    inputMat2 = getMat(line->buffer, matrixArray, NUM_OF_MATS);
    if (inputMat2 == NULL) 
        return Undefined_matrix_name;
    if (!(((line++)->thereIsCommaInTheEndOfTheWord)))
            return Missing_comma;
    inputMat3 = getMat(line->buffer, matrixArray, NUM_OF_MATS);
    if (inputMat3 == NULL) 
        return Undefined_matrix_name;
    if (line->thereIsCommaInTheEndOfTheWord)
        return Extraneous_text_after_end_of_command;
    mul_mat(*(inputMat1->matrix), *(inputMat2->matrix), inputMat3->matrix);
    return no_error;
}

/* 
 * Checks for errors in the mul_scalar command and handles them accordingly.
 * returns: An error code indicating the status of the mul_scalar command.
 */
errors mul_scalarCheckError(int numOfWords, word *line, matrixsWithName *matrixArray)
{
    /*there should be exactly 4 words in this line: first word is the command, second word is the first matrix the third word is the scalar and the last word is the second matrix*/
    matrixsWithName *inputMat1, *inputMat2;
    double scalar;
    char *scalarString;
    line++;/*we start from the second word*/
    if (numOfWords < NUM_OF_WORDS_FOR_MUL_SCALAR)/*there should be exactly 4 words in this line: first word is the command, second word is the first matrix*/
        return Missing_argument;/* the third word is the scalar and the last word is the second matrix*/
    if (numOfWords > NUM_OF_WORDS_FOR_MUL_SCALAR)/*there should be exactly 4 words in this line: first word is the command, second word is the second matrix*/
        return Extraneous_text_after_end_of_command;/* the third word is the scalar and the last word is the first number which is must*/
    inputMat1 = getMat(line->buffer, matrixArray, NUM_OF_MATS);
    if (inputMat1 == NULL) 
        return Undefined_matrix_name;
    if (!(((line++)->thereIsCommaInTheEndOfTheWord)))
            return Missing_comma;
    scalar = strtod(line->buffer, &scalarString);
    if (line->buffer == scalarString||*scalarString!='\0') 
        return Argument_is_not_a_scalar;
    if (!(((line++)->thereIsCommaInTheEndOfTheWord)))
            return Missing_comma;
    inputMat2 = getMat(line->buffer, matrixArray, NUM_OF_MATS);
    if (inputMat2 == NULL) 
        return Undefined_matrix_name;
    if (line->thereIsCommaInTheEndOfTheWord)
        return Extraneous_text_after_end_of_command;
    mul_scalar(*(inputMat1->matrix), scalar, inputMat2->matrix);
    return no_error;
}

/* 
 * Checks for errors in the trans_mat command and handles them accordingly.
 * returns: An error code indicating the status of the trans_mat command.
 */
errors trans_matCheckError(int numOfWords, word *line, matrixsWithName *matrixArray)
{
    matrixsWithName *inputMat1, *inputMat2;
    line++;/*we start from the second word*/
    if (numOfWords != NUM_OF_WORDS_FOR_TRANS_MAT)
        return(numOfWords < NUM_OF_WORDS_FOR_TRANS_MAT) ? Missing_argument : Extraneous_text_after_end_of_command;
    inputMat1 = getMat(line->buffer, matrixArray, NUM_OF_MATS);
    if (inputMat1 == NULL) 
        return Undefined_matrix_name;
    if (!(((line++)->thereIsCommaInTheEndOfTheWord)))
            return Missing_comma;
    inputMat2 = getMat(line->buffer, matrixArray, NUM_OF_MATS);
    if (inputMat2 == NULL) 
        return Undefined_matrix_name;
    if (line->thereIsCommaInTheEndOfTheWord)
        return Extraneous_text_after_end_of_command;
    trans_mat(*(inputMat1->matrix), inputMat2->matrix);
    return no_error;
}

/* 
 * Checks for errors in the stop command and handles them accordingly.
 * returns: An error code indicating the status of the stop command.
 */
errors stopCheckError(int numOfWords, word *line, matrixsWithName *matrixArr)
{
    if (numOfWords > NUM_OF_WORDS_FOR_STOP)
        return Extraneous_text_after_end_of_command;
    if (line->thereIsCommaInTheEndOfTheWord)
        return Extraneous_text_after_end_of_command;
    freeMatArray(matrixArr);
    stop();
    return no_error;
}

/*
 * Retrieves a matrix from the matrix array based on the given name.
 * word: The name of the matrix to retrieve.
 * matrixArr: Pointer to the array of matrices.
 * numOfMats: The number of matrices in the array.
 * returns: A pointer to the matrix if found, otherwise NULL.
 */
matrixsWithName *getMat(char *word, matrixsWithName *matrixArr, int numOfMats)
{
    while (numOfMats--)/*searching for the MAT*/
    {
        if (strcmp(word, matrixArr->name) == EQUAL)
            return (matrixArr);
        matrixArr++;
    }
    return NULL;/*if not found identical name*/
}

/*
 * Handles errors by printing corresponding messages.
 */
void handleErrors(errors error)
{
    switch (error)
    {
        case no_error:
            break;
        case Undefined_matrix_name:
            printf("Undefined matrix name\n");
            break;
        case Argument_is_not_a_real_number:
            printf("Argument is not a real number\n");
            break;
        case Extraneous_text_after_end_of_command:
            printf("Extraneous text after end of command\n");
            break;
        case Missing_argument:
            printf("Missing argument\n");
            break;
        case Missing_comma:
            printf("Missing_comma\n");
            break;
        case Multiple_consecutive_commas:
            printf("Multiple consecutive commas\n");
            break;
        case Argument_is_not_a_scalar:
            printf("Argument is not a scalar\n");
            break;
        case Illegal_comma:
            printf("Illegal comma\n");
            break;
        case Undefined_command_name:
            printf("Undefined command name\n");
            break;
        case endWithoutStop:
            printf("reached to the end without stop command\n");
            break;
        case notEnoughSpace:
            printf("not enough space,terminating");
            break;
    }
}
