/*
 * This file is responsible for parsing a given line and creating an Abstact Syntax Tree (AST) representing it.
 * The functions in this file get a line passed thorugh the 'line' argumet of the 'createAst' function, allocate memory for a new AST,
 * tokenize the given line using the 'tokenize' function, parse it and fill the AST with data according to the information parsed from the line.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "error.h"
#include "ast.h"

#define dirOperand ast->dirStatement.operand /* shortcut for the directive statement operand */

/* private function prototypes */

/*
 * Parses a statement, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * This function parses a statement. It checks if a label is defined and then handles the label part of the statement.
 * After that, it parses the no label statement and returns the error encountered during parsing.
 *
 * Parameters:
 * token - Pointer to a pointer to the current token in the token array.
 * ast - Pointer to the 'astNode' structure to store the parsed data.
 *
 * Returns:
 * - An 'errorStruct' pointer indicating any errors encountered during parsing.
 */
static errorStruct *statement(Token *token, astNode *ast);

/*
 * Parses a statement without a label and stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * This function processes a statement without a label. It checks if the statement is a directive statement,
 * an operation statement, or a constant definition statement, and then parses it accordingly.
 * It returns an error if the statement is illegal.
 *
 * Parameters:
 * labelDefined - Flag indicating if a label was defined before the statement.
 * token - Pointer to a pointer to the current token in the token array.
 * ast - Pointer to the 'astNode' structure to store the parsed data.
 *
 * Returns:
 * - An 'errorStruct' pointer indicating any errors encountered during parsing.
 */
static errorStruct *noLabelStatement(int labelDefined, Token *token, astNode *ast);

/*
 * Parses a directive statement, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * This function parses a directive statement. It identifies the type of directive
 * statement (e.g., .data, .string, .entry, .extern) and calls the corresponding parsing function
 * based on the directive type. If the directive statement is recognized and parsed successfully,
 * it updates the ast with the parsed data and returns any errors encountered during parsing.
 *
 * Parameters:
 * labelDefined - Flag indicating whether a label is defined before the directive statement.
 *                It's used for error handling in case of meaningless label definitions.
 * token - Pointer to a pointer to the current token in the token array.
 * ast - Pointer to the 'astNode' structure to store the parsed data.
 *
 * Returns:
 * - An 'errorStruct' pointer indicating any errors encountered during parsing.
 */
static errorStruct *directiveStatement(int labelDefined, Token *token, astNode *ast);

/*
 * Parses a constant definition statement, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * This function parses a constant definition statement. It checks if the token is a .define directive and
 * then stores the constant definition data in the AST. It returns an error if the constant definition statement is illegal.
 *
 * Parameters:
 * token - Pointer to a pointer to the current token in the token array.
 * ast - Pointer to the 'astNode' structure to store the parsed data.
 *
 * Returns:
 * - An 'errorStruct' pointer indicating any errors encountered during parsing.
 */
static errorStruct *constDefStatement(Token *token, astNode *ast);

/*
 * Parses an operation statement, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * This function parses an operation statement. It checks if the token is an operation, its number of operands
 * and then parses the operands accordingly and enters thier data. It returns an error if the operation statement is illegal.
 *
 * Parameters:
 * token - Pointer to a pointer to the current token in the token array.
 * ast - Pointer to the 'astNode' structure to store the parsed data.
 *
 * Returns:
 * - An 'errorStruct' pointer indicating any errors encountered during parsing.
 */
static errorStruct *operationStatement(Token *token, astNode *ast);

/*
 * Parses an operation operand, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * This function parses an operation operand. It checks if the token is a register, a label,
 *  or a number sign and enters their data accordingly. It returns an error if the operand is illegal.
 *
 * Parameters:
 * token - Pointer to a pointer to the current token in the token array.
 * ast - Pointer to the 'astNode' structure to store the parsed data.
 * operandNum - The number of the operand to be processed.
 *
 * Returns:
 * - An 'errorStruct' pointer indicating any errors encountered during parsing.
 */
static errorStruct *operand(Token *token, astNode *ast, int operandNum);

/*
 * Parses the operand index, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * This function parses the index provided after the operand. It checks if the token is a label (constant) or an integer.
 * If the token is a label, it enters data for a constant index. If the token is an integer, it enters data for an integer index.
 * It returns an error if the indexing is illegal.
 *
 * Parameters:
 * token - Pointer to a pointer to the current token in the token array.
 * ast - Pointer to the 'astNode' structure to store the parsed data.
 * operandNum - The number of the operand to be indexed.
 *
 * Returns:
 * - An 'errorStruct' pointer indicating any errors encountered during parsing.
 */
static errorStruct *index(Token *token, astNode *ast, int operandNum);

/*
 * Parses the integer values provided after the .data directive, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * This function parses the integer values provided after the .data directive.
 * It iterates over each token after the DATA_DIR token, checking if it is either an integer or a constant.
 * It enteres the integers and the constant names into the astNode struct and returns and appropriate error,
 * if any occured, or NO_ERROR otherwise.
 *
 * Parameters:
 * token - Pointer to a pointer to the current token in the token array.
 * ast - Pointer to the 'astNode' structure to store the parsed data.
 *
 * Returns:
 * - An 'errorStruct' pointer indicating any errors encountered during parsing.
 */
static errorStruct *getIntData(Token *token, astNode *ast);

/*
 * Parses the string token after a .string directive, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * This function parses the string token provided after a .string directive.
 * It checks if the current token is of type STRING. If it is, the function proceeds to copy the string data
 * into the astNode structure provided. If the current token is not a string, it returns an appropriate error.
 *
 * Parameters:
 * token - Pointer to a pointer to the current token in the token array.
 * ast - Pointer to the 'astNode' structure to store the parsed data.
 *
 * Returns:
 * - An 'errorStruct' pointer indicating any errors encountered during parsing.
 */
static errorStruct *getStringData(Token *token, astNode *ast);

/*
 * Enters a string into the data array of the ast and appends it with a null character.
 *
 * This function copies the characters of the provided string into the data array of the `astNode` structure,
 * appending a null character at the end to terminate the string, without including the starting and ending \" characters.
 *
 * Parameters:
 * ast - Pointer to the AST structure to store the parsed data.
 * str - Pointer to the string containing the data to be copied.
 */
static void copyStringData(astNode *ast, char *str);

/*
 * Parses the .entry / .extern directive cases, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * This function handles the parsing of the .entry / .extern directive cases.
 * It updates the abstract syntax tree (ast) based on the provided token and whether a label is defined.
 * If a label is defined before the directive, a warning is issued.
 * It checks the tokens following tokens and returns an approriate error (or NO_ERROR if successful).
 *
 * Parameters:
 * token - Pointer to a pointer to a token struct representing the current token.
 * ast - Pointer to the AST structure to store the parsed data.
 * labelDefined - Integer indicating whether a label is defined before the directive (TRUE if defined, FALSE otherwise).
 *
 * Returns:
 * - An 'errorStruct' pointer indicating any errors encountered during parsing.
 */
static errorStruct *getEntryExtern(Token *token, astNode *ast, int labelDefined);

/*
 * Returns the current token if its type matches the specified type and advances to the next token.
 *
 * This function checks if the type of the current token matches the specified type. If they match,
 * and the current type is not TERMINATOR,the function advances to the next token and returns it,
 * if the type is TERMINATOR, it returns it without advancing. If the types do not match, it returns NULL.
 *
 * Parameters:
 * type - The expected type of the current token.
 * token - Pointer to the pointer to the current token.
 *
 * Returns:
 * - The current token if its type matches the specified type. Otherwise, returns NULL.
 */
static Token term(TokenType type, Token *token);

/*
 * Checks if a given token type is a legal starting type for an operand.
 *
 * This function checks if a given token type is a legal starting type for an operand. It returns TRUE if
 * the token type is NUMBER_SIGN, LABEL, or REGISTER. otherwise, it returns FALSE.
 *
 * Parameters:
 * type - The token type to be checked.
 *
 * Returns:
 * - TRUE if the token type is a legal starting type for an operand, and FALSE otherwise.
 */
int isLegalOperandStartType(TokenType type);

/* end of private function prototypes */

/*
 * Creates an abstract syntax tree (AST) for the given line.
 *
 * Algorithm:
 * - Tokenize the line using the Tokenize function.
 * - Intialize the ast and handle memory allocation failure.
 * - Parse the line and store all valuable information about the parsed line in the AST.
 * - If an error occurs during the parsing process, it handles the error and returns NULL.
 * - If no errors occured it return the ast.
 */
astNode *createAst(char *line, int lineNum, char *fileName)
{
    Token tokenArray; /* array of tokens */
    Token token;      /* pointer to the array of tokens */
    astNode *ast;     /* pointer to ast - storing all valuable information about the line parsed */

    setErrorStruct(NO_ERROR, "");

    /* tokenize and check if an error occured during the tokenization process */
    if (tokenize(line, &tokenArray, lineNum, fileName))
    {
        free(tokenArray);
        return NULL;
    }

    /* allocate memory for ast and initalize token */
    ast = (astNode *)calloc(ONE_UNIT, sizeof(astNode));
    token = tokenArray;

    /* check if memory allocation failed */
    if (ast == NULL)
    {
        free(tokenArray);
        setErrorStruct(MALLOC_FAILED, "");
        return NULL;
    }

    /* parse the line */
    statement(&token, ast);

    /* free memory allocated */
    free(tokenArray);

    /* check if an error occured during the parsing process and handle it */
    if (getErrorStruct()->error != NO_ERROR)
    {
        handleError(getErrorStruct(), lineNum, fileName);
        free(ast);
        return NULL;
    }
    return ast;
}

/*
 * Parses a statement, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * Algorithm:
 * - Check if a label is defined. If it is, check its length, check for colons after it,
 *  check for a space between the label and the colons and return an error if needed.
 * - Parse the no label statement and return an error if it is missing.
 * - If the no label statement was parsed successfuly and the current token is not a terminator, return an appropriate error.
 * - Return the error encountered during parsing.
 *
 * Assumptions:
 * - 'ast' is a valid pointer to an astNode structure.
 */
static errorStruct *statement(Token *token, astNode *ast)
{
    errorStruct *error; /* stores the error encountered during parsing and is returned for error further handling */
    Token t;            /* temporarily stores the token returned from term */
    Token tSpace;       /* stores the token just to check the value of the spaceBefore field */

    /* checking if a label is defined */
    if ((t = term(LABEL, token)))
    {
        /* cheking if label length is valid */
        if (strlen(t->name) > MAX_LABEL_LENGTH)
            return setAndGetErrorStruct(LABEL_TOO_LONG, t->name);

        /* checking for colons token after label token */
        else if (!(tSpace = term(COLONS, token)))
            return setAndGetErrorStruct(MISS_COLONS, "");

        /* checking for space before colons token */
        else if (tSpace->spaceBefore == TRUE)
            return setAndGetErrorStruct(SPACE_BEFORE_TOKEN, "colons");

        /* the label part of the statement was handled. Now the no label statement needs to be parsed. */
        /* save the error from the parsing of no label statement in error. If there ins't a no label statement, return error */
        else if (!(error = noLabelStatement(TRUE, token, ast)))
            return setAndGetErrorStruct(MISS_DIR_OP_STATEMENT, "after label definition");

        /* Copy the label name into ast */
        strcpy(ast->labelName, t->name);
    }
    /* no label was defined - Parse the no label statement */
    /* save the error from the parsing of no label statement in error. If there ins't a no label statement, return error */
    else if (!(error = noLabelStatement(FALSE, token, ast)))
        return setAndGetErrorStruct(MISS_DIR_OP_STATEMENT, "");

    /* check if token was parsed successfuly but the current token is not a terminator, i.e. there is externeous text after the end of the commnad. */
    if (!term(TERMINATOR, token) && error->error == NO_ERROR)
    {
        return setAndGetErrorStruct(EX_TEXT, "after end of command");
    }
    else /* return the error occured (or NO_ERROR if no error occured) */
        return error;
}

/*
 * Parses a statement without a label, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * Algorithm:
 * - Check if the statement is a directive statement. If it is, enter the data and return an error accordingly.
 * - Check if the statement is an operation statement. If it is, enter the data and return an error accordingly.
 * - Check if the statement is a constant definition statement. If it is, check if a label was defined before it, enter the data and return an error accordingly.
 * - Check if token is comma and return an error accordingly.
 * - Otherwise, the statement is not a statement without a label. Return NULL.
 *
 * Assumptions:
 * - 'ast' is a valid pointer to an astNode structure.
 */
static errorStruct *noLabelStatement(int labelDefined, Token *token, astNode *ast)
{
    errorStruct *error; /* stores the error encountered during parsing and is returned for error further handling */

    /* check if this no label statement is a directive statement and parse it*/
    if ((error = directiveStatement(labelDefined, token, ast)))
    {
        ast->lineType = directive;
        return error;
    }
    /* check if this no label statement is an operation statement and parse it */
    else if ((error = operationStatement(token, ast)))
    {
        ast->lineType = operation;
        return error;
    }
    /* check if this no label statement is a constant definition statement and parse it*/
    else if ((error = constDefStatement(token, ast)))
    {
        /* check if a label was declared before the directive statemnt */
        if (labelDefined)
            return setAndGetErrorStruct(LABEL_BEFORE_CONST_DEF, "");
        else
        {
            ast->lineType = directive;
            return error;
        }
    }
    else if (term(COMMA, token))
    {
        return setAndGetErrorStruct(ILLEGAL_COMMA, "");
    }
    else /* not a no label statement */
        return NULL;
}

/*
 * Parses a directive statement, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * Algorithm:
 * - Check if token is a .data directive. If it is, enter the data, parse the directive and return an error accordingly.
 * - Check if token is a .string directive. If it is, enter the data, parse the directive and return an error accordingly.
 * - Check if token is a .extern directive. If it is, enter the data, parse the directive and return an error accordingly.
 * - Check if token is a .entry directive. If it is, enter the data, parse the directive and return an error accordingly.
 * - Otherwise, this is not a directive statement. Return NULL.
 *
 * Assumptions:
 * - 'ast' is a valid pointer to an astNode structure.
 */
static errorStruct *directiveStatement(int labelDefined, Token *token, astNode *ast)
{
    if (term(DATA_DIR, token))
    {
        ast->dirStatement.type = data;
        return getIntData(token, ast);
    }
    else if (term(STRING_DIR, token))
    {
        ast->dirStatement.type = data;
        return getStringData(token, ast);
    }
    else if (term(EXTERN_DIR, token))
    {
        ast->dirStatement.type = external;
        return getEntryExtern(token, ast, labelDefined);
    }
    else if (term(ENTRY_DIR, token))
    {
        ast->dirStatement.type = entry;
        return getEntryExtern(token, ast, labelDefined);
    }
    /* not a directive statement */
    return NULL;
}

/*
 * Parses a constant definition statement, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * Algorithm:
 * - Checks if the token is a .define directive.
 *  - If it is, check if a label is missing and return an appropriate error. If there is a label, enter its data.
 *  -  Check the label length and return an error if needed.
 *  - Check if an equal sign is missing and return an error if needed.
 *  - Check if an integer is missing and return appropriate error.
 *   If there is an integer, everything is correct - enter the data to the ast.
 * - Otherwise, this is not a constant definition statement. Return NULL.
 *
 * Assumptions:
 * - 'ast' is a valid pointer to an astNode structure.
 * - 'token' is a valid pointer to the current token in the token array.
 */
static errorStruct *constDefStatement(Token *token, astNode *ast)
{
    Token t; /* temporarily stores the token returned from term */

    /* check if token is a .define directive */
    if (term(DEF_DIR, token))
    {
        /* store the const def data in the ast */
        ast->dirStatement.type = const_def;

        if (!(t = term(LABEL, token)))
        {
            if (term(TERMINATOR, token) || term(EQUAL_SIGN, token))
                return setAndGetErrorStruct(MISS_CONST, "");
            else
                return setAndGetErrorStruct(ILLEGAL_CONST, (*token)->name);
        }
        else
        {
            /* store the label (constant) data in the ast */
            strcpy(ast->labelName, t->name);
        }

        /* return an error if label (constant) name is too long */
        if (strlen(t->name) > MAX_LABEL_LENGTH)
            return setAndGetErrorStruct(CONST_TOO_LONG, t->name);

        /* check if token is not an equal sign */
        if (!term(EQUAL_SIGN, token))
        {
            /* check for an integer token or no tokens after constant */
            if (term(TERMINATOR, token) || term(INTEGER, token))
                return setAndGetErrorStruct(MISS_EQUAL, "");
            else /* illegal token after constant */
                return setAndGetErrorStruct(ILLEGAL_CONST, "");
        }
        /* check if token is not an integer */
        if (!(t = term(INTEGER, token)))
        {
            /* check if no tokens were provided after eqaul sign */
            if (term(TERMINATOR, token))
                return setAndGetErrorStruct(MISS_INT_VAL, "");
            else /* illegal token after equal sign */
                return setAndGetErrorStruct(EXPC_INT, (*token)->name);
        }

        /* everything is correct, no errors encountered */

        /* store the integer data in the ast */
        ast->dirStatement.operand.data[0].value.num = atoi(t->name);
        ast->dirStatement.operand.dataCount = 1;
        ast->dirStatement.operand.data[0].type = num;

        return setAndGetErrorStruct(NO_ERROR, "");
    }

    /* not a constant definition directive */
    return NULL;
}

/*
 * Parses an operation statement, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * Algorithm:
 * - Check if the token is an operation.
 *  - If it is, check for the number of operands.
 *  - In case of two operands, parse the first one. If an error was encountered, return it.
 *   Check for a comma between operands and return appropriate error if any. Parse the second operand and return an error accordingly.
 *  - In case of one operand, parse it and return an error accordingly.
 *  - In case of zero operands, return NO_ERROR.
 * - Otherwise, this is not an operation statement. Return NULL.
 *
 * Assumptions:
 * - 'ast' is a valid pointer to an astNode structure.
 * - 'token' is a valid pointer to the current token in the token array.
 */
static errorStruct *operationStatement(Token *token, astNode *ast)
{
    errorStruct *error; /* stores the error encountered during parsing and is returned for error further handling */

    /* check if token is an operation */
    if (MOV <= (*token)->type && (*token)->type <= HLT)
    {
        ast->opcode = (*token)->type; /* the values of opcode in both enums are identical, so we can assign one enum to the other */
        (*token)++;

        /* check if token is an operation with 2 operands */
        if (mov <= ast->opcode && ast->opcode <= lea)
        {
            ast->numOfOperands = TWO_OPERANDS;

            /* parse the first operand, update error accordingly and return it only if an error was encountered */
            if ((error = operand(token, ast, FIRST_OPERAND))->error != NO_ERROR)
                return error;

            /* check if token is not a comma */
            if (!term(COMMA, token))
            {
                /* check for a legal token, or no tokens after the operand */
                if (term(TERMINATOR, token) || isLegalOperandStartType((*token)->type))
                    return setAndGetErrorStruct(MISS_COMMA, "between operands");
                else /* token after operand is illegal */
                    return setAndGetErrorStruct(ILLEGAL_OPRD, "First");
            }
            else /* token is a comma - parse the second operand and update error accordingly */
                return operand(token, ast, SECOND_OPERAND);
        }
        /* check if token is an operation with 1 operand*/
        else if (Not <= ast->opcode && ast->opcode <= jsr)
        {
            ast->numOfOperands = ONE_OPERAND;
            /* parse the operand and update error accordingly */
            return operand(token, ast, FIRST_OPERAND);
        }
        else /* token is an operation with 0 operands */
        {
            ast->numOfOperands = NO_OPERANDS;
            return setAndGetErrorStruct(NO_ERROR, "");
        }
    }
    /* not an operation statement */
    return NULL;
}

/*
 * Parses an operation operand, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * Algorithm:
 * - If the token is a register, store the register data in the AST.
 * - If the token is a label, store the label data in the AST and check for an index after the label.
 * - If the token is a number sign, store the immediate number or constant data in the AST, or return an error if an illegal token was provided.
 * - Otherwise, check if the token is a terminator and return appropriate errors.
 *
 * Assumptions:
 * - 'ast' is a valid pointer to an astNode structure.
 * - 'token' is a valid pointer to the current token in the token array.
 */
static errorStruct *operand(Token *token, astNode *ast, int operandNum)
{
    Token t;      /* temporarily stores the token returned from term */
    Token tSpace; /* stores the token just to check the value of the spaceBefore field */

    /* check if token is a register */
    if ((t = term(REGISTER, token)))
    {
        /* store the register data in the ast */
        ast->operands[operandNum].type = reg;
        ast->operands[operandNum].operand.reg = t->name[1] - '0'; /* calculating the number of register */
    }
    /* check if the token is a label - the operand can be just label or a label with an index */
    else if ((t = term(LABEL, token)))
    {
        /* storing the label data in the ast */
        ast->operands[operandNum].type = direct;
        strcpy(ast->operands[operandNum].operand.label, t->name);

        /* check for an index after the label */
        if ((tSpace = term(LEFT_SQ_BRACKET, token)))
        {
            /* repornt an error in case there is a space between the label the left bracket */
            if (tSpace->spaceBefore == TRUE)
                return setAndGetErrorStruct(SPACE_BEFORE_TOKEN, "left square bracket");

            /* parse the index and update error accordingly */
            return index(token, ast, operandNum);
        }
    }
    /* check if token is a number sign */
    else if (term(NUMBER_SIGN, token))
    {
        if ((t = term(INTEGER, token)))
        {
            /* store the immediate number data in the ast */
            ast->operands[operandNum].type = immediate_num;
            ast->operands[operandNum].operand.num = atoi(t->name);

            /* repornt an error in case there is a space between the number sign and the integer */
            if (t->spaceBefore == TRUE)
                return setAndGetErrorStruct(SPACE_BEFORE_TOKEN, "integer (after the '#' character)");
        }
        else if ((t = term(LABEL, token)))
        {
            /* store the immediate constant data in the ast */
            ast->operands[operandNum].type = immediate_const;
            strcpy(ast->operands[operandNum].operand.label, t->name);

            /* repornt an error in case there is a space between the number sign and the constant */
            if (t->spaceBefore == TRUE)
                return setAndGetErrorStruct(SPACE_BEFORE_TOKEN, "constant (after the '#' character)");
        }
        else /* illegal token after number sign */
        {
            return setAndGetErrorStruct(EXPC_INT_OR_CONST, (*token)->name);
        }
    }
    else /* operand is not a register, a label or a number sign */
    {
        /* check if no tokens were provided */
        if (term(TERMINATOR, token))
        {
            return setAndGetErrorStruct(MISS_OPRD, "");
        }
        else if (term(COMMA, token))
        {
            return setAndGetErrorStruct(ILLEGAL_COMMA, "");
        }
        else /* an illegal token was provided */
        {
            return setAndGetErrorStruct(ILLEGAL_OPRD, (operandNum == FIRST_OPERAND) ? "First" : "Second");
        }
    }

    return setAndGetErrorStruct(NO_ERROR, "");
}

/*
 * Parses the operand index, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * Algorithm:
 * - If the token is a label (constant) or an integer, set the boolean to TRUE, indicating a legal token was encountered.
 *  - If the token is a label, enter data for a constant index.
 *  - If the token is an integer, enter data for an integer index.
 * - If the next token is a right bracket, check if a legal token was encountered and return appropriate error.
 * - If the next token is a terminator, check if a legal token was encountered and return appropriate error.
 * - Otherwise, check if a legal token was encountered and return appropriate error.
 *
 * Assumptions:
 * - 'token' is a valid pointer to the current token in the token array.
 * - 'ast' is a valid pointer to an astNode structure.
 */
static errorStruct *index(Token *token, astNode *ast, int operandNum)
{
    Token t;                          /* temporarily stores the token returned from term */
    errorStruct *error;               /* stores the error encountered during parsing and is returned for error further handling */
    int firstIndexTokenLegal = FALSE; /* stores if the first token of the index is a legal token for an index */

    /* if the token is a label(constant) or an integer */
    if ((t = term(LABEL, token)) || (t = term(INTEGER, token)))
    {
        firstIndexTokenLegal = TRUE;
        /* check for the type of the token */
        if (t->type == LABEL)
        {
            /* enter data for constant index */
            ast->operands[operandNum].type = idx_const;
            strcpy(ast->operands[operandNum].operand.idx.idxLabel, t->name);
        }
        else /* t is of type INTEGER*/
        {
            /* enter data for integer index */
            ast->operands[operandNum].type = idx_num;
            ast->operands[operandNum].operand.idx.idxNum = atoi(t->name);
        }
    }

    if (term(RIGHT_SQ_BRACKET, token))
    {
        if (firstIndexTokenLegal)
            return setAndGetErrorStruct(NO_ERROR, "");
        else /* no token was provided between the left and right brackets - decrease the token to return to the indexed label token (skipping the right and the left bracket) */
            return setAndGetErrorStruct(MISS_IDX, ((*token) - DECREASE_TOKEN_LEGAL_INDEX)->name);
    }
    else if (term(TERMINATOR, token))
    {
        if (firstIndexTokenLegal)
            return setAndGetErrorStruct(MISS_RIGHT_BRACKET, "");
        else /* no token provided after right bracket - decrease the token to return to the indexed label token (skipping the terminator and the left bracket)*/
            return setAndGetErrorStruct(MISS_IDX, ((*token) - DECREASE_TOKEN_EMPTY_INDEX)->name);
    }
    else /* next token is an illegal token for an index, or an extreneous token after a legal index */
    {
        if (firstIndexTokenLegal)
            /* decrease the token to return to the indexed label token (skipping the current token, the legal token and the left bracket) */
            *token -= DECREASE_TOKEN_LEGAL_INDEX;
        else
            /* decrease the token to return to the indexed label token (skipping the current token and the left bracket) */
            *token -= DECREASE_TOKEN_EMPTY_INDEX;
        error = setAndGetErrorStruct(ILLEGAL_IDX, (*token)->name);
        return error;
    }
}

/*
 * Parses the integer values provided after the .data directive, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * Algorithm:
 * - Iterate over each token in the token stream.
 * - Check if the token is either an integer or a constant.
 * - If an integer is found, convert it to an integer value and enter it into the ast.
 * - If a constant is found, enter it into the ast structure as well.
 * - Ensure that each integer or constant value is followed by a comma. If not, return an error indicating
 *    a missing comma.
 * - Handle cases where the data list ends with a comma or contains no values.
 *
 * Assumptions:
 * - 'token' is a valid pointer to the current token in the token array.
 * - 'ast' is a valid pointer to an astNode structure.
 */
static errorStruct *getIntData(Token *token, astNode *ast)
{
    Token t; /* temporarily stores the token returned from term */

    /* checking for integers or constants in the list of values */
    for (dirOperand.dataCount = 0; (t = term(INTEGER, token)) || (t = term(LABEL, token)); dirOperand.dataCount++)
    {
        /* enter the value into ast */
        if (t->type == INTEGER)
        {
            dirOperand.data[dirOperand.dataCount].type = num;
            dirOperand.data[dirOperand.dataCount].value.num = atoi(t->name);
        }
        else
        {
            dirOperand.data[dirOperand.dataCount].type = constant;
            strcpy(dirOperand.data[dirOperand.dataCount].value.constant, t->name);
        }
        /* if int or const was found, but no comma after it*/
        if (!term(COMMA, token))
        {
            if (term(TERMINATOR, token))
            { /* data is correct, return no error */
                /* dataCount was not incremented yet */
                dirOperand.dataCount++;
                return setAndGetErrorStruct(NO_ERROR, "");
            }
            /* if token is another int or const, a comma between them is missing*/
            else if (term(INTEGER, token) || term(LABEL, token))
            {
                return setAndGetErrorStruct(MISS_COMMA, "after value for .data directive");
            }
            /* if token is of different type, it's ex text */
            else
            {
                return setAndGetErrorStruct(EX_TEXT, "after end of command");
            }
        }
    }

    /* error handling if data ended with a comma or empty data */
    /* if data is empty  */
    if (dirOperand.dataCount == 0 && term(TERMINATOR, token))
    {
        return setAndGetErrorStruct(MISS_DATA_VALS, "");
    }
    /* the currect token after the comma is not an int or const
    (becuase of loop condition, if token was int or const the loop would continue) */
    else
    {
        if (term(COMMA, token))
            return setAndGetErrorStruct(MUL_CON_COMMA, "");
        else if (term(TERMINATOR, token))
            return setAndGetErrorStruct(EX_COMMA, "");
        else
            return setAndGetErrorStruct(EXPC_INT_OR_CONST, (*token)->name);
    }
}

/*
 * Parses the string token after a .string directive, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * Algorithm:
 * - Check if the current token is of type STRING.
 * - If it is, copy the string data into the ast.
 * - If the current token is not a string, return an error indicating an expected string token.
 * - If the end of the line is reached without encountering a string token, return an error indicating
 *    a missing string after the directive.
 *
 * Assumptions:
 * - 'token' is a valid pointer to the current token in the token array.
 * - 'ast' is a valid pointer to an astNode structure.
 */

static errorStruct *getStringData(Token *token, astNode *ast)
{
    Token t; /* temporarily stores the token returned from term */

    /* if the currect token is not a string */
    if (!(t = term(STRING, token)))
    {
        /* if it's the end of the line*/
        if (term(TERMINATOR, token))
            return setAndGetErrorStruct(MISS_STRING, "");
        /* if there is another token instead of a string */
        else
            return setAndGetErrorStruct(EXPC_STRING, (*token)->name);
    }
    else
    {
        copyStringData(ast, t->name);
        return setAndGetErrorStruct(NO_ERROR, "");
    }
}

/*
 * Enters a string into the data array of the ast and appends it with a null character.
 *
 * Algorithm:
 * - Iterate over each character of the string, starting from the second character (skipping the opening '\"').
 * - Assign each character to the corresponding data cell in the astNode structure.
 * - Terminate the string by setting the last data cell to '\0'.
 *
 * Assumptions:
 * - 'ast' is a valid pointer to an astNode structure.
 * - 'str' points to a valid string.
 */
static void copyStringData(astNode *ast, char *str)
{
    /* always checking and assigning from the next cell (dataCount + 1) to avoid the opening \" character */
    for (dirOperand.dataCount = 0; dirOperand.dataCount < MAX_DATA_SIZE && str[dirOperand.dataCount + 1]; dirOperand.dataCount++)
    {
        /* store the character in the corresponding data cell in ast*/
        dirOperand.data[dirOperand.dataCount].type = num;
        dirOperand.data[dirOperand.dataCount].value.num = str[dirOperand.dataCount + 1];
    }
    /* setting the last data cell to '\0' to terminate the string instead of \" */
    dirOperand.data[dirOperand.dataCount - 1].value.num = 0;
}

/*
 * Parses the .entry / .extern directive cases, stores the data in the abstract syntax tree (AST), and returns an errorStruct accordingly.
 *
 * Algorithm:
 * - Check if a label is provided after the directive.
 *  - If not, check if the directive is followed by a terminator, return an error indicating a missing label parameter.
 *  - Otherwise, return an error indicating an expected label parameter with the name of the encountered token.
 * - Check if a label is defined before the directive. If so, generate a warning.
 * - If no error occured, return NO_ERROR;
 *
 * Assumptions:
 * - 'token' is a valid pointer to the current token in the token array.
 * - 'ast' is a valid pointer to an astNode structure.
 */
static errorStruct *getEntryExtern(Token *token, astNode *ast, int labelDefined)
{
    Token t; /* temporarily stores the token returned from term */

    /* check if a label wasn't defined after the entry/extern directive */
    if (!(t = term(LABEL, token)))
    {
        if (term(TERMINATOR, token))
            return setAndGetErrorStruct(MISS_LABEL_PARAM, "");
        else /* the current token is an unexpected for .entry/.extern directive */
            return setAndGetErrorStruct(EXPC_LABEL_PARAM, (*token)->name);
    }
    else if (labelDefined)
    {
        return setAndGetErrorStruct(MEANINGLESS_LABEL_DEF, "");
    }
    else
    {
        /* store the data in the ast */
        strcpy(dirOperand.label, t->name);
        return setAndGetErrorStruct(NO_ERROR, "");
    }
}

/*
 * Returns the current token if its type matches the specified type and advances to the next token.
 *
 * Algorithm:
 * - Check if the type of the current token matches the specified type.
 * - If the types match:
 *    - If the current token is not of type TERMINATOR, advance to the next token and return it.
 *    - If the current token is of type TERMINATOR, return the current token without advancing.
 * - If the types do not match, return NULL.
 *
 * Assumptions:
 * - 'token' is a valid pointer to the current token in the token array.
 */
static Token term(TokenType type, Token *token)
{
    /* check if the token type matches the specified type */
    if ((*token)->type == type)
    {
        /* advance token only if it's not terminator */
        if ((*token)->type != TERMINATOR)
            return (*token)++;
        return (*token);
    }
    return NULL;
}

/*
 * Checks if a given token type is a legal starting type for an operand.
 *
 * Algorithm:
 * - Return TRUE if the token type is NUMBER_SIGN, LABEL, or REGISTER. Otherwise, return FALSE.
 */
int isLegalOperandStartType(TokenType type)
{
    return type == NUMBER_SIGN || type == LABEL || type == REGISTER;
}
