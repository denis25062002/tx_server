#ifndef DATA_TO_TX
#define DATA_TO_TX

char DataToTx_byte_to_char(int DataToTx_byte_get);

void DataToTx_char_to_tx(char tx_char_get, char* tx_message_buf,
    char id, char* args, size_t args_lenght, char* responce, size_t max_responce_lenght, char* error);

#endif