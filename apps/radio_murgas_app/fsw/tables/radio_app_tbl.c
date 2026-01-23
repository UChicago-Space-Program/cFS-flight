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

#include "cfe_tbl_filedef.h"
#include "radio_app_table.h"

RADIO_APP_Table_t RadioAppTable = {
    /* CSP address (adr) */
    .CspAddress = 1,

    /* RF frequency (f) - 437 MHz default */
    .RfFrequency = 437000000,

    /* Power level (pwr/pwrm) - 30% default for data, 20% for Morse */
    .PowerData  = 30,
    .PowerMorse = 20,

    /* Power table calibration (pwrtab) - Initialize to zeros */
    .PowerTableCal = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

    /* PA voltage (pa) - Default amplifier voltage */
    .PaVoltage = 3300,

    /* Power enables/timers (pwren) - 100ms delay, 1000ms timer */
    .PowerEnableDelay = 100,
    .PowerEnableTimer = 1000,

    /* Interface speeds (baud) - Standard baudrates */
    .I2cBaudrate    = 100000,   /* 100 kHz I2C */
    .Rs485Baudrate  = 9600,     /* 9600 baud RS485 */
    .UartBaudrate   = 9600,     /* 9600 baud UART */
    .TrxBaudrate    = 9600,     /* 9600 baud TRX */

    /* Antenna deployment (afr/ant) - 5 second timer, sequence 0 */
    .AntennaDeployTimer    = 5000,
    .AntennaDeploySequence = 0,

    /* AX.25/Morse beacon/msg (bca/bcm/msg/mse) */
    .BeaconText   = "CALLSIGN BEACON",
    .BeaconPeriod = 60,        /* 60 seconds */
    .BeaconEnabled = 0,        /* Disabled by default */
    .MorseMessage = "CQ CQ CQ",
    .MorseEnabled = 0,         /* Disabled by default */

    /* Morse speed (msewpm) - 20 words per minute */
    .MorseSpeed = 20,

    /* Morse limiter (mselim) - 50ms minimum gap */
    .MorseLimiter = 50,

    /* Callsign (cs) */
    .Callsign = "N0CALL",

    /* Preamble length (pm) - 8 HDLC flags */
    .PreambleLength = 8,

    /* Inter-RX/TX delay (td) - 100ms delay */
    .InterRxTxDelay = 100,

    /* Max PA temp (mtl) - 85 degrees C */
    .MaxPaTemp = 85,

    /* VCTCXO freq. compensation (xc) - No offset */
    .VctcxoFreqComp = 0,

    /* Routing table & bridges (rt/rtr) - Disabled by default */
    .RoutingEnabled = 0,
    .BridgeEnabled  = 0,

    /* Service level/password (svc/pwd) - Level 1, no password */
    .ServiceLevel = 1,
    .Password     = "",

    /* Spare fields */
    .Spare = {0, 0, 0, 0}
};

CFE_TBL_FILEDEF(RadioAppTable, RADIO_APP.RadioAppTable, Radio App Config Table, radio_app_tbl.tbl)
