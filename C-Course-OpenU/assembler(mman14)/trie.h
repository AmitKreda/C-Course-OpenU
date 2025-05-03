#ifndef TRIE_H
#define TRIE_H 1

/*standard libraries*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*defines*/
#define TRUE 1
#define FALSE 0
#define TRIE_BASE_CHAR ' ' /*the starting charter of the trie*/
#define ALPHABET_SIZE 95   /*how much sons each trieNode has*/

typedef unsigned short int bool;

typedef struct TrieNode
{
    void *endOfWord;                      /*pointer to the end of the word value, if endOfWord is NULL there is no word in the trie that end in this trieNode*/
    struct TrieNode *sons[ALPHABET_SIZE]; /*sons of the current trie node*/
} TrieNode;

/*Function prototypes */

/*
 * Creates a new empty trie data structure.
 *
 * This function creates a new empty trie data structure by allocating memory for the root node and initializing
 * it appropriately.
 *
 * Returns:
 * - Pointer to the root node of the newly created trie.
 */
TrieNode *createTrie();
/*
 * Deletes a trie data structure.
 *
 * This function deletes a trie data structure represented by the TrieNode hierarchy starting from the specified
 * father node. It recursively deallocates memory for each node in the trie, including the father node itself.
 *
 * Parameters:
 * father - Pointer to the node from which the deletion begins.
 */
void deleteTrie(TrieNode *father);
/*
 * Searches for a string in a trie data structure.
 *
 * This function searches for a string in a trie data structure represented by the TrieNode. It iterates
 * through each character in the string, traversing the trie nodes accordingly. If the string is found in
 * the trie and the end of word ponter of the node associated with the end of the string is not NULL,
 * it returns a pointer to the node associated with the end of the string; otherwise, it returns NULL.
 *
 * Parameters:
 * father - Pointer to the node from which the search begins.
 * string - The string to search for in the trie.
 *
 * Returns:
 * - Pointer to the node associated with the end of the string if found.
 * - NULL if the string is not found in the trie or if the input string is NULL.
 */
TrieNode *search(TrieNode *father, char *string);
/*
 * Inserts a string into a trie data structure.
 *
 * This function inserts a string into a trie data structure represented by the TrieNode. It iterates through
 * each character in the string, creating new nodes as necessary, and finally associates the end of the string with a
 * specified node in the trie. If memory allocation fails during node creation, it returns TRUE; otherwise, it returns
 * FALSE upon successful insertion.
 * if a string is inserted twice, the new string's end of word overrides the old string's end of word.
 *
 * Parameters:
 * string - The string to be inserted into the trie.
 * associatedEndOfWord - Pointer to the node associated with the end of the string.
 * Father - Pointer to the parent node in the trie where insertion begins.
 *
 * Returns:
 * - TRUE if memory allocation fails during node creation.
 * - FALSE upon successful insertion.
 */
bool insertString(char *string, void *associatedEndOfWord, TrieNode *Father);

#endif