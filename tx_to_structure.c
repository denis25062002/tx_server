#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "tx_to_structure.h"

typedef struct
{
    char byte_a, byte_b;
} decoded_bytes;

void TTS_init(txstructure* txstruct,
    unsigned char tts_ad, unsigned char tts_id, unsigned char* tts_data, size_t tts_data_lenght)
{
    txstruct->tts_ad = tts_ad;
    txstruct->tts_id = tts_id;
    txstruct->tts_data = tts_data;
    txstruct->tts_data_lenght = tts_data_lenght;
}

void TTS_message_init(message* msg,
     char* buf, size_t buf_lenght)
{
    msg->buf = buf;
    msg->buf_lenght = buf_lenght;
};

decoded_bytes* TTS_decode(unsigned char byte_to_decode)
{
    decoded_bytes *dcd_bts;
    decoded_bytes decoded_bytes;
    char byte_temp[2];
    sprintf(byte_temp, "%X", byte_to_decode);
    decoded_bytes.byte_a = byte_temp[0];
    decoded_bytes.byte_b = byte_temp[1];
    dcd_bts = &decoded_bytes;
}

unsigned char TTS_code(char byte_1, char byte_2)
{
    char byte_a, byte_b;
    char byte_temp[2];
    sprintf(byte_temp, "%c%c", byte_1, byte_2);
    int result = strtol(byte_temp, NULL, 16);
    unsigned char byte_result = result;
    return byte_result;
}

static void TTS_make_crc(message* msg)
{
    int crc_counted = 0;
    char crc_temp[2];
    char crc_first_temp[2];
    for (size_t i = 2; i < msg->buf_lenght; i++)
    {
        crc_counted = crc_counted ^ msg->buf[i];
    }
    sprintf(crc_temp, "%02X", crc_counted);
    msg->buf_lenght = msg->buf_lenght + 2;
    msg->buf[msg->buf_lenght - 2] = crc_temp[0];
    msg->buf[msg->buf_lenght - 1] = crc_temp[1];
}

void TTS_process_message(txstructure* txstruct, char* buf, size_t lenght)
{
    txstruct->tts_data_lenght = 0;
    int data_lenght_temp = 0;

    txstruct->tts_ad = TTS_code(buf[2], buf[3]);

    txstruct->tts_id = TTS_code(buf[4], buf[5]);

    if ((buf[1] == 0x53) && (buf[8] != 0x0D))   // DATA if CRC
    {
        for (size_t i = 6; i < (lenght - 4); i++)
        {
            data_lenght_temp++;
        }
        if (data_lenght_temp % 2 == 0)
        {
            txstruct->tts_data_lenght = (data_lenght_temp / 2);
            for (int i = 0; i < txstruct->tts_data_lenght; i++)
            {
                txstruct->tts_data[i] = TTS_code(buf[(i * 2) + 6], buf[(i * 2) + 7]);
            }
        }
    }

    else if ((buf[1] == 0x58) && (buf[6] != 0x0D))   // DATA if no CRC
    {
        for (size_t i = 6; i < (lenght - 2); i++)
        {
            data_lenght_temp++;
        }
        if (data_lenght_temp % 2 == 0)
        {
            txstruct->tts_data_lenght = (data_lenght_temp / 2);
            for (int i = 0; i < txstruct->tts_data_lenght; i++)
            {
                txstruct->tts_data[i] = TTS_code(buf[(i * 2) + 6], buf[(i * 2) + 7]);
            }
        }
    }
}

void TTS_process_structure(txstructure* txstruct, message* msg, int need_crc)
{
    decoded_bytes *dcd_bts = NULL;
    msg->buf_lenght = 0;
    msg->buf[0] = 'T';

    if (need_crc == 0)                                   // Prefix
    {
        msg->buf[1] = 'X';
    }
    else
    {
        msg->buf[1] = 'S';
    }

    dcd_bts = TTS_decode(txstruct->tts_ad);     // AD
    msg->buf[2] = dcd_bts->byte_a;     
    msg->buf[3] = dcd_bts->byte_b;

    dcd_bts = TTS_decode(txstruct->tts_id);     // ID
    msg->buf[4] = dcd_bts->byte_a;     
    msg->buf[5] = dcd_bts->byte_b;    
    msg->buf_lenght = 6;

    if (txstruct->tts_data_lenght != 0)                 // DATA
    {
        for (size_t i = 0; i < txstruct->tts_data_lenght; i++)
        {
            dcd_bts = TTS_decode(txstruct->tts_data[i]);
            msg->buf[msg->buf_lenght] = dcd_bts->byte_a;
            msg->buf[msg->buf_lenght + 1] = dcd_bts->byte_b;
            msg->buf_lenght = msg->buf_lenght + 2;
        }
    }

    if (need_crc != 0)                              // CRC
    {
        TTS_make_crc(msg);
    }

    msg->buf[msg->buf_lenght] = 0x0D;     // Suffix
    msg->buf_lenght++;
    msg->buf[msg->buf_lenght] = 0x0A;
    msg->buf_lenght++;
}