#include "stack.h"

int bin(int a)
{
    int result = 0;
    for (int m = 1; a > 0; m *= 10)
    {
        result += m*(a%2);
        a /= 2;
    }
    return result;
}

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
