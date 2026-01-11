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
 *  The Radio Manager App header file containing version information
 */

#ifndef RADIO_APP_VERSION_H
#define RADIO_APP_VERSION_H

#define RADIO_APP_BUILD_NUMBER 1
#define RADIO_APP_BUILD_BASELINE "v1.0.0"

#define RADIO_APP_MAJOR_VERSION 1
#define RADIO_APP_MINOR_VERSION 0
#define RADIO_APP_REVISION      0
#define RADIO_APP_MISSION_REV   0

#define RADIO_APP_STR_HELPER(x) #x
#define RADIO_APP_STR(x) RADIO_APP_STR_HELPER(x)

#define RADIO_APP_VERSION RADIO_APP_BUILD_BASELINE "+dev" RADIO_APP_STR(RADIO_APP_BUILD_NUMBER)

#define RADIO_APP_VERSION_STRING \
    " Radio Manager App DEVELOPMENT BUILD " RADIO_APP_VERSION \
    ", Last Official Release: v1.0.0"

#endif /* RADIO_APP_VERSION_H */

