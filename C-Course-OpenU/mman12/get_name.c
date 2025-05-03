#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

/* Define constants*/
#define NUM_TO_PRINT 10
#define NUM_NAMES 30
#define LEN_NAME 21

/* Function to compare two strings for equality*/
int my_strcmp(char *str1, char *str2);

/* Function to get a random name from the global array 'names'*/
char *getname(void);

/*global two-dimensional array to store 30 names as long as 20 letters each*/
char names[NUM_NAMES][LEN_NAME];

/*This program gets 30 names and returns 10 random names out of the 30,
I assume that the user will give the function only 30 names as was written in the forum.
the program works like this:
first, the program gets 30 names and for each name, it gets its checking if the names that got before him are identical, is there are identical names the program transforms immediately and prints an error.
after that, the program prints all the input and then calls get_name 10 times to get 10 random names out of the 30, if there is no space for the 10 names,
the program prints: not enough space and terminates*/
int main()
{
    /* Variables*/
    int i; /* Loop variable for name input*/
	int k = 0; /* Loop variable for random name generation*/
	printf("Please enter 30 different names\n");/*requesting from the user to give the program 30 names*/
    /* Input loop for entering unique names*/
    for (i = 0; i < NUM_NAMES; i++)
    {
        int j = 0; /* Inner loop variable for duplicate checking*/
        scanf("%s", names[i]);
	
        /* Check for duplicate names in the array*/
        while (j < i)
        {
            if (my_strcmp(names[j], names[i]))
            {
                /* Print error message and exit if a duplicate is found*/
                printf("Error: Two identical names detected.\n");
                return 1; /* Exit with an error code*/
            }
            j++;
        }
	}
	/*printing the input*/
	printf("here is your input:\n");
	for (i = 0; i < NUM_NAMES; i++)/*printing the input*/
    {
		printf("%s\n",names[i]);
    }
    /* Seed the random number generator with the current time */
        srand(time(0));
	printf("here is the 10 random names:\n");
    /* Print loop for generating and printing random names*/
    while (k++ < NUM_TO_PRINT)
    {
        char *randomName;
        randomName= getname();

        /* Check for memory allocation failure*/
        if (randomName == NULL)
        {
            /* Print error message and exit if memory allocation fails*/
            printf("Error: Not enough space for name allocation.\n");
            return 1; /* Exit with an error code*/
        }

        /* Print the generated random name*/
        printf("Random Name %d: %s\n", k, randomName);
        free(randomName);
    }

    return 0; /* Exit successfully*/
}

/* Function to compare two strings for equality*/
int my_strcmp(char *str1, char *str2)
{
    /* Compare characters until the end of one or both strings*/
    while (tolower(*str1) == tolower(*str2) && *str1 != '\0')
    {
        str1++;
        str2++;
    }
    /* Return true if both strings are equal, false otherwise*/
    return (*str1 == '\0' && *str2 == '\0');
}

/* Function to get a random name from the global array 'names'*/
char *getname(void)
{
    /* Generate a random index within the range of available names*/
    unsigned random;
    /* Allocate memory for the new name*/
    char *name = malloc(sizeof(char) * LEN_NAME);
    /* Check for memory allocation failure*/
    if (name == NULL)
        return NULL;
    /* Generate a random index within the range of available names */
    random = rand() % NUM_NAMES;
    /* Copy the random name to the allocated space */
    strcpy(name, names[random]);
    /* Copy the random name to the allocated space*/
    strcpy(name,names[random]);
    return name;
}
