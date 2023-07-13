#ifndef TX_TO_STRUCTURE
#define TX_TO_STRUCTURE

typedef struct txstructure
{
    char* tts_message;
    char* tts_ad;
    char* tts_id;
    char* tts_data;
    size_t tts_data_size;
    size_t tts_message_lenght;
} txstructure;

void TTS_init(txstructure* txstruct,
    char* tts_message, char* tts_ad, char* tts_id, char* tts_data, size_t tts_data_size, size_t tts_message_lenght);
void TTS_process_message(txstructure* txstruct, char* buf, size_t lenght);
void TTS_print_structure(txstructure* txstruct);
#endif