#include <stdio.h>
#include <assert.h>
#include "data_to_tx.h"

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

TEST_RESULT simple_test()
{
    char test[] = {0x20, 0x25, 0x54, 0x58, 0x32, 0x35, 0x39, 0x37, 0x30, 0x33, 0x0D, 0x0A};
    char test_result[] = "TX259703\r\n"; 
    DataToTX converter;
    char test_buf[100];
    char tx_buf[100];
    DTX_init(&converter, send_tx_test, test_buf, tx_buf, 100);
    DTX_write_data(&converter, test, sizeof(test));
    for (int i = 0; i < sizeof(test_result) - 1; i++)
    {
        if (test_result[i] != tx_buf[i])
        {
            return(FAILED);
        }
    } 
    return(OK);
}

TEST_RESULT CRC_err_test()
{
    char test[] = {0x20, 0x25, 0x54, 0x53, 0x32, 0x35, 0x39, 0x37, 0x30, 0x33, 0x0D, 0x0A};
    char test_result[] = "TS259709\r\n"; 
    DataToTX converter;
    char test_buf[100];
    char tx_buf[100];
    DTX_init(&converter, send_tx_test, test_buf, tx_buf, 100);
    DTX_write_data(&converter, test, sizeof(test));
    for (int i = 0; i < sizeof(test_result) - 1; i++)
    {
        if (test_result[i] != tx_buf[i])
        {
            return(FAILED);
        }
    } 
    return(OK);
}

TEST_RESULT CRC_ok_test()
{
    char test[] = {0x20, 0x25, 0x54, 0x53, 0x32, 0x35, 0x39, 0x37, 0x30, 0x39, 0x0D, 0x0A};
    char test_result[] = "TS259709\r\n"; 
    DataToTX converter;
    char test_buf[100];
    char tx_buf[100];
    DTX_init(&converter, send_tx_test, test_buf, tx_buf, 100);
    DTX_write_data(&converter, test, sizeof(test));
    for (int i = 0; i < sizeof(test_result) - 1; i++)
    {
        if (test_result[i] != tx_buf[i])
        {
            return(FAILED);
        }
    } 
    return(OK);
}

TEST_RESULT multiple_commands_test()
{
    TestBuf multiple_command_buf;
    test_buf_init(&multiple_command_buf);
    char test[] = {0x20, 0x25, 0x54, 0x58, 0x32, 0x35, 0x39, 0x37, 0x30, 0x33, 0x0D, 0x0A, 0x54, 0x58, 0x32, 0x35, 0x39, 0x37, 0x30, 0x32, 0x0D, 0x0A};
    char test_result[] = "TX259703\r\nTX259702\r\n"; 
    DataToTX converter;
    char tx_buf[100];
    DTX_init(&converter, send_to_test_buf, &multiple_command_buf, tx_buf, 100);
    DTX_write_data(&converter, test, sizeof(test));
    for (int i = 0; i < multiple_command_buf.buf_lenght - 1; i++)
    {
        if (test_result[i] != multiple_command_buf.buf[i])
        {
            return(FAILED);
        }
    } 
    return(OK);
}

TEST_RESULT incomplete_test()
{
    char test[] = {0x20, 0x25, 0x54, 0x58, 0x32, 0x39, 0x37, 0x30, 0x33, 0x0D};
    char test_result[] = ""; 
    DataToTX converter;
    char test_buf[100];
    char tx_buf[100];
    DTX_init(&converter, send_tx_test, test_buf, tx_buf, 100);
    DTX_write_data(&converter, test, sizeof(test));
    for (int i = 0; i < sizeof(test_result) - 1; i++)
    {
        if (test_result[i] != tx_buf[i])
        {
            return(FAILED);
        }
    } 
    return(OK);
}

TEST_RESULT overload_test()
{
    TestBuf multiple_command_buf;
    test_buf_init(&multiple_command_buf);
    char test[] = {0x20, 0x25, 0x54, 0x58, 0x32, 0x35, 0x39, 0x37, 0x30, 0x33, 0x0D, 0x0A, 0x54, 0x58, 0x32, 0x35, 0x39, 0x37, 0x30, 0x32, 0x0D, 0x0A};
    char test_result[] = "TX259703\r\nTX259702\r\n"; 
    DataToTX converter;
    char tx_buf[100];
    DTX_init(&converter, send_to_test_buf, &multiple_command_buf, tx_buf, 8);
    DTX_write_data(&converter, test, sizeof(test));
    if (multiple_command_buf.buf_lenght == 0)
        {
            return(OK);
        }

    return(FAILED);
}

void main ()
{
    assert(simple_test() == OK);
    assert(CRC_err_test() == FAILED);
    assert(CRC_ok_test() == OK);
    assert(multiple_commands_test() == OK);
    assert(incomplete_test() == OK);
    assert(overload_test() == OK);
}