#ifndef ADCS_APP_INTER_MIDS_H
#define ADCS_APP_INTER_MIDS_H

#define ADCS_SUN_POINT_CC 0
#define ADCS_TURN_FROM_SUN_CC 1

typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader; 
} ADCS_APP_InterNoArgsCmd_t;

typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint8_t *data;
    uint16_t length;
} ADCS_APP_CSP_Cmd_t;

typedef ADCS_APP_InterNoArgsCmd_t ADCS_APP_PntSun_t;
typedef ADCS_APP_InterNoArgsCmd_t ADCS_APP_TrnSun_t;


#endif