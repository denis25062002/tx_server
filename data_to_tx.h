#ifndef DATA_TO_TX
#define DATA_TO_TX

typedef enum DTX_State
{
    WAIT_T = 0,
    WAIT_XS,
    COPY_PAYLOAD,
    WAIT_LF,
} DTX_State;

typedef struct DataToTX
{
    DTX_State state;
    size_t tx_buf_pointer;
    void(*send_tx) (void *state, char* tx_buf, size_t tx_buf_lenght);
    char* tx_buf;
    size_t tx_buf_lenght;
    void *send_tx_arg;
} DataToTX;

void DTX_init(DataToTX* converter, 
    void(*send_tx) (void *state, char* tx_buf, size_t tx_buf_lenght), void *state, char* tx_buf, size_t tx_buf_lenght);
void DTX_write_data(DataToTX* converter, char* buf, size_t lenght);
#endif