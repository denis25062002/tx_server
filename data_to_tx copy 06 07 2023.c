#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_to_tx.h"

void DTX_init(DataToTX* converter, 
    void(*send_tx) (void *state, char* tx_buf, size_t tx_buf_lenght), void *send_tx_arg, char* tx_buf, size_t tx_buf_lenght)
{
    converter->send_tx = send_tx;
    converter->send_tx_arg = send_tx_arg;
    converter->tx_buf = tx_buf;
    converter->tx_buf_lenght = tx_buf_lenght;
}   

void DTX_write_data(DataToTX* converter, char* buf, size_t lenght)
{
    int DTX_need_crc = 0;
    int DTX_crc_err = 0;
    int DTX_msg_err = 0;
    int DTX_char_count = 0;
    int DTX_data_lenght = 0;
    int DTX_crc_received_temp = 0;
    int DTX_crc_counted = 0;
    char DTX_ad[3] = "";
    char DTX_id[3] = "";
    char DTX_data[] = "";
    char DTX_crc[3] = "";

    for (int i = 0; i < lenght; i++)
    {
        if ((i != 0) && (buf[i] == 'T'))                                                // проверка если пришла T в середине сообщения
        {
            //for (int j = 0; j < i; j++) {converter->tx_buf[j] = '0';}
            memset(converter->tx_buf, 0, sizeof converter->tx_buf);
            converter->tx_buf[0] = 'T';
            DTX_char_count = 1;
            DTX_msg_err = 0;
        }

        if ((buf[i] == 'T') && (i == 0))                                                // ожидание T
        {
            converter->tx_buf[0] = 'T';
            DTX_char_count = 1;
            DTX_need_crc = 0;
        }                  

        if ((buf[i - 1] == 'T')&&(buf[i] == 'S'))                                       // TS
        {
            converter->tx_buf[1] = 'S';
            DTX_need_crc = 1;
            DTX_char_count = 2;
        }
        if ((buf[i - 1] == 'T')&&(buf[i] == 'S')&&(converter->tx_buf[1] != 'X'))        // TX
        {
            converter->tx_buf[1] = 'X';
            DTX_char_count = 2;
        }        

        if ((buf[i - 1] == 'T')&&(buf[i] != 'S')&&(buf[i] != 'X'))                      // если после T не S и не X
        {
            //for (int j = 0; j < i; j++) {converter->tx_buf[j] = '0';}
            memset(converter->tx_buf, 0, sizeof converter->tx_buf);
            DTX_char_count = 0;
        }

        if ((buf[i] != (0x0A)) && (buf[0] == 'T') && ((buf[1] == 'S') || (buf[1] == 'X')))  // записываем остальное
        {
            converter->tx_buf[DTX_char_count - 1 ] = buf[i];
            DTX_char_count++;
        }

        if ((buf[i] == 0x0A) && (buf[i - 1] != 0x0D))                                       // проверка на суффикс
        {
            //for (int j = 0; j < i; j++) {converter->tx_buf[j] = '0';}
            memset(converter->tx_buf, 0, sizeof converter->tx_buf);
            DTX_char_count = 0;
        }
        if ((buf[i] == 0x0A) && (buf[i - 1] == 0x0D))
        {
            char DTX_crc_temp_string[] = "";
            converter->tx_buf[i] = 0x0A;
            converter->tx_buf_lenght = DTX_char_count;
            for (int j = 0; j < DTX_char_count; j++)
            {
                printf("\n%d    %c\n", j, (converter->tx_buf[j]));                          // print
            }

            // начинается часть, где принятое сообщение разбивается по структуре
            // и выполняется проверка, если это необходимо


            // определение AD
            DTX_ad[0] = converter->tx_buf[2];
            DTX_ad[1] = converter->tx_buf[3];

            // определение ID
            DTX_id[0] = converter->tx_buf[4];
            DTX_id[1] = converter->tx_buf[5];

            // определение DATA
            if ((DTX_need_crc == 1) && (converter->tx_buf[8] != 0x0D))
            {
                for (int j = 6; j <= (DTX_char_count - 5); j++)
                {
                    DTX_data_lenght++;
                    DTX_data[j - 6] = converter->tx_buf[j];
                }
            }
            if ((DTX_need_crc == 0) && (converter->tx_buf[6] != 0x0D))
            {
                for (int j = 6; j <= (DTX_char_count - 3); j++)
                {
                    DTX_data_lenght++;
                    DTX_data[j - 6] = converter->tx_buf[j];
                }
            }

            // определение и проверка CRC
            if (DTX_need_crc == 1)
            {
                int DTX_crc_str_len = 4;
                DTX_crc_counted = 0;
                DTX_crc_str_len = DTX_crc_str_len + DTX_data_lenght;
                DTX_crc[0] = converter->tx_buf[DTX_char_count - 4];
                DTX_crc[1] = converter->tx_buf[DTX_char_count - 3];
                DTX_crc_received_temp = (int)strtol(DTX_crc, NULL, 16);
                DTX_crc_temp_string[0] = converter->tx_buf[2];
                DTX_crc_temp_string[1] = converter->tx_buf[3];
                DTX_crc_temp_string[2] = converter->tx_buf[4];
                DTX_crc_temp_string[3] = converter->tx_buf[5];
                if (DTX_data_lenght != 0)
                { for (int j = 0;  j < (DTX_data_lenght); j++) {DTX_crc_temp_string[4 + j] = DTX_data[j];} }
                for (int j = 0; j < DTX_crc_str_len; j++)
                {
                    DTX_crc_counted = DTX_crc_counted ^ (DTX_crc_temp_string[j]);
                }
            }

            // часть с выводом полученных значений
            if ((DTX_need_crc == 1) && (DTX_crc_received_temp == DTX_crc_counted))
            {
                printf("\nMessage complete!\n\n");                                                 // print
            }                                  
            if ((DTX_need_crc == 1) && (DTX_crc_received_temp != DTX_crc_counted))
            {
                printf("\nMessage error! (wrong CRC)\n\n");                                      // print
                DTX_msg_err = 1;
            }         
            if (DTX_need_crc == 0)
            {
                printf("\nMessage complete!\n\n");  // print
            } 
            printf("\nAD: %c%c\n", DTX_ad[0], DTX_ad[1]);
            printf("\nID: %c%c\n", DTX_id[0], DTX_id[1]);
            if (DTX_data_lenght != 0) 
            {
                printf("\nData lenght: %d, Data: ", DTX_data_lenght);
                for (int j = 0; j < DTX_data_lenght; j++) { printf("%c", DTX_data[j]); }
                printf("\n");
            }
            if (DTX_need_crc == 1)
            {
                printf("\nCRC Received: %c%c\n", DTX_crc[0], DTX_crc[1]);
                printf("\nCRC String Received: %s\n", DTX_crc_temp_string);
            }
        }
     
        if ((i == (lenght - 1)) && ((converter->tx_buf[DTX_char_count - 2] != 0x0D) || (buf[i] != 0x0A) ))
        {
            DTX_msg_err = 1;
            memset(converter->tx_buf, 0, sizeof converter->tx_buf);
            printf("\nMessage error!\n");                                                   // print
        }
    }
    converter->send_tx(converter->send_tx_arg, converter->tx_buf, sizeof converter->tx_buf);
}

void print_tx (void *state, char* tx_buf, size_t tx_buf_lenght)
{
    tx_buf_lenght++;
    for (int i = 0; i <= tx_buf_lenght; i++)
    {
        printf("%c", tx_buf[i]);
    }
}

#define TX_BUF_MAX_LENGHT 256

int main ()
{
    printf("\e[1;1H\e[2J");                                                                 // print

    DataToTX converter;
    char tx_buf_temp[TX_BUF_MAX_LENGHT];
    DTX_init(&converter, print_tx, NULL, tx_buf_temp, TX_BUF_MAX_LENGHT);

    //char test[] = {0x54, 0x53, 0x32, 0x31, 0x30, 0x30,  0x36, 0x38, 0x30, 0x33, 0x0D, 0x0A};          // test with CRC
    char test[] = {0x54, 0x53, 0x32, 0x31, 0x30, 0x30, 0x30, 0x33, 0x0D, 0x0A};                         // исходная с CRC
    //char test[] = {0x54, 0x58, 0x32, 0x31, 0x30, 0x31, 0x35, 0x37, 0x36, 0x38, 0x0D, 0x0A};           // test without CRC

    DTX_write_data(&converter, test, sizeof(test));
    printf("\nLenght: %d", sizeof(test));
}