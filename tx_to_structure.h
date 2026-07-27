#ifndef TX_TO_STRUCTURE_H
#define TX_TO_STRUCTURE_H

#include <stddef.h>

typedef struct txstructure
{
    unsigned char tts_ad;
    unsigned char tts_id;
    unsigned char *tts_data;
    size_t tts_data_length;
} txstructure;

typedef struct message
{
    char *buf;
    size_t buf_length;
} message;

typedef struct decoded_bytes
{
    char byte_a;
    char byte_b;
} decoded_bytes;

void TTS_init(txstructure *txstruct,
              unsigned char tts_ad, 
              unsigned char tts_id, 
              unsigned char *tts_data, 
              size_t tts_data_length);

void TTS_message_init(message *msg, char *buf, size_t buf_length);
void TTS_process_message(txstructure *txstruct, const char *buf, size_t length);
void TTS_process_structure(const txstructure *txstruct, message *msg, int need_crc);

void TTS_decode(unsigned char byte_to_decode, decoded_bytes *dcd_bts);
unsigned char TTS_code(char byte_1, char byte_2);

#endif // TX_TO_STRUCTURE_H