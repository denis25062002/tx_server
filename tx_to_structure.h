#ifndef TX_TO_STRUCTURE
#define TX_TO_STRUCTURE

typedef struct txstructure
{
    unsigned char tts_ad, tts_id;
    unsigned char* tts_data;
    size_t tts_data_lenght;
} txstructure;

typedef struct message
{
    char* buf;
    size_t buf_lenght;
} message;

void TTS_init(txstructure* txstruct,
     unsigned char tts_ad, unsigned char tts_id, unsigned char* tts_data, size_t tts_data_lenght);
void TTS_message_init(message* msg, char* buf, size_t buf_lenght);
void TTS_process_message(txstructure* txstruct, char* buf, size_t lenght);
void TTS_process_structure(txstructure* txstruct, message* msg, int need_crc);
unsigned char TTS_code(char byte_1, char byte_2);
#endif