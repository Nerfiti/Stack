#include "stack.h"

typedef long long CANARY_t;
typedef unsigned long long Hash;

const int factor = 2;
const CANARY_t CANARY               = 0xFEEEB1ED;

struct stack
{
    CANARY_t LeftCanary   = CANARY;
    elem_t *data          = nullptr;
    size_t size           = 0;
    size_t capacity       = 0;
    size_t start_capacity = 0;
    elem_t POISON         = 0;
    Hash STACK_HASH       = 0;
    Hash DATA_HASH        = 0;
    CANARY_t RightCanary  = CANARY;
};

static unsigned long long GenHash         (void *memptr, size_t size_of_memblock);
static int                StackResize_UP  (stack_id stk_id                      );
static int                StackResize_DOWN(stack_id stk_id                      );
static int                bin             (int number                           );

int SetBirds(stack_id stk_id)
{
    stack *stk = (stack *)stk_id;
    int    err = 0;
    if ((err = StackOk(stk_id)) < WRONG_STACK_HASH && err != 0)
    {
        return err;
    }
    *((CANARY_t *) stk->data - 1            ) = CANARY;
    *( CANARY_t *)(stk->data + stk->capacity) = CANARY;

    return StackOk(stk);
}

int StackCtor(stack_id *stk_id_ptr, size_t capacity, elem_t POISON)
{
    if (capacity < 1 || capacity >= __SIZE_MAX__ / sizeof(elem_t) - 2*sizeof(CANARY_t))
    {
        *stk_id_ptr = (stack_id)JUST_FREE_PTR;
        return WRONG_CAPACITY;
    }
    
    stack *stk          = (stack *)calloc(sizeof(stack), 1);
    
    stk->LeftCanary     = CANARY;
    stk->data           = (elem_t *)((CANARY_t *)calloc(capacity*sizeof(elem_t) + 2*sizeof(CANARY_t), 1) + 1);
    stk->size           = 0;
    stk->capacity       = capacity;
    stk->start_capacity = capacity;
    stk->POISON         = POISON;
    stk->STACK_HASH     = GenHash((CANARY_t *)stk + 1, sizeof(stack)-2*sizeof(CANARY_t)-2*sizeof(Hash));
    stk->RightCanary    = CANARY;
    
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
    int err    = 0;

    if (err = StackOk(stk))
    {
        return err;
    }

    printf("BEFORE PUSH:\n");
    PrintStack(stk_id);
    
    if (err = StackResize_UP(stk))
    {
        return err;
    }

    stk->data[stk->size] = item;
    stk->size++;

    stk->STACK_HASH = GenHash((CANARY_t *)stk + 1, sizeof(stack) - 2*sizeof(CANARY_t) - 2*sizeof(Hash));
    stk->DATA_HASH  = GenHash(stk->data, stk->capacity*sizeof(elem_t));

    printf("AFTER PUSH:\n");
    PrintStack(stk_id);

    return StackOk(stk);
}

int StackPop(stack_id stk_id, elem_t *target)
{
    stack *stk = (stack *)stk_id;
    int err    = 0;
    
    if (err = StackOk(stk))
    {
        printf("StackOK = %d", err);
        return err;
    }

    printf("BEFORE POP:\n");
    PrintStack(stk_id);
    
    stk->size--;
    *target              = stk->data[stk->size];
    stk->data[stk->size] = stk->POISON;

    if (err = StackResize_DOWN(stk))
    {
        return err;
    }

    stk->STACK_HASH = GenHash((CANARY_t *)stk + 1, sizeof(stack) - 2*sizeof(CANARY_t) - 2*sizeof(Hash));
    stk->DATA_HASH = GenHash(stk->data, stk->capacity*sizeof(elem_t));

    printf("AFTER POP:\n");
    PrintStack(stk_id);
    
    return StackOk(stk);
}

int StackDtor(stack_id *stk_id)
{
    stack *stk      = (stack *)stk_id;

    free((CANARY_t *)stk->data - 1);
    stk->data       = (elem_t *)JUST_FREE_PTR;

    stk->size       = -1;
    stk->capacity   = -1;
    stk->DATA_HASH  = -1;
    stk->STACK_HASH = -1;

    free(stk);
    *stk_id         = (stack_id)JUST_FREE_PTR;

    return 0;
}

static int StackResize_UP(stack_id stk_id)
{
    stack *stk    = (stack *)stk_id;

    while (stk->size >= stk->capacity)
    {
        size_t new_data_size = (stk->capacity*sizeof(elem_t)*factor) + 2*sizeof(CANARY_t);
        stk->data = (elem_t *)((CANARY_t *)(realloc((CANARY_t *)stk->data - 1, new_data_size)) + 1);
        assert(stk->data);

        stk->capacity *= factor;
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
    stack *stk   = (stack *)stk_id;
    
    if (factor*factor*(stk->size) <= stk->capacity && stk->size != 0 &&
                             stk->capacity >= factor*factor*stk->start_capacity)
    {
        size_t new_data_size = (stk->capacity*sizeof(elem_t)/factor) + 2*sizeof(CANARY_t);
        stk->data = (elem_t *)((CANARY_t *)(realloc((CANARY_t *)stk->data - 1, new_data_size)) + 1);
        assert(stk->data);
        
        stk->capacity /= factor;
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
        err |= NON_EXISTENT_STACK;
        return err;
    }
    if (stk->LeftCanary != CANARY || stk->RightCanary != CANARY)
    {
        err |= STACK_CANARY_SIGNAL;
    }
    if (stk->size < 0)
    {
        err |= NEGATIVE_SIZE;
    }
    if (stk->capacity < stk->size)
    {
        err |= CAPACITY_LESS_THAN_SIZE;
    }
    if (stk->data == nullptr || stk->data == JUST_FREE_PTR || 
        IsBadReadPtr((CANARY_t *)stk->data - 1, stk->capacity*sizeof(elem_t) + 2*sizeof(CANARY_t)))
    {
        err |= WRONG_DATA_PTR;
        return err;
    }
    if (stk->STACK_HASH != GenHash((CANARY_t *)stk + 1, sizeof(stack) - 2*sizeof(CANARY_t) - 2*sizeof(Hash)))
    {
        err |= WRONG_STACK_HASH;
    }
    if (*((CANARY_t *)stk->data - 1) != CANARY && 
        *((CANARY_t *)((char *)stk->data + stk->capacity)) != CANARY)
    {
        err |= DATA_CANARY_SIGNAL;
    }
    if (_msize((CANARY_t *)stk->data - 1) - 2*sizeof(CANARY_t) != stk->capacity*sizeof(elem_t))
    {
        err |= WRONG_CAPACITY;
    }
    if (stk->DATA_HASH != GenHash(stk->data, stk->capacity*sizeof(elem_t)))
    {
        err |= WRONG_DATA_HASH;
    }
    return err;
}

void PrintStack(stack_id stk_id)
{
    stack *stk = (stack *)stk_id;
    int err = StackOk(stk);
    if (err == 1)
    {
        printf("Stack Error: Wrong stack pointer\n\n");
        return;
    }
    printf("\nStack info:\n");
    printf("Left Stack Canary: %llu\nRight Stack Canary: %llu\n", stk->LeftCanary, stk->RightCanary);
    printf("Stack Hash: %llu\n", stk->STACK_HASH);
    printf("Data Pointer: 0x%p\n", stk->data);
    printf("Size: %lu\nCapacity: %lu\nData:\n", stk->size, stk->capacity);
    for (int i = 0; i < stk->capacity; ++i)
    {
        if (stk->data[i] == stk->POISON)
        {
            printf("stack[%d] = POISON\n", i);
        } 
        else
        {
            printf("stk[%d] = %d\n", i, stk->data[i]);
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

static int bin(int number)
{
    int result = 0;

    int sign = 1;
    if (number < 0)
    {
        number *= -1;
        sign = -1;
    }

    for (int m = 1; number > 0; m *= 10)
    {
        result += m * (number % 2);
        number /= 2;
    }

    return sign * result;
}