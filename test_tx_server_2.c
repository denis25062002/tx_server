#include <stdio.h>
#include <string.h>
#include "tx_server.h"

void responce_func_test(char* buf, size_t lenght)
{
    printf("\nResponce: ");
    for (size_t i = 0; i < lenght; i++)
    {
        printf("%c", buf[i]);
    }
}

void get_data_test(unsigned char id, unsigned char* args, size_t args_lenght,
    unsigned char* responce, size_t* responce_lenght, unsigned char* error)
{
    *error = 0;
    unsigned char test_responce[256] = "6789";
    printf("\nID: %X\nArgs: ", id);
    for (size_t i = 0; i < args_lenght; i++)
    {
        printf("%c", args[i]);
    }
    memcpy(responce, test_responce, 4);
    *responce_lenght = 4;
}



void main()
{
    char* test_input = "1212TX21011234\r\n";
    TxServer server;
    char tx_buf[256];
    size_t tx_buf_lenght;
    TxSrv_init(&server, 0x21, responce_func_test, get_data_test, tx_buf, tx_buf_lenght);
    TxSrv_write_data(&server, test_input, 16);
}