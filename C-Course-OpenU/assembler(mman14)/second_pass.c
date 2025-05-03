/*
 * This file is responsible for the second pass stage of the assebmling process:
 * Completing the machine word array (machine memory image) and creating the extern and entry arrays.
 * The functions in this file scan the firstPassAstBuffer (the lines the first pass didn't handle) and
 * For entry lines, they add the entry into the entry appearance array. For operation lines, they encode
 * them into the machine word array (machine code image). For lines that reference an extern label,
 * they add the address of the extern refrece into the extern reference array.
 */
#include "lexer.h"
#include "trie.h"
#include "ast.h"
#include "symbol.h"
#include "second_pass.h"
#include "first_pass.h"

#define EXT_ARR transUnit->extArr
#define EXT_ARR_SIZE transUnit->extArrSize
#define ENTRY_ARR transUnit->entryArr
#define ENTRY_ARR_SIZE transUnit->entryArrSize
/*
 * Retrieves the opcode corresponding to the given abstract syntax tree (AST) line.
 *
 * This function maps the opcode of the provided AST line to its corresponding static opcode value.
 *
 * Parameters:
 * curLine - The abstract syntax tree line whose opcode is to be retrieved.
 *
 * Returns:
 * - The static opcode value corresponding to the opcode of the provided AST line.
 * - ERROR if the opcode is not recognized.
 */
static opcodes getOpCode(astLine curLine);
/*
 * Retrieves a pointer to the index that keeps track of the current available space in the entry array.
 *
 * Returns:
 *  the address of astBufferIndex.
 */
static index *getEntryArrIndex(void);
/*
 * Retrieves a pointer to the index that keeps track of the current available space in the extern array.
 *
 * Returns:
 * The address of externArrIndex.
 */
static index *getExternArrIndex(void);
/*
 * Retrieves a pointer to the index that keeps track of the current available space in the machine word array.
 *
 * Returns:
 *  The address of machineWordIndex.
 */
static index *getMachineWordIndex(void);
/*
 * Adds the given externReference to the extern array.
 *
 * Parameters:
 * - toAdd: The externReference to add to the array.
 * - transUnit: Pointer to the translation unit containing the extern array.
 */
static void addToExternArray(translationUnit *transUnit, externReference toAdd);
/*
 * Adds the given entryAppearance to the entry array.
 *
 * Parameters:
 * - toAdd: The entryAppearance to add to the array.
 * - transUnit: Pointer to the translation unit containing the entry array.
 */
static void addToEntryArray(translationUnit *transUnit, entryAppearance toAdd);
/* Frees memory allocated during the first and second pass of assembly.
 *
 * Parameters:
 * - firstPassBuffer: Pointer to the first pass AST buffer.
 * - transUnit: The translation unit containing the symbol table to be freed.
 */
static void freeSecondPassMemory(FirstPassAstBuffer *firstPassBuffer, translationUnit *transUnit);
/*
 * Finds a label symbol by its name in the symbol table of the translation unit.
 *
 * Parameters:
 * - constName: Name of the label to be found.
 * - transUnit: Pointer to the translation unit containing the symbol table.
 *
 * Returns:
 * - Pointer to the label symbol if found.
 */
static symbol *findLabel(char *labelName, translationUnit *transUnit);
/*
 * Calculates the address of a label operand.
 *
 * This function calculates the address of a label operand by adding the instruction counter (IC)
 * if the operand symbol type is dataS, then add the address of the operand symbol.
 *
 * Parameters:
 * - labelSymbol: Pointer to the symbol associated with the label operand.
 * - transUnit: Pointer to the translation unit containing the instruction counter (IC).
 *
 * Returns:
 * - The calculated label address.
 */
static index calculateLabelAddress(symbol *labelSymbol, translationUnit *transUnit);
/*
 * encodes the address of a given label in the current machine word.
 *
 * This function encodes the address of a label in the current machine word by calculating the appropriate ARE (Absolute/Relative/External)
 * based on the type of symbol associated with the label. It sets the address field and ARE field in the current machine word
 * accordingly. If the label is external, it adds the extern appearance to the extern array.
 *
 * Parameters:
 * - transUnit: Pointer to the translation unit containing the machine word array.
 * - labelSymbol: Pointer to the symbol associated with the label.
 * - labelName: The name of the label.
 * - lineNum: The line number in the assembly code.
 */
static void labelAddressEncoding(translationUnit *transUnit, symbol *labelSymbol, char *labelName, index lineNum);

/*
 * Encodes a numerical value in the current machine word.
 *
 * This function encodes a numerical value by setting the value field in the current machine word
 * with the numerical value.
 *
 * Parameters:
 * - transUnit: Pointer to the translation unit containing the machine word array.
 * - numValue: The value of the operand.
 */
static void numValueEncoding(translationUnit *transUnit, short int numValue);

/*
 * Handles the encoding of an immediate operand.
 *
 * This function handles the encoding of an immediate operand by calling corresponding processing functions to
 * encode its numerical value and encode the Immediate addressing method in the corresponding bits(SRC_ADRESSING or DEST_ADRESSING)
 * of the first machine word of the instruction.
 *
 * Parameters:
 * - firstInstWord: Pointer to the first machine word of the instruction.
 * - transUnit: Pointer to the translation unit.
 * - curLine: The current line in the abstract syntax tree (AST).
 * - operandsIndex: The index of the operand in the AST.
 * - srcOrDest: A boolean indicating whether the operand is for source or destination.
 */
static void handleImmediateOperand(machineWord *firstInstWord, translationUnit *transUnit, astLine curLine, index operandsIndex, bool srcOrDest);

/*
 * Handles the encoding of a direct operand.
 *
 * This function handles the encoding of a direct operand by encoding the direct addressing method
 * in the corresponding bits(SRC_ADRESSING or DEST_ADRESSING) of the first machine word of the instruction,
 * and calling corresponding processing functions to encode its address value.
 *
 * Parameters:
 * - firstInstWord: Pointer to the first machine word of the instruction.
 * - transUnit: Pointer to the translation unit.
 * - curLine: The current line in the abstract syntax tree (AST).
 * - operandsIndex: The index of the operand in the AST.
 * - srcOrDest: A boolean indicating whether the operand is for source or destination.
 */
static void handleDirectOperand(machineWord *firstInstWord, translationUnit *transUnit, astLine curLine, index operandsIndex, bool srcOrDest);

/*
 * Handles the encoding of an indexed operand.
 *
 * This function handles the encoding of an indexed operand by encoding the indexed addressing method
 * in the corresponding bits(SRC_ADRESSING or DEST_ADRESSING) of the first machine word of the instruction,
 * and calling corresponding processing functions to encode its label address value and index value.
 *
 * Parameters:
 * - firstInstWord: Pointer to the first machine word of the instruction.
 * - transUnit: Pointer to the translation unit.
 * - curLine: The current line in the abstract syntax tree (AST).
 * - operandsIndex: The index of the operand in the AST.
 * - srcOrDest: A boolean indicating whether the operand is for source or destination.
 */
static void handleIndexedOperand(machineWord *firstInstWord, translationUnit *transUnit, astLine curLine, index operandsIndex, bool srcOrDest);

/*
 * Handles the encoding of a register operand.
 *
 * This function handles the encoding of a register operand by setting the register value in the current machine word
 * and setting the addressing mode (SRC_ADRESSING or DEST_ADRESSING) in the first machine word of the instruction.
 *
 * Parameters:
 * - firstInstWord: Pointer to the first machine word of the instruction.
 * - transUnit: Pointer to the translation unit.
 * - curLine: The current line in the abstract syntax tree (AST).
 * - operandsIndex: The index of the operand in the AST.
 * - srcOrDest: A boolean indicating whether the operand is for source or destination.
 * - lastOperandWasReg: A boolean indicating whether the last operand was a register operand.
 */
static void handleRegisterOperand(machineWord *firstInstWord, translationUnit *transUnit, astLine curLine, index operandsIndex, bool srcOrDest, bool lastOperandWasReg);

/*
 * Handles different addressing cases for operands.
 *
 * This function handles different addressing cases for operands based on the type of operand in the current AST line.
 * It calls corresponding functions to handle immediate, direct, indexed, and register operands.
 *
 * Parameters:
 * - curLine: The current line in the abstract syntax tree (AST).
 * - transUnit: Pointer to the translation unit.
 * - firstInstWord: Pointer to the first machine word of the instruction.
 * - operandsIndex: The index of the operand in the AST.
 * - srcOrDest: A boolean indicating whether the operand is for source or destination.
 */
static void addressingCases(astLine curLine, translationUnit *transUnit, machineWord *firstInstWord, bool srcOrDest, index operandsIndex);

/*
 * Processes the '.entry' assembly directive represented by ast of the line.
 *
 * Parameters:
 * - curLine: The current line in the abstract syntax tree (AST).
 * - transUnit: Pointer to the translation unit.
 */
static void processEntryLine(astLine curLine, translationUnit *transUnit);

/*
 * Processes an operation line represented by ast.
 *
 * This function processes an operation line in the assembly code by setting the opcode in the first machine word
 * of the instruction and handling of each operand.
 *
 * Parameters:
 * - curLine: The current line in the abstract syntax tree (AST).
 * - transUnit: Pointer to the translation unit.
 */
static void processOperationLine(astLine curLine, translationUnit *transUnit);

/*
 * Resizes the entry array in the translation unit.
 *
 * if the entry array size is not zero. this function resizes the entry array in the translation unit by updating its size and reallocating memory
 * to save memory.
 *
 * Parameters:
 * - transUnit: Pointer to the translation unit containing the entry arr size and the entry arr.
 */
void resizeEntryArr(translationUnit *transUnit);

/*
 * Resizes the extern array in the translation unit.
 *
 * if the extern array size is not zero. this function resizes the extern array in the translation unit by updating its size and reallocating memory
 * to save memory.
 *
 * Parameters:
 * - transUnit: Pointer to the translation unit.
 */
void resizeExternArr(translationUnit *transUnit);

/*end of prototype decleartion*/

/*
 * Retrieves a pointer to the index that keeps track of the current available space in the entry array.
 *
 * Algorithm:
 *  on the first call for this function, initialize entryArrIndex to 0.
 *  returning the address of entryArrIndex.
 */
static index *getEntryArrIndex(void)
{
    static index entryArrIndex = 0; /* Index that keeps track of the currently available space in the entry array */
    return &entryArrIndex;
}

/*
 * Retrieves a pointer to the index that keeps track of the current available space in the extern array.
 *
 * Algorithm:
 *  on the first call for this function, initialize externArrIndex to 0.
 *  returning the address of externArrIndex.
 */
static index *getExternArrIndex(void)
{
    static index externArrIndex = 0; /* Index that keeps track of the current available space in the extern array */
    return &externArrIndex;
}

/*
 * Retrieves a pointer to the index that keeps track of the current available space in the machine word array.
 *
 * Algorithm:
 *  on the first call for this function, initialize machWordIndex to 0.
 *  returning the address of machWordIndex.
 */
static index *getMachineWordIndex(void)
{
    static index machWordIndex = 0; /* Index indicating the current position in the machine word array */
    return &machWordIndex;
}

/*
 * Adds the given externReference to the extern array.
 *
 * Algorithm:
 * - Retrieves a pointer to the index that keeps track of the current available space in the extern array.
 * - Check if there is enough space in the extern array to save the externReference.
 * - If there is not enough space, double the array size.
 * - saves the given externReference in the extern array.
 *
 * Assumptions:
 * - 'transUnit' is a valid pointer to a translation unit structure.
 */
static void addToExternArray(translationUnit *transUnit, externReference toAdd)
{
    /* Get the index of the extern arr */
    index *externIndex = getExternArrIndex();

    /* If there is no space to save the current externReference */
    if (*externIndex == EXT_ARR_SIZE)
    {
        /* make sure EXT_ARR_SIZE is bigger than zero */
        EXT_ARR_SIZE = (EXT_ARR_SIZE == 0) ? 1 : EXT_ARR_SIZE;

        /* Double the arr size */
        EXT_ARR_SIZE *= SIZE_SCALER;

        /* Reallocate memory for the extern arr */
        reallocateMemory(sizeof(externReference), EXT_ARR_SIZE, (void **)&(EXT_ARR));
        if (getErrorStruct()->error != NO_ERROR)
            return; /* Exit */
    }
    /* Save the current externReference in extern arr */
    EXT_ARR[(*externIndex)++] = toAdd;
    return; /* Exit */
}

/*
 * Adds the given entryAppearance to the entry array.
 *
 * Algorithm:
 * - Retrieves a pointer to the index that keeps track of the current available space in the entry array.
 * - Check if there is enough space in the entry array to save the entryAppearance.
 * - If there is not enough space, double the array size.
 * - saves the given entryAppearance in the entry array.
 *
 * Assumptions:
 * - 'transUnit' is a valid pointer to a translation unit structure.
 */
static void addToEntryArray(translationUnit *transUnit, entryAppearance toAdd)
{
    /* Get the index of the AST buffer */
    index *entryIndex = getEntryArrIndex();

    /* If there is no space to save the current AST */
    if (*entryIndex == ENTRY_ARR_SIZE)
    {
        /* make sure EXT_ARR_SIZE is bigger than zero */
        ENTRY_ARR_SIZE = (ENTRY_ARR_SIZE == 0) ? 1 : EXT_ARR_SIZE;

        /* Double the buffer size */
        ENTRY_ARR_SIZE *= SIZE_SCALER;

        /* Reallocate memory for the AST buffer */
        reallocateMemory(sizeof(entryAppearance), ENTRY_ARR_SIZE, (void **)&(ENTRY_ARR));
        if (getErrorStruct()->error != NO_ERROR)
            return; /* Exit */
    }
    /* Save the current AST in astBuffer */
    ENTRY_ARR[(*entryIndex)++] = toAdd;
    return; /* Exit */
}

/*
 * Frees memory allocated during the first and second pass of assembly.
 *
 * Algorithm:
 * - Free all the memory allocated for the first pass using 'freeFirstPassMemory'.
 * - Free the extern and the entry arrays.
 *
 * Assumptions:
 * - 'transUnit' is a valid pointer to a translation unit structure.
 */
static void freeSecondPassMemory(FirstPassAstBuffer *firstPassBuffer, translationUnit *transUnit)
{
    /* Free memory allocated during the first pass */
    freeFirstPassMemory(transUnit, firstPassBuffer);

    /* Check if EXT_ARR is not NULL, then free memory allocated for it */
    if (EXT_ARR != NULL)
        free(EXT_ARR);

    /* Check if ENTRY_ARR is not NULL, then free memory allocated for it */
    if (ENTRY_ARR != NULL)
        free(ENTRY_ARR);

    /* Exit function */
    return;
}

/*
 * Finds a label symbol by its name in the symbol table of the translation unit.
 *
 * Algorithm:
 * - Searches for the label symbol in the symbol table using its name.
 * - If the label symbol doesn't exist,set an error and return NULL.
 * - If the type of the symbol found is constant (not label), set an error and return NULL.
 *
 * Assumptions:
 * - 'labelName' points to a valid string which is different than ""(because this case is already handled by createAst)..
 * - 'transUnit' is a valid pointer to a translation unit structure.
 * -  searchResultNode->endOfWord is not NULL.
 */
static symbol *findLabel(char *labelName, translationUnit *transUnit)
{
    symbol *labelSymbol = 0;
    /* Search for the labelName in the symbol table */
    TrieNode *searchResultNode = search(SYM_TABLE, labelName);

    /* if not found, set an error structure for an undefined label and return */
    if (searchResultNode == NULL)
    {
        setErrorStruct(UNDEFINED_LABEL, labelName);
        return NULL;
    }
    /* Assign the labelSymbol pointer to the symbol associated with the label */
    labelSymbol = searchResultNode->endOfWord;

    /* Check if the type of symbol is constS, if so, set an error structure for the expected label but got constant and return */
    if (labelSymbol->typeOfSymbol == constS)
    {
        setErrorStruct(EXPECTED_LABEL_GOT_CONST, labelName);
        return NULL;
    }
    return labelSymbol;
}

/*
 * Processes the '.entry' assembly directive represented by ast of the line.
 *
 * Algorithm:
 * - Check if the entry name exists in the symbol table. if it exists and is not of type label, set an error.
 * - Otherwise, add the new entry to the entry array.
 *
 * Assumptions:
 * - 'transUnit' is a valid pointer to a translation unit structure.
 * - 'curLine'  is a valid pointer to an ast line structure.
 * - 'entryName'  is not ""(because this case is already handled by createAst).
 */
static void processEntryLine(astLine curLine, translationUnit *transUnit)
{
    symbol *entryS = 0;                                        /* Pointer to the symbol associated with the entry name */
    entryAppearance currEntry = {0};                           /* Structure representing the current entry appearance */
    char *entryName = curLine.ast->dirStatement.operand.label; /* Name of the entry */
    entryS = findLabel(entryName, transUnit);                  /* find a label aymbol with entryName in the symbol tabel */

    /* If an error occurred during label search, exit */
    if (getErrorStruct()->error != NO_ERROR)
        return;
    /* Initialize an entry appearance structure with the entry name and the entry address */
    strcpy(currEntry.name, entryName);
    if (entryS->typeOfSymbol == dataS)
        currEntry.entAddress = entryS->sValue.address + IC;
    else
        currEntry.entAddress = entryS->sValue.address;

    /* Add the entry appearance to the entry array */
    addToEntryArray(transUnit, currEntry);
    return;
}

/*
 * Calculates the address of a label operand.
 *
 * Algorithm:
 * - if the operand symbol type is dataS, add the instruction counter (IC).
 * - add the address of the operand symbol and return the result.
 *
 * Assumptions:
 * - 'transUnit and 'labelSymbol' point to valid structs.
 */
static index calculateLabelAddress(symbol *labelSymbol, translationUnit *transUnit)
{
    /* Initialize the label address to 0 */
    index labelAddress = 0;

    /* If the operand symbol type is dataS, add the instruction counter (IC) */
    if (labelSymbol->typeOfSymbol == dataS)
        labelAddress += IC;

    /* Add the address of the operand symbol */
    labelAddress += labelSymbol->sValue.address;

    /* Return the calculated label address */
    return labelAddress;
}

/*
 * encodes the address of a given label in the current machine word.
 *
 * Algorithm:
 * - Obtain the machine word index and calculate the ARE.
 * - If the type of the label is external, set ARE to EXTERNAL. Otherwise, set ARE to RELOCATABLE.
 * - Calculate the label address by calling calculateLabelAddress and encode the address field and ARE field in the current machine word.
 * - If the type of the label is external: create an externReference structure. and add this structure to the extern array,
 * - Including the name of the label and the current address.
 *
 * Assumptions:
 * - 'labelName' points to a valid string that is different from ""(because this case is already handled by createAst).
 * - 'transUnit' is a valid pointer to a translation unit structure.
 */
static void labelAddressEncoding(translationUnit *transUnit, symbol *labelSymbol, char *labelName, index lineNum)
{
    index *mchWrdIdx = getMachineWordIndex();

    /* Pointer to the current machine word */
    machineWord *currMachineWord = transUnit->machWrdArr + DC + *mchWrdIdx;

    /* Calculate the ARE based on the type of symbol */
    index are = (labelSymbol->typeOfSymbol == externS) ? EXTERNAL : RELOCATABLE;

    /* Calculate the label address using the calculateLabelAddress function */
    index labelAddress = calculateLabelAddress(labelSymbol, transUnit);

    /* Set the address field and ARE field in the current machine word */
    setBits(currMachineWord, ADRESS, ADRESS_SIZE, labelAddress);
    setBits(currMachineWord, ARE, ARE_SIZE, are);

    /* If the addressing mode is EXTERNAL, add the extern appearance to the extern array */
    if (are == EXTERNAL)
    {
        /* Structure representing the extern appearance */
        externReference curExt;

        /* Initialize the extern appearance */
        curExt.extAddress = *mchWrdIdx + MEMORY_STARTING_POINT;
        strcpy(curExt.name, labelName);

        /* Add the extern appearance to the extern array */
        addToExternArray(transUnit, curExt);
    }

    (*mchWrdIdx)++;
    return;
}

/*
 * Encodes a numerical value in the current machine word.
 *
 * Algorithm:
 * - Obtain the machine word index.
 * - encode the numValue in the value field in the current machine word.
 *
 * Assumptions:
 * - 'transUnit' is a valid pointer to a translation unit structure.
 * - 'numValue' is a twelve-bit number.
 */
static void numValueEncoding(translationUnit *transUnit, short int numValue)
{
    index *mchWrdIdx = getMachineWordIndex();

    /* Pointer to the current machine word */
    machineWord *currMachineWord = (transUnit->machWrdArr) + DC + (*mchWrdIdx)++;

    /* Set the value field in the current machine word with the operand value */
    setBits(currMachineWord, VALUE, VALUE_SIZE, numValue);

    return;
}

/*
 * Handles the encoding of an immediate operand.
 *
 * Algorithm:
 * - Call numValueEncoding function to encode the numerical value of the immediate operand.
 * - Set the addressing mode (source or destination) in the first machine word of the instruction based on the srcOrDest parameter using the setBits function.
 *
 * Assumptions:
 * - 'transUnit' is a valid pointer to a translation unit structure.
 * - 'curLine'  is a valid pointer to an ast line structure.
 * -  'firstInstWord' is a valid pointer to a machineWord.
 */
static void handleImmediateOperand(machineWord *firstInstWord, translationUnit *transUnit, astLine curLine, index operandsIndex, bool srcOrDest)
{
    /* Calling numValueEncoding function */
    numValueEncoding(transUnit, curLine.ast->operands[operandsIndex].operand.num);

    /* Set the addressing mode (SRC_ADRESSING or DEST_ADRESSING) in the first machine word of the instruction */
    setBits(firstInstWord, (srcOrDest) ? SRC_ADRESSING : DEST_ADRESSING, ADRESSING_METHODS_SIZE, IMMEDIATE_ADDRESSING);
}
/*
 * Handles the encoding of an immediate operand.
 *
 * Algorithm:
 * - Initialize variables for label symbol and name.
 * - If the label name is not empty, find the associated symbol.
 * - Encode the label using labelAddressEncoding.
 * - Check for errors during processing, if found any:return.
 * - Set the addressing mode in the first machine word of the instruction.
 * - Return after setting the addressing mode.
 *
 * Assumptions:
 * - assuming the 'labelName' is not "" (because this case is already handled by createAst).
 * - 'transUnit' is a valid pointer to a translation unit structure.
 * - 'curLine'  is a valid pointer to an ast line structure.
 * -  'firstInstWord' is a valid pointer to a machineWord.
 */
static void handleDirectOperand(machineWord *firstInstWord, translationUnit *transUnit, astLine curLine, index operandsIndex, bool srcOrDest)
{
    /* Pointer to the symbol associated with the direct operand */
    symbol *labelSymbol = NULL;

    /* Name of the direct operand */
    char *labelName = curLine.ast->operands[operandsIndex].operand.label;

    /* find the associated symbol */
    labelSymbol = findLabel(labelName, transUnit);
    /* If an error occurs while finding the label, return */
    if (getErrorStruct()->error != NO_ERROR)
        return;

    /* Encode the label of the operand using labelAddressEncoding function */
    labelAddressEncoding(transUnit, labelSymbol, labelName, curLine.lineNumber);

    /* If an error occurs during processing, returnr */
    if (getErrorStruct()->error != NO_ERROR)
        return;

    /* Set the addressing mode (SRC_ADRESSING or DEST_ADRESSING) in the first machine word of the instruction */
    setBits(firstInstWord, (srcOrDest) ? SRC_ADRESSING : DEST_ADRESSING, ADRESSING_METHODS_SIZE, DIRECT_ADDRESSING);
    return;
}
/*
 * Handles the encoding of an indexed label operand.
 *
 * Algorithm:
 * - Find the associated symbol with the indexed label of operand.
 * - If the symbol is of type dataS or externS, set an error.
 * - If the label is not external and index value exceeds array bounds, set an error.
 * - Encode the label using labelAddressEncoding.
 * - Encode the index value using numValueEncoding.
 * - Set the addressing mode in the first machine word of the instruction.
 *
 * Assumptions:
 * - 'transUnit' is a valid pointer to a translation unit structure.
 * - 'curLine'  is a valid pointer to an ast line structure.
 * -  'firstInstWord' is a valid pointer to a machineWord.
 */
static void handleIndexedOperand(machineWord *firstInstWord, translationUnit *transUnit, astLine curLine, index operandsIndex, bool srcOrDest)
{
    /* Pointer to the symbol associated with the indexed label of operand */
    symbol *labelSymbol = NULL;

    /* Name of the indexed label of the operand */
    char *operandLabel = curLine.ast->operands[operandsIndex].operand.label;

    /* Index value of the index of the operand */
    int idxValue = curLine.ast->operands[operandsIndex].operand.idx.idxNum;

    /* Find the symbol associated with the indexed operand */
    labelSymbol = findLabel(operandLabel, transUnit);

    /* If an error occurs while finding the label, return */
    if (getErrorStruct()->error != NO_ERROR)
        return;

    /* If the operand symbol is not of type dataS or externS, set an error */
    if (labelSymbol->typeOfSymbol != dataS && labelSymbol->typeOfSymbol != externS)
    {
        setErrorStruct(ILLEGAL_IDX_ADDRESING, operandLabel);
        return;
    }

    /* If the label is not external and index exceeds the bounds of the array, set an error and return */
    if (labelSymbol->typeOfSymbol != externS && labelSymbol->numOfValues <= idxValue)
    {
        setErrorStruct(ARRAY_INDEX_OUT_OF_BOUNDS, operandLabel);
        return;
    }

    /* Handle the encoding of the label of the operand using labelAddressEncoding function */
    labelAddressEncoding(transUnit, labelSymbol, operandLabel, curLine.lineNumber);

    /* If an error occurs during processing, return */
    if (getErrorStruct()->error != NO_ERROR)
        return;

    /* Handle the encoding of the index of the operand using numValueEncoding function */
    numValueEncoding(transUnit, idxValue);

    /* Set the addressing mode (SRC_ADRESSING or DEST_ADRESSING) in the first machine word of the instruction */
    setBits(firstInstWord, (srcOrDest) ? SRC_ADRESSING : DEST_ADRESSING, ADRESSING_METHODS_SIZE, IDX_ADDRESSING);

    /* Return */
    return;
}
/*
 * Handles the encoding of a register operand.
 *
 * Algorithm:
 * - Extract the register value from the AST.
 * - Retrieve the index of the current machine word.
 * - Decrement the machine word index if the last operand was a register.
 * - Set the register value in the current machine word.
 * - Set the addressing mode in the first machine word of the instruction.
 *
 * Assumptions:
 * - 'transUnit' is a valid pointer to a translation unit structure.
 * - 'curLine'  is a valid pointer to an ast line structure.
 * -  'firstInstWord' is a valid pointer to a machineWord.
 */
static void handleRegisterOperand(machineWord *firstInstWord, translationUnit *transUnit, astLine curLine, index operandsIndex, bool srcOrDest, bool lastOperandWasReg)
{
    /* Register value */
    int regValue = curLine.ast->operands[operandsIndex].operand.reg;

    /* Pointer to the current machine word */
    machineWord *currMachineWord;

    /* Get the current machine word index */
    index *machineWordIndex = getMachineWordIndex();

    /* If the last operand was a register operand, decrement the machine word index */
    if (lastOperandWasReg)
        (*machineWordIndex)--;

    /* Get the current machine word from the translation unit's machine word array */
    currMachineWord = transUnit->machWrdArr + DC + (*machineWordIndex)++;

    /* Set the register value in the current machine word */
    setBits(currMachineWord, (srcOrDest) ? SRC_REG : DEST_REG, REG_SIZE, regValue);

    /* Set the addressing mode (SRC_ADRESSING or DEST_ADRESSING) in the first machine word of the instruction */
    setBits(firstInstWord, (srcOrDest) ? SRC_ADRESSING : DEST_ADRESSING, ADRESSING_METHODS_SIZE, REGISTER_ADDRESSING);

    return;
}
/*
 * Handles different addressing cases for operands.
 *
 * Algorithm:
 * - Call the corresponding handling function for each operand type.
 * - Update the operand index.
 * - Reset the operand index if the current operand is destination operand or if found any errors.
 *
 * Assumptions:
 * - 'transUnit' is a valid pointer to a translation unit structure.
 * - 'curLine'  is a valid pointer to an ast line structure.
 */
static void addressingCases(astLine curLine, translationUnit *transUnit, machineWord *firstInstWord, bool srcOrDest, index operandsIndex)
{
    /* Boolean indicating whether the last operand was a register operand */
    static bool lastOperandWasReg = FALSE;

    /* Switch on the type of operand in the current AST line */
    switch (curLine.ast->operands[operandsIndex].type)
    {
    case immediate_const:
    case immediate_num:
        lastOperandWasReg = FALSE;
        handleImmediateOperand(firstInstWord, transUnit, curLine, operandsIndex, srcOrDest);
        break;

    case direct:
        lastOperandWasReg = FALSE;
        handleDirectOperand(firstInstWord, transUnit, curLine, operandsIndex, srcOrDest);
        break;

    case idx_const:
    case idx_num:
        lastOperandWasReg = FALSE;
        handleIndexedOperand(firstInstWord, transUnit, curLine, operandsIndex, srcOrDest);
        break;

    case reg:
        handleRegisterOperand(firstInstWord, transUnit, curLine, operandsIndex, srcOrDest, lastOperandWasReg);
        lastOperandWasReg = TRUE;
        break;
    }

    /* If the current operand is a destination operand or an error occurred, reset the index for the operands array */
    if (!srcOrDest || getErrorStruct()->error != NO_ERROR)
        lastOperandWasReg = FALSE;
    return;
}
static void lineTypeCases(astLine curLine, translationUnit *transUnit)
{
    /* Switch on the line type of the current line in the AST */
    switch (curLine.ast->lineType)
    {
    case operation:
        /* Call the processOperationLine function to handle the operation line */
        processOperationLine(curLine, transUnit);
        break;
    case directive:
        /* Call the processEntryLine function to handle directive line because all other directive line types were already handled by first pass */
        processEntryLine(curLine, transUnit);
        break;
    }
    return;
}
/*
 * Algorithm:
 * - Call the corresponding handling function for each operand type.
 * - Update the operand index.
 * - Reset the operand index if the current operand is the destination operand or if found any errors.
 *
 * Assumptions:
 * - 'transUnit' is a valid pointer to a translation unit structure.
 * - 'curLine'  is a valid pointer to an ast line structure.
 */
static opcodes getOpCode(astLine curLine)
{
    switch (curLine.ast->opcode)
    {
    case mov:
        return movOpcode;
    case cmp:
        return cmpOpcode;
    case add:
        return addOpcode;
    case sub:
        return subOpcode;
    case lea:
        return leaOpcode;
    case Not:
        return notOpcode;
    case clr:
        return clrOpcode;
    case inc:
        return incOpcode;
    case dec:
        return decOpcode;
    case jmp:
        return jmpOpcode;
    case bne:
        return bneOpcode;
    case red:
        return redOpcode;
    case prn:
        return prnOpcode;
    case jsr:
        return jsrOpcode;
    case rts:
        return rtsOpcode;
    case hlt:
        return hltOpcode;
    default:
        return EXIT_FAILURE;
    }
}

/*
 * Processes an operation line represented by ast.
 *
 * Algorithm:
 * Set the opcode in the first machine word.
 * Process each operand using the addressingCases function.
 *
 * Assumptions:
 * - 'transUnit' is a valid pointer to a translation unit structure.
 * - 'curLine'  is a valid pointer to an ast line structure.
 */
static void processOperationLine(astLine curLine, translationUnit *transUnit)
{
    /* Boolean indicating whether the current operand is a source or destination operand */
    bool srcOrDest = 0;

    /* Get the current machine word index */
    index *machineWordIndex = getMachineWordIndex();

    /* Get the first machine word of the instruction from the translation unit's machine word array */
    machineWord *firstInstWord = transUnit->machWrdArr + DC + (*machineWordIndex)++;

    index operandsIndex = 0;

    /* Set the opcode in the first machine word of the instruction */
    setBits(firstInstWord, OPCODE, OPCODE_SIZE, getOpCode(curLine));

    /* Loop through each operand */
    while (curLine.ast->numOfOperands--)
    {
        /* Determine whether the current operand is a source or destination operand */
        srcOrDest = (curLine.ast->numOfOperands == 0) ? DEST : SRC;

        /* Call the addressingCases function to handle the operand */
        addressingCases(curLine, transUnit, firstInstWord, srcOrDest, operandsIndex);

        /* If an error occurs during processing, return the error */
        if (getErrorStruct()->error != NO_ERROR)
            return;
        operandsIndex++;
    }
    return;
}

/*
 * Resizes the entry array in the translation unit.
 *
 * Algorithm:
 * - Check if the entry array size is not zero.
 * - Update the buffer size to entry arr index.
 * - Reallocate memory for the entry array.
 *
 * Assumptions:
 * - 'transUnit' is a valid pointer to a translation unit structure.
 */
void resizeEntryArr(translationUnit *transUnit)
{
    if (ENTRY_ARR_SIZE != 0)
    {
        /* Set the buffer size in the first pass AST buffer */
        ENTRY_ARR_SIZE = *(getEntryArrIndex());

        /* Reallocate memory for the entry array to save memory */
        reallocateMemory(sizeof(entryAppearance), ENTRY_ARR_SIZE, (void **)&(ENTRY_ARR));
    }
    return;
}
/*
 * Resizes the extern array in the translation unit.
 *
 * Algorithm:
 * - Check if the extern array size is not zero.
 * - Update the buffer size to extern arr index.
 * - Reallocate memory for the extern array.
 *
 * Assumptions:
 * - 'transUnit' point to valid structs.
 */
void resizeExternArr(translationUnit *transUnit)
{
    if (EXT_ARR_SIZE != 0)
    {
        /* Set the buffer size in the first pass AST buffer */
        EXT_ARR_SIZE = *(getExternArrIndex());

        /* Reallocate memory for the AST buffer to save memory */
        reallocateMemory(sizeof(externReference), EXT_ARR_SIZE, (void **)&(EXT_ARR));
    }
    return;
}
/*
 * Performs the second pass of assembly code processing.
 *
 * Algorithm:
 * - Iterate through the AST buffer, processing each line and handling errors.
 * - Resize the entry array and handle any memory allocation errors.
 * - Resize the extern array and handle any memory allocation errors.
 * - Free memory allocated during the first pass.
 *
 * Assumptions:
 * - 'fileName' is a valid pointer to a string representing the file name.
 * - 'firstPassBuffer' is a valid pointer to a first pass AST buffer structure.
 * - 'transUnit' is a valid pointer to a translation unit structure.
 */
bool secondPass(translationUnit *transUnit, FirstPassAstBuffer *firstPassBuffer, char *fileName)
{
    index astBufferIndex = 0;
    bool error = FALSE;

    /* Iterate through the AST buffer generated during the first pass */
    for (astBufferIndex = 0; astBufferIndex < firstPassBuffer->bufferSize; astBufferIndex++)
    {
        /* Initialize errorStruct to default */
        setErrorStruct(NO_ERROR, "");
        /* Process the line using lineTypeCases */
        lineTypeCases(firstPassBuffer->astBuffer[astBufferIndex], transUnit);

        /* If an error occurs during processing, handle the error */
        if (getErrorStruct()->error != NO_ERROR)
        {
            if (getErrorStruct()->error == MALLOC_FAILED)
            {
                freeFirstPassMemory(transUnit, firstPassBuffer);
                return TRUE;
            }
            handleError(getErrorStruct(), firstPassBuffer->astBuffer[astBufferIndex].lineNumber, fileName);
            error = TRUE;
        }
    }

    /* Resize the entry array */
    resizeEntryArr(transUnit);

    /* If memory allocation fails, free memory */
    if (getErrorStruct()->error == MALLOC_FAILED)
    {
        freeSecondPassMemory(firstPassBuffer, transUnit);
        return TRUE;
    }

    /* Resize the extern array */
    resizeExternArr(transUnit);

    /* If memory allocation fails, free memory */
    if (getErrorStruct()->error == MALLOC_FAILED)
    {
        freeSecondPassMemory(firstPassBuffer, transUnit);
        return TRUE;
    }
    /* Free memory allocated during the first pass */
    freeFirstPassMemory(transUnit, firstPassBuffer);
    /* reset all indexes back to zero */
    (*getEntryArrIndex()) = 0;
    (*getMachineWordIndex()) = 0;
    (*getExternArrIndex()) = 0;
    return error;
}