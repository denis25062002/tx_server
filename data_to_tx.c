#include <stdio.h>
#include "data_to_tx.h"

char DataToTx_byte_to_char(int DataToTx_byte_get)
{
    char DataToTx_temp_char;
    DataToTx_temp_char = DataToTx_byte_get;
    return DataToTx_temp_char;
};

void DataToTx_char_to_tx(char tx_char_get, char* tx_message_buf,
    char id, char* args, size_t args_lenght, char* responce, size_t max_responce_lenght, char* error)
    {
        int need_crc;                   // нужна ли будет проверка

        if ((tx_message_buf == "") && (tx_char_get == 'T')) 
            {tx_message_buf = "T";}         // если в буфере ничего нет и принимаем Т, то записываем Т
        
        else if ((tx_message_buf != "") && (tx_char_get == 'T')) 
            {tx_message_buf = "T";}         // если в буфере что-то есть и принимаем Т, то очищаем и записываем Т
        
        else if ((tx_message_buf == "T") && (tx_char_get == 'S'))
            {
                tx_message_buf = "TS";
                need_crc = 1;
            }                               // если в буфере есть Т и принимаем S, то записываем S (нужна проверка)
        
        else if ((tx_message_buf == "T") && (tx_char_get == 'X')) 
            {
                tx_message_buf = "TS";
                need_crc = 0;
            }        // если в буфере есть T и принимаем X, то записываем X (проверка не нужна)
        
        else if ((tx_message_buf == "T") && ((tx_char_get != 'S') && (tx_char_get != 'X')))
            {                               
            tx_message_buf = "";
            char error[] = "Error!";
            }                               // если в буфере есть Т и принимаем не S и не X, то очищаем буфер и возвращаем ошибку

        else {strcat(tx_message_buf, tx_char_get);}  // если всё нормально, то записываем символ в строку

        if ((tx_char_get == 'R') && (tx_message_buf[strlen(tx_message_buf) - 1] != 'C'))
            {
                tx_message_buf = "";
             char error[] = "Error!";
            }                                   // если получаем R не сразу после C, то очищаем буфер и возвращаем ошибку
        
        else if ((tx_char_get == 'L') && (tx_message_buf[strlen(tx_message_buf) - 1] != 'R'))
            {
              tx_message_buf = "";
                char error[] = "Error!";
            }                                   // если получаем L не сразу после R, то очищаем буфер и возвращаем ошибку

        else if ((tx_char_get == 'F') && (tx_message_buf[strlen(tx_message_buf) - 1] != 'L'))
            {
                tx_message_buf = "";
                char error[] = "Error!";
            }                                   // если получаем F не сразу после CRL, то очищаем буфер и возвращаем ошибку
        
        else if ((tx_char_get == 'F')
        && (tx_message_buf[strlen(tx_message_buf) - 1] == 'L')
        && (tx_message_buf[strlen(tx_message_buf) - 2] == 'R')
        && (tx_message_buf[strlen(tx_message_buf) - 3] == 'C'))
            {
                tx_message_buf = "";
                char error[] = "Error!";
            }                                   // если получаем сразу после CRL, то записываем F и далее разбиваем сообщение по структуре
    };

int main ()
{
    printf("%c\n", DataToTx_byte_to_char(84)); // проверка функции, которая принимает число и возвращает символ
}