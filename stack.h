#ifndef STACK_H
#define STACK_H


#include <assert.h>
#include <cstdio>
#include <malloc.h>
#include <windows.h>

#include "MyGeneralFunctions.h"


typedef int elem_t;
typedef long long CANARY_t;

const float STACK_SIZE_INCREASE_COEFF = 2;
const CANARY_t CANARY = 0x00000009;

enum STACK_ERRORS
{
    NON_EXISTENT_STACK = 1,
    STACK_CANARY_SIGNAL = 2,
    NEGATIVE_SIZE = 4,
    CAPACITY_LESS_THAN_SIZE = 8,
    WRONG_DATA_PTR = 16,
    DATA_CANARY_SIGNAL = 32,
    WRONG_CAPACITY = 64,
    WRONG_DATA = 128
};


struct stack
{
    CANARY_t LeftCanary = CANARY;
    elem_t *data = nullptr;
    size_t size = 0;
    size_t capacity = 0;
    elem_t POISON = 0;
    CANARY_t RightCanary = CANARY;
};

int SetBirds(stack *stk);
int StackCtor(stack *stk, size_t capacity, elem_t POISON);
int StackPush(stack *stk, elem_t item);
int StackPop(stack *stk, elem_t *target);
int StackDtor(stack *stk);
int StackResize_UP(stack *stk);
int StackResize_DOWN(stack *stk);
int StackOk(stack *stk);
void PrintStack(stack stk);

int SetBirds(stack *stk)
{
    int err = 0;
    if ((err = StackOk(stk)) != 0 && err != DATA_CANARY_SIGNAL)
    {
        return err;
    }

    *((CANARY_t *)stk->data - 1) = CANARY;
    *(CANARY_t *)(stk->data + stk->capacity) = CANARY;
    return StackOk(stk);
}

int StackCtor(stack *stk, size_t capacity, elem_t POISON)
{
    if (stk == nullptr || IsBadReadPtr(stk, sizeof(stack))) //Remove for vulnerability
    {
        return NON_EXISTENT_STACK;
    }
    *stk = 
    {
        .LeftCanary = CANARY,
        .data = (elem_t *)((CANARY_t *)calloc(capacity*sizeof(elem_t) + 2*sizeof(CANARY_t), 1) + 1),
        .size = 0,
        .capacity = capacity,
        .POISON = POISON,
        .RightCanary = CANARY
    };

    SetBirds(stk);
    
    for (int i = 0; i < capacity; i++)
    {
        stk->data[i] = POISON;
    }
    if (int err = StackOk(stk))
    {
        return err;
    }

    return StackOk(stk);
}

int StackPush(stack *stk, elem_t item)
{
    int err = 0;
    if (err = StackOk(stk))
    {
        return err;
    }
    if (err = StackResize_UP(stk))
    {
        return err;
    }

    stk->data[stk->size] = item;
    stk->size++;

    return StackOk(stk);
}

int StackPop(stack *stk, elem_t *target)
{
    int err = 0;
    if (err = StackOk(stk))
    {
        return err;
    }
    if (err = StackResize_DOWN(stk))
    {
        return err;
    }

    stk->size--;
    *target = stk->data[stk->size];
    stk->data[stk->size] = stk->POISON;

    return StackOk(stk);
}

int StackDtor(stack *stk)
{
    if (int err = StackOk(stk))
    {
        return err;
    }

    free((CANARY_t *)stk->data - 1);
    stk->data = (elem_t *)JUST_FREE_PTR;

    stk->size = 0;
    stk->capacity = 0;

    return StackOk(stk);
}

int StackResize_UP(stack *stk)
{
    if (int err = StackOk(stk))
    {
        return err;
    }

    while (stk->size >= stk->capacity)
    {
        stk->data = (elem_t *)((CANARY_t *)(realloc((CANARY_t *)stk->data - 1,
                                       (int)(stk->capacity*sizeof(elem_t)*STACK_SIZE_INCREASE_COEFF) +
                                       2*sizeof(CANARY_t))) + 1);
        assert(stk->data);

        stk->capacity *= STACK_SIZE_INCREASE_COEFF;

        for (int i = stk->size; i < stk->capacity; ++i)
        {
            stk->data[i] = stk->POISON;
        }
        SetBirds(stk);
    }

    return StackOk(stk);
}

int StackResize_DOWN(stack *stk)
{
    if (int err = StackOk(stk))
    {
        return err;
    }

    if (STACK_SIZE_INCREASE_COEFF*STACK_SIZE_INCREASE_COEFF*(stk->size) <= stk->capacity)
    {
        stk->data = (elem_t *)((CANARY_t *)realloc((CANARY_t *)stk->data - 1,
                                       (int)(stk->capacity*sizeof(elem_t)/STACK_SIZE_INCREASE_COEFF) +
                                       2*sizeof(CANARY_t)) + 1);
        assert(stk->data);

        stk->capacity /= STACK_SIZE_INCREASE_COEFF;
        SetBirds(stk);

    }

    return StackOk(stk);
}

int StackOk(stack *stk)
{
    int err = 0;
    if (stk == nullptr || IsBadReadPtr(stk, sizeof(stack)))
    {
        err += NON_EXISTENT_STACK;
        return err;
    }
    if (stk->LeftCanary != CANARY || stk->RightCanary != CANARY)
    {
        err += STACK_CANARY_SIGNAL;
        return err;
    }
    if (stk->size < 0)
    {
        err += NEGATIVE_SIZE;
    }
    if (stk->capacity < stk->size)
    {
        err += CAPACITY_LESS_THAN_SIZE;
    }
    if (stk->data == nullptr || stk->data == JUST_FREE_PTR || 
        IsBadReadPtr((CANARY_t *)stk->data - 1, stk->capacity*sizeof(elem_t) + 2*sizeof(CANARY_t)))
    {
        err += WRONG_DATA_PTR;
        return err;
    }
    if (*((CANARY_t *)stk->data - 1) != CANARY && 
        *((CANARY_t *)((char *)stk->data + stk->capacity)) != CANARY)
    {
        err += DATA_CANARY_SIGNAL;
    }
    CANARY_t *ptr = (CANARY_t *)stk->data - 1;
    if (_msize((CANARY_t *)stk->data - 1) - 2*sizeof(CANARY_t) != stk->capacity*sizeof(elem_t))
    {
        err += WRONG_CAPACITY;
    }
    bool is_clean = true;
    for (int i = 0; i < stk->size; ++i)
    {
        if (stk->data[i] == stk->POISON)
        {
            is_clean = false;
        }
    }
    if (!is_clean)
    {
        err += WRONG_DATA;
    }
    return err;
}

void PrintStack(stack stk)
{
    printf("stk[-1] = %lld, stk[capacity] = %lld\n\n", *((CANARY_t *)stk.data - 1), *(CANARY_t *)(stk.data + stk.capacity));
    for (int i = 0; i < stk.size; ++i)
    {
        printf("stk[%d] = %d\n", i, stk.data[i]);
    }
    for (int i = stk.size; i < stk.capacity; ++i)
    {
        if (stk.data[i] == stk.POISON)
        {
            printf("stk[%d] = POISON\n", i);
        } 
        else
        {
            printf("stk[%d] = %d\n", i, stk.data[i]);
        }
    }
    printf("\n\n");
}

#endif //STACK_H