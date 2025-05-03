#include "mymat.h"
mat *createMat(void)
{
    mat *temp=calloc(ONE,sizeof(mat));
    if(temp==NULL)
    {
        printf("not enough space");
        exit(-1);   
    }
    return temp;
}
void read_mat(mat *matrix,double numbers[])
{
    int i;
    for(i=0;i<NUM_OF_NUMBERS_IN_MAT;i++)
        (*matrix).numbers[i]=numbers[i];
}
void print_mat(mat matrix)
{
    int i;
    int j;
	for (i = 0; i < NUM_OF_NUMBERS_IN_ONE_LINE ; i++) {
		for (j = 0; j < NUM_OF_NUMBERS_IN_ONE_LINE ; j++) {
			printf("%7.2f", matrix.numbers[i*NUM_OF_NUMBERS_IN_ONE_LINE +j]);/*i didn't use a constatnt because its a string*/
                if (j != NUM_OF_NUMBERS_IN_ONE_LINE -ONE) {
                    printf("\t");
                }
            }
	printf("\n");
        }
	return;
}
void add_mat(mat matrix1,mat matrix2,mat *matrix3)
{
    int i;
    for(i=0;i<NUM_OF_NUMBERS_IN_MAT;i++)
        (*matrix3).numbers[i]=(matrix1.numbers[i])+(matrix2.numbers[i]);
}
void sub_mat(mat matrix1,mat matrix2,mat *matrix3)
{
    int i;
    for(i=0;i<NUM_OF_NUMBERS_IN_MAT;i++)
        (*matrix3).numbers[i]= (matrix1.numbers[i]) - (matrix2.numbers[i]);
}
void mul_mat(mat matrix1,mat matrix2,mat *matrix3)
{
    int i;
    int j;
    for(i=0;i<NUM_OF_NUMBERS_IN_ONE_LINE;i++)
        for(j=0;j<NUM_OF_NUMBERS_IN_ONE_LINE;j++)
            (*matrix3).numbers[i]= (matrix1.numbers[j+i*NUM_OF_NUMBERS_IN_ONE_LINE]) * (matrix2.numbers[i+j*NUM_OF_NUMBERS_IN_ONE_LINE]);
}
void mul_scalar(mat matrix1,double scalar,mat *matrix2)
{
    int i;
    for(i=0;i<NUM_OF_NUMBERS_IN_MAT;i++)
        (*matrix2).numbers[i]=matrix1.numbers[i]*scalar;
}
void trans_mat(mat matrix1,mat *matrix2)
{
    int i;
    int j;
    mat temp;
    for(i=0;i<NUM_OF_NUMBERS_IN_ONE_LINE;i++)
        for(j=0;j<NUM_OF_NUMBERS_IN_ONE_LINE;j++)
            temp.numbers[i+j*NUM_OF_NUMBERS_IN_ONE_LINE]= matrix1.numbers[j+i*NUM_OF_NUMBERS_IN_ONE_LINE];
    read_mat(matrix2,temp.numbers);
}
void stop(void)
{
	printf("exiting from the mat calculator");
    exit(SUCCESS);
}
