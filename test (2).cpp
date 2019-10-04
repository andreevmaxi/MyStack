#include <cstdio>
#include <string>
#include "MaksStack.h"


int main()
    {
    FILE* f = fopen("errors.txt", "w+");
    fprintf(f, "sosat\n");
    fclose(f);

    DedStack_t stk1;
    std::string stkname1 = "stk1";
    stk1.Name = &stkname1;
    StackInit(&stk1);
    StackPush(&stk1, 10);
    StackPush(&stk1, -230);
    StackPush(&stk1, -230);
    printf("%d", PopStack(&stk1));

    StackDestruck(&stk1);

    return 0;
    }
