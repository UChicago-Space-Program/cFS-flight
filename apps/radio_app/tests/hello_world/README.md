# Hello World Testing for Gomspace AX100

Simplest test between a BeagleBone and its connected AX100 via CSP through CAN. The beaglebone sends a packet to the AX100, which immediately echos the message back. Importantly, this doesn't test the RF capabilities of the AX100.

## Setup AX100

```bash
sudo apt install minicom
minicom -D /dev/ttyO1 -b 115200 -8 -o
```
1. Replace `/dev/ttyO1` with your BBB UART device.
2. Baud rate 115200, 8 data bits, no parity, 1 stop bit.

Then run

```
set protocol csp
set uart0 baud 115200
set uart0 databits 8
set uart0 stopbits 1
set uart0 parity none
set uart0 enable
set csp nodeid 2
save
```

Then
```
set mode transparent (optionally lookback)
save
```

## Setup BBB

The AX100 can be configured via the BBB's connection.


Testing requires the AX100 to be connected to the BeagleBoneBlack will run the `hello_csp.c` script. This means libcsp must be installed on the BBB.
