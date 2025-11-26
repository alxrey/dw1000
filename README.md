# DW1000 Zephyr Driver

This project is a Zephyr RTOS driver for the DW1000 Ultra-Wideband (UWB) transceiver. It is primarily a fork of [foldedtoad/dwm1001](https://github.com/foldedtoad/dwm1001), extended and restructured to function as a proper Zephyr module while keeping most source files largely unchanged.

## Design Philosophy

This driver implementation stays as close as possible to the original Decawave driver to maintain consistency with the DW1000 device driver API and the original driver architecture.

However, some modifications have been made to improve code readability and ensure better integration with the Zephyr ecosystem:

- Replaced Decawave-specific type redefinitions with standard stdint types
- Additional modifications to enhance structure coherence with Zephyr conventions

> **Note:** This implementation preserves the original driver's functionality while adapting to Zephyr's coding standards and type system.

## Key Features

- Restructured as a native Zephyr module for easy integration
- West extension for streamlined building of all samples with a single command
- Support for the radino40_usbstick board - a compact USB development stick by in-circuit featuring an integrated DW1000 module, ideal for UWB prototyping

## Quick Start

To build all samples for a specific board using the west extension:

```bash
west build-samples -b <board_name>
```
