#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "data_to_tx.h"
#include "tx_to_structure.h"
#include "tx_server.h"

#define TEST_BUF_MAX_LENGTH 256

typedef struct TestBuf
{
    char buf[TEST_BUF_MAX_LENGTH];
    size_t buf_length;
} TestBuf;

void test_buf_init(TestBuf *buf)
{
    buf->buf_length = 0;
    memset(buf->buf, 0, sizeof(buf->buf));
}

void write_to_test_buf(TestBuf *buf, const char *input_buf, size_t input_buf_length)
{
    for (size_t i = 0; i < input_buf_length; i++)
    {
        buf->buf[buf->buf_length + i] = input_buf[i];
    }
    buf->buf_length += input_buf_length;
}

typedef enum TEST_RESULT { OK = 0, FAILED } TEST_RESULT;

void send_tx_test(void *state, char *tx_buf, size_t tx_buf_length)
{
    char *buf = (char *)state;
    memcpy(buf, tx_buf, tx_buf_length);
}

void send_to_test_buf(void *state, char *tx_buf, size_t tx_buf_length)
{
    TestBuf *buf = (TestBuf *)state;
    write_to_test_buf(buf, tx_buf, tx_buf_length);
}

TEST_RESULT simple_test()
{
    char test[] = {0x20, 0x25, 'T', 'X', '2', '5', '9', '7', '0', '3', '\r', '\n'};
    const char test_result[] = "TX259703\r\n"; 
    DataToTX converter;
    char test_buf[100] = {0};
    char tx_buf[100] = {0};
    DTX_init(&converter, send_tx_test, test_buf, tx_buf, 100);
    DTX_write_data(&converter, test, sizeof(test));

    return (memcmp(test_buf, test_result, strlen(test_result)) == 0) ? OK : FAILED;
}

TEST_RESULT CRC_ok_test()
{
    // 'T','S','2','5','9','7','0','9','\r','\n' -> XOR от "2597" = 0x09
    char test[] = {0x20, 0x25, 'T', 'S', '2', '5', '9', '7', '0', '9', '\r', '\n'};
    const char test_result[] = "TS259709\r\n"; 
    DataToTX converter;
    char test_buf[100] = {0};
    char tx_buf[100] = {0};
    DTX_init(&converter, send_tx_test, test_buf, tx_buf, 100);
    DTX_write_data(&converter, test, sizeof(test));

    return (memcmp(test_buf, test_result, strlen(test_result)) == 0) ? OK : FAILED;
}

TEST_RESULT CRC_err_test()
{
    // Неверная CRC '0', '0'
    char test[] = {0x20, 0x25, 'T', 'S', '2', '5', '9', '7', '0', '0', '\r', '\n'};
    DataToTX converter;
    char test_buf[100] = {0};
    char tx_buf[100] = {0};
    DTX_init(&converter, send_tx_test, test_buf, tx_buf, 100);
    DTX_write_data(&converter, test, sizeof(test));

    return (test_buf[0] == 0) ? OK : FAILED; // Не должен был вызваться callback
}

int main()
{
    printf("[RUNNING TESTS]\n");

    assert(simple_test() == OK);
    printf("  [OK] simple_test\n");

    assert(CRC_ok_test() == OK);
    printf("  [OK] CRC_ok_test\n");

    assert(CRC_err_test() == OK);
    printf("  [OK] CRC_err_test\n");

    printf("\n[ALL TESTS PASSED SUCCESSFULLY!]\n");
    return 0;
}