#include <stdio.h>
#include "data_to_tx.h"

    char test4[] = {0x32, 0x54, 0x58, 0x32, 0x31, 0x30, 0x30, 0x30}; 
    char test3[] = {0x20, 0x25, 0x54, 0x58, 0x32, 0x35, 0x39, 0x37, 0x30, 0x33, 0x0D, 0x0A};
    char test2[] = {0x30, 0x30, 0x33, 0x0D, 0x0A};
    char test1[] = {0x54, 0x53, 0x32, 0x31, 0x30};

void DTX_test(DataToTX* converter, char* test, size_t test_lenght)
{
    printf("\n\n\nInput: ");
    for (int i = 0; i <= test_lenght - 2; i++)
    {
        printf("%c", test[i]);
    }
    printf("\nReceived: ");
    DTX_write_data(converter, test, sizeof(test));
}