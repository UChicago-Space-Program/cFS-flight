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
 * Define RADIO App Messages and info
 */

#ifndef RADIO_APP_MSG_H
#define RADIO_APP_MSG_H

/*
** RADIO App command codes
*/
#define RADIO_APP_NOOP_CC           0
#define RADIO_APP_RESET_COUNTERS_CC 1
#define RADIO_APP_TRANSMIT_CC       2
#define RADIO_APP_CONFIGURE_CC      3
#define RADIO_APP_HK_REQUEST_CC     4

/*************************************************************************/

/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader; /**< \brief Command header */
} RADIO_APP_NoArgsCmd_t;

/*
** AX100 Transmit Command
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader; /**< \brief Command header */
    uint8  DestAddress;                /**< \brief CSP destination address */
    uint8  DestPort;                   /**< \brief CSP destination port */
    uint16 DataLength;                 /**< \brief Length of data to transmit */
    uint8  Data[256];                  /**< \brief Data payload to transmit */
} RADIO_APP_TransmitCmd_t;

/*
** AX100 Configuration Command
*/
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader; /**< \brief Command header */
    uint32 TxFrequency;                /**< \brief Transmit frequency in Hz */
    uint32 RxFrequency;                /**< \brief Receive frequency in Hz */
    uint8  TxPower;                    /**< \brief Transmit power level */
    uint8  Modulation;                 /**< \brief Modulation type */
    uint16 Spare;                      /**< \brief Padding */
} RADIO_APP_ConfigureCmd_t;

/*
** The following commands use the "NoArgs" format
*/
typedef RADIO_APP_NoArgsCmd_t RADIO_APP_NoopCmd_t;
typedef RADIO_APP_NoArgsCmd_t RADIO_APP_ResetCountersCmd_t;
typedef RADIO_APP_NoArgsCmd_t RADIO_APP_HkRequestCmd_t;

/*************************************************************************/
/*
** Type definition (RADIO App housekeeping)
*/

typedef struct
{
    uint8  CommandErrorCounter;
    uint8  CommandCounter;
    uint16 TxCount;                    /**< \brief Number of packets transmitted */
    uint16 RxCount;                    /**< \brief Number of packets received */
    int16  BoardTemp;                  /**< \brief AX100 board temperature (C * 100) */
    int16  PATemp;                     /**< \brief Power amplifier temperature (C * 100) */
    int16  LastRSSI;                   /**< \brief Last received signal strength (dBm) */
    uint8  RadioStatus;                /**< \brief Radio status flags */
    uint8  spare;
} RADIO_APP_HkTlm_Payload_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t   TelemetryHeader; /**< \brief Telemetry header */
    RADIO_APP_HkTlm_Payload_t   Payload;         /**< \brief Telemetry payload */
} RADIO_APP_HkTlm_t;

/*
** AX100 Receive Data Telemetry
*/
typedef struct
{
    uint8  SourceAddress;              /**< \brief CSP source address */
    uint8  SourcePort;                 /**< \brief CSP source port */
    uint16 DataLength;                 /**< \brief Length of received data */
    int16  RSSI;                       /**< \brief Received signal strength */
    uint8  spare[2];
    uint8  Data[256];                  /**< \brief Received data payload */
} RADIO_APP_RxTlm_Payload_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t   TelemetryHeader; /**< \brief Telemetry header */
    RADIO_APP_RxTlm_Payload_t   Payload;         /**< \brief Telemetry payload */
} RADIO_APP_RxTlm_t;

#endif /* RADIO_APP_MSG_H */
