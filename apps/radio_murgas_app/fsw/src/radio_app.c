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

#include "radio_app_events.h"
#include "radio_app_version.h"
#include "radio_app.h"
#include "bus_comms_msg.h"
#include "bus_comms_msgids.h"
#include <string.h>

#define RADIO_APP_RADIO_NODE_ADDR 2
#define RADIO_APP_CONFIG_PORT     7

RADIO_APP_Data_t RADIO_APP_Data;

void RADIO_APP_Main(void)
{
    int32            status;
    CFE_SB_Buffer_t *SBBufPtr;

    CFE_ES_PerfLogEntry(RADIO_APP_PERF_ID);

    status = RADIO_APP_Init();
    if (status != CFE_SUCCESS)
    {
        RADIO_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    while (CFE_ES_RunLoop(&RADIO_APP_Data.RunStatus) == true)
    {
        CFE_ES_PerfLogExit(RADIO_APP_PERF_ID);

        status = CFE_SB_ReceiveBuffer(&SBBufPtr, RADIO_APP_Data.CommandPipe, CFE_SB_PEND_FOREVER);

        CFE_ES_PerfLogEntry(RADIO_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            RADIO_APP_ProcessCommandPacket(SBBufPtr);
        }
        else
        {
            CFE_EVS_SendEvent(RADIO_APP_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "RADIO APP: SB Pipe Read Error, App Will Exit");

            RADIO_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    CFE_ES_PerfLogExit(RADIO_APP_PERF_ID);

    CFE_ES_ExitApp(RADIO_APP_Data.RunStatus);
}

int32 RADIO_APP_Init(void)
{
    int32 status;

    RADIO_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    RADIO_APP_Data.CmdCounter = 0;
    RADIO_APP_Data.ErrCounter = 0;

    RADIO_APP_Data.PipeDepth = RADIO_APP_PIPE_DEPTH;

    strncpy(RADIO_APP_Data.PipeName, "RADIO_APP_CMD_PIPE", sizeof(RADIO_APP_Data.PipeName));
    RADIO_APP_Data.PipeName[sizeof(RADIO_APP_Data.PipeName) - 1] = 0;

    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Radio App: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    CFE_MSG_Init(CFE_MSG_PTR(RADIO_APP_Data.HkTlm.TelemetryHeader), CFE_SB_ValueToMsgId(RADIO_APP_HK_TLM_MID),
                 sizeof(RADIO_APP_Data.HkTlm));

    status = CFE_SB_CreatePipe(&RADIO_APP_Data.CommandPipe, RADIO_APP_Data.PipeDepth, RADIO_APP_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Radio App: Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(RADIO_APP_SEND_HK_MID), RADIO_APP_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Radio App: Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(RADIO_APP_CMD_MID), RADIO_APP_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Radio App: Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }


    CFE_EVS_SendEvent(RADIO_APP_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "Radio App Initialized.%s",
                      RADIO_APP_VERSION_STRING);

    return CFE_SUCCESS;
}

void RADIO_APP_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case RADIO_APP_CMD_MID:
            RADIO_APP_ProcessGroundCommand(SBBufPtr);
            break;

        case RADIO_APP_SEND_HK_MID:
            RADIO_APP_ReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent(RADIO_APP_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "RADIO: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }
}

void RADIO_APP_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    switch (CommandCode)
    {
        case RADIO_APP_NOOP_CC:
            if (RADIO_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(RADIO_APP_NoopCmd_t)))
            {
                RADIO_APP_Noop((RADIO_APP_NoopCmd_t *)SBBufPtr);
            }
            break;

        case RADIO_APP_RESET_COUNTERS_CC:
            if (RADIO_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(RADIO_APP_ResetCountersCmd_t)))
            {
                RADIO_APP_ResetCounters((RADIO_APP_ResetCountersCmd_t *)SBBufPtr);
            }
            break;

        case RADIO_APP_CONFIGURE_CC:
            if (RADIO_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(RADIO_APP_ConfigureCmd_t)))
            {
                RADIO_APP_ConfigureRadio((RADIO_APP_ConfigureCmd_t *)SBBufPtr);
            }
            break;

        case RADIO_APP_TRANSMIT_CC:
            if (RADIO_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(RADIO_APP_TransmitCmd_t)))
            {
                RADIO_APP_TransmitData((RADIO_APP_TransmitCmd_t *)SBBufPtr);
            }
            break;

        case RADIO_APP_HK_REQUEST_CC:
            if (RADIO_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(RADIO_APP_HkRequestCmd_t)))
            {
                RADIO_APP_RequestHousekeeping((RADIO_APP_HkRequestCmd_t *)SBBufPtr);
            }
            break;

        default:
            CFE_EVS_SendEvent(RADIO_APP_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d", CommandCode);
            break;
    }
}

int32 RADIO_APP_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg)
{
    RADIO_APP_Data.HkTlm.Payload.CommandErrorCounter = RADIO_APP_Data.ErrCounter;
    RADIO_APP_Data.HkTlm.Payload.CommandCounter      = RADIO_APP_Data.CmdCounter;

    CFE_SB_TimeStampMsg(CFE_MSG_PTR(RADIO_APP_Data.HkTlm.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(RADIO_APP_Data.HkTlm.TelemetryHeader), true);

    return CFE_SUCCESS;
}

int32 RADIO_APP_Noop(const RADIO_APP_NoopCmd_t *Msg)
{
    RADIO_APP_Data.CmdCounter++;

    CFE_EVS_SendEvent(RADIO_APP_COMMANDNOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "RADIO: NOOP command %s",
                      RADIO_APP_VERSION);

    return CFE_SUCCESS;
}

int32 RADIO_APP_ResetCounters(const RADIO_APP_ResetCountersCmd_t *Msg)
{
    RADIO_APP_Data.CmdCounter = 0;
    RADIO_APP_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(RADIO_APP_COMMANDRESET_INF_EID, CFE_EVS_EventType_INFORMATION, "RADIO: RESET command");

    return CFE_SUCCESS;
}

int32 RADIO_APP_ConfigureRadio(const RADIO_APP_ConfigureCmd_t *Msg)
{
    int32 status;

    RADIO_APP_Data.CmdCounter++;

    if (Msg->CommandLength > 256)
    {
        CFE_EVS_SendEvent(RADIO_APP_CONFIG_ERR_EID, CFE_EVS_EventType_ERROR,
                          "RADIO: Command length exceeds maximum (256)");
        RADIO_APP_Data.ErrCounter++;
        return CFE_SB_BAD_ARGUMENT;
    }

    status = RADIO_APP_SendConfigToBusComms(Msg);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(RADIO_APP_CONFIG_ERR_EID, CFE_EVS_EventType_ERROR,
                          "RADIO: Failed to send config to bus_comms, RC = 0x%08lX", (unsigned long)status);
        RADIO_APP_Data.ErrCounter++;
        return status;
    }

    CFE_EVS_SendEvent(RADIO_APP_CONFIGURE_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "RADIO: Configuration command sent - Len=%u", Msg->CommandLength);

    return CFE_SUCCESS;
}

int32 RADIO_APP_TransmitData(const RADIO_APP_TransmitCmd_t *Msg)
{
    int32 status;

    RADIO_APP_Data.CmdCounter++;

    if (Msg->DataLength > 256)
    {
        CFE_EVS_SendEvent(RADIO_APP_TX_ERR_EID, CFE_EVS_EventType_ERROR,
                          "RADIO: Data length exceeds maximum (256)");
        RADIO_APP_Data.ErrCounter++;
        return CFE_SB_BAD_ARGUMENT;
    }

    status = RADIO_APP_SendTxToBusComms(Msg);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(RADIO_APP_TX_ERR_EID, CFE_EVS_EventType_ERROR,
                          "RADIO: Failed to send transmit command to bus_comms, RC = 0x%08lX", (unsigned long)status);
        RADIO_APP_Data.ErrCounter++;
        return status;
    }

    CFE_EVS_SendEvent(RADIO_APP_TRANSMIT_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "RADIO: Transmit command sent - Dest=%u, Port=%u, Len=%u", Msg->DestAddress, Msg->DestPort,
                      Msg->DataLength);

    return CFE_SUCCESS;
}

int32 RADIO_APP_RequestHousekeeping(const RADIO_APP_HkRequestCmd_t *Msg)
{
    return RADIO_APP_ReportHousekeeping((CFE_MSG_CommandHeader_t *)Msg);
}

int32 RADIO_APP_SendConfigToBusComms(const RADIO_APP_ConfigureCmd_t *ConfigCmd)
{
    BUS_COMMS_SendCspCmd_t BusCommsCmd;
    int32                  status;

    if (ConfigCmd->CommandLength > BUS_COMMS_MAX_SEND_LEN)
    {
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    memset(&BusCommsCmd, 0, sizeof(BusCommsCmd));

    memcpy(BusCommsCmd.data, ConfigCmd->Command, ConfigCmd->CommandLength);
    BusCommsCmd.dest = RADIO_APP_RADIO_NODE_ADDR;
    BusCommsCmd.port = RADIO_APP_CONFIG_PORT;
    BusCommsCmd.len  = ConfigCmd->CommandLength;

    CFE_MSG_Init(CFE_MSG_PTR(BusCommsCmd.CmdHdr), CFE_SB_ValueToMsgId(BUS_COMMS_CMD_MID), sizeof(BusCommsCmd));
    CFE_MSG_SetFcnCode(CFE_MSG_PTR(BusCommsCmd.CmdHdr), BUS_COMMS_SEND_CSP_CC);

    status = CFE_SB_TransmitMsg(CFE_MSG_PTR(BusCommsCmd.CmdHdr), true);
    if (status == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(RADIO_APP_BUS_COMMS_SEND_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "RADIO: Config command sent to bus_comms - Dest=%u, Port=%u, Len=%u",
                          BusCommsCmd.dest, BusCommsCmd.port, BusCommsCmd.len);
    }
    return status;
}

int32 RADIO_APP_SendTxToBusComms(const RADIO_APP_TransmitCmd_t *TxCmd)
{
    BUS_COMMS_SendCspCmd_t BusCommsCmd;
    int32                  status;

    if (TxCmd->DataLength > BUS_COMMS_MAX_SEND_LEN)
    {
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    memset(&BusCommsCmd, 0, sizeof(BusCommsCmd));

    memcpy(BusCommsCmd.data, TxCmd->Data, TxCmd->DataLength);
    BusCommsCmd.dest = TxCmd->DestAddress;
    BusCommsCmd.port = TxCmd->DestPort;
    BusCommsCmd.len  = TxCmd->DataLength;

    CFE_MSG_Init(CFE_MSG_PTR(BusCommsCmd.CmdHdr), CFE_SB_ValueToMsgId(BUS_COMMS_CMD_MID), sizeof(BusCommsCmd));
    CFE_MSG_SetFcnCode(CFE_MSG_PTR(BusCommsCmd.CmdHdr), BUS_COMMS_SEND_CSP_CC);

    status = CFE_SB_TransmitMsg(CFE_MSG_PTR(BusCommsCmd.CmdHdr), true);
    if (status == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(RADIO_APP_BUS_COMMS_SEND_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "RADIO: Transmit data sent to bus_comms - Dest=%u, Port=%u, Len=%u",
                          BusCommsCmd.dest, BusCommsCmd.port, BusCommsCmd.len);
    }
    return status;
}

bool RADIO_APP_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
{
    bool              result       = true;
    size_t            ActualLength = 0;
    CFE_SB_MsgId_t    MsgId        = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t FcnCode      = 0;

    CFE_MSG_GetSize(MsgPtr, &ActualLength);

    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(MsgPtr, &MsgId);
        CFE_MSG_GetFcnCode(MsgPtr, &FcnCode);

        CFE_EVS_SendEvent(RADIO_APP_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        RADIO_APP_Data.ErrCounter++;
    }

    return result;
}

