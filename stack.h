#ifndef STACK_H
#define STACK_H

#include <assert.h>
#include <cstdio>
#include <malloc.h>
#include <windows.h>

#include "MyGeneralFunctions.h"

typedef int elem_t;
typedef long long CANARY_t;
typedef void *stack_id;

const float STACK_SIZE_INCREASE_COEFF = 2;
const CANARY_t CANARY = 0xFEEEB1ED;

enum STACK_ERRORS
{
    NON_EXISTENT_STACK = 1,
    STACK_CANARY_SIGNAL = 2,
    NEGATIVE_SIZE = 4,
    CAPACITY_LESS_THAN_SIZE = 8,
    WRONG_DATA_PTR = 16,
    WRONG_STACK_HASH = 32,
    DATA_CANARY_SIGNAL = 64,
    WRONG_CAPACITY = 128,
    WRONG_DATA_HASH = 256,
};

int SetBirds(stack_id stk_id_ptr);
int StackCtor(stack_id *stk_id, size_t capacity = 100, elem_t POISON = -0x4FFFFFFF);
int StackPush(stack_id stk_id, elem_t item);
int StackPop(stack_id stk_id, elem_t *target);
int StackDtor(stack_id stk_id); 
int StackOk(stack_id stk_id);
void PrintStack(stack_id stk_id);

#endif //STACK_H