#ifndef AST_H
#define AST_H

#include "error.h"

#define MAX_DATA_SIZE 70 /* this is the maximum amount a data type directive (.data or .string) can hold. */
#define MAX_OPERANDS 2   /* this is the maximum amount of operands an operation can take */

#define DECREASE_TOKEN_EMPTY_INDEX 2 /* the number of tokens we need to go back in order to get to the indexed label, if the index is empty */
#define DECREASE_TOKEN_LEGAL_INDEX 3 /* the number of tokens we need to go back in order to get to the indexed label, if the index contains a first legal token */

#define TWO_OPERANDS 2 /* the operation takes two operands */
#define ONE_OPERAND 1  /* the operation takes one operand */
#define NO_OPERANDS 0  /* the operation take no operands */

#define FIRST_OPERAND 0  /* index of the first operand */
#define SECOND_OPERAND 1 /* index of the second operand */

/*
 * This stucture represents an operation/directive line in the assembly language, it contains all information
 * about the type of operation/directive, the label defined in the line, and the operands/data provided for
 * the opration/directive.
 */
typedef struct ast_node
{
    char labelName[MAX_LABEL_LENGTH]; /* name of label/constant defined */
    enum
    {
        operation, /* Type of line: operation */
        directive  /* Type of line: directive */
    } lineType;    /* Type of line - operation/directive */

    struct
    {
        enum
        {
            entry,    /* .entry directive */
            external, /* .extern directive */
            data,     /* .data or .string directive */
            const_def /* .define directive */
        } type;       /* Type of directive */
        struct
        {
            char label[MAX_LABEL_LENGTH]; /* Label name provided for entry/extern. */
            struct
            {
                enum
                {
                    constant, /* Type of data: constant name */
                    num       /* Type of data: integer */
                } type;       /* Type of data */
                union
                {
                    char constant[MAX_LABEL_LENGTH]; /* Constant name */
                    int num;                         /* Integer value */
                } value;                             /* Value of the data */
            } data[MAX_DATA_SIZE];                   /* Array storing the data of the directive statement */
            int dataCount;                           /* Amount of values in data */
        } operand;                                   /* Operand containing all the operands/data given in the directive statement */
    } dirStatement;                                  /* Structure containing information about a directive statement */

    enum
    {
        mov, /* the values of these constants match the values of opcodes in the enum TokenType */
        cmp,
        add,
        sub,
        lea,
        Not,
        clr,
        inc,
        dec,
        jmp,
        bne,
        red,
        prn,
        jsr,
        rts,
        hlt
    } opcode; /* Represents the different operations in the assembly language */
    struct
    {
        enum
        {
            immediate_num,   /* Addressing method: immediate with a number */
            immediate_const, /* Addressing method: immediate with a constant */
            direct,          /* Addressing method: direct addressing */
            idx_num,         /* Addressing method: indexed with a number */
            idx_const,       /* Addressing method: indexed with a constant */
            reg              /* Addressing method: register */
        } type;              /* Addressing method of the operand */
        struct
        {
            char label[MAX_LABEL_LENGTH]; /* Label */
            union
            {
                char idxLabel[MAX_LABEL_LENGTH]; /* index constant */
                int idxNum;                      /* Index number */
            } idx;                               /* Index */
            int num;                             /* Integer */
            int reg;                             /* Register */
        } operand;                               /* Data for the operand */
    } operands[MAX_OPERANDS];                    /* Array of operands provided for the operation */

    int numOfOperands;

} astNode;

/*
 * Creates an abstract syntax tree (AST) for the given line.
 *
 * This function creates an abstract syntax tree (AST) for the given line. It tokenizes the line,
 * parses it, and stores all valuable information about the parsed line in the AST. If an error
 * occurs during the parsing process, it handles the error and returns NULL.
 *
 * Parameters:
 * line - The line to be parsed.
 * lineNum - The current line number.
 * fileName - The name of the file being parsed.
 *
 * Returns:
 * - Pointer to the 'astNode' structure containing all the parsed data.
 */
astNode *createAst(char *line, int lineNum, char *fileName);

#endif