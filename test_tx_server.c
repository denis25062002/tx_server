#include <stdio.h>
#include <assert.h>
#include "data_to_tx.h"
#include "tx_to_structure.h"
#include "tx_server.h"

#define TEST_BUF_MAX_LENGHT 256

typedef struct TestBuf
{
    char buf[TEST_BUF_MAX_LENGHT];
    size_t buf_lenght;
}   TestBuf;

void test_buf_init(TestBuf* buf)
{
    buf->buf_lenght = 0;
}

void write_to_test_buf (TestBuf* buf, char* input_buf, size_t input_buf_lenght)
{
    for (size_t i = 0; i < (input_buf_lenght); i++)
    {
        buf->buf[buf->buf_lenght + i] = input_buf[i];
    }
    buf->buf_lenght = buf->buf_lenght + input_buf_lenght;
}

typedef enum TEST_RESULT
{
    OK = 0,
    FAILED,
} TEST_RESULT;

void send_tx_test (void *state, char* tx_buf, size_t tx_buf_lenght)
{
    char* buf = (char*) state;
    for (int i = 0; i < tx_buf_lenght; i++)
    {
        buf[i] = tx_buf[i];
    }
}

void send_to_test_buf(void *state, char* tx_buf, size_t tx_buf_lenght)
{
    TestBuf* buf = (TestBuf*) state;
    write_to_test_buf(buf, tx_buf, tx_buf_lenght);
}

void get_data_test(unsigned char id, unsigned char* args, size_t args_lenght,
    unsigned char* responce, size_t max_responce_lenght, unsigned char* error)
{
    unsigned char args_compare[100];
    unsigned char responce_0x03[3];
    responce_0x03[0] = 0x15;
    responce_0x03[1] = 0x34;
    size_t responce_0x03_lenght = 2;
    args_compare[0] = 0x03;
    int args_compared = 0;
    switch (id) 
    {
        case 0x97:
            for (int i = 0; i < args_lenght; i++)
            {
                if (args_compare[i] != args[i])
                {
                    args_compared++;
                }
            }
            break;
        default:
            break;
    }
    if (args_compared == 0)
    {
        for (int i = 0; i < responce_0x03_lenght; i++)
        {
            responce[i] = responce_0x03[i];
        }
        args_lenght = responce_0x03_lenght;
    }
}

void responce_func_test (char* buf, size_t lenght)
{
    printf("\n\nResponce to Client: ");
    for (int i = 0; i < lenght; i++)
    {
        printf("%c", buf[i]);
    }
}

void TxSrv_init(TxServer* server, char ad, 
    void(*responce_func) (char* buf, size_t lenght),
    void(*get_data) (unsigned char id, unsigned char* args, size_t args_lenght,
    unsigned char* responce, size_t max_responce_lenght, unsigned char* error))
{
    server->ad = ad;
    get_data = get_data_test;
    responce_func = responce_func_test;
}

void test_tx_server()
{
    char error_test[3];
    char test[] = {0x38, 0x39, 0x54, 0x58, 0x32, 0x31, 0x39, 0x37, 0x30, 0x33, 0x0D, 0x0A};
    char test_buf[100];
    char tx_buf[100];
    char ad = 0x21;
    unsigned char tts_ad;
    unsigned char tts_id;
    unsigned char tts_data[100];
    int need_crc = 0;
    size_t tts_data_lenght;
    DataToTX converter;
    TxServer server;
    message msg;
    txstructure txstruct;
    DTX_init(&converter, send_tx_test, test_buf, tx_buf, 100);
    TTS_init(&txstruct, tts_ad, tts_id, tts_data, tts_data_lenght);
    //TxSrv_init(&server, ad, get_data_test, responce_func_test);
    
    DTX_write_data(&converter, test, sizeof(test));
    printf("\nInput: ");
    for (int i = 0; i < sizeof(test); i++)
    {
        printf("%c", test[i]);
    }
    printf("\nMessage: ");
    for (int i = 0; i < 10; i++)
    {
        printf("%c", tx_buf[i]);
    }
    
    TTS_process_message(&txstruct, tx_buf, 10);
    printf("\nProcessed message: \nAD: %X\nID: %X\nDATA: ", txstruct.tts_ad, txstruct.tts_id);
    for (int i = 0; i < txstruct.tts_data_lenght; i++)
    {
        printf("%X", txstruct.tts_data[i]);
    }
    get_data_test(txstruct.tts_id, txstruct.tts_data, txstruct.tts_data_lenght,
    txstruct.tts_data, txstruct.tts_data_lenght, error_test);

    printf("\n\nGet_data: ");
    for (int i = 0; i < 2; i++)
    {
        printf("%X ", txstruct.tts_data[i]);
    }
    txstruct.tts_data_lenght = 2;

    TTS_process_structure(&txstruct, &msg, need_crc);
    responce_func_test(msg.buf, msg.buf_lenght);
}

void main ()
{
    test_tx_server();
}