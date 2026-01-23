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

#ifndef RADIO_APP_TABLE_H
#define RADIO_APP_TABLE_H

#include "cfe.h"

typedef struct
{
    /* CSP address (adr) - Set module address */
    uint8 CspAddress;

    /* RF frequency (f) - Set radio TX/RX frequency (Hz) */
    uint32 RfFrequency;

    /* Power level (pwr/pwrm) - Set power for data/Morse (0-100) */
    uint8  PowerData;
    uint8  PowerMorse;

    /* Power table calibration (pwrtab) - Calibrate fine output levels */
    uint8  PowerTableCal[16]; /* Array for power calibration table */

    /* PA voltage (pa) - Amplifier control voltage (mV) */
    uint16 PaVoltage;

    /* Power enables/timers (pwren) - TX enable delays/timers (ms) */
    uint16 PowerEnableDelay;
    uint16 PowerEnableTimer;

    /* Interface speeds (baud) - Set I2C, RS485, UART, TRX baudrate */
    uint32 I2cBaudrate;
    uint32 Rs485Baudrate;
    uint32 UartBaudrate;
    uint32 TrxBaudrate;

    /* Antenna deployment (afr/ant) - Timers/sequences for antenna board (ms) */
    uint16 AntennaDeployTimer;
    uint16 AntennaDeploySequence;

    /* AX.25/Morse beacon/msg (bca/bcm/msg/mse) - Configure text, period, start */
    char   BeaconText[64];      /* Beacon message text */
    uint16 BeaconPeriod;        /* Beacon period (seconds) */
    uint8  BeaconEnabled;       /* Beacon enable flag */
    char   MorseMessage[64];    /* Morse message text */
    uint8  MorseEnabled;        /* Morse enable flag */

    /* Morse speed (msewpm) - Set Morse rate (words per minute) */
    uint8  MorseSpeed;

    /* Morse limiter (mselim) - Set Morse minimum gap (ms) */
    uint16 MorseLimiter;

    /* Callsign (cs) - Set callsign */
    char   Callsign[16];

    /* Preamble length (pm) - Number of HDLC flag at TX start */
    uint16 PreambleLength;

    /* Inter-RX/TX delay (td) - Time after RX before allowing TX (ms) */
    uint16 InterRxTxDelay;

    /* Max PA temp (mtl) - Overtemp rating (degrees C) */
    uint8  MaxPaTemp;

    /* VCTCXO freq. compensation (xc) - Oscillator calibration (Hz offset) */
    int32  VctcxoFreqComp;

    /* Routing table & bridges (rt/rtr) - Set/disable packet routing */
    uint8  RoutingEnabled;
    uint8  BridgeEnabled;

    /* Service level/password (svc/pwd) - Authorization controls */
    uint8  ServiceLevel;
    char   Password[32];

    /* Reserved for future use */
    uint8  Spare[4];
} RADIO_APP_Table_t;

#endif /* RADIO_APP_TABLE_H */
