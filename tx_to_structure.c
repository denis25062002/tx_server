#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tx_to_structure.h"

static const char HEX_CHARS[] = "0123456789ABCDEF";

void TTS_init(txstructure *txstruct,
              unsigned char tts_ad, 
              unsigned char tts_id, 
              unsigned char *tts_data, 
              size_t tts_data_length)
{
    txstruct->tts_ad = tts_ad;
    txstruct->tts_id = tts_id;
    txstruct->tts_data = tts_data;
    txstruct->tts_data_length = tts_data_length;
}

void TTS_message_init(message *msg, char *buf, size_t buf_length)
{
    msg->buf = buf;
    msg->buf_length = buf_length;
}

void TTS_decode(unsigned char byte_to_decode, decoded_bytes *dcd_bts)
{
    if (!dcd_bts) return;
    dcd_bts->byte_a = HEX_CHARS[(byte_to_decode >> 4) & 0x0F];
    dcd_bts->byte_b = HEX_CHARS[byte_to_decode & 0x0F];
}

unsigned char TTS_code(char byte_1, char byte_2)
{
    char temp[3] = { byte_1, byte_2, '\0' };
    return (unsigned char)strtol(temp, NULL, 16);
}

static void TTS_make_crc(message *msg)
{
    int crc_counted = 0;
    for (size_t i = 2; i < msg->buf_length; i++)
    {
        crc_counted ^= (unsigned char)msg->buf[i];
    }
    
    decoded_bytes crc_bytes;
    TTS_decode((unsigned char)crc_counted, &crc_bytes);
    
    msg->buf[msg->buf_length++] = crc_bytes.byte_a;
    msg->buf[msg->buf_length++] = crc_bytes.byte_b;
}

void TTS_process_message(txstructure *txstruct, const char *buf, size_t length)
{
    txstruct->tts_data_length = 0;
    if (length < 6) return;

    txstruct->tts_ad = TTS_code(buf[2], buf[3]);
    txstruct->tts_id = TTS_code(buf[4], buf[5]);

    size_t data_length_temp = 0;

    if (buf[1] == 'S' && length >= 8) // С CRC
    {
        data_length_temp = length - 8; // T, S, AD(2), ID(2), [DATA], CRC(2), \r, \n
        if (data_length_temp % 2 == 0)
        {
            txstruct->tts_data_length = data_length_temp / 2;
            for (size_t i = 0; i < txstruct->tts_data_length; i++)
            {
                txstruct->tts_data[i] = TTS_code(buf[(i * 2) + 6], buf[(i * 2) + 7]);
            }
        }
    }
    else if (buf[1] == 'X' && length >= 6) // Без CRC
    {
        data_length_temp = length - 6; // T, X, AD(2), ID(2), [DATA], \r, \n
        if (data_length_temp % 2 == 0)
        {
            txstruct->tts_data_length = data_length_temp / 2;
            for (size_t i = 0; i < txstruct->tts_data_length; i++)
            {
                txstruct->tts_data[i] = TTS_code(buf[(i * 2) + 6], buf[(i * 2) + 7]);
            }
        }
    }
}

void TTS_process_structure(const txstructure *txstruct, message *msg, int need_crc)
{
    decoded_bytes dcd;
    msg->buf_length = 0;
    msg->buf[msg->buf_length++] = 'T';
    msg->buf[msg->buf_length++] = need_crc ? 'S' : 'X';

    TTS_decode(txstruct->tts_ad, &dcd);
    msg->buf[msg->buf_length++] = dcd.byte_a;
    msg->buf[msg->buf_length++] = dcd.byte_b;

    TTS_decode(txstruct->tts_id, &dcd);
    msg->buf[msg->buf_length++] = dcd.byte_a;
    msg->buf[msg->buf_length++] = dcd.byte_b;

    for (size_t i = 0; i < txstruct->tts_data_length; i++)
    {
        TTS_decode(txstruct->tts_data[i], &dcd);
        msg->buf[msg->buf_length++] = dcd.byte_a;
        msg->buf[msg->buf_length++] = dcd.byte_b;
    }

    if (need_crc)
    {
        TTS_make_crc(msg);
    }

    msg->buf[msg->buf_length++] = '\r';
    msg->buf[msg->buf_length++] = '\n';
}