#include <stdio.h>/*for scanf and printf*/
#include <math.h>/*for cos and pow and fabs*/


#define HALF_MAX_ANGLE 180
#define QUARTER_MAX_ANGLE 90
#define NEGATIVE 	-1
#define POSITIVE 	1
#define PI 	3.14159265358979323846
#define ACCURACY 	0.000001
#define MAX_ANGLE 	360
#define TAYLOR_QUEUE(current,element,counter)	current = pow((NEGATIVE),counter)*pow(element,(2*counter))/(factorial[counter])/*calculates the element in the counter place in Taylor queue and the result is stored in current*/
#define TO_RADINAS(degree)	degree/HALF_MAX_ANGLE*PI  /*convert degree to radians*/


double my_cos(double);


/*receives a double value representing angle, and prints the result of the calculation of
my_cos, as well as the result of the cos function from the standard library. */
int main()
{
	double angle=0;/*THE ANGLE TO RECEIVE*/
	printf("Please enter the angle to which the cosine should be calculated		");
	scanf("%lf",&angle);	
	printf("the angle to which the cosine should be calculated is  %f",angle);
	/*Reduce the angle to the range between 0 and 360 degrees*/
	angle=fabs(angle);/*cos(x)=cos(-x)*/
	angle=fmod(angle,MAX_ANGLE);/*cos(x-360)=cos(x)*/
	printf("\nmath.h cosine function result:	%f	\n",cos(TO_RADINAS(angle)));
	printf("my_cos result:	%f	\n",my_cos(angle));
	
	return 0;
}
/*calculates cosine with Taylor queue with an accuracy of 1.0e-6, before it calculates it converts the angle to the range 0<=x<=90 and then converts it to radians, and then it uses Taylor queue*/
double my_cos(double x)
{	
	int i=0;
	double current;/*current element in Taylor queue*/
	double result=0;/*store the result of taylor queue between angles 0 to 90  */
	int sign=POSITIVE; /*says if the result should be negative or positive*/
	int factorial[] ={1,2,24,720,40320,3628800,479001600};/*an array that represents the factorial of all the even numbers that are less than 13 and bigger than -1*/
	/*reducing x to be in the range 0<=x<=90*/
	if(x>=HALF_MAX_ANGLE)
	{
		x-=HALF_MAX_ANGLE;		/*because cos(x-180)=-cos(x)*/
		sign*=NEGATIVE;
		
	}
	if(x>=QUARTER_MAX_ANGLE)
	{
			x=HALF_MAX_ANGLE-x;/*because cos(180-x)=-cos(x)*/
			sign*=NEGATIVE;
	}
	x=TO_RADINAS(x);/*convert angle to radians*/
	
	/*calculate the sum of all Taylor queue elements until the next element's absolute value is less than 1.0e-6*/
	for(TAYLOR_QUEUE(current,x,i);fabs(current)>=ACCURACY;i++,TAYLOR_QUEUE(current,x,i))
		result+=current;
	return sign*result;/*returning the result*/
}
