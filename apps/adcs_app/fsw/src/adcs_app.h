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
 * Main header file for the ADCS application
 */

#ifndef ADCS_APP_H
#define ADCS_APP_H

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "math.h"

#include "adcs_app_perfids.h"
#include "adcs_app_msgids.h"
#include "adcs_app_msg.h"
#include "adcs_app_inter_mids.h"

/***********************************************************************/
#define ADCS_APP_PIPE_DEPTH 32 /* Depth of the Command Pipe for Application */

#define ADCS_APP_NUMBER_OF_TABLES 1 /* Number of Table(s) */

/* Define filenames of default data images for tables */
#define ADCS_APP_TABLE_FILE "/cf/adcs_app_tbl.tbl"

#define ADCS_APP_TABLE_OUT_OF_RANGE_ERR_CODE -1

#define ADCS_APP_TBL_ELEMENT_1_MAX 10

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
    ADCS_APP_HkTlm_t HkTlm;

    /*
    ** Run Status variable used in the main processing loop
    */
    uint32 RunStatus;

    /*
    ** Operational data (not reported in housekeeping)...
    */
    CFE_SB_PipeId_t CommandPipe;

    /*
    ** Initialization data (not reported in housekeeping)...
    */
    char   PipeName[CFE_MISSION_MAX_API_LEN];
    uint16 PipeDepth;

    CFE_TBL_Handle_t TblHandles[ADCS_APP_NUMBER_OF_TABLES];
} ADCS_APP_Data_t;

typedef enum {
    CON_NONE = 0,
    CON_SUN_TRACK = 13,
    CON_TGT_TRACK = 14,
} conModeEnum;

typedef enum {
    EST_NONE = 0,
    EST_GYRO = 1,
    EST_MAG_RKF = 2,
    EST_FULL_EKF = 5,
    EST_GYRO_EKF = 6,
} estModeEnum;

typedef enum {
    TO_SUN = 0,
    FROM_SUN = 1,
} configToFromSun_t;

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (ADCS_APP_Main), these
**       functions are not called from any other source module.
*/
void  ADCS_APP_Main(void);
int32 ADCS_APP_Init(void);
void  ADCS_APP_ProcessCommandPacket(CFE_SB_Buffer_t *SBBufPtr);
void  ADCS_APP_ProcessGroundCommand(CFE_SB_Buffer_t *SBBufPtr);
int32 ADCS_APP_ReportHousekeeping(const CFE_MSG_CommandHeader_t *Msg);
int32 ADCS_APP_ResetCounters(const ADCS_APP_ResetCountersCmd_t *Msg);
int32 ADCS_APP_Process(const ADCS_APP_ProcessCmd_t *Msg);
int32 ADCS_APP_Noop(const ADCS_APP_NoopCmd_t *Msg);
void  ADCS_APP_GetCrc(const char *TableName);
void ADCS_APP_InterAppCmds(CFE_SB_Buffer_t *SBBufPtr);
int32 ADCS_APP_TblValidationFunc(void *TblData);

bool ADCS_APP_VerifyCmdLength(CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);
void ADCS_APP_PointSun(void);
void ADCS_APP_TurnFromSun(void);
void ADCS_APP_GndPnt(float lat, float lon, float alt);
void ADCS_APP_StopCon(void);
void ADCS_APP_InitRateEst(void);
void sendPower(uint8_t rw0, uint8_t rw1, uint8_t rw2, uint8_t mag0, uint8_t mag1, uint8_t gyr, uint8_t fss, uint8_t str);
void sendConfigADCSSatellite(int16_t sunVecX, int16_t sunVecY, int16_t sunVecZ, int16_t tgVecX, int16_t tgVecY, int16_t tgVecZ);
void sendRef(float lat, float lon, float alt);
void sendConMode(conModeEnum controlType);
void sendEstMode(estModeEnum estimatorType, estModeEnum backup);
#endif /* ADCS_APP_H */
