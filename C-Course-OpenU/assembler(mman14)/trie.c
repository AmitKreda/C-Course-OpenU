/*
 * Trie data structure implementation.
 *
 * This file provides functions to create, delete, search, and insert strings into a Trie data structure.
 * The Trie structure is used for efficient string storage and retrieval operations.
 */

#include "trie.h"

/*
 * Creates a new Trie node.
 *
 * Algorithm:
 * - Allocates memory for a new TrieNode structure initializes and returns it.
 *
 */
TrieNode *createTrie()
{
    return (TrieNode *)calloc(1, sizeof(TrieNode));
}

/*
 * Deletes a Trie node and all its descendants.
 *
 * Algorithm:
 * - If the father node is NULL, return.
 * - Iterate through all the sons of the current trieNode and Recursively call deleteTrie to delete all of them.
 * - Free memory allocated for the Trie node's endOfWord if it's not NULL.
 * - Free memory allocated for the current Trie node.
 */
void deleteTrie(TrieNode *father)
{
    int currChar; /* Iterator for characters */

    /* Base case: If the father node is NULL, return */
    if (father == NULL)
        return;

    /* Recursively delete all child nodes */
    for (currChar = 0; currChar < ALPHABET_SIZE; currChar++)
    {
        deleteTrie(father->sons[currChar]);
    }

    /* Free memory allocated for endOfWord if it's not NULL */
    if (father->endOfWord != NULL)
    {
        free(father->endOfWord);
    }

    /* Free memory allocated for the current node */
    free(father);

    /* Exit the function */
    return;
}

/*
 * Searches for a string in the Trie.
 *
 * Algorithm:
 * - Iterate through each character in the string until the end or until a NULL trieNode pointer is encountered.
 * - If the current character's node exists in the Trie, move to the next node corresponding to the current character.
 * - otherwise, if the string is not in the Trie, return NULL.
 * - Return the endOfWord node if the end of the string is reached and it exists, otherwise return NULL.
 */
TrieNode *search(TrieNode *father, char *string)
{
    TrieNode *temp = father; /* Initialize temporary pointer to the father node */

    /* Iterate through each character in the string until the end or until a NULL pointer is encountered */
    while (string != NULL && *string != '\0')
    {
        /* Check if the current character's node exists in the trie */
        if (temp->sons[(*string) - TRIE_BASE_CHAR] != NULL)
        {
            /* Move to the next node corresponding to the current character*/
            temp = temp->sons[(*string) - TRIE_BASE_CHAR];
        }
        else
        {
            /* If the node for the current character does not exist, the string is not in the tree, return NULL */
            return NULL;
        }
        /* Move to the next character in the string */
        string++;
    }

    /* Return the endOfWord node if the end of the string is reached and it exists, otherwise return NULL */
    return (temp->endOfWord == NULL) ? NULL : temp;
}
/*
 * Inserts a string into the Trie.
 *
 * Algorithm:
 * - Iterate through each character in the string until the end.
 * - Check if the current character's node exists in the Trie.
 * - If the node does not exist, allocate memory for the new node, if memory allocation fails return TRUE.
 * - Set the new node as the son of the current father node.
 * - Move to the newly created node.
 * - If the node already exists, move to the existing node corresponding to the current character.
 * - when the end of the string is reached, Associate the symbol with the endOfWord node.
 * - Return FALSE indicating successful insertion.
 */
bool insertString(char *string, void *associatedEndOfWord, TrieNode *Father)
{
    TrieNode *currentFather = Father; /* Initialize currentFather pointer to the father node */

    /* Iterate through each character in the string until the end */
    while (*string != '\0')
    {
        /* Check if the current character's node exists in the trie */
        if (currentFather->sons[(int)(*string - TRIE_BASE_CHAR)] == NULL)
        {
            /* Allocate memory for the new node */
            TrieNode *son = calloc(1, sizeof(TrieNode));
            if (son == NULL)
                return TRUE; /* Memory allocation failed, return TRUE */

            /* Set the new node as the son of the currentFather node */
            currentFather->sons[(int)(*string - TRIE_BASE_CHAR)] = son;

            /* Move to the newly created node */
            currentFather = son;
        }
        else
        {
            /* Move to the existing node corresponding to the current character */
            currentFather = currentFather->sons[(int)(*string - TRIE_BASE_CHAR)];
        }

        /* Move to the next character in the string */
        string++;
    }

    /* Associate the symbol with the endOfWord node */
    currentFather->endOfWord = associatedEndOfWord;

    /* Return FALSE indicating successful insertion */
    return FALSE;
}