#include <stdio.h>
#include <string.h>
#include "tx_to_structure.h"

void TTS_init(txstructure* txstruct,
    char* tts_message, char* tts_ad, char* tts_id, char* tts_data, size_t tts_data_lenght, size_t tts_message_lenght, int tts_need_crc)
{
    txstruct->tts_message = tts_message;
    txstruct->tts_ad = tts_ad;
    txstruct->tts_id = tts_id;
    txstruct->tts_data = tts_data;
    txstruct->tts_data_lenght = tts_data_lenght;
    txstruct->tts_message_lenght = tts_message_lenght;
    txstruct->tts_need_crc = tts_need_crc;
}

static void TTS_make_crc(txstructure* txstruct)
{
    int crc_counted = 0;
    char crc_temp[2];
    char crc_first_temp[2];
    for (size_t i = 2; i < txstruct->tts_message_lenght; i++)
    {
        crc_counted = crc_counted ^ txstruct->tts_message[i];
    }
    //printf("\ncrc counted: %d\n", crc_counted);
    sprintf(crc_temp, "%X", crc_counted);
    txstruct->tts_message_lenght = txstruct->tts_message_lenght + 2;
    //printf("\nmessage lenght with crc: %d\n", txstruct->tts_message_lenght);
    //printf("\ncrc temp 0 char: %c\n", crc_temp[0]);
    //printf("\ncrc temp 1 char: %c\n", crc_temp[1]);

    if (crc_counted < 16)
    {
        txstruct->tts_message[txstruct->tts_message_lenght - 2] = '0';
        txstruct->tts_message[txstruct->tts_message_lenght - 1] = crc_temp[0];    
    }
    else 
    {
        txstruct->tts_message[txstruct->tts_message_lenght - 2] = crc_temp[0];
        txstruct->tts_message[txstruct->tts_message_lenght - 1] = crc_temp[1];
    }
}

void TTS_process_message(txstructure* txstruct, char* buf, size_t lenght)
{
    txstruct->tts_message_lenght = lenght;
    txstruct->tts_data_lenght = 0;

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
            txstruct->tts_data_lenght++;
        }
    }

    else if ((txstruct->tts_message[1] == 0x58) && (txstruct->tts_message[6] != 0x0D))   // DATA if no CRC
    {
        for (size_t i = 6; i < (txstruct->tts_message_lenght - 2); i++)
        {
            txstruct->tts_data[i - 6] = txstruct->tts_message[i];
            txstruct->tts_data_lenght++;
        }
    }
}

void TTS_process_structure(txstructure* txstruct, char* stt_ad, char* stt_id, char* stt_data, size_t stt_data_lenght, int tts_need_crc)
{
    txstruct->tts_message_lenght = 0;
    txstruct->tts_message[0] = 'T';

    if (txstruct->tts_need_crc == 0)                    // Prefix
    {
        txstruct->tts_message[1] = 'X';
    }
    else
    {
        txstruct->tts_message[1] = 'S';
    }

    txstruct->tts_message[2] = txstruct->tts_ad[0];     // AD
    txstruct->tts_message[3] = txstruct->tts_ad[1];

    txstruct->tts_message[4] = txstruct->tts_id[0];     // ID
    txstruct->tts_message[5] = txstruct->tts_id[1];
    txstruct->tts_message_lenght = 6;

    if (txstruct->tts_data_lenght != 0)                 // DATA
    {
        for (size_t i = 0; i < txstruct->tts_data_lenght; i++)
        {
            txstruct->tts_message[i + 6] = txstruct->tts_data[i];
            txstruct->tts_message_lenght++;
        }
    }
    if (txstruct->tts_need_crc != 0)                              // CRC
    {
        TTS_make_crc(txstruct);
    }

    txstruct->tts_message[txstruct->tts_message_lenght] = 0x0D;     // Suffix
    txstruct->tts_message_lenght++;
    txstruct->tts_message[txstruct->tts_message_lenght] = 0x0A;
    txstruct->tts_message_lenght++;

    printf("\n\nGenerated message: ");
    for (size_t i = 0; i < txstruct->tts_message_lenght; i++)
    {
        printf("%c", txstruct->tts_message[i]);
    }
    printf("\nMessage lenght: %d\n", txstruct->tts_message_lenght);
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
    if (txstruct->tts_data_lenght != 0)
    {
        printf("\nDATA: ");
        for (int i = 0; i < txstruct->tts_data_lenght; i++)
        {
            printf("%c", txstruct->tts_data[i]);
        }
    }
}