/************************************************************************
 * Bus Communications App - message IDs (temporary, adjust to mission)
 ************************************************************************/
#ifndef BUS_COMMS_MSGIDS_H
#define BUS_COMMS_MSGIDS_H

/* Ground commands and HK */
#define BUS_COMMS_CMD_MID     0x1888
#define BUS_COMMS_SEND_HK_MID 0x1889
#define BUS_COMMS_HK_TLM_MID  0x0889

/*
 * SBN/Software Bus message IDs for inter-app communication
 * 
 * Other apps (GPS, Radio, ADCS) send requests to BUS_COMMS_CAN_REQUEST_MID.
 * BUS_COMMS publishes responses on BUS_COMMS_CAN_RESPONSE_MID.
 */
#define BUS_COMMS_CAN_REQUEST_MID   0x1890   /* Apps send CAN requests here */
#define BUS_COMMS_CAN_RESPONSE_MID  0x0890   /* BUS_COMMS publishes responses here */

#endif /* BUS_COMMS_MSGIDS_H */
