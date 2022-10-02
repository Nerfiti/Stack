#include "stack.h"

static unsigned long long GenHash(void *memptr, size_t size_of_memblock);
static int StackResize_UP(stack_id stk_id);//TODO: Удалить для уязвимости
static int StackResize_DOWN(stack_id stk_id);

typedef unsigned long long Hash;
struct stack
{
    CANARY_t LeftCanary = CANARY;
    elem_t *data = nullptr;
    size_t size = 0;
    size_t capacity = 0;
    elem_t POISON = 0;
    Hash STACK_HASH;
    Hash DATA_HASH;
    CANARY_t RightCanary = CANARY;
};



int SetBirds(stack_id stk_id)
{
    stack *stk = (stack *)stk_id;
    int err = 0;
    if ((err = StackOk(stk_id)) < WRONG_STACK_HASH && err != 0)
    {
        return err;
    }
    
    *((CANARY_t *)stk->data - 1) = CANARY;
    *(CANARY_t *)(stk->data + stk->capacity) = CANARY;
    return StackOk(stk);
}

int StackCtor(stack_id *stk_id_ptr, size_t capacity, elem_t POISON)
{
    if (capacity < 1)
    {
        return WRONG_CAPACITY;
    }
    
    stack *stk = (stack *)calloc(sizeof(stack), 1);
    
    stk->LeftCanary = CANARY;
    stk->data = (elem_t *)((CANARY_t *)calloc(capacity*sizeof(elem_t) + 2*sizeof(CANARY_t), 1) + 1);
    stk->size = 0;
    stk->capacity = capacity;
    stk->POISON = POISON;
    stk->STACK_HASH = GenHash((CANARY_t *)stk + 1, sizeof(stack)-2*sizeof(CANARY_t)-2*sizeof(Hash));
    stk->RightCanary = CANARY;
    
    for (int i = 0; i < capacity; i++)
    {
        stk->data[i] = POISON;
    }
    stk->DATA_HASH = GenHash(stk->data, capacity*sizeof(elem_t));
    
    SetBirds(stk);
    
    *stk_id_ptr = (stack_id)stk;
    return StackOk(stk);
}

int StackPush(stack_id stk_id, elem_t item)
{
    stack *stk = (stack *)stk_id;
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
    stk->STACK_HASH = GenHash((CANARY_t *)stk + 1, sizeof(stack) - 2*sizeof(CANARY_t) - 2*sizeof(Hash));
    stk->DATA_HASH = GenHash(stk->data, stk->capacity*sizeof(elem_t));

    return StackOk(stk);
}

int StackPop(stack_id stk_id, elem_t *target)
{
    stack *stk = (stack *)stk_id;
    int err = 0;
    if (err = StackOk(stk))
    {
        return err;
    }
    stk->size--;
    *target = stk->data[stk->size];
    stk->data[stk->size] = stk->POISON;
    if (err = StackResize_DOWN(stk))
    {
        return err;
    }
    stk->STACK_HASH = GenHash((CANARY_t *)stk + 1, sizeof(stack) - 2*sizeof(CANARY_t) - 2*sizeof(Hash));
    stk->DATA_HASH = GenHash(stk->data, stk->capacity*sizeof(elem_t));

    return StackOk(stk);
}

int StackDtor(stack_id stk_id)
{
    stack *stk = (stack *)stk_id;

    free((CANARY_t *)stk->data - 1);
    stk->data = (elem_t *)JUST_FREE_PTR;

    stk->size = 0;
    stk->capacity = 0;
    stk->DATA_HASH = 0;
    stk->STACK_HASH = 0;

    free(stk);
    stk_id = (stack_id)JUST_FREE_PTR;
    return 0;
}

static int StackResize_UP(stack_id stk_id)
{
    stack *stk = (stack *)stk_id;

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
    return 0;
}

static int StackResize_DOWN(stack_id stk_id)
{
    stack *stk = (stack *)stk_id;
    
    if (STACK_SIZE_INCREASE_COEFF*STACK_SIZE_INCREASE_COEFF*(stk->size) <= stk->capacity && stk->size != 0)
    {
        stk->data = (elem_t *)((CANARY_t *)realloc((CANARY_t *)stk->data - 1,
                                       (int)(stk->capacity*sizeof(elem_t)/STACK_SIZE_INCREASE_COEFF) +
                                       2*sizeof(CANARY_t)) + 1);
        assert(stk->data);
        
        stk->capacity /= STACK_SIZE_INCREASE_COEFF;
        SetBirds(stk);
    }
    return 0;
}

int StackOk(stack_id stk_id)
{
    stack *stk = (stack *)stk_id;
    int err = 0;
    if (stk == nullptr || stk == JUST_FREE_PTR || IsBadReadPtr(stk, sizeof(stack_id)))
    {
        err += NON_EXISTENT_STACK;
        return err;
    }
    if (stk->LeftCanary != CANARY || stk->RightCanary != CANARY)
    {
        err += STACK_CANARY_SIGNAL;
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
    if (stk->STACK_HASH != GenHash((CANARY_t *)stk + 1, sizeof(stack) - 2*sizeof(CANARY_t) - 2*sizeof(Hash)))
    {
        err += WRONG_STACK_HASH;
    }
    if (*((CANARY_t *)stk->data - 1) != CANARY && 
        *((CANARY_t *)((char *)stk->data + stk->capacity)) != CANARY)
    {
        err += DATA_CANARY_SIGNAL;
    }
    if (_msize((CANARY_t *)stk->data - 1) - 2*sizeof(CANARY_t) != stk->capacity*sizeof(elem_t))
    {
        err += WRONG_CAPACITY;
    }
    if (stk->DATA_HASH != GenHash(stk->data, stk->capacity*sizeof(elem_t)))
    {
        err += WRONG_DATA_HASH;
    }
    return err;
}

void PrintStack(stack_id stk_id)
{
    stack stk = *(stack *)stk_id;
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

static unsigned long long GenHash(void *memptr, size_t size_of_memblock)
{
    if (memptr == nullptr || IsBadReadPtr(memptr, size_of_memblock)){return 0;}
    unsigned long long hash = 5381;
    char *ptr = (char *)memptr;
    int i = 0;
    while (i < size_of_memblock)
    {
        hash = (~hash) * (ptr[i] + hash + 3) + ~(hash) ^ (CANARY);
        ++i;
    }
    return hash;
}
