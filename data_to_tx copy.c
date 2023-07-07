#include <stdio.h>
#include <string.h>
#include "data_to_tx.h"

void DTX_init(DataToTX* converter, 
    void(*send_tx) (void *state, char* tx_buf, size_t tx_buf_lenght), void *send_tx_arg, char* tx_buf, size_t tx_buf_lenght)
{
    converter->send_tx = send_tx;
    converter->send_tx_arg = send_tx_arg;
    converter->tx_buf = tx_buf;
    converter->tx_buf_lenght = tx_buf_lenght;
}   

void DTX_write_data(DataToTX* converter, char* buf, size_t lenght)
{
    converter->tx_buf[0] = 'T';
    converter->send_tx(converter->send_tx_arg, converter->tx_buf, 1);
}

void print_tx (void *state, char* tx_buf, size_t tx_buf_lenght)
{
    for (int i = 0; i < tx_buf_lenght; i++)
    {
        printf("%c", tx_buf[i]);
    }
    printf("\n");
}

#define TX_BUF_MAX_LENGHT 10

int main ()
{
    DataToTX converter;
    char tx_buf_temp[TX_BUF_MAX_LENGHT];
    DTX_init(&converter, print_tx, NULL, tx_buf_temp, TX_BUF_MAX_LENGHT);
    char test[] = "TX2100\r\n";
    DTX_write_data(&converter, test, sizeof(test));
}