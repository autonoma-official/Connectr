#include "stdio.h"
#include "cJSON.h"
#include "i2c_handler.h"
#include "string.h"

void conv_str_to_hex(char* hex, uint8_t len, uint8_t* hex_arr){
    if(len%2 != 0){
        printf("invalid hex-string length\n");
    }
    for(uint8_t i=0;i<len/2;i++){
        hex_arr[i] = (hextable[(uint8_t)hex[i*2]] << 4) | hextable[(uint8_t)hex[i*2+1]];
        printf("conv hex-number: %02x\n", hex_arr[i]);
    }
}

uint8_t i2c_parse_config(char* configMsg){

    cJSON *msg = cJSON_Parse(configMsg);
    uint8_t status = 0;

    if (msg == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            printf("Error before: %s\n", error_ptr);
        }
        status = 0;
        return -1;
    }
    const cJSON *dpoints = NULL;
    dpoints = cJSON_GetObjectItemCaseSensitive(msg, "dpoints");
    printf("in parse json array \n");
    uint8_t dp_nr = 0;
    const cJSON *dpoint = NULL;
    cJSON_ArrayForEach(dpoint, dpoints)
    {
        cJSON *dp_id = cJSON_GetObjectItemCaseSensitive(dpoint, "id"); 
        cJSON *dp_calc = cJSON_GetObjectItemCaseSensitive(dpoint, "calc");

        printf("Datapoint id: %s\n", dp_id->valuestring);
        printf("Formula: %s\n", dp_calc->valuestring);

        strcpy(dataPoints[dp_nr].id, dp_id->valuestring);
        strcpy(dataPoints[dp_nr].formula, dp_calc->valuestring);

        cJSON *bytes = cJSON_GetObjectItemCaseSensitive(dpoint, "bytes");
        uint8_t bytes_nr = 0;
        const cJSON *e_byte = NULL;
        dataPoints[dp_nr].byte_cnt =0;
        cJSON_ArrayForEach(e_byte, bytes)
        {
            cJSON *cmd = cJSON_GetObjectItemCaseSensitive(e_byte, "cmd");
            cJSON *byte = cJSON_GetObjectItemCaseSensitive(e_byte, "byte");

            printf("Command Nr: %d\n", cmd->valueint);
            printf("Byte Nr: %d\n", byte->valueint);

            dataPoints[dp_nr].cmd_nr[bytes_nr] = cmd->valueint;
            dataPoints[dp_nr].byte_nr[bytes_nr] = byte->valueint;
            bytes_nr++;
            dataPoints[dp_nr].byte_cnt++;
        }
        dp_nr++;
        nr_datapoints++;
    }

    const cJSON *commands = NULL;
    commands = cJSON_GetObjectItemCaseSensitive(msg, "rd_cmds");
    printf("in parse json array \n");
    const cJSON *command = NULL;
    cJSON_ArrayForEach(command, commands)
    {
        cJSON *i2c_address = cJSON_GetObjectItemCaseSensitive(command, "adr");
        cJSON *i2c_cmd = cJSON_GetObjectItemCaseSensitive(command, "cmd");
        cJSON *i2c_cmd_len = cJSON_GetObjectItemCaseSensitive(command, "len");
        cJSON *i2c_read_len = cJSON_GetObjectItemCaseSensitive(command, "rd_len");
        if (!cJSON_IsString(i2c_address) || !cJSON_IsString(i2c_cmd) || !cJSON_IsNumber(i2c_cmd_len) || !cJSON_IsNumber(i2c_read_len))
        {
            status = 0;
            printf("json parameter is not number!\n");
        }

        printf("I2C Address: %s\n", i2c_address->valuestring);
        printf("CMD: %s\n", i2c_cmd->valuestring);
        printf("CMD len: %d\n", i2c_cmd_len->valueint);
        printf("Read len: %d\n", i2c_read_len->valueint);


        conv_str_to_hex(i2c_address->valuestring, strlen(i2c_address->valuestring), &readCommands[nr_readcommands].i2c_adr);
        printf("Safed I2C Address %d\n", readCommands[nr_readcommands].i2c_adr );
        conv_str_to_hex(i2c_cmd->valuestring, strlen(i2c_cmd->valuestring), readCommands[nr_readcommands].cmd);
        
        //readCommands[i2c_entries].i2c_adr = i2c_address->valueint;
        readCommands[nr_readcommands].cmd_len = i2c_cmd_len->valueint;
        //readCommands[i2c_entries].cmd = i2c_cmd->valueint;
        readCommands[nr_readcommands].read_len = i2c_read_len->valueint;
        nr_readcommands++;

    }

    cJSON_Delete(msg);
    return 0;
}