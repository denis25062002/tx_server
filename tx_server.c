#include <stdio.h>
#include "tx_server.h"

void send_tx(void* state, unsigned char* message_buf, size_t message_buf_lenght)
{
    unsigned char* data_responce;
    unsigned char* error;
    char responce_msg[MAX_RESPONCE_LENGHT];
    size_t* data_responce_lenght;
    txstructure txstruct;
    txstructure responce_txstruct;
    TxServer* server = (TxServer*) state;
    unsigned char data_buf[MAX_RESPONCE_LENGHT];
    txstruct.tts_data = data_buf;
    TTS_process_message(&txstruct, message_buf, message_buf_lenght);
    if (txstruct.tts_ad == server->ad)
    {
        server->get_data(txstruct.tts_id, txstruct.tts_data, txstruct.tts_data_lenght, data_responce, &data_responce_lenght, &error);
        TTS_init(&responce_txstruct, txstruct.tts_ad, txstruct.tts_id, data_responce, data_responce_lenght);
        TTS_process_structure(&responce_txstruct, responce_msg, 0);
        message msg;
        msg.buf = responce_msg;
        server->responce_func(responce_msg, msg.buf_lenght);
    }
}

void TxSrv_init(TxServer* server, char ad, 
    void(*responce_func) (char* buf, size_t lenght),
    void(*get_data) (unsigned char id, unsigned char* args, size_t args_lenght,
    unsigned char* responce, size_t* responce_lenght, unsigned char* error), char* tx_buf, size_t tx_buf_lenght)
{
    DTX_init(&server->converter, send_tx, server, tx_buf, tx_buf_lenght);
}

void TxSrv_write_data(TxServer* server, char* buf, size_t lenght)
{
    
}