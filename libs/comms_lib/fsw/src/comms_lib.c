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
 *   Comms CFS library
 */

/*************************************************************************
** Includes
*************************************************************************/
#include "comms_lib_version.h"
#include "comms_lib_internal.h"

/* for "strncpy()" */
#include <string.h>

/*************************************************************************
** Private Data Structures
*************************************************************************/
char COMMS_LIB_Buffer[COMMS_LIB_BUFFER_SIZE];

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Library Initialization Routine                                  */
/* cFE requires that a library have an initialization routine      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 COMMS_LIB_Init(void)
{
    /*
     * Call a C library function, like strcpy(), and test its result.
     *
     * This is primary for a unit test example, to have more than
     * one code path to exercise.
     *
     * The specification for strncpy() indicates that it should return
     * the pointer to the destination buffer, so it should be impossible
     * for this to ever fail when linked with a compliant C library.
     */
    if (strncpy(COMMS_LIB_Buffer, "COMMS DATA", sizeof(COMMS_LIB_Buffer) - 1) != COMMS_LIB_Buffer)
    {
        return CFE_STATUS_NOT_IMPLEMENTED;
    }

    /* ensure termination */
    COMMS_LIB_Buffer[sizeof(COMMS_LIB_Buffer) - 1] = 0;

    OS_printf("COMMS Lib Initialized.%s\n", COMMS_LIB_VERSION_STRING);

    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Comms Lib function                                             */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 COMMS_LIB_Function(void)
{
    OS_printf("COMMS_LIB_Function called, buffer=\'%s\'\n", COMMS_LIB_Buffer);

    return CFE_SUCCESS;
}
