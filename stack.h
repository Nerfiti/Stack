#ifndef STACK_H
#define STACK_H

#include <assert.h>
#include <cstdio>
#include <malloc.h>
#include <windows.h>

#include "MyGeneralFunctions.h"

typedef int elem_t;
typedef void *stack_id;

enum STACK_ERRORS
{
    NON_EXISTENT_STACK = 1,
    STACK_CANARY_SIGNAL = 1 << 1,
    NEGATIVE_SIZE = 1 << 2,
    CAPACITY_LESS_THAN_SIZE = 1 << 3,
    WRONG_DATA_PTR = 1 << 4,
    WRONG_STACK_HASH = 1 << 5,
    DATA_CANARY_SIGNAL = 1 << 6,
    WRONG_CAPACITY = 1 << 7,
    WRONG_DATA_HASH = 1 << 8
};

int SetBirds(stack_id stk_id_ptr);
int StackCtor(stack_id *stk_id, size_t capacity = 10, elem_t POISON = -0x4FFFFFFF);
int StackPush(stack_id stk_id, elem_t item);
int StackPop(stack_id stk_id, elem_t *target);
int StackDtor(stack_id *stk_id); 
int StackOk(stack_id stk_id);
void PrintStack(stack_id stk_id);

#endif //STACK_H