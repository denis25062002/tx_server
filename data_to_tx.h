#ifndef DATA_TO_TX
#define DATA_TO_TX

typedef struct DataToTX
{
    void(*send_tx) (void *state, char* tx_buf, size_t tx_buf_lenght);
    char* tx_buf;
    size_t tx_buf_lenght;
    void *send_tx_arg;
} DataToTX;

void DTX_init(DataToTX* converter, 
    void(*send_tx) (void *state, char* tx_buf, size_t tx_buf_lenght), void *state, char* tx_buf, size_t tx_buf_lenght);

void DTX_write_data(DataToTX* converter, char* buf, size_t lenght);
#endif