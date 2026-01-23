/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as "core Flight System: Bootes"
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *
 * Define RADIO App Messages and info
 */

#ifndef RADIO_APP_MSG_H
#define RADIO_APP_MSG_H

#include "cfe.h"

#define RADIO_APP_NOOP_CC           0
#define RADIO_APP_RESET_COUNTERS_CC 1
#define RADIO_APP_TRANSMIT_CC       2
#define RADIO_APP_CONFIGURE_CC      3
#define RADIO_APP_HK_REQUEST_CC     4
#define RADIO_APP_TRANSMIT_FILE_CC 5

typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
} RADIO_APP_NoArgsCmd_t;

typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint8  DestAddress;
    uint8  DestPort;
    uint16 DataLength;
    uint8  Data[256];
} RADIO_APP_TransmitCmd_t;

typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint16 CommandLength;
    char   Command[256];
} RADIO_APP_ConfigureCmd_t;

typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader;
    uint8  DestAddress;
    uint8  DestPort;
    char   Filename[OS_MAX_PATH_LEN];
} RADIO_APP_TransmitFileCmd_t;

typedef RADIO_APP_NoArgsCmd_t RADIO_APP_NoopCmd_t;
typedef RADIO_APP_NoArgsCmd_t RADIO_APP_ResetCountersCmd_t;
typedef RADIO_APP_NoArgsCmd_t RADIO_APP_HkRequestCmd_t;

typedef struct
{
    uint8 CommandErrorCounter;
    uint8 CommandCounter;
} RADIO_APP_HkTlm_Payload_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t   TelemetryHeader;
    RADIO_APP_HkTlm_Payload_t   Payload;
} RADIO_APP_HkTlm_t;

#endif /* RADIO_APP_MSG_H */

