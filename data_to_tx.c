#include <stdio.h>
#include <string.h>
#include "data_to_tx.h"

typedef enum ERR
{
    NO_ERR = 0,
    BUFF_OVERFLOW = 1
} ERR;

static int hex_char_to_int(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

static int char_to_int(char char_0, char char_1)
{
    int high = hex_char_to_int(char_0);
    int low = hex_char_to_int(char_1);
    if (high < 0 || low < 0) return -1;
    return (high << 4) | low;
}

static int check_CRC(DataToTX *converter)
{
    if (converter->tx_buf_pointer < 6) return 1;

    int crc_counted = 0;
    for (size_t i = 0; i < (converter->tx_buf_pointer - 6); i++)
    {
        crc_counted ^= (unsigned char)converter->tx_buf[i + 2];
    }

    int packet_crc = char_to_int(converter->tx_buf[converter->tx_buf_pointer - 4],
                                 converter->tx_buf[converter->tx_buf_pointer - 3]);

    return (crc_counted == packet_crc) ? 0 : 1;
}

static void switch_to_wait_t(DataToTX *converter)
{
    converter->state = WAIT_T;
    converter->tx_buf_pointer = 0;
}

static int write_to_buf(DataToTX *converter, char byte_to_process)
{
    if (converter->tx_buf_pointer >= converter->tx_buf_length)
    {
        return BUFF_OVERFLOW;
    }
    converter->tx_buf[converter->tx_buf_pointer++] = byte_to_process;
    return NO_ERR;
}

void DTX_init(DataToTX *converter, 
              void (*send_tx)(void *state, char *tx_buf, size_t tx_buf_length), 
              void *send_tx_arg, 
              char *tx_buf, 
              size_t tx_buf_length)
{
    converter->send_tx = send_tx;
    converter->send_tx_arg = send_tx_arg;
    converter->tx_buf = tx_buf;
    converter->tx_buf_length = tx_buf_length;
    switch_to_wait_t(converter);
}   

static ERR wait_t(DataToTX *converter, char byte_to_process)
{
    ERR err = NO_ERR;
    memset(converter->tx_buf, 0x00, converter->tx_buf_length);
    if (byte_to_process == 'T') 
    {
        err = write_to_buf(converter, byte_to_process);
        if (err) return err;
        converter->state = WAIT_XS;
    }
    return err;
}

static ERR wait_xs(DataToTX *converter, char byte_to_process)
{
    ERR err = NO_ERR;
    if (byte_to_process == 'X' || byte_to_process == 'S')
    {
        err = write_to_buf(converter, byte_to_process);
        if (err) return err;
        converter->state = COPY_PAYLOAD;
    }
    else 
    {
        switch_to_wait_t(converter);
    }
    return err;
}

static ERR copy_payload(DataToTX *converter, char byte_to_process)
{
    ERR err = NO_ERR;
    if ((byte_to_process >= '0' && byte_to_process <= '9') ||
        (byte_to_process >= 'A' && byte_to_process <= 'F'))
    {
        err = write_to_buf(converter, byte_to_process);
        if (err) return err;
    }
    else if (byte_to_process == '\r') // 0x0D
    {
        err = write_to_buf(converter, byte_to_process);
        if (err) return err;
        converter->state = WAIT_LF;
    }
    else
    {
        switch_to_wait_t(converter);
    }
    return err;
}

static ERR wait_lf(DataToTX *converter, char byte_to_process)
{
    ERR err = NO_ERR;
    if (byte_to_process == '\n') // 0x0A
    {
        err = write_to_buf(converter, byte_to_process);
        if (err) return err;

        if (converter->send_tx != NULL)
        {
            int is_s_frame = (converter->tx_buf[1] == 'S');
            int is_x_frame = (converter->tx_buf[1] == 'X');

            if ((is_s_frame && check_CRC(converter) == 0) || is_x_frame)
            {
                converter->send_tx(converter->send_tx_arg, converter->tx_buf, converter->tx_buf_pointer);
            }
        }
    }
    switch_to_wait_t(converter);
    return err;
}

static void process_byte(DataToTX *converter, char byte_to_process)
{
    ERR err = NO_ERR;
    switch (converter->state)
    {
        case WAIT_T:
            err = wait_t(converter, byte_to_process);
            break;
        case WAIT_XS:
            err = wait_xs(converter, byte_to_process);
            break;
        case COPY_PAYLOAD:
            err = copy_payload(converter, byte_to_process);
            break;
        case WAIT_LF:
            err = wait_lf(converter, byte_to_process);
            break;
    }
    if (err)
    {
        switch_to_wait_t(converter);
    }
}

void DTX_write_data(DataToTX *converter, const char *buf, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        process_byte(converter, buf[i]);
    }
}