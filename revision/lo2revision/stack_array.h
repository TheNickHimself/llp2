#ifndef REVISION_LO2_STACK_ARRAY_INCLUDED
#define REVISION_LO2_STACK_ARRAY_INCLUDED

#include <limits.h> /* INT_MIN */

#define EMPTY -1
#define MAX_SIZE 100
#define NO_VALUE INT_MIN

/* The array that holds the values on the stack */
int stack[MAX_SIZE];

/* Top of the stack */
int top = EMPTY;

/* Returns true if stack is empty */
bool is_empty(void) 
{
	return true;
}

/* Returns true if stack is full */
bool is_full(void) 
{
	return false;
}

/* Pop the int on top of the stack or return NO_VALUE if stack empty */
int pop(void) 
{
	return NO_VALUE;
}

/* Push value on top of the stack. 
 * Returns true if pushed or false if stack is full 
 */
bool push(int value) 
{
	return false;
}

#endif //REVISION_LO2_STACK_ARRAY_INCLUDED