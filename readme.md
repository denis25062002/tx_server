# Embedded TX Protocol Parser

**A lightweight, robust C library for parsing, validating, and handling custom text-based communication protocols over UART/RS-232/RS-485 interfaces.**


---

## Overview

**Embedded TX Protocol Parser** is a zero-dynamic-allocation stream parser designed for embedded systems and microcontroller communication (STM32, ESP32, AVR, PIC, etc.). 

It processes raw, chunked byte streams coming from serial interfaces, discards noise, checks packet integrity via CRC, and parses ASCII-HEX payloads directly into native C structures to execute handler callbacks.

---

## Features

* **Finite State Machine (FSM):** Efficiently isolates frames from noisy incoming byte streams.
* **Zero Allocation:** Operates entirely with pre-allocated buffers — ideal for bare-metal systems and RTOS.
* **CRC Verification:** Built-in checksum validation (XOR-based) for error-tolerant transmission.
* **Layered Architecture:** Clear separation between byte-stream parsing, structural decoding, and server dispatching.
* **Hardware Agnostic:** Easily ties into any hardware abstraction layer (HAL) or Serial API.

---

## Frame Format

The parser works with ASCII-HEX formatted frames structured as follows:

$$\text{T} + \text{[Type]} + \text{[Address]} + \text{[Command ID]} + \text{[Payload]} + \text{[CRC]} + \text{\textbackslash r\textbackslash n}$$

| Field | Size | Description |
| :--- | :--- | :--- |
| **Start Byte** | `1 byte` | Fixed character `'T'` |
| **Frame Type** | `1 byte` | `'X'` (No CRC) or `'S'` (With CRC) |
| **Address (`AD`)** | `2 hex chars` | Target device address (1 byte) |
| **Command (`ID`)** | `2 hex chars` | Command / Message ID (1 byte) |
| **Payload** | `N hex chars` | Optional data bytes |
| **CRC** | `2 hex chars` | XOR checksum (required only if Frame Type = `'S'`) |
| **Delimiter** | `2 bytes` | End of frame sequence (`\r\n` / `0x0D 0x0A`) |

#### Example Frames:
* `TX259703\r\n` — Device `0x25`, Command `0x97`, Data `0x03`, No CRC.
* `TS259709\r\n` — Device `0x25`, Command `0x97`, Validated with CRC `0x09`.