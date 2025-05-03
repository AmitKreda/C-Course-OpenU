#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H

#define IN_MCR 1  /* flag indicating that the file pointer is currently in macro definition */
#define OUT_MCR 0 /* flag indicating that the file pointer is currently out of macro definition */

#define ONE_BIT_FOR_NULL_CHAR 1 /* an extra one bit of memory allocation for the '\0' NULL character */

/*
 * Pre-processes the assembly file before assembling: removes empty lines, comments, and expands macros.
 *
 * This function pre-processes the assembly file by reading the file with postfix ".as",
 * processing it (removing empty lines, comments, and expanding macros), and writing the
 * processed content into a new file with the postfix changed to ".am".
 * It also handles errors that occur during the pre-assembly process.
 *
 * Parameters:
 * fileName - Pointer to the name of the assembly file.
 *
 * Returns:
 * - EXIT_SUCCESS if the pre-assembly process is successful.
 * - EXIT_FAILURE if any error occurs during the pre-assembly process.
 */
int preAssembler(char *fileName);

#endif