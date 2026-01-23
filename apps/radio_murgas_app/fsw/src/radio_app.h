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
 * Main header file for the RADIO application
 */

#ifndef RADIO_APP_H
#define RADIO_APP_H

#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"

#include "radio_app_perfids.h"
#include "radio_app_msgids.h"
#include "radio_app_msg.h"

#define RADIO_APP_PIPE_DEPTH 32

#define RADIO_APP_NUMBER_OF_TABLES 1

#define RADIO_APP_TABLE_FILE "/cf/radio_app_tbl.tbl"

#define RADIO_APP_TABLE_OUT_OF_RANGE_ERR_CODE -1

typedef struct
{
    uint8 CmdCounter;
    uint8 ErrCounter;

    RADIO_APP_HkTlm_t HkTlm;

    uint32 RunStatus;

    CFE_SB_PipeId_t CommandPipe;

    char   PipeName[CFE_MISSION_MAX_API_LEN];
    uint16 PipeDepth;

    CFE_TBL_Handle_t TblHandles[RADIO_APP_NUMBER_OF_TABLES];
} RADIO_APP_Data_t;

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
int32 RADIO_APP_SendConfigToBusComms(const RADIO_APP_ConfigureCmd_t *ConfigCmd);
int32 RADIO_APP_SendTxToBusComms(const RADIO_APP_TransmitCmd_t *TxCmd);
int32 RADIO_APP_TransmitFile(const RADIO_APP_TransmitFileCmd_t *Msg);
int32 RADIO_APP_SendFileChunkToBusComms(const uint8 *ChunkData, uint16 ChunkSize, uint8 DestAddr, uint8 DestPort);
bool  RADIO_APP_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);
int32 RADIO_APP_TblValidationFunc(void *TblData);
void  RADIO_APP_ProcessTableUpdate(void);

#endif /* RADIO_APP_H */

