#ifndef TX_SERVER
#define TX_SERVER
#include "data_to_tx.h"
#include "tx_to_structure.h"
typedef struct TxServer
{
    char ad;
    void(*responce_func) (char* buf, size_t lenght);
    void(*get_data) (unsigned char id, unsigned char* args, size_t args_lenght, unsigned char* responce, size_t max_responce_lenght);
} TxServer;

void TxSrv_init(TxServer* server, char ad, 
    void(*responce_func) (char* buf, size_t lenght),
    void(*get_data) (unsigned char id, unsigned char* args, size_t args_lenght,
    unsigned char* responce, size_t max_responce_lenght, unsigned char* error));

void TxSrv_write_data(TxServer* server, char* buf, size_t lenght);
#endif