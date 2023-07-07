#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_to_tx.h"

#define TX_BUF_MAX_LENGHT 256

static void switch_to_wait_t(DataToTX* converter)
{
    converter->state = WAIT_T;
    converter->tx_buf_pointer = 0;
}

static void write_to_buf (DataToTX* converter, char byte_to_process)
{
    if (converter->tx_buf_pointer >= TX_BUF_MAX_LENGHT)
    {
        switch_to_wait_t(converter);
        return;
    }
    converter->tx_buf[converter->tx_buf_pointer] = byte_to_process;
    converter->tx_buf_pointer++;
}

void DTX_init(DataToTX* converter, 
    void(*send_tx) (void *state, char* tx_buf, size_t tx_buf_lenght), void *send_tx_arg, char* tx_buf, size_t tx_buf_lenght)
{
    converter->send_tx = send_tx;
    converter->send_tx_arg = send_tx_arg;
    converter->tx_buf = tx_buf;
    converter->tx_buf_lenght = tx_buf_lenght;
    switch_to_wait_t(converter);
}   

static void wait_t(DataToTX* converter, char byte_to_process)
{
    if (byte_to_process == 'T') 
    {
        write_to_buf(converter, byte_to_process);
        converter->state = WAIT_XS;
    }
}

static void wait_xs(DataToTX* converter, char byte_to_process)
{
    if ((byte_to_process == 'X') || (byte_to_process == 'S'))
    {
        write_to_buf(converter, byte_to_process);
        converter->state = COPY_PAYLOAD;
    }
    else 
    {
        switch_to_wait_t(converter);
    }
}

static void copy_payload(DataToTX* converter, char byte_to_process)
{
    if (((byte_to_process >= 0x30) && (byte_to_process <= 0x39)) ||
        ((byte_to_process >= 0x41) && (byte_to_process <= 0x46)))
    {
        write_to_buf(converter, byte_to_process);
    }

    else if (byte_to_process == 0x0D)
    {
        write_to_buf(converter, byte_to_process);
        converter->state = WAIT_LF;
    }

    else
    {
        switch_to_wait_t(converter);
    }
}

static void wait_lf(DataToTX* converter, char byte_to_process)
{
    if (byte_to_process == 0x0A)
    {
        write_to_buf(converter, byte_to_process);
        if (converter->send_tx != NULL)
        {
            converter->send_tx(converter->send_tx_arg, converter->tx_buf, converter->tx_buf_pointer);
        }
    }
    switch_to_wait_t(converter);
}

static void process_byte (DataToTX* converter, char byte_to_process)
{
    switch (converter->state)
    {
        case WAIT_T:
            wait_t(converter, byte_to_process);
            break;
        case WAIT_XS:
            wait_xs(converter, byte_to_process);
            break;
        case COPY_PAYLOAD:
            copy_payload(converter, byte_to_process);
            break;
        case WAIT_LF:
            wait_lf(converter, byte_to_process);
            break;
    }
}

void DTX_write_data(DataToTX* converter, char* buf, size_t lenght)
{
    for (int i = 0; i < lenght; i++)
    {
        process_byte(converter, buf[i]);
    }
}

void print_tx (void *state, char* tx_buf, size_t tx_buf_lenght)
{
    for (int i = 0; i <= tx_buf_lenght; i++)
    {
        printf("%c", tx_buf[i]);
    }
}

int main ()
{
    printf("\e[1;1H\e[2J");                                                                 // print

    DataToTX converter;
    char tx_buf_temp[TX_BUF_MAX_LENGHT];
    DTX_init(&converter, print_tx, NULL, tx_buf_temp, TX_BUF_MAX_LENGHT);

    //char test[] = {0x54, 0x53, 0x32, 0x31, 0x30, 0x30,  0x36, 0x38, 0x30, 0x33, 0x0D, 0x0A};          // test with CRC
    char test[] = {0x54, 0x53, 0x32, 0x31, 0x30, 0x30, 0x30, 0x33, 0x0D, 0x0A};                         // исходная с CRC
    //char test[] = {0x54, 0x58, 0x32, 0x31, 0x30, 0x31, 0x35, 0x37, 0x36, 0x38, 0x0D, 0x0A};           // test without CRC

    DTX_write_data(&converter, test, sizeof(test));
}