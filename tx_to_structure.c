#include <stdio.h>
#include "tx_to_structure.h"

void TTS_init(txstructure* txstruct,
    char* tts_message, char* tts_ad, char* tts_id, char* tts_data, size_t tts_data_size, size_t tts_message_lenght)
{
    txstruct->tts_message = tts_message;
    txstruct->tts_ad = tts_ad;
    txstruct->tts_id = tts_id;
    txstruct->tts_data = tts_data;
    txstruct->tts_data_size = tts_data_size;
    txstruct->tts_message_lenght = tts_message_lenght;
}

void TTS_process_message(txstructure* txstruct, char* buf, size_t lenght)
{
    txstruct->tts_message_lenght = lenght;
    txstruct->tts_data_size = 0;

    printf("\n\nTTS Input message: ");
    for (size_t i = 0; i < lenght; i++)
    {
        txstruct->tts_message[i] = buf[i];
        printf("%c", txstruct->tts_message[i]);
    }

    txstruct->tts_ad[0] = txstruct->tts_message[2];
    txstruct->tts_ad[1] = txstruct->tts_message[3];

    txstruct->tts_id[0] = txstruct->tts_message[4];
    txstruct->tts_id[1] = txstruct->tts_message[5];

    if ((txstruct->tts_message[1] == 0x53) && (txstruct->tts_message[8] != 0x0D))   // DATA if CRC
    {
        for (size_t i = 6; i < txstruct->tts_message_lenght - 4; i++)
        {
            txstruct->tts_data[i - 6] = txstruct->tts_message[i];
            txstruct->tts_data_size++;
        }
    }

    else if ((txstruct->tts_message[1] == 0x58) && (txstruct->tts_message[6] != 0x0D))   // DATA if no CRC
    {
        for (size_t i = 6; i < (txstruct->tts_message_lenght - 2); i++)
        {
            txstruct->tts_data[i - 6] = txstruct->tts_message[i];
            txstruct->tts_data_size++;
        }
    }
}

void TTS_print_structure(txstructure* txstruct)
{
    if (txstruct->tts_message[1] == 0x53)
    {
        printf("CRC: Yes\n");
    }
    if (txstruct->tts_message[1] == 0x58)
    {
        printf("CRC: No\n");
    }

    printf("AD: ");
    for (int i = 0; i < 2; i++)
    {
        printf("%c", txstruct->tts_ad[i]);
    }
    printf("\nID: ");
    for (int i = 0; i < 2; i++)
    {
        printf("%c", txstruct->tts_id[i]);
    }
    if (txstruct->tts_data_size != 0)
    {
        printf("\nDATA: ");
        for (int i = 0; i < txstruct->tts_data_size; i++)
        {
            printf("%c", txstruct->tts_data[i]);
        }
    }
}