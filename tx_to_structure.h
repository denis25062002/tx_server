#ifndef TX_TO_STRUCTURE
#define TX_TO_STRUCTURE

typedef struct txstructure
{
    char* tts_message;
    char* tts_ad;
    char* tts_id;
    char* tts_data;
    size_t tts_data_lenght;
    size_t tts_message_lenght;
    int tts_need_crc;
} txstructure;

void TTS_init(txstructure* txstruct,
    char* tts_message, char* tts_ad, char* tts_id, char* tts_data, size_t tts_data_lenght, size_t tts_message_lenght, int tts_need_crc);
void TTS_process_message(txstructure* txstruct, char* buf, size_t lenght);
void TTS_process_structure(txstructure* txstruct, char* stt_ad, char* stt_id, char* stt_data, size_t stt_data_lenght, int tts_need_crc);
void TTS_print_structure(txstructure* txstruct);
#endif