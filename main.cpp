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
    //stack stk = {};
    int results[10] = {};
    int err = 0;

    stack *stk = (stack *)7;
    printf("%d", StackCtor(stk, 2, -239239));

    // err = StackCtor(&stk, 1, -239239);
    // PrintStack(stk);
    // printf("err = %d\n", bin(err));

    // err = StackPush(&stk, 1);
    // PrintStack(stk);
    // printf("err = %d\n", bin(err));

    // err = StackPush(&stk, 2);
    // PrintStack(stk);
    // printf("err = %d\n", bin(err));


    // err = StackPush(&stk, 3);
    // PrintStack(stk);
    // printf("err = %d\n", bin(err));


    // err = StackPush(&stk, 4);
    // PrintStack(stk);
    // printf("err = %d\n", bin(err));

    // err = StackPush(&stk, 5);
    // PrintStack(stk);
    // printf("err = %d\n", bin(err));

    // err = StackPush(&stk, 6);
    // PrintStack(stk);
    // printf("err = %d\n", bin(err));

    // err = StackPush(&stk, 7);
    // PrintStack(stk);
    // printf("err = %d\n", bin(err));

    // err = StackPop(&stk, results);
    // PrintStack(stk);
    // printf("err = %d\n", bin(err));

    // err = StackPop(&stk, results);
    // PrintStack(stk);
    // printf("err = %d\n", bin(err));

    // err = StackPop(&stk, results);
    // PrintStack(stk);
    // printf("err = %d\n", bin(err));

    // err = StackPop(&stk, results);
    // PrintStack(stk);
    // printf("err = %d\n", bin(err));

    // err = StackPop(&stk, results);
    // PrintStack(stk);
    // printf("err = %d\n", bin(err));

    // err = StackPop(&stk, results+1);
    // PrintStack(stk);
    // printf("err = %d\n", bin(err));

    // printf("%d, %d\n", results[0], results[1]);
    // return StackDtor(&stk);
}
