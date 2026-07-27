#ifndef DATA_TO_TX_H
#define DATA_TO_TX_H

#include <stddef.h>

typedef enum DTX_State
{
    WAIT_T = 0,
    WAIT_XS,
    COPY_PAYLOAD,
    WAIT_LF
} DTX_State;

typedef struct DataToTX
{
    DTX_State state;
    size_t tx_buf_pointer;
    void (*send_tx)(void *state, char *tx_buf, size_t tx_buf_length);
    char *tx_buf;
    size_t tx_buf_length;
    void *send_tx_arg;
} DataToTX;

void DTX_init(DataToTX *converter, 
              void (*send_tx)(void *state, char *tx_buf, size_t tx_buf_length), 
              void *send_tx_arg, 
              char *tx_buf, 
              size_t tx_buf_length);

void DTX_write_data(DataToTX *converter, const char *buf, size_t length);

#endif // DATA_TO_TX_H