/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
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
 * Main header file for the RADIO application
 */

#ifndef RADIO_APP_H
#define RADIO_APP_H

/*
** Required header files.
*/
#include <csp/csp.h>

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "radio_app_perfids.h"
#include "radio_app_msgids.h"
#include "radio_app_msg.h"

/***********************************************************************/
#define RADIO_APP_PIPE_DEPTH 32 /* Depth of the Command Pipe for Application */

#define RADIO_APP_NUMBER_OF_TABLES 1 /* Number of Table(s) */

/* Define filenames of default data images for tables */
#define RADIO_APP_TABLE_FILE "/cf/radio_app_tbl.tbl"

#define RADIO_APP_TABLE_OUT_OF_RANGE_ERR_CODE -1

/* AX100 Configuration Limits */
#define RADIO_APP_MAX_PACKET_SIZE 256
#define RADIO_APP_CSP_TIMEOUT_MS  1000
#define RADIO_APP_HK_CYCLE_MS     5000

/* CSP Constants */
#define CSP_OBC_NODE 1
#define CPS_OBC_PORT 5
#define RADIO_OBC_NODE 2
#define RADIO_BOC_PORT 5
#define RADIO_BIT_RATE 1000000

/************************************************************************
** Type Definitions
*************************************************************************/

/*
** Global Data
*/
typedef struct
{
    /*
    ** Command interface counters...
    */
    uint8 CmdCounter;
    uint8 ErrCounter;

    /*
    ** Housekeeping telemetry packet...
    */
    RADIO_APP_HkTlm_t HkTlm;

    /*
    ** Receive telemetry packet...
    */
    RADIO_APP_RxTlm_t RxTlm;

    /*
    ** Run Status variable used in the main processing loop
    */
    uint32 RunStatus;

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_PipeId_t CommandPipe;
    CFE_SB_PipeId_t DataPipe;

    /*
    ** AX100 Radio State
    */
    uint16 TxCount;
    uint16 RxCount;
    int16  LastRSSI;
    int16  BoardTemp;
    int16  PATemp;
    uint8  RadioStatus;
    
    /*
    ** AX100 Configuration
    */
    uint32 TxFrequency;
    uint32 RxFrequency;
    uint8  TxPower;
    uint8  Modulation;

    /*
    ** csp variables 
    */
    csp_iface_t *radio_iface;
    csp_socket_t socket = { 0 };

    /*
    ** Initialization data (not reported in housekeeping)...
    */
    char   PipeName[CFE_MISSION_MAX_API_LEN];
    char   DataPipeName[CFE_MISSION_MAX_API_LEN];
    uint16 PipeDepth;

    CFE_TBL_Handle_t TblHandles[RADIO_APP_NUMBER_OF_TABLES];
    
    CFE_TBL_Handle_t RadioConfigHandle;
} RADIO_APP_Data_t;


typedef struct {

} RADIO_APP_Command_Packet;

typedef struct {

} RADIO_APP_Telemetry_Packet;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (SAMPLE_APP_Main), these
**       functions are not called from any other source module.
*/
void  RADIO_APP_Main(void);
int32 RADIO_APP_Init(void);
void  RADIO_APP_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr);
void  RADIO_APP_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr);
int32 RADIO_APP_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg);
int32 RADIO_APP_ResetCounters(const RADIO_APP_ResetCountersCmd_t *Msg);
int32 RADIO_APP_Noop(const RADIO_APP_NoopCmd_t *Msg);
int32 RADIO_APP_TransmitData(const RADIO_APP_TransmitCmd_t *Msg);
int32 RADIO_APP_ConfigureRadio(const RADIO_APP_ConfigureCmd_t *Msg);
int32 RADIO_APP_RequestHousekeeping(const RADIO_APP_HkRequestCmd_t *Msg);
void  RADIO_APP_ProcessAX100Data(void);
void  RADIO_APP_UpdateAX100Status(void);
void  RADIO_APP_GetCrc(const char *TableName);

int32 RADIO_APP_TblValidationFunc(void *TblData);

bool RADIO_APP_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

#endif /* RADIO_APP_H */
