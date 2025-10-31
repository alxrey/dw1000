# Overview

## USB
The USB can either be connected to the native USB peripheral of the nRF52840 SoC (R3/R4 mounted) or to the UART-to-USB bridge (R7/R8 mounted).
When using the native USB peripheral, the CDC ACM UART device will be used as the console and shell interface. When using the UART-to-USB bridge, the UART0 peripheral will be used instead.