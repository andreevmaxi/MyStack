#include <cstdio>
#include <string>
#include "MaksStack.h"

int main()
    {
    DedStack_t stk1;
    std::string stkname1 = "stk1";
    stk1.Name = &stkname1;
    StackInit(&stk1);
    StackPush(&stk1, 10);
    printf("%d", PopStack(&stk1));

    StackDestruck(&stk1);
    return 0;
    }
