#include <stdio.h>/*for printf and scanf*/
#include <math.h>/*for ceil*/

/*
 This program receives from the standard input, a list of real numbers, the first of which is the representative exchange rate of the dollar,
 on the day the program runs.
 The values that follow are representations of sums money in dollars.
 The program converts the dollar into shekels and then,
 it prints in a table format: for each dollar value the equivalent shekel value.
 In the bottom row of the table, the program will print the total amount of dollars and the total amount of shekels that were printed.

 How this program works:
 The program first gets the dollar exchange rate and saves the value in a variable named dollar.
 Then, the program gets all the sums of dollar one by one and each time it saves it in the variable named current.
 Then, it adds that value to variable named sum which stores the sum all of the amount the dollars that received,
 and then the program prints the dollar amount and the equivalent shekel amount.
 at last, the program prints the sum of all the dollars and the equivalent shekel amount.

 judi said to us not to round our results.
*/
int main()
{
	float dollar;/*the exchange rate of the dollar,on the day the program runs.*/ 
	float sum=0;/*the sum of all the list members.*/
	float current;/*currents ammount of dollars to convert*/
	scanf("%f",&dollar);/*getting the exchange rate of the dollar*/
	printf("\n$  \t\t\t\tIS");/*creating the table*/
	while(scanf("%f",&current)!=EOF)/*run until there are no more dollars to convert*/
	{
		sum+=current;/*add the current amount of dollars to sum*/
		printf("\n%.2f\t\t\t\t%.2f\n",current,current*dollar);/*print the dollar amount and the equivalent shekel amount*/
	}
	printf("\n%.2f\t\t\t\t%.2f\n",sum,sum*dollar);/*print the sum of all the dollars and the equivalent shekel amount*/
	return 0;
}
