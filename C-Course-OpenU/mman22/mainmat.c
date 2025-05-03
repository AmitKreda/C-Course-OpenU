#include "mainmat.h"
/*
 * The function manages the flow of the program, processing commands and handling errors accordingly.
 * It follows these steps:
 * 1. Initializes variables and arrays required for program execution.
 * 2. Enters a loop to continuously read and process user input until the end of the file is encountered.
 * 3. Reads a line of input from the user, parsing it into words.
 * 4. Identifies the command to execute based on the parsed words and dispatches it for processing.
 * 5. Handles any errors that occur during command execution.
 * 6. Frees memory allocated for the parsed words.
 * 7. Resets the word count for the next iteration.
 * 8. Frees memory allocated for matrixes at the end of program execution.
 * 9. Prints an error message if the end of the file is reached without a stop command.
 * 10. Returns SUCSSES upon successful execution.
 * return SUCSSES if there was no error with memory
 */
int main() {
    int numOfWords;                 /* Number of words in the current line of input */
    word *wordArr;
    flag endOfFile = False;         /* Flag indicating if the end of the file has been reached */
    functionWithName *command;      /* Pointer to the function to execute based on user input */
    matrixsWithName matArr[NUM_OF_MATS] = {{"MAT_A", NULL}, {"MAT_B", NULL}, {"MAT_C", NULL},/* Array of matrices with their corresponding names */ 
                                        {"MAT_D", NULL}, {"MAT_E", NULL}, {"MAT_F", NULL}};
    functionWithName possibleCommands[NUM_OF_FUNCTIONS] = {{read_matCheckError, "read_mat"},/* Array of all possible commands with their associated error-checking functions */
                                                            {print_matCheckError, "print_mat"},
                                                            {add_matCheckError, "add_mat"},
                                                            {sub_matCheckError, "sub_mat"},
                                                            {mul_matCheckError, "mul_mat"},
                                                            {mul_scalarCheckError, "mul_scalar"},
                                                            {trans_matCheckError, "trans_mat"},
                                                            {stopCheckError, "stop"}};
    INITIALIZE_MAT_ARRAY();/* Initializes the matrix array */
    /* Continuously process input until the end of the file is reached */
    while (!endOfFile) {
        printf("enter your command\n");
        /* Read a line of input from the user and parse it into words */
        wordArr = myGetLine(&numOfWords, &endOfFile);

        /* If there was an error or the line is empty, continue to the next iteration */
        if (wordArr == NULL)
            continue;

        /* Identify the command to execute based on the parsed words */
        command = getcommand(wordArr, possibleCommands);

        /* If the command is not recognized, free memory and continue to the next iteration */
        if (command == NULL) {
            freeLine(numOfWords, wordArr);
            continue;
        }

        /* Execute the identified command and handle any errors that occur */
        handleErrors(command->functionPtr(numOfWords, wordArr, matArr));

        /* Free memory allocated for the parsed words */
        freeLine(numOfWords, wordArr);
        numOfWords = START_OF_THE_LINE; /* Reset the word count for the next iteration */
    }
    /* Free memory allocated for matrices at the end of program execution */
    freeMatArray(matArr);
    /* Print an error message if the end of the file is reached without a stop command */
    handleErrors(endWithoutStop);
    return SUCCESS; /* Return SUCSSES upon successful execution */
}
/**
 * Searches for a command in a list of possible commands and returns a pointer to the corresponding functionWithName structure.
 * If the command is not found, it returns NULL.*/
functionWithName *getcommand(word *line,functionWithName *possibleCommands)/*return function and if not found, return undefined*/
{
    int numOfFunctions=NUM_OF_FUNCTIONS;/* Total number of possible commands.*/
    char *command=line->buffer;/* The command to search for*/
    /* Check if the command has a comma at the end, which is an illegal format*/
    if(line->thereIsCommaInTheEndOfTheWord)
    {
        handleErrors(Illegal_comma);
        return NULL;
    }
    while(numOfFunctions--)/*searching for the command*/
    {
        if(strcmp(possibleCommands->name,command)==EQUAL) /*Compare the name of the current possible command with the provided command.*/
            return possibleCommands;/* Return a pointer to the matching functionWithName structure.*/
        possibleCommands++;/* Move to the next possible command.*/
    }
    handleErrors(Undefined_command_name);/* If no match is found, handle the error and return NULL.*/
    return NULL;
}
/*
 * This function retrieves a word from the standard input stream.
 * It reads characters until encountering a space, comma, or newline character, 
 * stores the characters in the provided word structure, and handles multipale consacutive commas error.
 * Returns False if the word retrieval was successful, indicating no errors.
 * 
 * - This function reads characters from the standard input until it encounters a space or newline character.
 * - If it encounters a comma, it checks for consecutive commas.
 * - After reading a word, it checks if the end of the line or end of the file has been reached.
 * - The function dynamically allocates memory for the word as needed, reallocating it if necessary.
 * - If an error occurs, such as multiple consecutive commas or illegal comma positions, it prints an error message and returns True.
 * - If the end of the line or end of the file is reached and no word is retrieved, it updates the respective flags and returns False.
 */
flag getWord(flag *endOfFile, flag *endOfLine, word *currentWord, flag *ThereIsCommaInTheEndOfThePrevWord, flag *emptyWord) {
    static flag lastCharWasComma = False;
    int c;
    flag inWord = False;
    unsigned short int maxSizeOfWord = MAX_WORD_LEN, wordLen = START_OF_THE_WORD;
    currentWord->thereIsCommaInTheEndOfTheWord = False;

    /* Read characters until encountering a space, comma, or newline character */
    while ((c = getchar()) != EOF && c != '\n' && ((!isspace(c) && c != ',') || !inWord)) {
        printf("%c", c); /* print the character to the output */

        if (c == ',') {
            CHECK_COMMA_ERRORS(); /* Check for consecutive commas*/
            continue;
        } else if(isspace(c))
            continue; /* Skip whitespace characters */
        else
            lastCharWasComma = False; /* Reset the flag indicating the last character was a comma */

        if (!inWord) {
            /* Initialize memory for the word */
            currentWord->buffer = malloc(sizeof(char) * maxSizeOfWord);
            NOT_ENOUGH_SPACE(currentWord); /* Check for memory allocation errors */
            inWord = True; /* Set the flag indicating we are in a word */
        }

        if (wordLen >= maxSizeOfWord) {
            /* Resize the word word buffer if necessary */
            maxSizeOfWord += MAX_WORD_LEN;
            currentWord->buffer = reallocMemory(maxSizeOfWord, currentWord, currentWord->buffer);
        }

        /* Store the character in the word buffer */
        currentWord->buffer[wordLen++] = c;
    }

    /* Print the last character read */
    if (c != EOF) {
        printf("%c", c);
    }

    if (c == ',') {
        CHECK_COMMA_ERRORS(); /* Check for errors related to comma placement */
    }
    else
        lastCharWasComma=False;
    if (inWord) {
        /* If a word was being read, null-terminate it */
        currentWord->buffer[wordLen] = '\0';
        currentWord->buffer = reallocMemory(sizeof(char) * wordLen, currentWord, currentWord->buffer);
    }

    /* Update flags based on the last character read */
    if (c == '\n') {
        *endOfLine = True;
    } else if (c == EOF) {
        *endOfFile = True;
    }

    /* If the end of the line or end of the file is reached and no word is retrieved, update flags and return False */
    if ((*endOfLine || *endOfFile) && !inWord) {
        *emptyWord = True;
    }

    /* Indicate successful word retrieval by returning False */
    return False;
}
/*
 * Retrieves a line of words from the standard input stream.
 * Reads characters until reaching the end of the file or the end of the line.
 * Dynamically allocates memory for words and handles illigal comma error.
 * Returns a pointer to the array of words read from the line. Returns NULL if there are errors or no words are read.
 *
 * - Initializes variables for line processing, including the line buffer, word counter, and flags.
 * - Dynamically allocates memory for the line buffer to store words.
 * - Initializes flags for tracking the end of the line and whether a word is empty.
 * - Initializes a pointer to track the presence of a comma at the end of the previous word.
 * - Reads characters until reaching the end of the file or the end of the line.
 * - If the word counter exceeds the line buffer size, reallocates memory for the line buffer.
 * - Calls the getWord function to retrieve each word from the line.
 * - Updates the word counter and flags based on word retrieval.
 * - Checks for errors related to illegal comma positions or empty lines.
 * - Frees memory allocated for the line buffer if no words are read and returns NULL.
 * - Returns the line buffer with the words read from the line.
 */
word *myGetLine(int *numOfWords, flag *endOfFile) {
    unsigned short int sizeOfLine = MAX_WORD_AMOUNT, wordCounter = 0;
    flag endOfLine = False;
    flag emptyWord = False;
    
    /* Allocate memory for the line buffer */
    word *line = malloc(sizeof(word) * MAX_WORD_AMOUNT);
    word *startOfTheLinePtr = line; /* Pointer to the current last word of the line */
    flag *prevWordCommaFlag;
    NOT_ENOUGH_SPACE(line); /* Check for memory allocation errors */
    prevWordCommaFlag = &(line->thereIsCommaInTheEndOfTheWord); /* Pointer to track comma presence */
    /* Continue reading until reaching the end of the file or the end of the line */
    while (!(*endOfFile || endOfLine)) {
        emptyWord = False;

        /* Check if the word counter exceeds the line buffer size and reallocate memory if necessary */
        if (wordCounter >= sizeOfLine) {
            sizeOfLine += MAX_WORD_AMOUNT;
            line = reallocMemory(sizeOfLine * sizeof(word), startOfTheLinePtr, startOfTheLinePtr);
            startOfTheLinePtr = line;
            line += wordCounter;
            prevWordCommaFlag = &((line - PRVE_WORD)->thereIsCommaInTheEndOfTheWord);
        }

        /* Retrieve a word from the line */
        if (getWord(endOfFile, &endOfLine, line, prevWordCommaFlag, &emptyWord)) {
            freeLine(wordCounter,startOfTheLinePtr);
            return NULL; /* If a new line, return NULL */
        }

        /* Update pointers and counters if the word is not empty */
        if (!emptyWord) {
            prevWordCommaFlag = &(line->thereIsCommaInTheEndOfTheWord);
            line++;
            wordCounter++;
        }
    }

    /* Update the number of words read */
    *numOfWords = wordCounter;

    /* Check for errors related to illegal comma positions or empty lines */
    if (wordCounter == START_OF_THE_LINE && (*prevWordCommaFlag)) {
        handleErrors(Illegal_comma);
    }

    /* Free memory allocated for the line buffer if no words are read */
    if (wordCounter == START_OF_THE_LINE) {
        freeLine(wordCounter, startOfTheLinePtr);
        return NULL;
    } else {
        /* Reallocate memory for the line buffer based on the number of words read */
        line = reallocMemory(wordCounter * sizeof(word), startOfTheLinePtr, startOfTheLinePtr);
    }

    /* Return the line buffer with the words read from the line */
    return line;
}

/*
 * Reallocates memory for a given pointer to accommodate a new size.
 * If reallocation fails, it frees the original memory, handles the error, and exits.
 * 
 * Returns a pointer to the reallocated memory block if successful, exit the proogram if fails reallocation fails.
 * 
 * - Attempts to reallocate memory using the provided size and pointer.
 * - If reallocation succeeds, it returns a pointer to the reallocated memory block.
 * - If reallocation fails (temp == NULL), it frees the original memory block.
 * - Calls the handleErrors function to handle the "not enough space" error.
 * - Exits the program with the MEMORY_ERROR code to indicate memory allocation failure.
 */
void *reallocMemory(int newSize, void *ptrToFree, void *ptrToReallocate) {
    void *temp;
    /* Attempt to reallocate memory using the provided size and pointer */
    temp = realloc(ptrToReallocate, newSize);

    /* If reallocation fails, handle the error and exit */
    if (temp == NULL) {
        /* Free the original memory block to avoid memory leaks */
        free(ptrToFree);

        /* Handle the "not enough space" error */
        handleErrors(notEnoughSpace);

        /* Exit the program with the MEMORY_ERROR code to indicate memory allocation failure */
        exit(MEMORY_ERROR);
    }

    /* Return a pointer to the reallocated memory block if successful */
    return temp;
}
 /* Frees memory allocated for a line of words */ 
void freeLine(int numOfWords, word *line) {
    int currWord;
    for (currWord = 0; currWord < numOfWords; currWord++) {
        free((line + currWord)->buffer);
    }
    free(line); /* Free the memory allocated for the array of word structs*/
}
void freeMatArray(matrixsWithName *matArr) /* Frees memory allocated for matrices */ 
{ 
    int i = 0; 
    for (i = 0; i < NUM_OF_MATS; i++)
        free((matArr+i)->matrix);
}
