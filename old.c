#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int cc_mc = 0;

int asciitohex(char* input, char* output)
{
    int loop = 0;
    int i = 0;
    while (input[loop] != '\0') {
        sprintf((char*)(output + i), "%02X", input[loop]);
        loop += 1;
        i += 2;
    }
    output[i++] = '\0';
}

int make_request()
{
    int need_crc = 0;
    char prefix[3] = "";
    char id[3] = "";
    int id_lenght = 0;
    char data[1000] = "";
    char crc[3] = "";
    char suffix[5] = "0D0A";
    char ad[3] = "21";

    printf("\nNeed CRC? 0 or 1: ");             // CRC
    scanf("%d", &need_crc);
    if (need_crc == 0) {strcpy(prefix, "TX");}
    if (need_crc == 1) {strcpy(prefix, "TS");}
    if ((need_crc != 0) && (need_crc != 1))
    {
        printf("\nincorrent input!");
        exit (1);
    }

    while (getchar() != '\n');                  // ID
    printf("\nMessage ID (2 letters only): ");  
    fgets(id, 3, stdin);
    while(id[id_lenght]) id_lenght++;
    if(id[id_lenght-1]=='\n') id[--id_lenght]=0;
    if (id_lenght == 2) {while (getchar() != '\n');}
    if (id_lenght == 1) 
    {
        id[1] = id[0];
        id[0] = '0';
    }
    if (id_lenght == 0) {strcpy(id, "00");}

    if (cc_mc == 0)                             // DATA
    {
        printf("\nData: ");
        fgets(data, 1000, stdin);
        printf("\nData Lenght:\n%d", (strlen(data) - 1));
        if (((strlen(data) - 1) % 2) == 1)
        {
            for (int i = (strlen(data)); i >= 0; i--)
            {
                data[i+1] = data[i];
            }
            data[0] = '0';
        }
    }


printf("\nRequest message:\n%s 21 %s %s %s %s", prefix, id, data, crc, suffix);    // формируем сообщение-запрос

printf("\n\bmake CRC:");

char prefix_hexstr[5];                                                                         // prefix to hex str
asciitohex(prefix, prefix_hexstr);                                                             
char ad_hexstr[5];                                                                             // ad to hex str
asciitohex(ad, ad_hexstr);
char id_hexstr[5];                                                                             // id to hex str
asciitohex(id, id_hexstr);
char data_hexstr_temp[((strlen(data)))];                                                       // data to hex str
char data_hexstr[(strlen(data) - 2 )];
if (cc_mc == 0) 
{
    asciitohex(data, data_hexstr_temp);
    for (int i = 0; i < (strlen(data_hexstr_temp) - 2); i++)
    {
        data_hexstr[i] = data_hexstr_temp[i];
    }
}
printf("\nuseful message to hex: \n%s %s %s %s", prefix_hexstr, ad_hexstr, id_hexstr, data_hexstr);

int crc_result = 0;
int crc_temp = 0;
char twochartemp[] = "00";
char crc_result_str[3] = "";
for (int i = 0; i < 2; i++)                 //  ad crc
{
    twochartemp[0] = ad_hexstr[i*2];
    twochartemp[1] = ad_hexstr[i*2+1];
    int crc_temp = (int)strtol(twochartemp, NULL, 16);
    crc_result = crc_temp ^ crc_result;
}
for (int i = 0; i < 2; i++)                 //  id crc
{
    twochartemp[0] = id_hexstr[i*2];
    twochartemp[1] = id_hexstr[i*2+1];
    int crc_temp = (int)strtol(twochartemp, NULL, 16);
    crc_result = crc_temp ^ crc_result;
}
if (cc_mc == 0)                          //  data crc
{
    for (int i = 0; i < (((strlen(data_hexstr))/2)); i++)                 
{
    twochartemp[0] = data_hexstr[i*2];
    twochartemp[1] = data_hexstr[i*2+1];
    int crc_temp = (int)strtol(twochartemp, NULL, 16);
    crc_result = crc_temp ^ crc_result;
}
}

printf("\nCRC Result: %X\nint: %d", crc_result, crc_result);

char msgtohex[1000];
strcat(msgtohex,prefix_hexstr);
strcat(msgtohex,ad_hexstr);
strcat(msgtohex,id_hexstr);
if (cc_mc == 0) {strcat(msgtohex, data_hexstr);}
if (need_crc == 1) 
{
    if (crc_result == 0) { char crc_result_str[] = "00"; }
    else if (crc_result == 1) { char crc_result_str[] = "01"; }
    else if (crc_result == 2) { char crc_result_str[] = "02"; }
    else if (crc_result == 3) { char crc_result_str[] = "03"; }
    else if (crc_result == 4) { char crc_result_str[] = "04"; }
    else if (crc_result == 5) { char crc_result_str[] = "05"; }
    else if (crc_result == 6) { char crc_result_str[] = "06"; }
    else if (crc_result == 7) { char crc_result_str[] = "07"; }
    else if (crc_result == 8) { char crc_result_str[] = "08"; }
    else if (crc_result == 9) { char crc_result_str[] = "09"; }
    else if (crc_result == 10) { char crc_result_str[] = "0A"; }
    else if (crc_result == 11) { char crc_result_str[] = "0B"; }
    else if (crc_result == 12) { char crc_result_str[] = "0C"; }
    else if (crc_result == 13) { char crc_result_str[] = "0D"; }
    else if (crc_result == 14) { char crc_result_str[] = "0E"; }
    else if (crc_result == 15) { char crc_result_str[] = "0F"; }
    else if (crc_result > 15) {sprintf(crc_result_str, "%X", crc_result_str);}
    strcat(msgtohex, crc_result_str);
}
strcat(msgtohex, suffix);
printf("\n\nREquest message to HEX: %s", msgtohex);

}


int main()
{
    printf("Make request message for the control command (0) or  the monitoring command (1): ");
    scanf("%d", &cc_mc);
    while (getchar() != '\n');
    if ((cc_mc == 0) || (cc_mc == 1))
    {
        make_request();
    }
    else printf("\nincorrent input!");
}
