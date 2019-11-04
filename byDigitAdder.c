/*-------------------------------------------------------------------------*
 *---								                                	---*
 *---		                        byDigitAdder.c						---*
 *---								                                	---*
 *---	    This file defines a C program that represents positive	    ---*
 *---	decimal integers as linked lists of digits.  It asks the user	---*
 *---	for 2 integers, computes the sum, and outputs it.	        	---*
 *---								                                	---*
 *---	----	----	----	----	----	----	----	----	---*
 *---								                                	---*
 *---	     Version 1a		2019 October 17		Joseph Phillips	        ---*
 *---								                                	---*
 *-------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUM_TEXT_LEN 256

//  PURPOSE:  To represent one digit of a decimal number.
struct DigitNode
{
    int digit_;                 // I suggest you range this in [0..9]
    struct DigitNode *nextPtr_; // I suggest you make this point to
                                //  the next most significant digit.
};

//  PURPOSE:  To obtain the text of a decimal integer into array 'numberCPtr'
//	of length 'numberTextLen'.  'descriptionCPtr' is printed because it
//	tells the user the integer that is expected.   Ending '\n' from
//	'fgets()' is replaced with '\0'.  No return value.
void obtainPostiveInt(char *numberCPtr,
                      int numberTextLen,
                      const char *descriptionCPtr)
{
    //  YOUR CODE HERE
    int len, fl;
    char x;
    while (1)
    {
        printf("Please enter %s integer: ", descriptionCPtr);
        for (int i = 0;; i++)
        {
            x = getchar();
            if (x == '\n')
            {
                numberCPtr[i] = '\0';
                break;
            }
            numberCPtr[i] = x;
        }
        len = strlen(numberCPtr);
        fl = 0;
        for (int i = 0; i < len; i++)
            if (numberCPtr[i] < '0' || numberCPtr[i] > '9')
            {
                fl = 1;
                break;
            }
        if (!fl)
            break;
    }
}

//  PURPOSE:  To build and return a linked list IN LITTLE ENDIAN ORDER
//	of the decimal number whose text is pointed to by 'numberCPtr'.
//	If 'numberCPtr' points to the string "123" then the linked list
//	returned is 'digit_=3' -> 'digit_=2' -> 'digit_=1' -> NULL.
struct DigitNode *numberList(const char *numberCPtr)
{
    //  YOUR CODE HERE
    int len = strlen(numberCPtr);
    struct DigitNode *head = (struct DigitNode *)malloc(sizeof(struct DigitNode)), *tail = head;

    for (int i = len - 1; i >= 0; i--)
    {
        struct DigitNode *x = (struct DigitNode *)malloc(sizeof(struct DigitNode));
        x->digit_ = (int)(numberCPtr[i] - '0');
        x->nextPtr_ = NULL;
        tail->nextPtr_ = x;
        tail = x;
    }
    tail = head->nextPtr_;
    free(head);
    return tail;
}

//  PURPOSE:  To build and return a linked list IN LITTLE ENDIAN ORDER
//	of the decimal number that results from adding the decimal numbers
//	whose digits are pointed to by 'list0' and 'list1'.
struct DigitNode *add(const struct DigitNode *list0, const struct DigitNode *list1)
{
    //  YOUR CODE HERE
    int carry = 0, sum;
    struct DigitNode *head = (struct DigitNode *)malloc(sizeof(struct DigitNode)), *tail = head;

    while ((list0 != NULL) && (list1 != NULL))
    {
        sum = (carry + list0->digit_ + list1->digit_);
        carry = sum / 10;
        struct DigitNode *x = (struct DigitNode *)malloc(sizeof(struct DigitNode));
        x->digit_ = sum % 10;
        x->nextPtr_ = NULL;
        tail->nextPtr_ = x;
        tail = x;
        list0 = list0->nextPtr_;
        list1 = list1->nextPtr_;
    }
    if (list0 == NULL)
        list0 = list1;

    while (list0 != NULL)
    {
        sum = (carry + list0->digit_);
        carry = sum / 10;
        struct DigitNode *x = (struct DigitNode *)malloc(sizeof(struct DigitNode));
        x->digit_ = sum % 10;
        x->nextPtr_ = NULL;
        tail->nextPtr_ = x;
        tail = x;
        list0 = list0->nextPtr_;
    }
    if (carry > 0)
    {
        struct DigitNode *x = (struct DigitNode *)malloc(sizeof(struct DigitNode));
        x->digit_ = carry;
        x->nextPtr_ = NULL;
        tail->nextPtr_ = x;
        tail = x;
    }
    tail = head->nextPtr_;
    free(head);
    return tail;
}

//  PURPOSE:  To print the decimal number whose digits are pointed to by 'list'.
//	Note that the digits are IN LITTLE ENDIAN ORDER.  No return value.
void printList(const struct DigitNode *list)
{
    //  YOUR CODE HERE
    if(list == NULL)
    return;
    printList(list->nextPtr_);
    printf("%d",list->digit_);
}

//  PURPOSE:  To print the nodes of 'list'.  No return value.
void freeList(struct DigitNode *list)
{
    //  YOUR CODE HERE
    struct DigitNode *x = list;
    while (list != NULL)
    {
        x = list;
        list = list->nextPtr_;
        free(x);
    }
    return;
}

//  PURPOSE:  To coordinate the running of the program.  Ignores command line
//	arguments.  Returns 'EXIT_SUCCESS' to OS.
int main()
{
    char numberText0[NUM_TEXT_LEN];
    char numberText1[NUM_TEXT_LEN];
    struct DigitNode *operand0List = NULL;
    struct DigitNode *operand1List = NULL;
    struct DigitNode *sumList = NULL;

    obtainPostiveInt(numberText0, NUM_TEXT_LEN, "first");
    obtainPostiveInt(numberText1, NUM_TEXT_LEN, "second");

    operand0List = numberList(numberText0);
    operand1List = numberList(numberText1);
    sumList = add(operand0List, operand1List);

    printList(operand0List);
    printf(" + ");
    printList(operand1List);
    printf(" = ");
    printList(sumList);
    printf("\n");

    freeList(sumList);
    freeList(operand1List);
    freeList(operand0List);

    return (EXIT_SUCCESS);
}