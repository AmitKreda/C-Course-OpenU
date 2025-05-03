#ifndef _BACKEND_H
#define _BACKEND_H

#include "first_pass.h"

#define MAX_THREE_DIGIT_NUMBER 999     /* the maximum number with three digits */
#define MASK_OF_TWO_BITS 3             /* in binary this translates to: 0000 0000 0011 which is a mask of two bits */
#define NUM_BINARY_DIGITS_TO_CONVERT 2 /* this is the amount of binary digits to convert into one symbol in the encrypted base 4 */

#define AM_POSTFIX_LENGTH 2 /* the length of the postfix 'am' */
#define OB_POSTFIX_LENGTH 2 /* the length of the posfix 'ob' */
#define EXT_POSTIX_LENGTH 3 /* the length of the posfix 'ext' */

/*
 * Creates the three output files .ob .ext .ent with their data.
 *
 * This function creates the three output files .ob .ext .ent, which contain an encrypted base 4
 * representation of the machine code image, and lists of extern and entry symbols together with thier addresses.
 *
 * Parameters:
 * transUnit - Pointer to a translationUnit containing all the data from the first and second passes.
 * fileName - Pointer to the name of the object file.
 */
void createOutputFiles(translationUnit *transUnit, char *fileName);

#endif