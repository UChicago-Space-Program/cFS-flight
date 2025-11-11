#include <stdio.h>
#include <csp/csp.h>
#include <csp/csp_interface.h>
#include <csp/csp_uart.h>
#include <csp/csp_debug.h>
#include <csp/csp_crc32.h>
#include <unistd.h>

#define BBB_NODE     1
#define AX100_NODE   2
#define BBB_PORT    10
#define AX100_PORT  11

int main(void) {
    csp_init();

    // open can interface to the ax100
    csp_iface_t *iface = NULL;

    int ret = csp_can_socketcan_open_and_add_interface(
        "can0",
        CSP_IF_CAN_DEFAULT_NAME,
        BBB_NODE,
        1000000, 
        true, 
        &iface
    );
    iface->is_default = 1;

    // Omitting csp_route_work() function--required for loopbacks

    // Setup listening socket
    csp_socket_t sock = { 0 };
    csp_bind(&sock, BBB_PORT);
    // backlog of 5 connections
    csp_listen(&sock, 5);

    // Send hello to AX100
    csp_conn_t *conn = csp_connect(CSP_PRIO_NORM, AX100_NODE, AX100_PORT, 1000, CSP_O_NONE);
    if (!conn) {
        printf("Connection failed\n");
        return -1;
    }

    csp_packet_t *packet = csp_buffer_get(0);
    if (!packet) {
        printf("Failed to retrieve package\n");
        return -1;
    }
    memcpy(packet->data, "Hello AX100 \0", 13);
    packet->length = (strlen((char *) packet->data) + 1);
    
    csp_send(conn, packet);

    printf("Message Sent\n");
    
    sleep(1);

    csp_packet_t *rx_packet = csp_read(conn, 5000);
    if (rx_packet) {
        printf("Packet Received: %s\n", (char *) rx_packet->data);
    } else {
        printf("Packet not received");
        return -1;
    }
    
    csp_close(conn);
    return 0;
}