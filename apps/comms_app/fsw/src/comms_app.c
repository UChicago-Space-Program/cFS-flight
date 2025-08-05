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
 * \file
 *   This file contains the source code for the Comms App.
 */

/*
** Include Files:
*/
#include "comms_app_events.h"
#include "comms_app_version.h"
#include "comms_app.h"
#include "comms_app_table.h"

/* The comms_lib module provides the COMMS_LIB_Function() prototype */
#include <string.h>
#include "comms_lib.h"
#include <csp/csp.h>
#include <csp/interfaces/csp_if_can.h>
#include <csp/drivers/can_socketcan.h>

/*
** global data
*/
COMMS_APP_Data_t COMMS_APP_Data;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
/*                                                                            */
/* Application entry point and main process loop                              */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void COMMS_APP_Main(void)
{
    int32            status;
    CFE_SB_Buffer_t *SBBufPtr;

    /*
    ** Create the first Performance Log entry
    */
    CFE_ES_PerfLogEntry(COMMS_APP_PERF_ID);

    /*
    ** Perform application specific initialization
    ** If the Initialization fails, set the RunStatus to
    ** CFE_ES_RunStatus_APP_ERROR and the App will not enter the RunLoop
    */
    status = COMMS_APP_Init();
    if (status != CFE_SUCCESS)
    {
        COMMS_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /*
    ** COMMS Runloop
    */
    while (CFE_ES_RunLoop(&COMMS_APP_Data.RunStatus) == true)
    {
        /*
        ** Performance Log Exit Stamp
        */
        CFE_ES_PerfLogExit(COMMS_APP_PERF_ID);

        /* Pend on receipt of command packet */
        status = CFE_SB_ReceiveBuffer(&SBBufPtr, COMMS_APP_Data.CommandPipe, CFE_SB_PEND_FOREVER);

        /*
        ** Performance Log Entry Stamp
        */
        CFE_ES_PerfLogEntry(COMMS_APP_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            COMMS_APP_ProcessCommandPacket(SBBufPtr);
        }
        else
        {
            CFE_EVS_SendEvent(COMMS_APP_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "COMMS APP: SB Pipe Read Error, App Will Exit");

            COMMS_APP_Data.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }

    /*
    ** Performance Log Exit Stamp
    */
    CFE_ES_PerfLogExit(COMMS_APP_PERF_ID);

    CFE_ES_ExitApp(COMMS_APP_Data.RunStatus);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* Initialization                                                             */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 COMMS_APP_Init(void)
{
    int32 status;

    COMMS_APP_Data.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /*
    ** Initialize app command execution counters
    */
    COMMS_APP_Data.CmdCounter = 0;
    COMMS_APP_Data.ErrCounter = 0;

    /*
    ** Initialize app configuration data
    */
    COMMS_APP_Data.PipeDepth = COMMS_APP_PIPE_DEPTH;

    strncpy(COMMS_APP_Data.PipeName, "COMMS_APP_CMD_PIPE", sizeof(COMMS_APP_Data.PipeName));
    COMMS_APP_Data.PipeName[sizeof(COMMS_APP_Data.PipeName) - 1] = 0;

    /*
    ** Register the events
    */
    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Comms App: Error Registering Events, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    /*
    ** Initialize housekeeping packet (clear user data area).
    */
    CFE_MSG_Init(CFE_MSG_PTR(COMMS_APP_Data.HkTlm.TelemetryHeader), CFE_SB_ValueToMsgId(COMMS_APP_HK_TLM_MID),
                 sizeof(COMMS_APP_Data.HkTlm));

    /*
    ** Create Software Bus message pipe.
    */
    status = CFE_SB_CreatePipe(&COMMS_APP_Data.CommandPipe, COMMS_APP_Data.PipeDepth, COMMS_APP_Data.PipeName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Comms App: Error creating pipe, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    /*
    ** Subscribe to Housekeeping request commands
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(COMMS_APP_SEND_HK_MID), COMMS_APP_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Comms App: Error Subscribing to HK request, RC = 0x%08lX\n", (unsigned long)status);
        return status;
    }

    /*
    ** Subscribe to ground command packets
    */
    status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(COMMS_APP_CMD_MID), COMMS_APP_Data.CommandPipe);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Comms App: Error Subscribing to Command, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }

    /*
    ** Register Table(s)
    */
    status = CFE_TBL_Register(&COMMS_APP_Data.TblHandles[0], "CommsAppTable", sizeof(COMMS_APP_Table_t),
                              CFE_TBL_OPT_DEFAULT, COMMS_APP_TblValidationFunc);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Comms App: Error Registering Table, RC = 0x%08lX\n", (unsigned long)status);

        return status;
    }
    else
    {
        status = CFE_TBL_Load(COMMS_APP_Data.TblHandles[0], CFE_TBL_SRC_FILE, COMMS_APP_TABLE_FILE);
    }

    CFE_EVS_SendEvent(COMMS_APP_STARTUP_INF_EID, CFE_EVS_EventType_INFORMATION, "COMMS App Initialized.%s",
                      COMMS_APP_VERSION_STRING);
    COMMS_APP_InitCAN("vcan0");
    

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the COMMS    */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void COMMS_APP_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MsgId);

    switch (CFE_SB_MsgIdToValue(MsgId))
    {
        case COMMS_APP_CMD_MID:
            COMMS_APP_ProcessGroundCommand(SBBufPtr);
            break;

        case COMMS_APP_SEND_HK_MID:
            COMMS_APP_ReportHousekeeping((CFE_MSG_CommandHeader_t *)SBBufPtr);
            break;

        default:
            CFE_EVS_SendEvent(COMMS_APP_INVALID_MSGID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "COMMS: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* COMMS ground commands                                                     */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void COMMS_APP_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /*
    ** Process "known" COMMS app ground commands
    */
    switch (CommandCode)
    {
        case COMMS_APP_NOOP_CC:
            if (COMMS_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(COMMS_APP_NoopCmd_t)))
            {
                COMMS_APP_Noop((COMMS_APP_NoopCmd_t *)SBBufPtr);
            }

            break;

        case COMMS_APP_RESET_COUNTERS_CC:
            if (COMMS_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(COMMS_APP_ResetCountersCmd_t)))
            {
                COMMS_APP_ResetCounters((COMMS_APP_ResetCountersCmd_t *)SBBufPtr);
            }

            break;

        case COMMS_APP_PROCESS_CC:
            if (COMMS_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(COMMS_APP_ProcessCmd_t)))
            {
                COMMS_APP_Process((COMMS_APP_ProcessCmd_t *)SBBufPtr);
            }

            break;
        case COMMS_APP_CANWR_CC:
            if (COMMS_APP_VerifyCmdLength(&SBBufPtr->Msg, sizeof(COMMS_APP_CANCmd_t))){
                COMMS_APP_CANCmd_t *cmd = (COMMS_APP_CANCmd_t *)SBBufPtr;
                COMMS_APP_SendCAN(cmd->Bus, cmd->Id, cmd->Data);
            }
            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(COMMS_APP_COMMAND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid ground command code: CC = %d", CommandCode);
            break;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 COMMS_APP_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg)
{
    int i;

    /*
    ** Get command execution counters...
    */
    COMMS_APP_Data.HkTlm.Payload.CommandErrorCounter = COMMS_APP_Data.ErrCounter;
    COMMS_APP_Data.HkTlm.Payload.CommandCounter      = COMMS_APP_Data.CmdCounter;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(COMMS_APP_Data.HkTlm.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(COMMS_APP_Data.HkTlm.TelemetryHeader), true);

    /*
    ** Manage any pending table loads, validations, etc.
    */
    for (i = 0; i < COMMS_APP_NUMBER_OF_TABLES; i++)
    {
        CFE_TBL_Manage(COMMS_APP_Data.TblHandles[i]);
    }

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* COMMS NOOP commands                                                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
int32 COMMS_APP_Noop(const COMMS_APP_NoopCmd_t *Msg)
{
    COMMS_APP_Data.CmdCounter++;

    CFE_EVS_SendEvent(COMMS_APP_COMMANDNOP_INF_EID, CFE_EVS_EventType_INFORMATION, "COMMS: NOOP command %s",
                      COMMS_APP_VERSION);
    COMMS_APP_SendCAN("2", "123", "COCO");
    COMMS_APP_SendCAN("2", "123", "YODA");
    COMMS_APP_SendCAN("2", "123", "SAM");

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 COMMS_APP_ResetCounters(const COMMS_APP_ResetCountersCmd_t *Msg)
{
    COMMS_APP_Data.CmdCounter = 0;
    COMMS_APP_Data.ErrCounter = 0;

    CFE_EVS_SendEvent(COMMS_APP_COMMANDRST_INF_EID, CFE_EVS_EventType_INFORMATION, "COMMS: RESET command");

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function Process Ground Station Command                       */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
int32 COMMS_APP_Process(const COMMS_APP_ProcessCmd_t *Msg)
{
    int32               status;
    COMMS_APP_Table_t *TblPtr;
    const char *        TableName = "COMMS_APP.CommsAppTable";

    /* Comms Use of Table */

    status = CFE_TBL_GetAddress((void *)&TblPtr, COMMS_APP_Data.TblHandles[0]);

    if (status < CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Comms App: Fail to get table address: 0x%08lx", (unsigned long)status);
        return status;
    }

    CFE_ES_WriteToSysLog("Comms App: Table Value 1: %d  Value 2: %d", TblPtr->Int1, TblPtr->Int2);

    COMMS_APP_GetCrc(TableName);

    status = CFE_TBL_ReleaseAddress(COMMS_APP_Data.TblHandles[0]);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Comms App: Fail to release table address: 0x%08lx", (unsigned long)status);
        return status;
    }

    /* Invoke a function provided by COMMS_APP_LIB */
    COMMS_LIB_Function();

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* Verify command packet length                                               */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
bool COMMS_APP_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
{
    bool              result       = true;
    size_t            ActualLength = 0;
    CFE_SB_MsgId_t    MsgId        = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t FcnCode      = 0;

    CFE_MSG_GetSize(MsgPtr, &ActualLength);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(MsgPtr, &MsgId);
        CFE_MSG_GetFcnCode(MsgPtr, &FcnCode);

        CFE_EVS_SendEvent(COMMS_APP_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)FcnCode, (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;

        COMMS_APP_Data.ErrCounter++;
    }

    return result;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Verify contents of First Table buffer contents                  */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 COMMS_APP_TblValidationFunc(void *TblData)
{
    int32               ReturnCode = CFE_SUCCESS;
    COMMS_APP_Table_t *TblDataPtr = (COMMS_APP_Table_t *)TblData;

    /*
    ** Comms Table Validation
    */
    if (TblDataPtr->Int1 > COMMS_APP_TBL_ELEMENT_1_MAX)
    {
        /* First element is out of range, return an appropriate error code */
        ReturnCode = COMMS_APP_TABLE_OUT_OF_RANGE_ERR_CODE;
    }

    return ReturnCode;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Output CRC                                                      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void COMMS_APP_GetCrc(const char *TableName)
{
    int32          status;
    uint32         Crc;
    CFE_TBL_Info_t TblInfoPtr;

    status = CFE_TBL_GetInfo(&TblInfoPtr, TableName);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Comms App: Error Getting Table Info");
    }
    else
    {
        Crc = TblInfoPtr.Crc;
        CFE_ES_WriteToSysLog("Comms App: CRC: 0x%08lX\n\n", (unsigned long)Crc);
    }
}

int COMMS_APP_SendCAN(const char *dest_str, const char *port_str, const char *message) {
    uint8_t dest = (uint8_t) strtol(dest_str, NULL, 10);
    uint8_t port = (uint8_t) strtol(port_str, NULL, 10);
    size_t len = strlen(message);

    if (len > CSP_BUFFER_SIZE) {
        CFE_EVS_SendEvent(COMMS_APP_CAN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Message too long for CSP buffer (%u bytes)", len);
        return -1;
    }

    csp_conn_t *conn = csp_connect(CSP_PRIO_NORM, dest, port, 1000, CSP_O_NONE);
    if (conn == NULL) {
        CFE_EVS_SendEvent(COMMS_APP_CAN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Failed to connect to CSP node %u port %u", dest, port);
        return -1;
    }

    csp_packet_t *packet = csp_buffer_get(len);
    if (packet == NULL) {
        CFE_EVS_SendEvent(COMMS_APP_CAN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Failed to get CSP buffer");
        csp_close(conn);
        return -1;
    }

    memcpy(packet->data, message, len);
    packet->length = len;

    if (!csp_send(conn, packet)) {
        CFE_EVS_SendEvent(COMMS_APP_CAN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Failed to send CSP packet");
        csp_buffer_free(packet);
        csp_close(conn);
        return -1;
    }

    csp_close(conn);

    CFE_EVS_SendEvent(COMMS_APP_CAN_ERR_EID, CFE_EVS_EventType_INFORMATION,
                      "CSP CAN message sent to node %u port %u", dest, port);
    return 0;
}
//static csp_can_socketcan_handle_t can_handle;
static csp_iface_t *COMMS_CAN_IFACE = NULL;


int COMMS_APP_InitCAN(const char *bus) {
    COMMS_CAN_IFACE = csp_can_socketcan_init(bus, 1 /* node ID */, 1000000, true);
    if (COMMS_CAN_IFACE == NULL) {
        CFE_EVS_SendEvent(COMMS_APP_CAN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CSP CAN interface init failed");
        return -1;
    }

    csp_init();

    CFE_EVS_SendEvent(COMMS_APP_CAN_ERR_EID, CFE_EVS_EventType_INFORMATION,
                      "CSP CAN initialized successfully on %s", bus);
    return 0;
}