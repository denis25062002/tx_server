#ifndef TX_SERVER_H
#define TX_SERVER_H

#include "data_to_tx.h"
#include "tx_to_structure.h"

#define MAX_RESPONSE_LENGTH 256

typedef struct TxServer
{
    unsigned char ad;
    void (*response_func)(char *buf, size_t length);
    void (*get_data)(unsigned char id, unsigned char *args, size_t args_length,
                     unsigned char *response, size_t *response_length, unsigned char *error);
    DataToTX converter;
} TxServer;

void TxSrv_init(TxServer *server, unsigned char ad, 
                void (*response_func)(char *buf, size_t length),
                void (*get_data)(unsigned char id, unsigned char *args, size_t args_length,
                                 unsigned char *response, size_t *response_length, unsigned char *error), 
                char *tx_buf, size_t tx_buf_length);

void TxSrv_write_data(TxServer *server, const char *buf, size_t length);

#endif // TX_SERVER_H