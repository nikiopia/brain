// ===== INCLUDES ===== //
#include <stdio.h>

// ===== DEFINES ===== //
#define MEM_SIZE        (1024)
#define MAX_OPERATIONS  (10000)

#define LINE_LENGTH     (80)
#define DATA_PADDING    (8)

// ===== TYPEDEFS ===== //
typedef unsigned char uint8;

// ===== FUNCTION PROTOTYPES ===== //
int stackPush(int*, int*, int);
int stackPop(int*, int*, int*);
int bf_processor(uint8[MEM_SIZE], int*, char[MEM_SIZE], int*, int[MEM_SIZE],
        int*, int);
void drawUI(uint8[MEM_SIZE], int*, char[MEM_SIZE], int*);

// ===== HELPER FUNCTIONS ===== //
void drawUI(uint8 dataMem[MEM_SIZE], int *dataPtr, char instMem[MEM_SIZE],
        int *instPtr)
{
    if (!dataMem || !dataPtr || !instMem || !instPtr)
    {
        printf("drawUI - null pointer(s)\n");
        return;
    }
    int activeDP = *dataPtr;
    int activeIP = *instPtr;
    if (activeDP < 0 || activeDP >= MEM_SIZE)
    {
        printf("drawUI - DP out of bounds\n");
        return;
    }
    if (activeIP < 0 || activeIP >= MEM_SIZE)
    {
        printf("drawUI - IP out of bounds\n");
        return;
    }

    // Blank slate pass
    char instString1[LINE_LENGTH + 1];
    char instString2[LINE_LENGTH + 1];
    char dataString1[LINE_LENGTH + 1];
    char dataString2[LINE_LENGTH + 1];
    char write = ' ';
    for (int i = 0; i < LINE_LENGTH + 1; i++)
    {
        if (i == LINE_LENGTH)
        {
            write = 0;
        }
        instString1[i] = write;
        instString2[i] = write;
        dataString1[i] = write;
        dataString2[i] = write;
    }

    // Populate instruction strings
    int writeIndex;
    for (int i = 0; i < LINE_LENGTH; i++)
    {
        writeIndex = i - (LINE_LENGTH / 2) + activeIP;
        if (writeIndex < 0) { continue; }
        if (instMem[writeIndex] == 0) { break; }

        instString1[i] = instMem[writeIndex];
    }
    instString2[LINE_LENGTH / 2] = '*';

    // Populate data strings
    char hexLookup[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C',
        'D','E','F'};
    int byteIndex;
    for (int i = 0; i < (2 * DATA_PADDING + 1); i++)
    {
        writeIndex = 3 * (i - DATA_PADDING) + (LINE_LENGTH / 2);
        if (writeIndex < 0 || writeIndex >= LINE_LENGTH) { continue; }

        byteIndex = activeDP - DATA_PADDING + i;
        if (byteIndex < 0 || byteIndex >= MEM_SIZE)
        {
            dataString1[writeIndex] = 'F';
            dataString1[writeIndex + 1] = 'F';
            continue;
        }

        dataString1[writeIndex] = hexLookup[(dataMem[byteIndex] & 0xF0) >> 4];
        dataString1[writeIndex + 1] = hexLookup[dataMem[byteIndex] & 0xF];
    }
    dataString2[LINE_LENGTH / 2] = '*';
    dataString2[LINE_LENGTH / 2 + 1] = '*';

    // Do the printout
    printf("<EVAL>\nProgram Memory:\n(%s)\n(%s)\n\nInstruction Memory:\n(%s)\n(%s)\n",
        dataString1,dataString2,instString1,instString2);
}

int stackPush(int *index, int *stack, int value)
{
    // Parameter checks
    if (!index || !stack)
    {
        printf("stackPush - null pointer(s)\n");
        return 1;
    }
    int activeIndex = *index;
    if (activeIndex < 0 || activeIndex >= MEM_SIZE)
    {
        printf("stackPush - stack index < 0 or >= MEM_SIZE - check brackets\n");
        return 1;
    }

    // Check for empty location at activeIndex
    if (stack[activeIndex] != -1)
    {
        printf("stackPush - push not possible - check brackets\n");
        return 1;
    }

    stack[activeIndex] = value;
    if (activeIndex < MEM_SIZE - 1)
    {
        *index = activeIndex + 1;
    }
    return 0;
}

int stackPop(int *index, int *stack, int *value)
{
    // Parameter checks
    if (!index || !stack || !value)
    {
        printf("stackPop - null pointer(s)\n");
        return 1;
    }
    int activeIndex = *index;
    if (activeIndex <= 0 || activeIndex >= MEM_SIZE)
    {
        printf("stackPop - stack index <= 0 or >= MEM_SIZE - check brackets\n");
        return 1;
    }

    // Does the index need adjusted to read data from stack?
    if ((activeIndex < MEM_SIZE - 1) || (activeIndex == MEM_SIZE - 1 && 
        stack[activeIndex] < 0))
    {
        activeIndex--;
        *index = activeIndex;
    }

    *value = stack[activeIndex];
    stack[activeIndex] = -1;
    return 0;
}

/*
 * operation table
 * 0 - add
 * 1 - subtract
 * 2 - left shift
 * 3 - right shift
 * 4 - begin bracket
 * 5 - end bracket
 * 6 - output value (decimal)
 * 7 - read one byte of input -- TODO
 */
int bf_processor(uint8 dataMem[MEM_SIZE], int *dataPtr, char instMem[MEM_SIZE],
        int *instPtr, int jumpTable[MEM_SIZE], int *running, int debug)
{
    // Verify arguments
    // Null pointer checks
    if (!dataMem || !dataPtr || !instMem || !instPtr || !jumpTable || !running)
    {
        printf("bf_processor - null pointer(s)\n");
        return 1;
    }
    // Get local copies of data/inst pointer values
    int activeDP = *dataPtr;
    int activeIP = *instPtr;
    // Check data/inst pointer values
    if (activeDP < 0 || activeDP >= MEM_SIZE)
    {
        printf("bf_processor - DP out of bounds\n");
        return 1;
    }
    if (activeIP < 0 || activeIP >= MEM_SIZE)
    {
        printf("bf_processor - IP out of bounds\n");
        return 1;
    }

    // Determine operation
    char op = instMem[activeIP];
    if (debug)
    {
        printf("IP=%i, @IP=0x%02X, DP=%i, @DP=0x%02X\n",activeIP,op,activeDP,
            dataMem[activeDP]);
    }

    // Perform operation
    switch (op)
    {
        case '+':
            dataMem[activeDP] = dataMem[activeDP] + 1;            
            break;
        case '-':
            dataMem[activeDP] = dataMem[activeDP] - 1;
            break;
        case '<':
            if (activeDP > 0)
            {
                --activeDP;
            }
            break;
        case '>':
            if (activeDP < MEM_SIZE - 1)
            {
                ++activeDP;
            }
            break;
        case '[':
            if (dataMem[activeDP] == 0)
            {
                activeIP = jumpTable[activeIP];
            }
            break;
        case ']':
            if (dataMem[activeDP] != 0)
            {
                activeIP = jumpTable[activeIP];
            }
            break;
        case '.':
            if (debug)
            {
                printf("0x%02X\n",dataMem[activeDP]);
            }
            else
            {
                printf("%c",dataMem[activeDP]);
            }
            break;
        default:
            printf("Halting.\n");
            *running = 0;
    }

    // Ensure pointed values updated
    *instPtr = activeIP;
    *dataPtr = activeDP;
    // Increment instruction pointer
    if (activeIP < MEM_SIZE - 1)
    {
        *instPtr = ++activeIP;
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        printf("<DEBUG> Enabled\n");
    }
    printf("<SETUP> Beginning...\n");
    // Init instruction memory and data memory
    char instMem[MEM_SIZE];
    uint8 dataMem[MEM_SIZE];
    int instPointer = 0;
    int dataPointer = 0;
    for (int i = 0; i < MEM_SIZE; i++)
    {
        instMem[i] = 0;
        dataMem[i] = 0;
    }

    // Attempt code file open
    FILE *codeFile = fopen("code.bf","rb");
    if (!codeFile)
    {
        printf("main - Couldnt open code.bf\n");
        return 1;
    }
    rewind(codeFile);

    // Read file contents into memory (bounded)
    printf("File 'code.bf' contents:\n");
    char inputChar;
    int writeIndex = 0;
    char safeChars[] = {'+','-','<','>','.',',','[',']'};
    for (int i = 0; i < MEM_SIZE; i++)
    {
        if (feof(codeFile) || ferror(codeFile)) { break; }

        // Get and check char
        inputChar = fgetc(codeFile);
        int charIsSafe = 0;
        for (int j = 0; j < 8; j++)
        {
            if (inputChar == safeChars[j])
            {
                charIsSafe = 1;
                break;
            }
        }

        if (!charIsSafe) { continue; }

        // Char is OK
        instMem[writeIndex] = inputChar;
        printf("%c",inputChar);
        writeIndex++;
    }
    printf("\n");
    fclose(codeFile);

    // Prep for building jump table
    int jumpTable[MEM_SIZE];
    int jumpStack[MEM_SIZE];
    int stackIndex = 0;
    for (int i = 0; i < MEM_SIZE; i++)
    {
        jumpTable[i] = -1;
        jumpStack[i] = -1;
    }

    // Try to build jump table
    char activeChar;
    int returnValue, stackContents;
    for (int i = 0; i < MEM_SIZE; i++)
    {
        activeChar = instMem[i];
        if (activeChar == 0) { break; }
        if (activeChar == '[')
        {
            returnValue = stackPush(&stackIndex, jumpStack, i);
            if (returnValue) { return 1; }
        }
        if (activeChar == ']')
        {
            returnValue = stackPop(&stackIndex, jumpStack, &stackContents);
            if (returnValue) { return 1; }

            jumpTable[i] = stackContents;
            jumpTable[stackContents] = i;
        }
    }
    if (stackIndex != 0)
    {
        printf("Error - Detected unbalanced brackets in code\n");
        return 1;
    }

    if (argc > 1)
    {
        printf("<DEBUG> Printing jumpTable sample:\n");
        for (int i = 0; i < 20; i++)
        {
            printf("%i ",jumpTable[i]);
        }
        printf("\n");
    }

    int operationCount = 0;
    int running = 1;
    printf("<EVAL> Beginning...\n");
    if (argc > 1)
    {
        printf("<DEBUG> Using verbose execution\n");
    }
    while (running && operationCount < MAX_OPERATIONS)
    {
        if (argc > 1)
        {
            drawUI(dataMem, &dataPointer, instMem, &instPointer);

            while ((activeChar = (char)(fgetc(stdin))))
            {
                if (activeChar == 0x0a) { break; }
            }
        }

        returnValue = bf_processor(dataMem, &dataPointer, instMem,
            &instPointer, jumpTable, &running, argc>1);
        if (returnValue) { return 1; }
        operationCount++;
    }

    return 0;
}
