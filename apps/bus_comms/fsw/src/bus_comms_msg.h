/************************************************************************
 * Bus Communications App - messages
 ************************************************************************/
#ifndef BUS_COMMS_MSG_H
#define BUS_COMMS_MSG_H

#include "cfe_msg.h"
#include <stdint.h>

/* Command codes for ground commands */
#define BUS_COMMS_NOOP_CC           0
#define BUS_COMMS_RESET_COUNTERS_CC 1
#define BUS_COMMS_TEST_CAN_CC       0x12  /* Send a test CAN message via SBN pipe */

/* Max payload size for CAN messages (standard CAN is 8 bytes, but allow more for CAN FD or fragmented) */
#define BUS_COMMS_CAN_MAX_DATA_LEN  64

/*
 * Message direction indicator for CAN request/response messages.
 * Used to distinguish between outbound requests and inbound responses.
 */
typedef enum
{
    BUS_COMMS_DIR_REQUEST  = 0,   /* Outbound: app wants to send data to hardware via CAN */
    BUS_COMMS_DIR_RESPONSE = 1    /* Inbound: hardware response coming back to the requesting app */
} BUS_COMMS_MsgDirection_t;

/*
 * Error codes returned in CAN response messages
 */
typedef enum
{
    BUS_COMMS_ERR_NONE           = 0,
    BUS_COMMS_ERR_TIMEOUT        = 1,   /* No response from hardware within timeout */
    BUS_COMMS_ERR_CAN_TX_FAIL    = 2,   /* Failed to transmit on CAN bus */
    BUS_COMMS_ERR_CAN_RX_FAIL    = 3,   /* Failed to receive on CAN bus */
    BUS_COMMS_ERR_QUEUE_FULL     = 4,   /* Request queue is full, try again later */
    BUS_COMMS_ERR_INVALID_DEST   = 5,   /* Invalid destination node */
    BUS_COMMS_ERR_PAYLOAD_SIZE   = 6    /* Payload too large */
} BUS_COMMS_ErrorCode_t;

/*
 * Identifies which app/subsystem is making the request.
 * Useful for routing responses and debugging.
 */
typedef enum
{
    BUS_COMMS_SUBSYS_UNKNOWN = 0,
    BUS_COMMS_SUBSYS_GPS     = 1,
    BUS_COMMS_SUBSYS_RADIO   = 2,
    BUS_COMMS_SUBSYS_ADCS    = 3,
    BUS_COMMS_SUBSYS_GROUND  = 4    /* For ground-initiated test commands */
} BUS_COMMS_Subsystem_t;

/*
 * CAN Request Message
 * 
 * Apps (GPS, ADCS, Radio) send this to BUS_COMMS_CAN_REQUEST_MID to request
 * data be transmitted over the CAN bus to a hardware node.
 * 
 * Fields:
 *   direction   - Should be BUS_COMMS_DIR_REQUEST for outbound messages
 *   subsystem   - Which app is making the request (for response routing)
 *   dest_node   - CSP destination node ID (the hardware target)
 *   dest_port   - CSP destination port
 *   sequence    - Sequence number set by requester (echoed in response)
 *   data_len    - Length of payload data in bytes
 *   data        - Payload to send over CAN
 */
typedef struct
{
    CFE_MSG_CommandHeader_t Header;

    uint8_t  direction;     /* BUS_COMMS_MsgDirection_t */
    uint8_t  subsystem;     /* BUS_COMMS_Subsystem_t - who is sending */
    uint8_t  dest_node;     /* CSP node ID for target hardware */
    uint8_t  dest_port;     /* CSP port on target */
    uint16_t sequence;      /* Sequence number (echoed in response) */
    uint16_t data_len;      /* Bytes of valid data */
    uint8_t  data[BUS_COMMS_CAN_MAX_DATA_LEN];

} BUS_COMMS_CanRequest_t;

/*
 * CAN Response Message
 * 
 * BUS_COMMS publishes this on BUS_COMMS_CAN_RESPONSE_MID after processing
 * a CAN transaction (either successfully or with an error).
 * 
 * Fields:
 *   direction   - BUS_COMMS_DIR_RESPONSE for inbound data
 *   subsystem   - Echoed from the original request
 *   src_node    - CSP source node ID (which hardware sent this)
 *   src_port    - CSP source port
 *   sequence    - Echoed from the original request
 *   error_code  - 0 if success, otherwise an error code
 *   data_len    - Length of response payload
 *   data        - Response payload from hardware
 */
typedef struct
{
    CFE_MSG_TelemetryHeader_t Header;

    uint8_t  direction;     /* BUS_COMMS_MsgDirection_t */
    uint8_t  subsystem;     /* Echoed from request */
    uint8_t  src_node;      /* CSP node that sent the response */
    uint8_t  src_port;      /* CSP port */
    uint16_t sequence;      /* Echoed from request */
    uint8_t  error_code;    /* BUS_COMMS_ErrorCode_t */
    uint8_t  spare;
    uint16_t data_len;
    uint8_t  data[BUS_COMMS_CAN_MAX_DATA_LEN];

} BUS_COMMS_CanResponse_t;

/*
 * Test CAN Command
 * 
 * Ground command to test CAN transmission. This simulates what another
 * app (GPS, ADCS, Radio) would do - sends a request through the SBN pipe.
 */
typedef struct
{
    CFE_MSG_CommandHeader_t Header;
    uint8_t  dest_node;     /* CSP node ID for target hardware */
    uint8_t  dest_port;     /* CSP port on target */
    uint16_t data_len;      /* Bytes of test data to send */
    uint8_t  data[BUS_COMMS_CAN_MAX_DATA_LEN];
} BUS_COMMS_TestCanCmd_t;

/*
 * Housekeeping telemetry
 */
typedef struct
{
    CFE_MSG_TelemetryHeader_t TelemetryHeader;
    struct {
        uint8  CommandCounter;
        uint8  CommandErrorCounter;
        uint16 CanTxCount;       /* Total CAN messages transmitted */
        uint16 CanRxCount;       /* Total CAN messages received */
        uint16 SbnRequestCount;  /* Requests received from other apps */
        uint16 SbnResponseCount; /* Responses published to other apps */
        uint8  QueueDepth;       /* Current pending requests in queue */
        uint8  Spare;
    } Payload;
} BUS_COMMS_HkTlm_t;

#endif /* BUS_COMMS_MSG_H */
