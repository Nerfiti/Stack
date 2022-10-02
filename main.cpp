#include "stack.h"

int main()
{
    int err = 0;
    stack_id stk;
    err = StackCtor(&stk, 3);
    printf("StackCtor: %d\n", err);
    PrintStack(stk);
    for (int i = 0; i < 10; ++i)
    {
        err = StackPush(stk, i);
        printf("StackPush: %d\n", err);
        PrintStack(stk);
    }
    for (int i = 0; i < 10; ++i)
    {
        printf("StackPop: %d\n", StackPop(stk, &err));
        printf("Value: %d\n", err);
        PrintStack(stk);    
    }

}
