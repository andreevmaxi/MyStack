#include <cstdio>
#include <cstdlib>
#include <string>
#include <cassert>

#ifdef _DEBUG
    #define DEB( code ) code
#else
    #define DEB( code )
#endif

typedef int StackElem_t;

const int ReallocConst      = 2;
const int SizeDecreeseConst = 10;

struct DedStack_t
{
    DEB(int CanaryLeft);
    int NowElem; // index of next element in data
    int SizeStack;  // size of used memory
    DEB(int CanaryDataLeft);
    StackElem_t* data; // data that needs to be saved
    DEB(int CanaryDataRight);
    DEB(unsigned long long StkHash);
    DEB(int Err);  // number of error !(USED ONLY WHILE DEBUGED)!
    // 0 - all good
    // 1 - if there isn't any memory
    // 2 - if in stack maybe a poison element
    // 3 - if size of stack is under the 0
    // 4 - if Canary isn't good
    // 5 - if data pointer in NULL
    // 6 - if hash isn't good
    DEB(std::string* Name); // name of our stack !(USED ONLY WHILE DEBUGED)!
    DEB(int CanaryRight);
};

bool NormalizeSize(DedStack_t* aStack);

bool StackInit(DedStack_t* aStack);

bool StackPush(DedStack_t* str, StackElem_t tmp);

void ASSERT_OK(DedStack_t* aStack, std::string NowFile, int Line, std::string FuncName);

void DUMP(DedStack_t* aStack, std::string NowFile, int Line, std::string FuncName);

bool IsEmptyStack(DedStack_t* aStack);

StackElem_t PopStack(DedStack_t* aStack);

void StuckDestruck(DedStack_t* aStack);

bool StackResize(DedStack_t* aStack);

unsigned long long StackHash(DedStack_t* aStack);

int main()
    {
    FILE* f = fopen("errors.txt", "w");
    fclose(f);

    DedStack_t dStack1 = {};

    DEB(std::string TmpName1  = "dStack1");
    DEB(dStack1.Name = &TmpName1);

    StackInit(&dStack1);
    StackPush(&dStack1, 10);
    StackPush(&dStack1, -230);
    StackPush(&dStack1, 20);
    StackPush(&dStack1, 30);
    StackPush(&dStack1, 50);
    for(int i = 0; i < 40; ++i)
        {
        StackPush(&dStack1, 30);
        }

    for(int i = 0; i < 40; ++i)
        {
        PopStack(&dStack1);
        }

    PopStack(&dStack1);
    PopStack(&dStack1);
    PopStack(&dStack1);
    PopStack(&dStack1);

    StackElem_t x = PopStack(&dStack1);
    printf("x: %d and Is Stack empty: %d", x, IsEmptyStack(&dStack1));
    StuckDestruck(&dStack1);

    return 0;
    }

unsigned long long StackHash(DedStack_t* aStack)
    {

    assert(aStack != NULL);
    assert(&aStack->SizeStack != NULL);
    assert(&aStack->data != NULL);

    const unsigned long long MaxHashKey = 0xbad1337C;

	const int ByteShift = 36;

    unsigned long long StackKey = MaxHashKey * aStack->SizeStack;

    unsigned long long HashSum = 0;

    StackElem_t tmp = 0;

    // adding NowElem to hash

    tmp = aStack->NowElem;
    tmp *= MaxHashKey;
    tmp ^= tmp >> ByteShift;
    tmp *= MaxHashKey;

    HashSum *= MaxHashKey;
    HashSum ^= tmp;

    // adding SizeStack to hash

    tmp = aStack->SizeStack;
    tmp *= MaxHashKey;
    tmp ^= tmp >> ByteShift;
    tmp *= MaxHashKey;

    HashSum *= MaxHashKey;
    HashSum ^= tmp;

    // adding CanaryDataLeft to hash

    tmp = aStack->CanaryDataLeft;
    tmp *= MaxHashKey;
    tmp ^= tmp >> ByteShift;
    tmp *= MaxHashKey;

    HashSum *= MaxHashKey;
    HashSum ^= tmp;

    // adding all Data to hash

    for (int i = 0; i < aStack->SizeStack; ++i)
        {
        tmp = aStack->CanaryDataLeft;
        tmp *= MaxHashKey;
        tmp ^= tmp >> ByteShift;
        tmp *= MaxHashKey;

        HashSum *= MaxHashKey;
        HashSum ^= tmp;
        }

    // adding CanaryDataRight to hash

    tmp = aStack->CanaryDataRight;
    tmp *= MaxHashKey;
    tmp ^= tmp >> ByteShift;
    tmp *= MaxHashKey;

    HashSum *= MaxHashKey;
    HashSum ^= tmp;

    return HashSum;
    }

bool IsEmptyStack(DedStack_t* aStack)
    {

    assert(aStack != NULL);

    DEB(ASSERT_OK(aStack, __FILE__, __LINE__, __PRETTY_FUNCTION__));
    if(aStack->NowElem == 0)
        {
        return 1;
        }
    return 0;
    }

StackElem_t PopStack(DedStack_t* aStack)
    {

    assert(aStack != NULL);

    DEB(ASSERT_OK(aStack, __FILE__, __LINE__, __PRETTY_FUNCTION__));

    if( aStack->SizeStack > 25 && (double)aStack->SizeStack / (double)aStack->NowElem > 1.5)
    {
        NormalizeSize(aStack);
    }

    if(aStack->NowElem > 0)
        {
            StackElem_t tmp = *( aStack->data + ( --aStack->NowElem ) );
            *( aStack->data + aStack->NowElem ) = -230;
            DEB(aStack->StkHash   = StackHash(aStack));
            return (tmp);
        }

    aStack->Err = 3;
    return (-230);
    }

bool StackInit(DedStack_t* aStack)
    {

    assert(aStack != NULL);

    DEB(aStack->CanaryLeft  = 230);
    DEB(aStack->CanaryRight = 230);
    DEB(aStack->CanaryDataRight = 230);
    DEB(aStack->CanaryDataLeft = 230);
    aStack->SizeStack = 4;
    aStack->data = (StackElem_t*) calloc(aStack->SizeStack, sizeof(StackElem_t));
    aStack->NowElem   = 0;
    DEB(aStack->StkHash   = StackHash(aStack));

    DEB(ASSERT_OK(aStack, __FILE__, __LINE__, __PRETTY_FUNCTION__));
    for(int i = 0; i < aStack->SizeStack; ++i)
        {
            *(aStack->data + i) = -230; // poison element
        }
    DEB(aStack->Err = 0);
    DEB(aStack->StkHash   = StackHash(aStack));
    DEB(ASSERT_OK(aStack, __FILE__, __LINE__, __PRETTY_FUNCTION__));

    return 1;

    }

bool StackPush(DedStack_t* aStack, StackElem_t tmp)
    {
    assert(aStack != NULL);

    DEB(ASSERT_OK(aStack, __FILE__, __LINE__, __PRETTY_FUNCTION__));

    if(aStack->NowElem + 1 > aStack->SizeStack)
        {
        StackResize(aStack);
        DEB(ASSERT_OK(aStack, __FILE__, __LINE__, __PRETTY_FUNCTION__));
        }
    if(aStack->SizeStack < 0)
        {
        aStack->Err = 3;
        DEB(ASSERT_OK(aStack, __FILE__, __LINE__, __PRETTY_FUNCTION__));
        }
    *(aStack->data + aStack->NowElem) = tmp;
    ++aStack->NowElem;
    DEB(aStack->StkHash   = StackHash(aStack));

    DEB(ASSERT_OK(aStack, __FILE__, __LINE__, __PRETTY_FUNCTION__));

    return 1;
    }

void ASSERT_OK(DedStack_t* aStack, std::string NowFile, int Line, std::string FuncName)
    {
    assert(aStack != NULL);
    assert(&aStack->StkHash != NULL);

    bool FlagOfSecondError = 0;

    if(aStack->StkHash != StackHash(aStack))
        {
        aStack->Err = 6;
        DUMP(aStack, NowFile, Line, FuncName);
        }

    if(aStack->data == NULL)
        {
        aStack->Err = 5;
        DUMP(aStack, NowFile, Line, FuncName);
        }

    if(aStack->CanaryRight != 230 || aStack->CanaryLeft != 230)
        {
        aStack->Err = 4;
        DUMP(aStack, NowFile, Line, FuncName);
        }

    if(aStack->CanaryDataRight != 230 || aStack->CanaryDataLeft != 230)
        {
        aStack->Err = 4;
        DUMP(aStack, NowFile, Line, FuncName);
        }

    for(int i = 0; i < aStack->NowElem; ++i)
        {
        if( *(aStack->data + i) == -230)
            {
            FlagOfSecondError = 1;
            }
        }

    if(FlagOfSecondError)
        {
        aStack->Err = 2;
        } else
        {
        aStack->Err = 0;
        }

    if(aStack->Err != 0)
        {
        DUMP(aStack, NowFile, Line, FuncName);
        }
    return;
    }

void DUMP(DedStack_t* aStack, std::string NowFile, int Line, std::string FuncName)
    {
    FILE* f = fopen("errors.txt", "a");

    fprintf(f, "StuckDump with pointer: [%o]\n", aStack);
    fprintf(f, "Printed from: %s in line: %d in FUCtion: %s\n", NowFile.c_str(), Line, FuncName.c_str());

    fclose(f);
    f = fopen("errors.txt", "a");

    std::string status = {};

    if(aStack->Err != 0)
        {
        if(aStack->Err == 2)
            {
            status = "UNK"; // unknown
            } else
            {
            status = "BAD";
            }
        } else
        {
        status = "OK";
        }

    fprintf(f, "Stack \"%s\" [%o](%s)\n", (*aStack->Name).c_str(), (int)aStack, status.c_str());
    fprintf(f, "    {\n");
    fprintf(f, "    size = %d\n", aStack->SizeStack);
    fprintf(f, "    data[%d] = [%o]\n", aStack->SizeStack, (int)&aStack->data);
    fprintf(f, "    memorized hash    = [%o] \n", aStack->StkHash);
    if(StackHash(aStack) != aStack->StkHash)
        {
        fprintf(f, "*!!!hash of stack now = [%o]  !!!*\n", StackHash(aStack));
        }

    fprintf(f, "        {\n");
    fclose(f);
    f = fopen("errors.txt", "a");

    for(int i = 0; i < aStack->SizeStack; ++i)
        {
        if(i < aStack->NowElem)
            {
            fprintf(f, "       *");
            }else
            {
            fprintf(f, "        ");
            }
        fprintf(f, "[%d] = %d", i, *(aStack->data + i));
        if( *(aStack->data + i) == -230)
            {
            fprintf(f, "[MB Poison]");
            }
        fprintf(f, "\n");
        }
    fprintf(f, "        }\n");
    fprintf(f, "err = %d (%s)\n", aStack->Err, status.c_str());
    fprintf(f, "    }\n\n");

    fclose(f);
    return;
    }

void StuckDestruck(DedStack_t* aStack)
    {
    DEB(ASSERT_OK(aStack, __FILE__, __LINE__, __PRETTY_FUNCTION__));

    while (!IsEmptyStack)
        {
        PopStack(aStack);
        DEB(ASSERT_OK(aStack, __FILE__, __LINE__, __PRETTY_FUNCTION__));
        }

    return;
    }

 bool StackResize(DedStack_t* aStack)
    {
        DEB(ASSERT_OK(aStack, __FILE__, __LINE__, __PRETTY_FUNCTION__));

        aStack->SizeStack *= ReallocConst;
        aStack->data = (StackElem_t*) realloc(aStack->data, sizeof(StackElem_t) * aStack->SizeStack);
        if(aStack->data == NULL)
            {
            aStack->Err = 1;
            DEB(ASSERT_OK(aStack, __FILE__, __LINE__, __PRETTY_FUNCTION__));
            }
        for(int i = aStack->NowElem; i < aStack->SizeStack; ++i)
            {
            *(aStack->data + i) = -230;
            }
        DEB(aStack->StkHash   = StackHash(aStack));
        DEB(ASSERT_OK(aStack, __FILE__, __LINE__, __PRETTY_FUNCTION__));

        return 1;
    }

bool NormalizeSize(DedStack_t* aStack)
    {
    DEB(ASSERT_OK(aStack, __FILE__, __LINE__, __PRETTY_FUNCTION__));

    aStack->SizeStack -= SizeDecreeseConst;
    aStack->data = (StackElem_t*) realloc(aStack->data, sizeof(StackElem_t) * aStack->SizeStack);

    if(aStack->data == NULL)
        {
        aStack->Err = 1;
        DEB(ASSERT_OK(aStack, __FILE__, __LINE__, __PRETTY_FUNCTION__));
        }

    DEB(aStack->StkHash   = StackHash(aStack));

    DEB(ASSERT_OK(aStack, __FILE__, __LINE__, __PRETTY_FUNCTION__));
    }
