#ifndef SECOND_PASS_H
#define SECOND_PASS_H 1

#include "first_pass.h"

/*src or dest constants*/
#define DEST 0 /* */
#define SRC 1

/*ARE constants*/
#define ABSOLUTE 0    /* A */
#define EXTERNAL 1    /* E */
#define RELOCATABLE 2 /* R */

/*operation machine words constants*/
#define FIRST_OPERAND 0 /* */

#define ARE 0                    /* ARE bits starting point in machine word */
#define ARE_SIZE 2               /* ARE size in machine word */
#define DEST_ADRESSING 2         /* destination adderssing methond bits starting point in machine word */
#define SRC_ADRESSING 4          /* source adderssing methond bits starting point in machine word */
#define ADRESSING_METHODS_SIZE 2 /* size of both addressing methods in machine words */
#define OPCODE 6                 /* opcode bits starting point in machine word */
#define OPCODE_SIZE 4            /* opcode size in machine word */
#define NOT_USED 10              /* not used bits bits starting point  in machine word */

/*operation immdiate machine words constants */
#define VALUE 2                              /* value bits starting point in machine word */
#define VALUE_SIZE MACHINE_WORD_SIZE - VALUE /* value size in machine word */

/*operation direct machine words constants */
#define ADRESS 2                               /* address bits starting point in machine word */
#define ADRESS_SIZE MACHINE_WORD_SIZE - ADRESS /* address size in machine word */

/*operation register machine words constants*/
#define REG_SIZE 3            /* size of register in machine word */
#define DEST_REG 2            /* destination register operand bits starting point in united machine word for two registers */
#define SRC_REG 5             /* source register operand bits starting point in united machine word for two registers */
#define NOT_IN_USE_REGISTER 8 /* unused bits starting point in united machine word for two registers */

/* enum representing the different operations in the assembly language, ordered by opcode values */
typedef enum
{
    movOpcode = 0,
    cmpOpcode,
    addOpcode,
    subOpcode,
    notOpcode,
    clrOpcode,
    leaOpcode,
    incOpcode,
    decOpcode,
    jmpOpcode,
    bneOpcode,
    redOpcode,
    prnOpcode,
    jsrOpcode,
    rtsOpcode,
    hltOpcode
} opcodes;

/*
 * Performs the second pass of assembly code processing.
 *
 * This function performs the second pass of assembly code processing by iterating through the AST buffer
 * generated during the first pass. It calls lineTypeCases to process each line, handles errors if encountered,
 * resizes the entry and extern arrays, and frees memory allocated during the first pass(symbol table and first pass buffer).
 *
 * Parameters:
 * - transUnit: Pointer to the translation unit.
 * - firstPassBuffer: The first pass AST buffer.
 * - fileName: The name of the assembly file.
 *
 * Returns:
 * - TRUE if there was an error encountered during the second pass, FALSE otherwise.
 */
bool secondPass(translationUnit *transUnit, FirstPassAstBuffer *firstPassBuffer, char *fileName);

#endif