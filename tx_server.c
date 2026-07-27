#include <stdio.h>
#include "tx_server.h"

static void send_tx(void *state, char *message_buf, size_t message_buf_length)
{
    TxServer *server = (TxServer *)state;
    if (!server) return;

    unsigned char data_buf[MAX_RESPONSE_LENGTH];
    txstructure txstruct;
    txstruct.tts_data = data_buf;

    TTS_process_message(&txstruct, message_buf, message_buf_length);

    if (txstruct.tts_ad == server->ad)
    {
        unsigned char response_payload[MAX_RESPONSE_LENGTH];
        size_t response_payload_len = 0;
        unsigned char error = 0;

        if (server->get_data)
        {
            server->get_data(txstruct.tts_id, txstruct.tts_data, txstruct.tts_data_length,
                             response_payload, &response_payload_len, &error);
        }

        if (error == 0)
        {
            txstructure response_txstruct;
            TTS_init(&response_txstruct, txstruct.tts_ad, txstruct.tts_id, response_payload, response_payload_len);

            char response_msg_buf[MAX_RESPONSE_LENGTH];
            message msg;
            TTS_message_init(&msg, response_msg_buf, 0);

            TTS_process_structure(&response_txstruct, &msg, 0);

            if (server->response_func)
            {
                server->response_func(msg.buf, msg.buf_length);
            }
        }
    }
}

void TxSrv_init(TxServer *server, unsigned char ad, 
                void (*response_func)(char *buf, size_t length),
                void (*get_data)(unsigned char id, unsigned char *args, size_t args_length,
                                 unsigned char *response, size_t *response_length, unsigned char *error), 
                char *tx_buf, size_t tx_buf_length)
{
    server->ad = ad;
    server->response_func = response_func;
    server->get_data = get_data;
    DTX_init(&server->converter, send_tx, server, tx_buf, tx_buf_length);
}

void TxSrv_write_data(TxServer *server, const char *buf, size_t length)
{
    DTX_write_data(&server->converter, buf, length);
}