/*
 * This file is responsible for the first pass stage of the assembling process:
 * Creating the symbol table, encoding the data machine words, calculates IC and DC
 * and replacing constants with their integer values. The functions in this file loop through the lines of the pre-processed (.am) file,
 * creating an AST for each one of them using the 'createAst' function. Then they analyize the AST, and encode data line into the
 * machine word array (data image). They add all the symbols (labels and constants) to the symbol table.
 * For extern/constant definition directives, they add the label/constant specified in them into the symbol table.
 */
#include "first_pass.h"
#include "lexer.h"
#include "trie.h"
#include "ast.h"
#include "symbol.h"
/*
 * Retrieves a pointer to the index that keeps track of the current available space in the AST buffer.
 *
 * Returns:
 *  the address of astBufferIndex.
 */
static index *getAstBufferIndex(void);
/*
 * Adds the given abstract syntax tree (AST) line to the first pass AST buffer.
 *
 * Parameters:
 * - curLine: The AST line to add to the buffer.
 * - firstPassBuffer: Pointer to the structure containing the first pass AST buffer information.
 */
static void addAstToBuffer(astLine curLine, FirstPassAstBuffer *firstPassBuffer);
/*
 * Adds a new external symbol to the symbol table.
 *
 * Parameters:
 * - labelName: The name of the symbol to add.
 * - transUnit: Pointer to the translation unit containing the symbol table.
 */
static void addNewExternSymbol(char *labelName, translationUnit *transUnit);
/*
 * Adds a new constant to the symbol table.
 *
 * Parameters:
 * - constName: The name of the constant to add.
 * - constantValue is the value of the constant to add.
 * - transUnit: Pointer to the translation unit containing the symbol table.
 */
static void addNewConstant(char *constName, short int constantValue, translationUnit *transUnit);
/*
 * Adds a new instruction symbol to the symbol table.
 *
 * Parameters:
 * - labelName: The name of the label associated with the instruction symbol.
 * - transUnit: Pointer to the translation unit containing the symbol table.
 *
 */
static void addNewInstSymbol(char *labelName, translationUnit *transUnit);
/*
 * Adds a new data symbol to the symbol table.
 *
 * Parameters:
 * - labelName: The name of the label associated with the data symbol.
 * - dataCount: The number of values associated with the data symbol.
 * - transUnit: Pointer to the translation unit containing the symbol table.
 *
 */
static void addNewDataSymbol(char *labelName, int dataCount, translationUnit *transUnit);
/*
 * Processes a constant of the immediate constant operand in the given abstract syntax tree (AST) line, and changes the ast to prepare it for the second pass.
 *
 * This function handles the processing of an ast representing an instruction that has an immediate constant operand.
 * It searches for constant and checks if been defined, if so: changes the operand to an immediate num.
 * Parameters:
 * - currLine: The AST line containing the operand to be processed.
 * - transUnit: Pointer to the translation unit containing the symbol table.
 * - operandsIndex: Index of the operand in the AST line's operands array.
 */
static void processImmediateConstantOperand(astLine currLine, translationUnit *transUnit, index operandsIndex);
/*
 * Processes the constant of idxConstant operand in the given abstract syntax tree (ast) line, and changes the ast to prepare it for the second pass.
 *
 * This function handles the processing of an ast representing an instruction that has an idx constant operand.
 * It searches for constant and checks if been defined, if so: changes the operand to idx num.
 *
 * Parameters:
 * - currLine: The AST line containing the operand to be processed.
 * - transUnit: Pointer to the translation unit containing the symbol table.
 * - operandsIndex: Index of the operand in the AST line's operands array.
 */
static void processIdxConstantOperand(astLine currLine, translationUnit *transUnit, index operandsIndex);

/*
 * an ast representing constant definition directive line and adds it(the constant) to the symbol table.
 *
 * This function handles the processing of an ast representing constant definition directive line.
 * It checks for constant redefinition, adds a new constant definition to the symbol table if not redefined,
 * and stores the constant value.
 *
 * Parameters:
 * - currLine: The current AST line representing the operation.
 * - transUnit: Pointer to the translation unit containing the symbol table.
 *
 */
static void processConstDef(astLine curLine, translationUnit *transUnit);
/*
 * Processes data values specified in the assembly directive and encodes them to the translation unit.
 *
 * This function handles the processing of an ast representing data definition directive line.
 * It checks for label redefinition, adds a new label symbol to the symbol table if found,
 *
 * Parameters:
 * - curLine: The current AST line represents the assembly directive.
 * - transUnit: Pointer to the translation unit containing the machine word array and symbol table.
 */
static void processDataValues(astLine curLine, translationUnit *transUnit);

/*
 * Processes an operation in the assembly code.
 *
 * This function handles the processing of an ast representing an operation line.
 * It checks for constant redefinition, adds a new instruction symbol to the symbol table if not redefined.
 *
 * Parameters:
 * - currLine: The current AST line representing the operation.
 * - transUnit: Pointer to the translation unit containing the symbol table.
 *
 */
static void processOperationLine(astLine currLine, translationUnit *transUnit);
/*
 * Checks if the source operand matches the allowed types for the given opcode.
 *
 * Parameters:
 * - opCode: The token type representing the opcode.
 * - srcOperand: The addressing method of the source operand.
 */
static void checkSrcOpMatching(TokenType opCode, int srcOperand);
/*
 * Checks if the destination operand matches the allowed types for the given opcode.
 *
 * Parameters:
 * - opCode: The token type representing the opcode.
 * - destOperand: The addressing method of the destination operand.
 */
static void checkDestOpMatching(TokenType opCode, int destOperand);
/*
 * Checks if the operands match the allowed types for the opcode in the given abstract syntax tree (AST) line.
 *
 * Parameters:
 * - currLine: The AST line to be checked.
 * - operandsAmount: The number of operands in the AST line.
 *
 */
static void checkOpsMatching(astLine currLine);
/*
 * Checks for the definition of a label in the symbol table.
 *
 * Parameters:
 * - labelName: The name of the label to check for definition.
 * - transUnit: Pointer to the translation unit containing the symbol table.
 *
 */
static void checkForLabelDefinition(char *labelName, translationUnit *transUnit);
/*
 * Checks if an external symbol already exists in the symbol table.
 *
 * Parameters:
 * - labelName: The name of the symbol to add.
 * - transUnit: Pointer to the translation unit containing the symbol table.
 *
 */
static void checkForExistingSymbolExtern(char *labelName, translationUnit *transUnit);
/*
 * Checks for a redefinition of constant in the symbol table.
 *
 * Parameters:
 * - constName: The name of the constant.
 * - transUnit: Pointer to the translation unit containing the symbol table.
 *
 */
static void checkForConstantRedefinition(char *constName, translationUnit *transUnit);

/*
 * calculates the L attribute for the given AST line in the translation unit.
 *
 * Parameters:
 * - currLine: The AST line to find the L attribute for.
 * - transUnit: Pointer to the translation unit containing the symbol table.
 *
 * Returns:
 * The calculated value of L(instruction length) for the assembly line.
 */
static index calculateL(astLine currLine, translationUnit *transUnit);
/*
 * Frees the memory allocated for the AST buffer.
 *
 * Parameters:
 * - bufferSize: Size of the AST buffer.
 * - buffer: Pointer to the array containing AST lines.
 *
 */
static void cleanAstBuffer(astLine buffer[]);
/*
 * Finds a constant symbol by its name in the symbol table of the translation unit.
 *
 * Parameters:
 * - constName: Name of the constant to be found.
 * - transUnit: Pointer to the translation unit containing the symbol table.
 *
 * Returns:
 * - Pointer to the constant symbol if found.
 */
static symbol *findConst(char *constName, translationUnit *transUnit);

/*
 * Processes the '.data' assembly directive represented by ast of the line.
 *
 * Parameters:
 * - curLine: The current AST line represents the assembly directive.
 * - transUnit: Pointer to the translation unit containing the machine word array and symbol table.
 *
 */
static void processDataLine(astLine curLine, translationUnit *transUnit);
/*
 * Processes the '.extern' assembly directive represented by ast of the line.
 *
 * Parameters:
 * - curLine: The current AST line represents the assembly directive.
 * - transUnit: Pointer to the translation unit containing the machine word array and symbol table.
 */
static void processExternLine(astLine curLine, translationUnit *transUnit);
/*
 * Handles directive lines during the first pass of assembly code processing represented by ast of the line.
 *
 * This function processes directive lines encountered during the first pass of assembly code processing.
 * It determines the type of directive line and invokes corresponding processing functions.
 *
 * Parameters:
 * - curLine: The current AST line represents the assembly directive.
 * - transUnit: Pointer to the translation unit.
 * - firstPassBuffer: Pointer to the structure containing the first pass AST buffer information.
 */
static void handleDirectiveLines(astLine curLine, FirstPassAstBuffer *firstPassBuffer, translationUnit *transUnit);
/*
 * Handles different types of lines during the first pass of assembly code processing.
 *
 * This function processes different types of lines encountered during the first pass of assembly code processing.
 * It determines the type of line (operation or directive) and invokes corresponding processing functions.
 *
 * Parameters:
 * - curLine: The current line that is being processed.
 * - firstPassBuffer: Pointer to the first pass AST buffer.
 * - transUnit: The translation unit containing the symbol table and other data.
 *
 */
static void lineTypeCases(astLine curLine, FirstPassAstBuffer *firstPassBuffer, translationUnit *transUnit);
/*
 * Handles a single line of assembly code during the first pass.
 *
 * This function processes a single line of assembly code during the first pass of assembly code processing.
 * It parses the line to generate an abstract syntax tree (AST), determines the type of line (operation or directive),
 * and calls corresponding processing functions.
 *
 * Parameters:
 * - currentLine: The string containing the current line of assembly code.
 * - firstPassBuffer: Pointer to the first pass AST buffer.
 * - linecounter: The index of the current line in the assembly code.
 * - transUnit: The translation unit.
 * - fileName: the name of the assembly file.
 *
 * Returns:
 * - TRUE if there was an error encountered while processing the line, FALSE otherwise.
 */
static bool handleLine(char currentLine[], FirstPassAstBuffer *firstPassBuffer, index linecounter, translationUnit *transUnit, char *fileName);

/*end of prototype decleartion*/

/*
 * Retrieves a pointer to the index that keeps track of the current available space in the AST buffer.
 *
 * Algorithm:
 *  on the first call for this function, initialize astBufferIndex to 0.
 *  returning the address of astBufferIndex.
 */
static index *getAstBufferIndex(void)
{
    static index astBufferIndex = 0; /* Index that keeps track of the currently available space in the ast buffer */
    return &astBufferIndex;
}
/*
 * Adds the given abstract syntax tree (AST) line to the first pass AST buffer.
 *
 * Algorithm:
 * - Retrieves a pointer to the index that keeps track of the current available space in the AST buffer.
 * - Check if there is enough space in the buffer to save the current AST line.
 * - If there is not enough space, double the buffer.
 * - Saves the current AST line in the AST buffer.
 *
 * Assumptions:
 * - The 'curLine' parameter contains a valid AST line.
 * - The 'firstPassBuffer' parameter points to a valid first pass AST buffer structure.
 */
static void addAstToBuffer(astLine curLine, FirstPassAstBuffer *firstPassBuffer)
{
    /* Get the index of the AST buffer */
    index *astBufferIndex = getAstBufferIndex();

    /* If there is no space to save the current AST */
    if (*astBufferIndex == firstPassBuffer->bufferSize)
    {
        /* ensure buffer size is greater than zero */
        firstPassBuffer->bufferSize = (firstPassBuffer->bufferSize == 0) ? 1 : firstPassBuffer->bufferSize;

        /* Double the buffer size */
        firstPassBuffer->bufferSize *= SIZE_SCALER;

        /* Reallocate memory for the AST buffer */
        reallocateMemory(sizeof(astLine), firstPassBuffer->bufferSize, (void **)&(firstPassBuffer->astBuffer));
        if (getErrorStruct()->error != NO_ERROR)
        {
            free(curLine.ast);
            return; /* Exit */
        }
    }
    /* Save the current AST in astBuffer */
    firstPassBuffer->astBuffer[(*astBufferIndex)++] = curLine;
    return; /* Exit */
}

/*
 * Frees the memory allocated for the AST buffer.
 *
 * Algorithm:
 * - Checks if the array pointer is NULL, if so, returns immediately.
 * - Iterates through the array of elements.
 * - Checks if the AST pointer is not NULL.
 * - Frees the memory block pointed to by the pointer.
 * - Frees the memory allocated for the array of pointers.
 */
static void cleanAstBuffer(astLine buffer[])
{
    index *bufferSize = getAstBufferIndex();
    /* Check if the buffer is already empty, if so, return */
    if (buffer == NULL)
        return;

    /* Iterate through the buffer elements */
    while (*bufferSize)
    {
        (*bufferSize)--;
        /* Check if the AST pointer is not NULL */
        if (buffer[(*bufferSize)].ast != NULL)
        {
            /* Free the memory block pointed to by the pointer */
            free(buffer[(*bufferSize)].ast);
        }
    }

    /* Free the memory allocated for the buffer */
    free(buffer);

    /* Exit function */
    return;
}

/*
 * Finds a constant symbol by its name in the symbol table of the translation unit.
 *
 * Algorithm:
 * - Searches for the constant symbol in the symbol table using its name.
 * - If the constant symbol doesn't exist,set an error and exit
 * - If the type of the symbol found is not constant,set an error and exit.
 *
 * Assumptions:
 * - The constName transUnit and constantSymbol are valid.
 * - searchResultNode->endOfWord is not NULL
 */
static symbol *findConst(char *constName, translationUnit *transUnit)
{
    symbol *constSymbol = NULL;
    /*search for the constant*/
    TrieNode *searchResultNode = search(SYM_TABLE, constName);
    if (searchResultNode == NULL) /*if the constant doesn't exist,return error*/
    {
        setErrorStruct(UNDEFINED_CONSTANT, constName);
        return NULL;
    }
    constSymbol = searchResultNode->endOfWord;

    /*if the type of the symbol that has been found isn't constant*/
    if (constSymbol->typeOfSymbol != constS)
    {
        setErrorStruct(EXPECTED_CONST_GOT_LABEL, constName);
        return NULL;
    }
    return constSymbol;
}

/*
 * Processes an immediate constant operand in the given abstract syntax tree (ast) line, and changes the ast to prepare it for the second pass.
 *
 * Algorithm:
 * - Retrieves the name of the constant operand from the AST line.
 * - Searches for the constant symbol in the symbol table.
 * - If the constant is found, change the operand to an immediate num, by Assigning the constant value to the operand.
 *
 * Assumptions:
 * - The 'currLine' parameter points to a valid AST line.
 * - The 'transUnit' parameter points to a valid translation unit.
 */
static void processImmediateConstantOperand(astLine currLine, translationUnit *transUnit, index operandsIndex)
{
    char *constName = currLine.ast->operands[operandsIndex].operand.label;
    /*symbol to store the result of the constant search */
    symbol *sConstant = sConstant = findConst(constName, transUnit); /* Searching for the constant */

    if (getErrorStruct()->error != NO_ERROR)
        return;

    /* Assigning the constant value to the operand */
    (currLine.ast->operands)[operandsIndex].operand.num = sConstant->sValue.constant;

    return;
}
/*
 * Processes the constant of idxConstant operand in the given abstract syntax tree (ast) line, and changes the ast to prepare it for the second pass.
 *
 * Algorithm:
 * - Retrieves the name of the constant operand from the AST line.
 * - Searches for the constant symbol in the symbol table.
 * - If the constant is found, change the operand to an idx num, by Assigning the constant value to the operand.
 *
 * Assumptions:
 * - The 'currLine' parameter points to a valid AST line.
 * - The 'transUnit' parameter points to a valid translation unit.
 */
static void processIdxConstantOperand(astLine currLine, translationUnit *transUnit, index operandsIndex)
{
    char *constName = currLine.ast->operands[operandsIndex].operand.idx.idxLabel;
    /*symbol to store the result of the constant search */
    symbol *sConstant = findConst(constName, transUnit); /* searching for the constant */

    if (getErrorStruct()->error != NO_ERROR)
        return;

    /* changing the operand type to idx num */
    currLine.ast->operands[operandsIndex].operand.idx.idxNum = sConstant->sValue.constant;
    return;
}
/*
 * Checks if the source operand matches the allowed types for the given opcode.
 *
 * Algorithm:
 *   - It checks if the destination operand is compatible with the opcode's requirements.
 *   - otherwize, set an error and exit
 */

static void checkSrcOpMatching(TokenType opCode, int srcOperand)
{
    switch (opCode)
    {
    case mov:
    case add:
    case sub:
    case cmp:
        /* Source operand matches allowed types,exit */
        return;
    case lea:
        /* Check if the source operand is compatible with the opcode of lea instruction */
        if (srcOperand == direct || srcOperand == idx_num || srcOperand == idx_const)
            return;
        else
        {
            /* Source operand mismatch, set error */
            setErrorStruct(MISMATCH_SRC_OPERAND, "");
            return;
        }
    default:
        /* Unknown opcode or opcode of an instruction that doesn't receive src operand,set an error */
        setErrorStruct(MISMATCH_SRC_OPERAND, "");
        return;
    }
}

/*
 * Checks if the destination operand matches the allowed types for the given opcode.
 *
 * Algorithm:
 *   - It checks if the destination operand is compatible with the opcode's requirements.
 *   - otherwise, set an error and exit
 *
 * Assumptions:
 * - The 'destOperand' parameter represents a valid addressing method of the destination operand for a destination operand.
 */
static void checkDestOpMatching(TokenType opCode, int destOperand)
{
    switch (opCode)
    {
    case cmp:
    case prn:
        /* Destination operand matches allowed types,exit */
        return;
    case mov:
    case add:
    case sub:
    case lea:
    case Not:
    case clr:
    case inc:
    case dec:
    case red:
        /* Check if the destination operand is compatible with the opcode */
        if (destOperand == direct || destOperand == idx_num || destOperand == idx_const || destOperand == reg)
            return;
        else
        {
            /* Destination operand mismatch, return error */
            setErrorStruct(MISMATCH_DEST_OPERAND, "");
            return;
        }
    case jmp:
    case bne:
    case jsr:
        /* Check if the destination operand is compatible with the opcode */
        if (destOperand == direct || destOperand == reg)
            return;
        else
        {
            /* Destination operand mismatch, set an error */
            setErrorStruct(MISMATCH_DEST_OPERAND, "");
            return;
        }
    default:
        /* Unknown opcode or opcode that doesn't receive destination operand, set an error */
        setErrorStruct(MISMATCH_DEST_OPERAND, "");
        return;
    }
}

/*
 * Checks if the operands match the allowed types for the opcode in the given abstract syntax tree (AST) line.
 * Algorithm:
 * - Checks the number of operands in the AST line.
 * - If there are two operands:
 *   - Checks if the source operand matches the allowed types for the opcode.
 *   - If there is a mismatch, exit.
 *   - Checks if the destination operand matches the allowed types for the opcode.
 * - If there is only one operand:
 *   - Checks if the destination operand matches the allowed types for the opcode.
 *
 * Assumptions:
 * - The 'currLine' parameter points to a valid AST line.

 */
static void checkOpsMatching(astLine currLine)
{
    if (currLine.ast->numOfOperands == TWO_OPERANDS) /*If there are two operands.*/
    {
        /* Check matching for source and destination operands.*/
        checkSrcOpMatching(currLine.ast->opcode, currLine.ast->operands[FIRST_OPERAND].type);
        if (getErrorStruct()->error != NO_ERROR) /* If there's an error, return an error struct.*/
            return;

        checkDestOpMatching(currLine.ast->opcode, currLine.ast->operands[SECOND_OPERAND].type);
    }
    else if (currLine.ast->numOfOperands == ONE_OPERAND)
    { /* If there's only one operand.*/
        /* Check matching for the single operand (destination).*/
        checkDestOpMatching(currLine.ast->opcode, currLine.ast->operands[FIRST_OPERAND].type);
    }
    /* Create and return an error struct with the final error status.*/
    return;
}
/*
 * Calculates the L attribute for the given AST line in the translation unit.
 *
 * Algorithm:
 * - Checks if the operands match the allowed types for the opcode.
 * - Loops through each operand:
 *   - Processes each operand based on its type.
 *   - Updates the L attribute accordingly.
 *
 * Assumptions:
 * - The 'currLine' parameter points to a valid AST line.
 * - The 'transUnit' parameter points to a valid translation unit structure.
 */
static index calculateL(astLine currLine, translationUnit *transUnit)
{
    index L = 0;
    index i = 0;                          /* Loop counter. */
    bool registerBeenEncountered = FALSE; /* Flag to track if a register has been encountered. */
    checkOpsMatching(currLine);           /* Check operand matching. */

    if (getErrorStruct()->error != NO_ERROR) /* If there's an error, return it. */
        return 0;

    for (i = 0; i < currLine.ast->numOfOperands; i++) /* Loop through each operand. */
    {
        switch (currLine.ast->operands[i].type) /* Check the type of operand. */
        {
        case immediate_const: /* Handle cases for different operand types. */
            processImmediateConstantOperand(currLine, transUnit, i);
            if (getErrorStruct()->error != NO_ERROR) /* If there's an error, return it. */
                return 0;
        case immediate_num:
        case direct:
            L++; /* Increment L for these operand types. */
            break;
        case idx_const:
            processIdxConstantOperand(currLine, transUnit, i);
            if (getErrorStruct()->error != NO_ERROR) /* If there's an error, return it. */
                return 0;
        case idx_num:
            L += INDEX_WORD_AMOUNT; /* Increment L by amount of index words for these operand types. */
            break;
        case reg:
            if (!registerBeenEncountered)   /* If a register hasn't been encountered yet. */
                L++;                        /* Increment L. */
            registerBeenEncountered = TRUE; /* Set the flag to indicate a register has been encountered. */
            break;
        }
    }
    return L;
}

/*
 * Checks for the definition of a label in the symbol table.
 *
 * Algorithm:
 * - Checks if the label name is empty. If so, exit(with no error)
 * - Searches the symbol table to see if the label already exists.
 * - If the label already exists,set an error
 *
 * Assumptions:
 * - The 'labelName' parameter points to a valid string.
 * - The 'transUnit' parameter points to a valid translation unit structure containing a symbol table.
 */

static void checkForLabelDefinition(char *labelName, translationUnit *transUnit)
{
    TrieNode *searchResultNode = NULL;

    /* Check if the label name is empty. If so, exit(with no error) */
    if (strcmp(labelName, "") == 0)
        return;

    /* Search to see if the label already exists. */
    searchResultNode = search(SYM_TABLE, labelName);

    /* If the label already exists, set an error structure indicating the redefinition of the symbol. */
    if (searchResultNode != NULL)
        setErrorStruct(REDEFINITION_OF_SYMBOL, labelName);

    /* exit */
    return;
}
/*
 * Adds a new instruction symbol to the symbol table.
 *
 * Algorithm:
 * - Insert the symbol into the symbol table.
 * - set an error if found any.
 *
 * Assumptions:
 * - The 'labelName' parameter points to a valid string and is not ""..
 * - The 'transUnit' parameter points to a valid translation unit structure.
 */
static void addNewInstSymbol(char *labelName, translationUnit *transUnit)
{
    symbol *symbolToAdd = calloc(ONE_UNIT, sizeof(symbol)); /* Allocate memory for the new symbol. */
    if (symbolToAdd == NULL)
    {
        setErrorStruct(MALLOC_FAILED, ""); /* set error if memory allocation fails. */
        return;
    }
    symbolToAdd->sValue.address = IC + MEMORY_STARTING_POINT; /* Set the address of the symbol. */
    symbolToAdd->typeOfSymbol = operationS;                   /* Set the type of the symbol. */
    if (insertString(labelName, symbolToAdd, SYM_TABLE))      /* Insert the symbol into the symbol table. */
        setErrorStruct(MALLOC_FAILED, "");                    /* set an error if insertion fails. */
    return;
}
/*
 * Handles the case of an operation in the assembly code.
 *
 * Algorithm:
 * - Check if the label is already defined in the symbol table.
 * - Calculate the length of the instruction.
 * - Check if the program memory runs out.
 * - If the label name is not empty, add the new instruction symbol to the symbol table.
 * - Update the instruction counter.
 * - set an error if found any.
 *
 * Assumptions:
 * - The 'currLine' parameter points to a valid AST line.
 * - The 'transUnit' parameter points to a valid translation unit structure.
 */
static void processOperationLine(astLine currLine, translationUnit *transUnit)
{
    index L = 1;                                   /* Initialize the length of the instruction. */
    char *labelName = currLine.ast->labelName;     /* Get the label name from the AST line. */
    checkForLabelDefinition(labelName, transUnit); /* Check if the label is already defined. */
    if (getErrorStruct()->error != NO_ERROR)
        return;                           /* set an error if the label is already defined. */
    L += calculateL(currLine, transUnit); /* Calculate the length of the instruction. */
    if (getErrorStruct()->error != NO_ERROR)
        return;
    if (L + DC + IC >= SIZE_OF_MEMORY)
    {
        setErrorStruct(PROGRAM_MEMORY_RUN_OUT, ""); /* set an error if the program memory runs out. */
        return;
    }
    if (strcmp(labelName, "") != 0)             /* If the label name is not empty. */
        addNewInstSymbol(labelName, transUnit); /* Add the new instruction symbol to the symbol table. */
    IC += L;                                    /* Update the instruction counter. */
    return;                                     /* exit. */
}
/*
 * Adds a new data symbol to the symbol table.
 *
 * Algorithm:
 * - Allocate memory for the new symbol and set its type to dataS (data symbol).
 * - Set the number of values associated with the symbol and its address to the current data counter plus the memory starting point.
 * - Insert the symbol into the symbol table.
 *
 * Assumptions:
 * - The 'labelName' parameter points to a valid string and is not ""..
 * - The 'transUnit' parameter points to a valid translation unit structure.
 */
static void addNewDataSymbol(char *labelName, int dataCount, translationUnit *transUnit)
{
    /* Allocate memory for the new symbol */
    symbol *symbolToAdd = calloc(ONE_UNIT, sizeof(symbol));
    if (symbolToAdd == NULL)
    {
        setErrorStruct(MALLOC_FAILED, "");
        return; /* set an error if memory allocation fails */
    }
    /* Set the type of the symbol to dataS */
    symbolToAdd->typeOfSymbol = dataS;
    /* Set the number of values associated with the symbol */
    symbolToAdd->numOfValues = dataCount;
    /* Set the address of the symbol */
    symbolToAdd->sValue.address = DC + MEMORY_STARTING_POINT;

    /* Insert the symbol into the symbol table */
    if (insertString(labelName, symbolToAdd, SYM_TABLE))
        setErrorStruct(MALLOC_FAILED, ""); /* set an error if insertion fails */

    return; /* exit */
}
/*
 * Processes data values specified in the assembly directive and encodes them to the translation unit.
 *
 * Algorithm:
 * - Iterate over each data value specified in the AST line's operand.
 * - For each data value:
 *   - if it is a constant, search for it in the symbol table. If it is found, retrieve its value; otherwise,set an error and exit.
 *   - if its not a constant, get the numeric value.
 *   - Calculate the memory address where the data value should be stored.
 *   - Check if the calculated memory address exceeds the available memory.
 *   - encode the current data value in the machine word array at the appropriate memory address.
 *
 * Assumptions:
 * - The 'curLine' parameter points to a valid AST line representing an assembly directive.
 * - The 'transUnit' parameter points to a valid translation unit structure.
 */
static void processDataValues(astLine curLine, translationUnit *transUnit)
{
    index dataIndex = 0;                                           /* Loop counter for iterating over data values. */
    index dataCount = curLine.ast->dirStatement.operand.dataCount; /* Total number of data values. */
    index dataMemoryAddress = 0;

    /* Iterate over each data value in the AST line's operand. */
    for (dataIndex = 0; dataIndex < dataCount; dataIndex++)
    {
        short int currentNum; /* Current data value to be processed. */

        /* Check the type of the current data value (constant or numeric). */
        if (curLine.ast->dirStatement.operand.data[dataIndex].type == constant)
        {
            /* Search for the constant data value in the symbol table. */
            symbol *searchResultNode = findConst(curLine.ast->dirStatement.operand.data[dataIndex].value.constant, transUnit);
            /* set an error if finding constant fails. */
            if (getErrorStruct()->error != NO_ERROR)
                return;
            currentNum = searchResultNode->sValue.constant; /* Get the constant value. */
        }
        else                                                                          /* type == num */
            currentNum = curLine.ast->dirStatement.operand.data[dataIndex].value.num; /* Get the numeric value. */

        /* Calculate the memory address where the current data value should be stored. */
        dataMemoryAddress = DC + dataIndex;

        /* Check if the calculated memory address exceeds the available memory. */
        if (dataMemoryAddress >= SIZE_OF_MEMORY)
        {
            setErrorStruct(PROGRAM_MEMORY_RUN_OUT, ""); /* set an error if memory runs out. */
            return;
        }
        /* Set the current data value in the machine word array at the appropriate memory address. */
        setBits(transUnit->machWrdArr + dataMemoryAddress, 0, MACHINE_WORD_SIZE, currentNum);
    }
    return;
}
/*
 * Processes the '.data' assembly directive represented by ast of the line.
 *
 * Algorithm:
 * - Extract the label name from the AST line. and check if its defined. set an error if it is.
 * - Process the data values specified in the '.data' directive.
 * - If the label is not empty, add a new data symbol to the symbol table.
 * - Update the data counter with the count of data values processed.
 *
 * Assumptions:
 * - The 'curLine' parameter points to a valid AST line representing the '.data' directive.
 * - The 'transUnit' parameter points to a valid translation unit structure.
 */
static void processDataLine(astLine curLine, translationUnit *transUnit)
{
    char *labelName = curLine.ast->labelName;                   /* Extract label name from the AST line. */
    checkForLabelDefinition(curLine.ast->labelName, transUnit); /* Check for label definition. */
    if (getErrorStruct()->error != NO_ERROR)
        return;
    processDataValues(curLine, transUnit); /* Process data values. */
    if (getErrorStruct()->error != NO_ERROR)
        return;
    if (strcmp(labelName, "") != 0) /* If the label is not empty, add a new data symbol. */
        addNewDataSymbol(curLine.ast->labelName, curLine.ast->dirStatement.operand.dataCount, transUnit);
    DC += curLine.ast->dirStatement.operand.dataCount; /* Update data counter. */
    return;                                            /* Return any error encountered during processing. */
}
/*
 * Processes the '.extern' assembly directive represented by ast of the line.
 *
 * Algorithm:
 * - Check if the external symbol already exists in the symbol table.if its existing and is not of type 'extern', set an error.
 * - Otherwise, add the new extern symbol to the symbol table.
 *
 * Assumptions:
 * - The 'curLine' parameter points to a valid AST line representing the '.data' directive.
 * - The 'transUnit' parameter points to a valid translation unit structure.
 */
static void processExternLine(astLine curLine, translationUnit *transUnit)
{
    checkForExistingSymbolExtern(curLine.ast->dirStatement.operand.label, transUnit); /* Check for existing extern symbol. */
    if (getErrorStruct()->error != NO_ERROR)
        return;                                                             /* Return error if a symbol already exists or is not extern type. */
    addNewExternSymbol(curLine.ast->dirStatement.operand.label, transUnit); /* Add the new extern symbol to the symbol table. */
    return;
}
/*
 * Checks if an external symbol already exists in the symbol table.
 *
 * Algorithm:
 * - Search for the symbol in the symbol table and check if its type is 'extern', if it doesn't set an error.
 * - Otherwise,exit.
 *
 * Assumptions:
 * - The 'labelName' parameter points to a valid string representing the symbol name.
 * - The 'transUnit' parameter points to a valid translation unit structure.
 */
static void checkForExistingSymbolExtern(char *labelName, translationUnit *transUnit)
{
    TrieNode *searchResultNode = search(SYM_TABLE, labelName); /* Search for the symbol in the symbol table. */
    if (searchResultNode != NULL)
    {
        symbol *searchResultSymbol = searchResultNode->endOfWord; /* Get the symbol if found. */
        if (searchResultSymbol != NULL && searchResultSymbol->typeOfSymbol != externS)
            setErrorStruct(REDEFINITION_OF_SYMBOL, labelName); /* set an error if symbol is not extern type. */
    }
    return; /* exit */
}

/*
 * Adds a new external symbol to the symbol table.
 *
 * Algorithm:
 * - Allocate memory for the new symbol and set the type of the symbol to 'extern' and the address to 0.
 * - Insert the symbol into the symbol table using the insertString function.
 *
 * Assumptions:
 * - The 'labelName' parameter points to a valid string representing the extern name and is not "".
 * - The 'transUnit' parameter points to a valid translation unit structure.
 */
static void addNewExternSymbol(char *labelName, translationUnit *transUnit)
{
    symbol *symbolToAdd = calloc(ONE_UNIT, sizeof(symbol)); /* Allocate memory for the new symbol. */
    if (symbolToAdd == NULL)
    {
        setErrorStruct(MALLOC_FAILED, ""); /* set an error if memory allocation fails. */
        return;
    }
    symbolToAdd->typeOfSymbol = externS; /* Set the type of the symbol to extern. */
    symbolToAdd->sValue.address = 0;     /* Set the address of the symbol to 0. */
    if (insertString(labelName, symbolToAdd, SYM_TABLE))
    {
        setErrorStruct(MALLOC_FAILED, ""); /* set an error if memory allocation fails. */
        return;
    }
    return; /* exit */
}
/*
 * Checks for a redefinition of constant in the symbol table.
 *
 * Algorithm:
 * - If the constant name is NULL or an empty string,exit(with no error).
 * - Search for the constant in the symbol table., if its found, set an error
 *
 * Assumptions:
 * - The 'constName' parameter points to a valid string representing the constant name.
 * - The 'transUnit' parameter points to a valid translation unit structure.
 */
static void checkForConstantRedefinition(char *constName, translationUnit *transUnit)
{
    TrieNode *searchResultNode = NULL;

    /* Check if constName is NULL or an empty string*/
    if (constName == NULL || strcmp(constName, "") == 0)
    {
        setErrorStruct(NO_ERROR, "");
        return;
    }
    /* Search for the constant in the symbol table*/
    searchResultNode = search(SYM_TABLE, constName);

    /* If constant found, return an error indicating redefinition of constant*/
    if (searchResultNode != NULL)
        setErrorStruct(REDEFINITION_OF_CONSTANT, constName);
    return;
}
/*
 * Adds a new constant to the symbol table.
 *
 * Algorithm:
 * - Set the constant value and type of symbol in the symbol structure and insert the symbol into the symbol table.
 *
 * Assumptions:
 * - The 'constName' parameter points to a valid string representing the symbol name and is not ""..
 * - The 'transUnit' parameter points to a valid translation unit structure.
 */
static void addNewConstant(char *constName, short int constantValue, translationUnit *transUnit)
{
    symbol *symbolToAdd = calloc(ONE_UNIT, sizeof(symbol));
    if (symbolToAdd == NULL)
    {
        setErrorStruct(MALLOC_FAILED, ""); /*set an error*/
        return;
    }

    symbolToAdd->sValue.constant = constantValue;
    symbolToAdd->typeOfSymbol = constS;
    if (insertString(constName, symbolToAdd, SYM_TABLE))
        setErrorStruct(MALLOC_FAILED, ""); /*set an error*/

    return;
}
/*
 * Processes a constant definition and adds it to the symbol table.
 *
 * Algorithm:
 * - Check for constant redefinition and Extract the constant value from the curLine.
 * - Add a new constant definition to the symbol table.
 *
 * Assumptions:
 * - The 'curLine' parameter points to a valid AST line with valid astNode.
 * - The 'transUnit' parameter points to a valid translation unit structure.
 */
static void processConstDef(astLine curLine, translationUnit *transUnit)
{
    short int constantValue = 0;
    checkForConstantRedefinition(curLine.ast->labelName, transUnit);

    /*Check if there's an error indicating constant redefinition*/
    if (getErrorStruct()->error != NO_ERROR)
        return;

    /*Extract the constant value from the curLine.*/
    constantValue = curLine.ast->dirStatement.operand.data[0].value.num;

    /*Add a new constant to the symbol table*/
    addNewConstant(curLine.ast->labelName, constantValue, transUnit);
    return;
}
/*
 * Handles directive lines during the first pass of assembly code processing.
 *
 * Algorithm:
 * - Determine the type of directive line and call corresponding processing function based on the directive type:
 *   - For entry, add the AST to the first pass AST buffer.
 *   - For external, process the line and free the AST memory.
 *   - For a constant definition, process the line and free the AST memory.
 *   - For data, process the line and free the AST memory.
 *
 * Assumptions:
 * - 'transUnit','curLine' and 'firstPassBuffer' point to valid structs.
 */
static void handleDirectiveLines(astLine curLine, FirstPassAstBuffer *firstPassBuffer, translationUnit *transUnit)
{
    /* Determine the type of directive line*/
    switch (curLine.ast->dirStatement.type)
    {
    case entry:
        addAstToBuffer(curLine, firstPassBuffer);
        break;
    case external:
        processExternLine(curLine, transUnit);
        free(curLine.ast);
        break;
    case const_def:
        processConstDef(curLine, transUnit);
        free(curLine.ast);
        break;
    case data:
        processDataLine(curLine, transUnit);
        free(curLine.ast);
        break;
    }
    return;
}
/*
 * Handles different types of lines during the first pass of assembly code processing.
 *
 *
 * Algorithm:
 * - Determine the type of line and call the corresponding processing function based on the line type:
 *   - For the operation line, call the processOperationLine function and add the AST to ast buffer if no error occurs.
 *   - For the directive line, call the handleDirectiveLines function to handle the directive.
 * Assumptions:
 * - 'transUnit','curLine' and 'firstPassBuffer' point to valid structs.
 */
static void lineTypeCases(astLine curLine, FirstPassAstBuffer *firstPassBuffer, translationUnit *transUnit)
{
    /* Determine the type of line */
    switch (curLine.ast->lineType)
    {
    case operation:
        /*Call the operation case*/
        processOperationLine(curLine, transUnit);

        /* If an error occurred during operation processing,exit*/
        if (getErrorStruct()->error != NO_ERROR)
        {
            free(curLine.ast);
            return;
        }
        /* Add the AST to buffer */
        addAstToBuffer(curLine, firstPassBuffer);
        break;
    case directive:
        /* Call the directive case */
        handleDirectiveLines(curLine, firstPassBuffer, transUnit);
        break;
    }

    return;
}
/*
 * Handles a single line of assembly code during the first pass.
 *
 *
 * Algorithm:
 * - Parse the current line to generate an AST.
 * - Determine the type of line (operation or directive) using the AST.
 * - calls corresponding processing function based on the line type.
 * - If an error occurs during processing, handle the error and return TRUE.
 * - Otherwise, return FALSE indicating successful processing.
 *
 * Assumptions:
 * - The 'currrentLine' parameter contains a valid string.
 * - The 'firstPassBuffer' parameter points to a valid first pass AST buffer structure.
 * - fileName parameter points to a valid string representing the file name.
 */
static bool handleLine(char currentLine[], FirstPassAstBuffer *firstPassBuffer, index linecounter, translationUnit *transUnit, char *fileName)
{
    astLine currLine = {0};
    bool error = 0;

    /*Initialize errorStruct to default*/
    setErrorStruct(NO_ERROR, "");
    /* Initialize the current line structure with line number and parsed AST*/
    currLine.lineNumber = linecounter;
    currLine.ast = createAst(currentLine, linecounter, fileName);

    /* If parsing the line fails, return TRUE (indicating an error)*/
    if (currLine.ast == NULL)
    {
        if (getErrorStruct()->error == MALLOC_FAILED)
            freeFirstPassMemory(transUnit, firstPassBuffer);
        return TRUE;
    }
    /* Determine the type of line and process it*/
    lineTypeCases(currLine, firstPassBuffer, transUnit);
    /* Set error to TRUE if an error occurred during processing, otherwise FALSE*/
    error = (getErrorStruct()->error != NO_ERROR) ? TRUE : FALSE;

    /* Handle the error (if any)*/
    if (getErrorStruct()->error != NO_ERROR)
    {
        if (getErrorStruct()->error == MALLOC_FAILED || getErrorStruct()->error == PROGRAM_MEMORY_RUN_OUT)
            freeFirstPassMemory(transUnit, firstPassBuffer);
        else
            handleError(getErrorStruct(), linecounter, fileName);
    }
    return error;
}
/*
 * Performs the first pass of assembly code processing, builds the translation unit, and encodes the data in the machine word array.
 *
 * Algorithm:
 * - Read each line of the assembly code:
 *   - Remove the newline character from the current line.
 *   - Handle the current line using the handleLine function.
 *   - Update the boolean error indicator if an error occurs during processing.
 * - Set the buffer size in the first pass AST buffer and resize it to save memory.
 *
 * Assumptions:
 * - fileName parameter points to a valid string representing the file name.
 * - The 'firstPassBuffer' parameter points to a valid first pass AST buffer structure.
 * - The 'transUnit' parameter points to a valid translation unit structure.
 * - toRead is a valid file pointer
 */
bool firstPass(FirstPassAstBuffer *firstPassBuffer, translationUnit *transUnit, FILE *toRead, char *fileName)
{
    index lineCounter = 0;
    char currentLine[MAX_LINE_LEN] = {0};
    bool error = FALSE;       /* Indicates if an error was found */
    SYM_TABLE = createTrie(); /* Create symbol table */
    (*getAstBufferIndex()) = 0;

    /* Read each line of the assembly code */
    for (lineCounter = 1; fgets(currentLine, MAX_LINE_LEN, toRead); lineCounter++)
    {

        if (currentLine[strlen(currentLine) - 1] == '\n')
            currentLine[strlen(currentLine) - 1] = '\0'; /* Remove newline character */

        /* Process the current line */
        error |= handleLine(currentLine, firstPassBuffer, lineCounter, transUnit, fileName);
        if (getErrorStruct()->error == MALLOC_FAILED || getErrorStruct()->error == PROGRAM_MEMORY_RUN_OUT)
            return TRUE;
    }
    if (firstPassBuffer->bufferSize != 0)
    {
        /* Set the buffer size in the first pass AST buffer */
        firstPassBuffer->bufferSize = *(getAstBufferIndex());

        /* Reallocate memory for the AST buffer to save memory */
        reallocateMemory(sizeof(astLine), firstPassBuffer->bufferSize, (void **)&(firstPassBuffer->astBuffer));

        if (getErrorStruct()->error == MALLOC_FAILED)
        {
            freeFirstPassMemory(transUnit, firstPassBuffer);
            return TRUE;
        }
    }
    return error;
}

/*
 * Reallocates memory for a given pointer.
 *
 * Algorithm:
 * - Allocate memory of size 'newSize * objectSize' using 'realloc' for the pointer pointed by 'ptrToReallocate'.
 * - Check if the allocation was successful:
 *    - If successful, update the pointer to the newly allocated memory.
 *
 * Assumptions:
 * - The 'ptrToReallocate' parameter points to a valid pointer.
 * - 'objectSize' and 'newSize' are greater than 0.
 */
void reallocateMemory(unsigned int objectSize, unsigned int newSize, void **ptrToReallocate)
{
    void *temp; /* Temporary void pointer for reallocation */
    temp = realloc((*ptrToReallocate), newSize * objectSize);
    if (temp == NULL)
    {
        setErrorStruct(MALLOC_FAILED, "");
        return;
    }
    (*ptrToReallocate) = temp;
    return;
}
/*
 * Frees memory allocated during the first pass of assembly.
 *
 * Algorithm:
 * - Clean the AST buffer by calling 'cleanAstBuffer' with the buffer size and the AST buffer itself.
 * - Delete the trie associated with the symbol table by calling 'deleteTrie' with the symbol table identifier ('SYM_TABLE').
 * Assumptions:
 * - 'transUnit' and 'firstPassBuffer' point to valid structs.
 * - Memory associated with 'firstPassBuffer->astBuffer' and the symbol table (SYM_TABLE) has been properly allocated.
 */
void freeFirstPassMemory(translationUnit *transUnit, FirstPassAstBuffer *firstPassBuffer)
{
    cleanAstBuffer(firstPassBuffer->astBuffer); /* Free memory associated with the AST buffer */
    deleteTrie(SYM_TABLE);                      /* Deallocate memory associated with the symbol table */
}

/**
 * Sets a specific range of bits in a machine word to a given value.
 *
 * Algorithm:
 * clear the bits at positions 'k' to ('k' + 'numBits' - 1) in the original machine word 'num'.
 * Then, it sets the bits in the specified range with the corresponding bits from 'value',
 *
 * Assumptions:
 * the machine word 'num' is large enough to contain the specified range of bits without overflow.
 */
void setBits(machineWord *num, int k, int numBits, short int value)
{
    /* Create a temporary mask with 'numBits' bits set to 1, starting from the 'k'th place */
    machineWord temp = ((1 << numBits) - 1) << k;

    /* Clear the bits at positions 'k' to ('k' + 'numBits' - 1) in 'num' */
    *num &= ~temp;

    /* Set the bits with the corresponding bits from 'value' */
    (*num) |= (value << k) & temp;
}